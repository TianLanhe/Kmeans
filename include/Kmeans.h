#ifndef KMEANS_H
#define KMEANS_H

#include "Common.h"
#include "cluster.h"
#include "Log.h"

#include <vector>

class ClusterTree;
class ClusterNode;

// �㷨����ѡ��
struct KOptions
{
	KOptions() :Dimension(18), Unique(false), Print(true), Consistency(true), ClusterPrecision(0.1),
		MaxLevel(10), ThreadNum(4), PrecisionIncreaseLevel(3), KValue(5),LogFile("Log.txt") {}

	int Dimension;		// ����ά����Ĭ�� 18
	bool Unique;		// �Ƿ�ȥ�أ�Ĭ�� false
	bool Print;			// �Ƿ��ӡ��־��Ĭ�� true
	bool Consistency;	// ����Ƿ���Ҫǿһ��(���̲߳���������Կ��ܵ���ѵ������Ĳ�ȷ����)��Ĭ�� true
	double ClusterPrecision;	// ���ྫ����ֵ��Ĭ�� 0.1
	int MaxLevel;				// ��������������Ĭ�� 10
	int ThreadNum;				// �����߳�������Ĭ�� 4
	int PrecisionIncreaseLevel;	// ���ྫ�ȵ����Ĳ�����ֵ��Ĭ�� 3
	int KValue;			// Kֵ��Ҫ�ֳɼ������࣬Ĭ�� 5
	std::string LogFile;		// ��־�ļ���Ĭ��Ϊ "Log.txt"����Ϊ�գ���д���ļ�
};

class CKMeans : public Object
{
public:
	//���캯��1
	CKMeans(KOptions options = KOptions());

	~CKMeans();

	//��ȡ��������Ԥ����ļ�¼
	bool ReadTrainingRecords();

	//K-means�㷨���������
	ClusterTree* RunKMeans();

	// ��ȡ�㷨�Ĳ�������
	KOptions GetOptions() const { return m_options; }

private:
	CKMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList, KOptions options,KMeans::Log* l);

	//�жϸ�����¼��֮ǰ�ľ��������Ƿ���ȫ��ͬ
	bool isSameAsCluster(strMyRecord *pRecord) const;

	//K-means�㷨�ĵ�һ������n�����ݶ�������ѡ��k��������Ϊ��ʼ��������
	void InitClusters(unsigned int NumClusters);

	//K-means�㷨�ĵڶ�������ÿ������������֮��������ľ���
	void DistributeSamples();

	//K-means�㷨�ĵ����������¼���ÿ�����������
	bool CalcNewClustCenters();

	//�ҵ���������ݶ��������һ������
	int FindClosestCluster(strMyRecord *pRecord) const;

	//�������һ���еķ����Ƿ����
	bool IsClusterOK(int i) const;

	//��ñ�����������i������ͬ��Label����
	int GetDiffLabelofCluster(int i) const;

	//�ж��Ƿ�����ݹ����
	bool IsStopRecursion(int i) const { return m_ClusterLevel >= m_options.MaxLevel; }

	//�����������ڵ�
	void CreatClusterTreeNode();

	// �������غ���
	int initRandSeed();
	int nrand(int n);
	// ��ȡ [start,start+lenth) ֮����������
	int getRandomNumByLength(int start, int length);
	// ��ȡ [start,end) ֮����������
	int getRandomNumByRange(int start, int end);

private:
	record_list m_RecordsList;	//���ݼ�¼����
	unsigned int m_iNumClusters;	//����������(��Kֵ)
	std::vector<Cluster> m_Cluster;	//��������
	int m_ClusterLevel;	//������������Ĳ��
	int m_KmeansID;		//CKMeans�����ID��
	ClusterTree *pClusterTree;	//��������ָ��
	ClusterNode *pSelfClusterNode;	//�뱾KMeans������صľ���ڵ��ָ��

	KOptions m_options;

	KMeans::Log *m_log;
};

#endif // !KMEANS_H
