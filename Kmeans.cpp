#include "Kmeans.h"
#include "Common.h"
#include "ClusterTree.h"
#include "Log.h"
#include "ThreadPool/include/ThreadPoolFactory.h"
#include "ThreadPool/include/ThreadPool.h"
#include "ThreadPool/include/Task.h"

#include <fstream>
#include <cmath>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <mutex>
#include <algorithm>

using namespace std;


////////////// �������غ��� ///////////////
int CKMeans::initRandSeed() {
	srand(time(NULL));
	return 0;
}

int CKMeans::nrand(int n) {
	if (!m_options.Consistency)
		static int r = initRandSeed();

	if (n > RAND_MAX)
		n = RAND_MAX;

	int ret;
	int bucket_size = RAND_MAX / n;
	do {
		ret = rand() / bucket_size;
	} while (ret >= n);

	return ret;
}

int CKMeans::getRandomNumByLength(int start, int length) {
	if (RAND_MAX - start < length)
		length = RAND_MAX - start;

	return nrand(length) + start;
}

int CKMeans::getRandomNumByRange(int start, int end) {
	if (end > RAND_MAX) {
		end = RAND_MAX;
	}

	return nrand(end - start) + start;
}
///////////////////////////////////////////////


////////////// ���̲߳��������� ///////////////

// �ַ���¼�����������
class DistributeTask : public Task, public Object
{
public:
	DistributeTask(record_const_iterator cbeg, record_const_iterator cend,
		const vector<Cluster*>& clusters, const vector<mutex*>& mutexs) :m_cbeg(cbeg), m_cend(cend), m_clusters(clusters), m_mutexs(mutexs) { }

	Status Run() {
		int index;
		for (record_const_iterator cit = m_cbeg; cit != m_cend; ++cit) {
			index = _findClosestCluster(*cit);
			m_mutexs[index]->lock();
			m_clusters[index]->Add(*cit);
			m_mutexs[index]->unlock();
		}
		return OK;
	}

private:

	int _findClosestCluster(strMyRecord *pRecord) const {
		int minindex = 0;
		double dist;
		double mindist = m_clusters[minindex]->CalcDistance(pRecord);
		for (int i = 1; i < m_clusters.size(); ++i) {
			if ((dist = m_clusters[i]->CalcDistance(pRecord)) < mindist) {
				mindist = dist;
				minindex = i;
			}
		}
		return minindex;
	}

private:
	vector<Cluster*> m_clusters;
	vector<mutex*> m_mutexs;
	record_const_iterator m_cbeg;
	record_const_iterator m_cend;
};

// ���¼������ĵ�����
class CalcNewCenterTask : public Task
{
public:
	CalcNewCenterTask(Cluster* cluster):m_cluster(cluster){
		m_isChanged = false;
	}

	static bool GetIsChanged() { return m_isChanged; }

	Status Run() {
		if (m_cluster->UpdateCenter())
			m_isChanged = true;
		return OK;
	}

private:
	static bool m_isChanged;
	Cluster* m_cluster;
};

bool CalcNewCenterTask::m_isChanged = false;

///////////////////////////////////////////////

CKMeans::CKMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList, KOptions options) :pSelfClusterNode(pSelf), pClusterTree(pTree), m_KmeansID(KmeansID), m_ClusterLevel(Level), m_RecordsList(pDataList), m_iNumClusters(0), m_options(options)
{
	KMeans::log << "********** Creat a new Kmeans, ID = " << m_KmeansID << " **********" << endl;
}

CKMeans::CKMeans(KOptions options)
	: m_KmeansID(0),
	m_ClusterLevel(1), m_iNumClusters(0),
	m_options(options)
{
	pClusterTree = new ClusterTree();
	pSelfClusterNode = pClusterTree->GetRootNode();

	KMeans::log << "********** Creat a new Kmeans, ID = " << m_KmeansID << " **********" << endl;
}

CKMeans::~CKMeans() {
	if (pSelfClusterNode->pParentNode == NULL)
		for (record_iterator it = m_RecordsList.begin(); it != m_RecordsList.end(); ++it)
			delete *it;
}

