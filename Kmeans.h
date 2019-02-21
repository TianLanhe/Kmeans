#ifndef KMEANS_H
#define KMEANS_H

#include "Common.h"
#include "cluster.h"

class ClusterTree;
class ClusterNode;

class CKMeans : public Object
{
public:
	//���캯��1
	CKMeans(ClusterTree *pTree);
	//���캯��2
	CKMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList);
	~CKMeans();

	//��ȡ��������Ԥ����ļ�¼
	bool ReadTrainingRecords();

	//K-means�㷨�ĵ�һ������n�����ݶ�������ѡ��k��������Ϊ��ʼ��������
	void InitClusters(unsigned int NumClusters);

	//K-means�㷨�ĵڶ�������ÿ������������֮��������ľ���
	void DistributeSamples();

	//K-means�㷨�ĵ����������¼���ÿ�����������
	bool CalcNewClustCenters();

	//����ָ�����ݶ�����������ĵ�ŷ����þ���
	double CalcEucNorm(strMyRecord *pRecord, int id);

	//�ҵ���������ݶ��������һ������
	int FindClosestCluster(strMyRecord *pRecord);

	//��ӡ��������
	//void PrintClusters();

	//K-means�㷨���������
	void RunKMeans(int Kvalue);

	//�������һ���еķ����Ƿ����
	bool IsClusterOK(int i);

	//��ӡ��CKMeans����������i��label
	//void PrintClusterLabel(int i);

	//��ñ�����������i������ͬ��Label����
	int GetDiffLabelofCluster(int i);

	//�ж��Ƿ�����ݹ����
	bool IsStopRecursion(int i) { return m_ClusterLevel >= MAX_ITERATION; }

	//�����������ڵ�
	void CreatClusterTreeNode(ClusterNode *pParent);

private:
	//�жϸ�����¼��֮ǰ�ľ��������Ƿ���ȫ��ͬ
	bool isSameAsCluster(int i, strMyRecord *pRecord);

private:
	record_list m_RecordsList;	//���ݼ�¼����
	unsigned int m_iNumClusters;	//����������(��Kֵ)
	Cluster m_Cluster[MAXCLUSTER];	//��������
	int m_ClusterLevel;	//������������Ĳ��
	int m_KmeansID;		//CKMeans�����ID��
	ClusterTree *pClusterTree;	//��������ָ��
	ClusterNode *pClusterNode[MAXCLUSTER];	//�������ӽڵ��ָ��
	ClusterNode *pSelfClusterNode;	//�뱾KMeans������صľ���ڵ��ָ��
};

#endif // !KMEANS_H
