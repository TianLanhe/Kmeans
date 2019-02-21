#include "Kmeans.h"
#include "Common.h"
#include "ClusterTree.h"
#include "Log.h"

#include <fstream>
#include <cmath>
#include <map>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;

int initRandSeed() {
	srand(time(NULL));
	return 0;
}

int nrand(int n) {
	static int r = initRandSeed();

	if (n > RAND_MAX)
		n = RAND_MAX;

	int bucket_size = RAND_MAX / n;
	do {
		r = rand() / bucket_size;
	} while (r >= n);

	return r;
}

int getRandomNumByLength(int start, int length) {
	if (RAND_MAX - start < length)
		length = RAND_MAX - start;

	return nrand(length) + start;
}

int getRandomNumByRange(int start, int end) {
	if (end > RAND_MAX) {
		end = RAND_MAX;
	}

	return nrand(end - start) + start;
}


CKMeans::CKMeans(ClusterTree *pTree)
	:pSelfClusterNode(pTree->GetRootNode()), pClusterTree(pTree), m_KmeansID(0),
	m_ClusterLevel(1), m_iNumClusters(0) {
	KMeans::log << "********** Creat a new Kmeans, ID = " << m_KmeansID << " **********" << endl;
}

CKMeans::CKMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList) :pSelfClusterNode(pSelf), pClusterTree(pTree), m_KmeansID(KmeansID), m_ClusterLevel(Level), m_RecordsList(pDataList), m_iNumClusters(0){
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

	map<string, int> m;
	string line;
	int count = 0;
	while (in >> line) {
		m[line]++;
	
		if (++count % 10000 == 0) {
			KMeans::log << "----------- " << count << "  lines have read ----------" << endl;
		}
	}
	in.close();

	KMeans::log << count << " records have read from training file" << endl;

	strMyRecord *record;
	stringstream s;
	for (map<string, int>::const_iterator cit = m.begin(); cit != m.end(); ++cit) {
		record = new strMyRecord;
		s.clear();
		s.str(cit->first);
		s >> *record;
		m_RecordsList.push_back(record);
	}

	/*strMyRecord *record;
	while (in) {
		record = new strMyRecord;
		in >> *record;
		m_RecordsList.push_back(record);

		if (m_RecordsList.size() % 10000 == 0) {
			KMeans::log << "----------- " << m_RecordsList.size() << "  lines have read ----------" << endl;
		}
	}
	in.close();*/

	return true;
}

bool CKMeans::isSameAsCluster(int i, strMyRecord *pRecord) {
	for (int j = 0; j < i && j < m_iNumClusters; ++j) {
		if (m_Cluster[j].GetMainLabel() == pRecord->GetLabel() || m_Cluster[j].Equal(pRecord))
			return true;
	}
	return false;
}

void CKMeans::InitClusters(unsigned int NumClusters) {
	strMyRecord *pRecord;						// 遍历记录的指针
	record_iterator RecdListIter = m_RecordsList.begin();

	//初始化 m_iNumClusters 个类的中心点
	m_iNumClusters = NumClusters;				// 对预测的聚类树k进行赋值
	for (int i = 0; i < m_iNumClusters; ++i) {
		pRecord = *RecdListIter;

		//在记录链表中查找出一个与之前的聚类中心数字不同的记录作为中心
		while (isSameAsCluster(i, pRecord)) {
			++RecdListIter;
			pRecord = *RecdListIter;
		}

		// 将找到的记录作为一个新的聚类中心保存起来
		// 将记录插入该类的成员链表
		// m_Cluster[i].MemberList.push_back(pRecord);
		// 将当前记录的数值赋给第i个聚类中心
		// EvaluateCluster(i, pRecord);
		m_Cluster[i].Init(pRecord);
	}
}

double CKMeans::CalcEucNorm(strMyRecord *pRecord, int id) {
	return m_Cluster[id].CalcDistance(pRecord);
}

int CKMeans::FindClosestCluster(strMyRecord *pRecord) {
	int minindex = 0;
	double dist;
	double mindist = CalcEucNorm(pRecord, minindex);
	for (int i = 1; i < m_iNumClusters; ++i) {
		if ((dist = CalcEucNorm(pRecord, i)) < mindist) {
			mindist = dist;
			minindex = i;
		}
	}
	return minindex;
}

void CKMeans::DistributeSamples() {
	int index;
	for (int i = 0; i < m_iNumClusters; ++i)
		m_Cluster[i].Clear();
	for (record_const_iterator cit = m_RecordsList.begin(); cit != m_RecordsList.end(); ++cit) {
		index = FindClosestCluster(*cit);
		m_Cluster[index].Add(*cit);
	}
}

bool CKMeans::CalcNewClustCenters() {
	strMyRecord *pRecord;
	bool isNotChanged = true;
	for (int i = 0; i < m_iNumClusters; ++i) {
		if (m_Cluster[i].UpdateCenter())
			isNotChanged = false;
	}

	// 如果出现某个簇中对象为空的情况，则随机选择一个簇，挑选距离该簇中心最远且标签不同的对象分配给该空簇
	for (int i = 0; i < m_iNumClusters; ++i) {
		if (m_Cluster[i].Empty()) {
			isNotChanged = false;

			int index;
			do {
				index = getRandomNumByLength(0, m_iNumClusters);
			} while (m_Cluster[index].Empty());

			strMyRecord *tempRecord;
			double maxDist = 0, tempDist;
			for (record_const_iterator cit = m_Cluster[index].GetRecordList().begin(); cit != m_Cluster[index].GetRecordList().end(); ++cit) {
				pRecord = *cit;
				tempDist = CalcEucNorm(pRecord, index);
				if (tempDist > maxDist && pRecord->GetLabel() != m_Cluster[index].GetMainLabel()) {
					maxDist = tempDist;
					tempRecord = pRecord;
				}
			}

			m_Cluster[i].Init(pRecord);
		}
	}

	return isNotChanged;
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
			if(precition >= CLUSTER_PRECITION)
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
			if (precition < (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION)
				KMeans::log << "Cluster " << i << " Precition = " << precition << " >= " << (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION << endl;
			return precition < (m_ClusterLevel - INTERLEVEL) * CLUSTER_PRECITION;
		}
	}
}

int ext_global_kmeansID;
void CKMeans::RunKMeans(int Kvalue) {
	KMeans::log << "Init K-value = " << Kvalue << endl;

	// 初始化本次聚类的中心
	InitClusters(Kvalue);

	// 循环进行下面的过程：将每条记录分配给最近的聚类中心，并计算新的聚类中心
	// 直至每个聚类中心都没有发生变化，则停止迭代
	int count = 0;
	while (1) {
		DistributeSamples();

		KMeans::log << "Cirle " << setw(2) << ++count <<  " | ";
		for (int i = 0; i < m_iNumClusters; ++i) {
			KMeans::log << "Cluster " << i << ": " << setw(6) << m_Cluster[i].GetRecordList().size() << " | ";
		}
		KMeans::log << endl;

		if (CalcNewClustCenters())
			break;
	}

	// 打印本次聚类的结果，成员数，以及组成情况
	KMeans::log << "************** KMeans ID:" << m_KmeansID << " Level:"  << m_ClusterLevel << " **************" << endl;
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
			CKMeans(pClusterNode[i], pClusterTree, ++ext_global_kmeansID, m_ClusterLevel + 1, m_Cluster[i].GetRecordList()).RunKMeans(GetDiffLabelofCluster(i));
		}
	}
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