bool CKMeans::ReadTrainingRecords() {
	ifstream in(TRAINING_FILE);
	if (!in)
		return false;

	KMeans::log << "Start reading training records from " << TRAINING_FILE << " ..." << endl;

	if (m_options.Unique) {
		map<string, int> m;
		string line;
		int count = 0;
		while (in >> line) {
			m[line]++;

			if (++count % 10000 == 0) {
				KMeans::log << "----------- " << count << "  lines have read ----------" << endl;
			}
		}

		strMyRecord *record;
		stringstream s;
		for (map<string, int>::const_iterator cit = m.begin(); cit != m.end(); ++cit) {
			record = new strMyRecord;
			s.clear();
			s.str(cit->first);
			s >> *record;
			m_RecordsList.push_back(record);
		}
	}
	else {
		strMyRecord *record;
		do {
			record = new strMyRecord;
			in >> *record;
			if (in)
				m_RecordsList.push_back(record);
			else
				delete record;

			if (m_RecordsList.size() % 10000 == 0) {
				KMeans::log << "----------- " << m_RecordsList.size() << "  lines have read ----------" << endl;
			}
		} while (in);
	}
	KMeans::log << m_RecordsList.size() << "  lines have read" << endl;
	in.close();

	return true;
}

bool CKMeans::isSameAsCluster(int i, strMyRecord *pRecord) {
	for (int j = 0; j < i && j < m_iNumClusters; ++j) {
		if (m_Cluster[j].GetMainLabel() == pRecord->GetLabel() || m_Cluster[j].Equal(pRecord))
			return true;
	}
	return false;
}

bool RecordComp(strMyRecord *r1, strMyRecord* r2) {
	return r1->GetID() < r2->GetID();
}

void CKMeans::InitClusters(unsigned int NumClusters) {
	strMyRecord *pRecord;						// ������¼��ָ��

	if (m_options.Consistency)
		sort(m_RecordsList.begin(), m_RecordsList.end(), RecordComp);

	record_iterator RecdListIter = m_RecordsList.begin();

	//��ʼ�� m_iNumClusters ��������ĵ�
	m_iNumClusters = NumClusters;				// ��Ԥ��ľ�����k���и�ֵ
	for (int i = 0; i < m_iNumClusters; ++i) {
		pRecord = *RecdListIter;

		//�ڼ�¼�����в��ҳ�һ����֮ǰ�ľ����������ֲ�ͬ�ļ�¼��Ϊ����
		while (isSameAsCluster(i, pRecord)) {
			++RecdListIter;
			if (RecdListIter == m_RecordsList.end()) {
				m_iNumClusters = i + 1;
				KMeans::log << "can only find " << m_iNumClusters << " kind of different label" << endl;
				return;
			}

			pRecord = *RecdListIter;
		}

		// ���ҵ��ļ�¼��Ϊһ���µľ������ı�������
		// ����¼�������ĳ�Ա����
		// m_Cluster[i].MemberList.push_back(pRecord);
		// ����ǰ��¼����ֵ������i����������
		// EvaluateCluster(i, pRecord);
		m_Cluster[i].Init(pRecord);
	}
}

int CKMeans::FindClosestCluster(strMyRecord *pRecord) {
	int minindex = 0;
	double dist;
	double mindist = m_Cluster[minindex].CalcDistance(pRecord);
	for (int i = 1; i < m_iNumClusters; ++i) {
		if ((dist = m_Cluster[i].CalcDistance(pRecord)) < mindist) {
			mindist = dist;
			minindex = i;
		}
	}
	return minindex;
}


