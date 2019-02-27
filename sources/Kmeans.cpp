#include "Kmeans.h"
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


////////////// 随机数相关函数 ///////////////
int CKMeans::initRandSeed() {
	srand(time(NULL));
	return 0;
}

int CKMeans::nrand(int n) {
	// 如果要求强一直，则不初始随机数种子
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


////////////// 多线程并发子任务 ///////////////

// 分发记录到聚类的任务
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

// 重新计算质心的任务
class CalcNewCenterTask : public Task
{
public:
	CalcNewCenterTask(Cluster* cluster) :m_cluster(cluster) {
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

CKMeans::CKMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList, KOptions options, Log* l) :pSelfClusterNode(pSelf), pClusterTree(pTree), m_KmeansID(KmeansID), m_ClusterLevel(Level), m_RecordsList(pDataList), m_options(options), m_log(l)
{
	(*m_log) << "********** Creat a new Kmeans, ID = " << m_KmeansID << " **********" << endl;
}

CKMeans::CKMeans(KOptions options)
	: m_KmeansID(0),
	m_ClusterLevel(1),
	m_options(options)
{
	pClusterTree = new ClusterTree();
	pSelfClusterNode = pClusterTree->GetRootNode();

	m_log = new Log();
	if (m_options.Print)
		m_log->add(&cout);
	if (!m_options.LogFile.empty())
		m_log->add(new ofstream(m_options.LogFile.c_str()));

	(*m_log) << "********** Creat a new Kmeans, ID = " << m_KmeansID << " **********" << endl;
}

CKMeans::~CKMeans() {
	// 最顶上的 KMeans 对象负责销毁 Logger
	if (pSelfClusterNode->GetParentNode() == NULL)
		delete m_log;
}

/*bool CKMeans::ReadTrainingRecords() {
	ifstream in(TRAINING_FILE);
	if (!in)
		return false;

	(*m_log) << "Start reading training records from " << TRAINING_FILE << " ..." << endl;

	if (m_options.Unique) {
		map<string, int> m;
		string line;
		int count = 0;
		while (in >> line) {
			m[line]++;

			if (++count % 10000 == 0) {
				(*m_log) << "----------- " << count << "  lines have read ----------" << endl;
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
			if (in) {
				m_RecordsList.push_back(record);

				if (m_RecordsList.size() % 10000 == 0) {
					(*m_log) << "----------- " << m_RecordsList.size() << "  lines have read ----------" << endl;
				}
			}
			else
				delete record;
		} while (in);
	}
	(*m_log) << m_RecordsList.size() << "  lines have read" << endl;
	in.close();

	return true;
}*/

bool CKMeans::isSameAsCluster(strMyRecord *pRecord) const {
	for (int j = 0; j < m_Cluster.size(); ++j) {
		if (m_Cluster[j].GetMainLabel() == pRecord->GetLabel() || m_Cluster[j].Equal(pRecord))
			return true;
	}
	return false;
}

bool RecordComp(strMyRecord *r1, strMyRecord* r2) {
	return r1->GetID() < r2->GetID();
}

void CKMeans::InitClusters(unsigned int NumClusters) {
	strMyRecord *pRecord;						// 遍历记录的指针

	if (m_options.Consistency)
		sort(m_RecordsList.begin(), m_RecordsList.end(), RecordComp);

	//初始化 m_iNumClusters 个类的中心点
	for (record_const_iterator cit = m_RecordsList.begin(); cit != m_RecordsList.end(); ++cit) {
		if (m_Cluster.size() == NumClusters)
			break;

		pRecord = *cit;
		if (!isSameAsCluster(pRecord)) {
			// 将找到的记录作为一个新的聚类中心保存起来
			Cluster c(m_options.Dimension);
			c.Init(pRecord);
			m_Cluster.push_back(c);
		}
	}

	if (m_Cluster.size() != NumClusters) {
		(*m_log) << "can only find " << m_Cluster.size() << " kind of different label" << endl;
	}
}

int CKMeans::FindClosestCluster(strMyRecord *pRecord) const {
	int minindex = 0;
	double dist;
	double mindist = m_Cluster[minindex].CalcDistance(pRecord);
	for (int i = 1; i < m_Cluster.size(); ++i) {
		if ((dist = m_Cluster[i].CalcDistance(pRecord)) < mindist) {
			mindist = dist;
			minindex = i;
		}
	}
	return minindex;
}


void CKMeans::DistributeSamples() {
	int index;

	// 清空所有聚类拥有的记录，重新计算分配
	for (int i = 0; i < m_Cluster.size(); ++i)
		m_Cluster[i].Clear();

	if (m_options.ThreadNum <= 0) {
		(*m_log) << "THREAD_NUM = " << m_options.ThreadNum << " must be greater than 0" << endl;
		throw(string("THREAD_NUM error"));
	}
	else if (m_options.ThreadNum == 1) {	// 单线程
		for (record_const_iterator cit = m_RecordsList.begin(); cit != m_RecordsList.end(); ++cit) {
			index = FindClosestCluster(*cit);
			m_Cluster[index].Add(*cit);
		}
	}
	else {	// 多线程
		// 创建线程池并设置并发线程数为 THREAD_NUM
		ThreadPool *pool = ThreadPoolFactory::GetInstance()->GetThreadPool();
		pool->SetThreadsNum(m_options.ThreadNum);

		// 将所有记录平均分成 THREAD_NUM 份，交给 THREAD_NUM 条线程并发计算并分配给最近的聚类
		vector<record_const_iterator> cits(m_options.ThreadNum);
		for (int i = 0; i < m_options.ThreadNum; ++i)
			cits[i] = m_RecordsList.begin();

		int count;
		while (1) {
			count = 0;
			while (count < m_options.ThreadNum && cits[m_options.ThreadNum - 1] != m_RecordsList.end())
				++cits[m_options.ThreadNum - 1], ++count;

			if (count != m_options.ThreadNum)
				break;

			for (int i = 0; i < m_options.ThreadNum - 1; ++i) {
				for (count = 0; count <= i; ++count) {
					++cits[i];
				}
			}
		}

		// 创建互斥量
		vector<Cluster*> clusters(m_Cluster.size());
		vector<mutex> mutexs(m_Cluster.size());
		vector<mutex*> pmutexs(m_Cluster.size());
		for (int i = 0; i < m_Cluster.size(); ++i) {
			clusters[i] = &m_Cluster[i];
			pmutexs[i] = &mutexs[i];
		}

		// 把任务添加到线程池
		pool->AddTask(new DistributeTask(m_RecordsList.cbegin(), cits[0], clusters, pmutexs));
		for (int i = 0; i < m_options.ThreadNum - 1; ++i) {
			pool->AddTask(new DistributeTask(cits[i], cits[i + 1], clusters, pmutexs));
		}

		// 开始并发计算
		Status status = pool->StartTasks();
		if (status != OK) {
			(*m_log) << "Start to Distribute Sample Task Error!" << endl;
			return;
		}

		// 等待计算完毕
		status = pool->WaitForThreads();
		if (status != OK) {
			(*m_log) << "Wait for Finishing Distribute Sample Task Error!" << endl;
			return;
		}
	}
}

bool CKMeans::CalcNewClustCenters() {
	strMyRecord *pRecord;

	bool isChanged = false;
	if (m_options.ThreadNum <= 0) {
		(*m_log) << "THREAD_NUM = " << m_options.ThreadNum << " must be greater than 0" << endl;
		throw(string("THREAD_NUM error"));
	}
	else if (m_options.ThreadNum == 1) {	// 单线程
		for (int i = 0; i < m_Cluster.size(); ++i)
			if (m_Cluster[i].UpdateCenter())
				isChanged = true;
	}
	else {
		ThreadPool *pool = ThreadPoolFactory::GetInstance()->GetThreadPool();
		pool->SetThreadsNum(m_options.ThreadNum);

		for (int i = 0; i < m_Cluster.size(); ++i)
			pool->AddTask(new CalcNewCenterTask(&m_Cluster[i]));

		// 开始并发计算
		Status status = pool->StartTasks();
		if (status != OK) {
			(*m_log) << "Start to Calculate New Cluster Center Task Error!" << endl;
			return true;
		}

		// 等待计算完毕
		status = pool->WaitForThreads();
		if (status != OK) {
			(*m_log) << "Wait for Finishing Calculate New Cluster Center Task Error!" << endl;
			return true;
		}

		isChanged = CalcNewCenterTask::GetIsChanged();
	}

	// 如果出现某个簇中对象为空的情况，则随机选择一个簇，挑选距离该簇中心最远且标签不同的对象分配给该空簇
	for (int i = 0; i < m_Cluster.size(); ++i) {
		if (m_Cluster[i].Empty()) {
			isChanged = true;

			int index;
			do {
				index = getRandomNumByLength(0, m_Cluster.size());
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

bool CKMeans::IsClusterOK(int i)  const {
	int count = m_Cluster[i].GetIncorrectNum();
	double precition = m_Cluster[i].GetClusterPrecition();

	if (m_ClusterLevel <= m_options.PrecisionIncreaseLevel) {
		if (count > 100) {
			(*m_log) << "Cluster " << i << " IncorrectNum = " << count << " > 100" << endl;
			return false;
		}
		else {
			if (precition >= m_options.ClusterPrecision)
				(*m_log) << "Cluster " << i << " Precition = " << precition << " >= " << m_options.ClusterPrecision << endl;
			return precition < m_options.ClusterPrecision;
		}
	}
	else {
		if (count > 500) {
			(*m_log) << "Cluster " << i << " IncorrectNum = " << count << " > 500" << endl;
			return false;
		}
		else {
			if (precition >= (m_ClusterLevel - m_options.PrecisionIncreaseLevel) * m_options.ClusterPrecision)
				(*m_log) << "Cluster " << i << " Precition = " << precition << " >= " << (m_ClusterLevel - m_options.PrecisionIncreaseLevel) * m_options.ClusterPrecision << endl;
			return precition < (m_ClusterLevel - m_options.PrecisionIncreaseLevel) * m_options.ClusterPrecision;
		}
	}
}

int ext_global_kmeansID;
ClusterTree* CKMeans::_runKMeans() {
	(*m_log) << "Init K-value = " << m_options.KValue << endl;

	// 初始化本次聚类的中心
	InitClusters(m_options.KValue);

	// 循环进行下面的过程：将每条记录分配给最近的聚类中心，并计算新的聚类中心
	// 直至每个聚类中心都没有发生变化，则停止迭代
	int count = 0;
	while (1) {
		// 重新分配记录到最近的聚类
		DistributeSamples();

		(*m_log) << "Cirle " << setw(2) << ++count << " | ";
		for (int i = 0; i < m_Cluster.size(); ++i) {
			(*m_log) << "Cluster " << i << ": " << setw(6) << m_Cluster[i].GetRecordList().size() << " | ";
		}
		(*m_log) << endl;

		// 重新计算聚类中心，若所有聚类中心都没有发生变化，停止迭代
		if (!CalcNewClustCenters())
			break;
	}

	// 打印本次聚类的结果
	(*m_log) << "************** KMeans ID:" << m_KmeansID << " Level:" << m_ClusterLevel << " **************" << endl;
	for (int i = 0; i < m_Cluster.size(); ++i) {
		(*m_log) << "Cluster " << i << ": " << m_Cluster[i] << endl;
	}
	(*m_log) << "******************************************" << endl;

	// 根据聚类中心结果扩展聚类树
	CreatClusterTreeNode();

	if (pSelfClusterNode->GetParentNode() == NULL)
		ext_global_kmeansID = m_KmeansID;

	for (int i = 0; i < m_Cluster.size(); ++i) {
		if (!IsClusterOK(i) && !IsStopRecursion(i)) {
			KOptions options = m_options;
			options.KValue = GetDiffLabelofCluster(i);
			CKMeans(pSelfClusterNode->GetChildNode(i), pClusterTree, ++ext_global_kmeansID, m_ClusterLevel + 1, m_Cluster[i].GetRecordList(), options, m_log)._runKMeans();
		}
	}

	return pClusterTree;
}

int CKMeans::GetDiffLabelofCluster(int i) const {
	return m_Cluster[i].GetLabelNum();
}

void CKMeans::CreatClusterTreeNode() {
	for (int i = 0; i < m_Cluster.size(); ++i) {
		m_Cluster[i].CalcInfo();

		bool isOk = IsClusterOK(i);
		int isLeaf = (isOk ? 1 : IsStopRecursion(i) ? 2 : 0);
		ClusterNode *pNode = new ClusterNode(m_Cluster[i], isOk, isLeaf);

		pClusterTree->InsertNode(pSelfClusterNode, pNode);
	}
}