void CKMeans::DistributeSamples() {
	int index;

	// ������о���ӵ�еļ�¼�����¼������
	for (int i = 0; i < m_iNumClusters; ++i)
		m_Cluster[i].Clear();

	if (THREAD_NUM <= 0) {
		KMeans::log << "THREAD_NUM = " << THREAD_NUM << " must be greater than 0" << endl;
		throw(exception("THREAD_NUM error"));
	}
	else if (THREAD_NUM == 1) {
		for (record_const_iterator cit = m_RecordsList.begin(); cit != m_RecordsList.end(); ++cit) {
			index = FindClosestCluster(*cit);
			m_Cluster[index].Add(*cit);
		}
	}
	else {
		// �����̳߳ز����ò����߳���Ϊ THREAD_NUM
		ThreadPool *pool = ThreadPoolFactory::GetInstance()->GetThreadPool();
		pool->SetThreadsNum(THREAD_NUM);


		// �����м�¼ƽ���ֳ� THREAD_NUM �ݣ����� THREAD_NUM ���̲߳������㲢���������ľ���
		record_const_iterator cits[THREAD_NUM];
		for (int i = 0; i < THREAD_NUM; ++i)
			cits[i] = m_RecordsList.begin();

		int count;
		while (1) {
			count = 0;
			while (count < THREAD_NUM && cits[THREAD_NUM - 1] != m_RecordsList.end())
				++cits[THREAD_NUM - 1], ++count;

			if (count != THREAD_NUM)
				break;

			for (int i = 0; i < THREAD_NUM - 1; ++i) {
				for (count = 0; count <= i; ++count) {
					++cits[i];
				}
			}
		}

		// ����������
		vector<Cluster*> clusters(m_iNumClusters);
		vector<mutex> mutexs(m_iNumClusters);
		vector<mutex*> pmutexs(m_iNumClusters);
		for (int i = 0; i < m_iNumClusters; ++i) {
			clusters[i] = &m_Cluster[i];
			pmutexs[i] = &mutexs[i];
		}

		// ��������ӵ��̳߳�
		pool->AddTask(new DistributeTask(m_RecordsList.cbegin(), cits[0], clusters, pmutexs));
		for (int i = 0; i < THREAD_NUM - 1; ++i) {
			pool->AddTask(new DistributeTask(cits[i], cits[i + 1], clusters, pmutexs));
		}

		// ��ʼ��������
		Status status = pool->StartTasks();
		if (status != OK) {
			KMeans::log << "Start to Distribute Sample Task Error!" << endl;
			return;
		}

		// �ȴ��������
		status = pool->WaitForThreads();
		if (status != OK) {
			KMeans::log << "Wait for Finishing Distribute Sample Task Error!" << endl;
			return;
		}
	}
}

bool CKMeans::CalcNewClustCenters() {
	strMyRecord *pRecord;


	bool isChanged = false;
	if (THREAD_NUM <= 0) {
		KMeans::log << "THREAD_NUM = " << THREAD_NUM << " must be greater than 0" << endl;
		throw(exception("THREAD_NUM error"));
	}
	else if (THREAD_NUM == 1) {
		for (int i = 0; i < m_iNumClusters; ++i)
			if (m_Cluster[i].UpdateCenter())
				isChanged = true;
	}
	else {
		ThreadPool *pool = ThreadPoolFactory::GetInstance()->GetThreadPool();
		pool->SetThreadsNum(THREAD_NUM);

		for (int i = 0; i < m_iNumClusters; ++i)
			pool->AddTask(new CalcNewCenterTask(&m_Cluster[i]));

		// ��ʼ��������
		Status status = pool->StartTasks();
		if (status != OK) {
			KMeans::log << "Start to Distribute Sample Task Error!" << endl;
			return true;
		}

		// �ȴ��������
		status = pool->WaitForThreads();
		if (status != OK) {
			KMeans::log << "Wait for Finishing Distribute Sample Task Error!" << endl;
			return true;
		}

		isChanged = CalcNewCenterTask::GetIsChanged();
	}

	// �������ĳ�����ж���Ϊ�յ�����������ѡ��һ���أ���ѡ����ô�������Զ�ұ�ǩ��ͬ�Ķ��������ÿմ�
	for (int i = 0; i < m_iNumClusters; ++i) {
		if (m_Cluster[i].Empty()) {
			isChanged = true;

			int index;
			do {
				index = getRandomNumByLength(0, m_iNumClusters);
			} while (m_Cluster[index].Empty());

			strMyRecord *tempRecord;
			double maxDist = 0, tempDist;
			for (record_const_iterator cit = m_Cluster[index].GetRecordList().begin(); cit != m_Cluster[index].GetRecordList().end(); ++cit) {
				pRecord = *cit;
				tempDist = m_Cluster[index].CalcDistance(pRecord);
				if (tempDist > maxDist && pRecord->GetLabel() != m_Cluster[index].GetMainLabel()) {
					maxDist = tempDist;
					tempRecord = pRecord;
				}
			}

			m_Cluster[i].Init(pRecord);
		}
	}

	return isChanged;
}

bool CKMeans::IsClusterOK(int i) {
	int count = m_Cluster[i].GetIncorrectNum();
	double precition = m_Cluster[i].GetClusterPrecition();

	if (m_ClusterLevel <= INTERLEVEL) {
		if (count > 100) {
			KMeans::log << "Cluster " << i << " IncorrectNum = " << count << " > 100" << endl;
			return false;
		}
		else {
			if (precition >= CLUSTER_PRECITION)
				KMeans::log << "Cluster " << i << " Precition = " << precition << " >= " << CLUSTER_PRECITION << endl;
			return precition < CLUSTER_PRECITION;
		}
	}
	else {
		if (count > 500) {
			KMeans::log << "Cluster " << i << " IncorrectNum = " << count << " > 500" << endl;
			return false;
		}
		else {
			if (precition >= (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION)
				KMeans::log << "Cluster " << i << " Precition = " << precition << " >= " << (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION << endl;
			return precition < (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION;
		}
	}
}

int ext_global_kmeansID;
ClusterTree* CKMeans::RunKMeans(int Kvalue) {
	KMeans::log << "Init K-value = " << Kvalue << endl;

	// ��ʼ�����ξ��������
	InitClusters(Kvalue);

	// ѭ����������Ĺ��̣���ÿ����¼���������ľ������ģ��������µľ�������
	// ֱ��ÿ���������Ķ�û�з����仯����ֹͣ����
	int count = 0;
	while (1) {
		DistributeSamples();

		KMeans::log << "Cirle " << setw(2) << ++count << " | ";
		for (int i = 0; i < m_iNumClusters; ++i) {
			KMeans::log << "Cluster " << i << ": " << setw(6) << m_Cluster[i].GetRecordList().size() << " | ";
		}
		KMeans::log << endl;

		if (!CalcNewClustCenters())
			break;
	}

	// ��ӡ���ξ���Ľ��
	KMeans::log << "************** KMeans ID:" << m_KmeansID << " Level:" << m_ClusterLevel << " **************" << endl;
	for (int i = 0; i < m_iNumClusters; ++i) {
		KMeans::log << "Cluster " << i << ": " << m_Cluster[i] << endl;
	}
	KMeans::log << "******************************************" << endl;

	CreatClusterTreeNode(pSelfClusterNode);

	if (pSelfClusterNode->pParentNode == NULL)
		ext_global_kmeansID = m_KmeansID;

	for (int i = 0; i < m_iNumClusters; ++i) {
		if (IsClusterOK(i))
			pClusterNode[i]->IsLeaf = 1;
		else if (IsStopRecursion(i))
			pClusterNode[i]->IsLeaf = 2;
		else {
			pClusterNode[i]->IsLeaf = 0;
			CKMeans(pClusterNode[i], pClusterTree, ++ext_global_kmeansID, m_ClusterLevel + 1, m_Cluster[i].GetRecordList(), m_options).RunKMeans(GetDiffLabelofCluster(i));
		}
	}

	return pClusterTree;
}

int CKMeans::GetDiffLabelofCluster(int i) {
	return m_Cluster[i].GetLabelNum();
}

std::string intToString(int i) {
	std::stringstream s;
	s << i;
	return s.str();
}

void CKMeans::CreatClusterTreeNode(ClusterNode *pParent) {
	for (int i = 0; i < m_iNumClusters; ++i) {
		ClusterNode *pNode = new ClusterNode();

		pNode->strPath = pParent->strPath + '.' + intToString(i);
		m_Cluster[i].CalcInfo();
		pNode->m_cluster = m_Cluster[i];
		pNode->m_cluster.GetRecordList().clear();
		pNode->IsClusterOK = IsClusterOK(i);
		pNode->IsLeaf = (pNode->IsClusterOK ? 1 : IsStopRecursion(i) ? 2 : 0);

		pClusterNode[i] = pNode;
		pClusterTree->InsertNode(pParent, pNode, i);
	}
}
