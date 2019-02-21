#ifndef CLUSTER_TREE_H
#define CLUSTER_TREE_H

#include "Common.h"
#include "cluster.h"
#include <string>

class ClusterNode
{
public:
	//���캯��
	ClusterNode() :pParentNode(NULL), IsClusterOK(false), IsLeaf(0) { for (int i = 0; i < MAXCLUSTER; ++i) pChildNode[i] = NULL; }
	~ClusterNode();

	//���������¼���ýڵ����ĵľ���
	double CalCenterDistance(strMyRecord* pRecord);

	//��ú���i��ָ��
	ClusterNode* GetChildNode(int i) { return pChildNode[i]; }

	//��ñ��ڵ�ľ����ǩ
	int GetClusterNodeLabel() { return m_cluster.GetMainLabel(); }

	//�ݹ麯�������Ըýڵ�Ϊ���׵������У���������ݼ�¼��������ľ���ڵ�
	ClusterNode* GetNearestCluster(strMyRecord* pRecord);

	//���������������־�ļ���
	void PrintLog();

public:
	Cluster m_cluster;				//������Ϣ
	bool IsClusterOK;				//�������Ƿ���ϱ�׼

	std::string strPath;			//����ڵ��·��

	ClusterNode *pParentNode;		//ָ���������ڵ�ĸ��ڵ��ָ��
	ClusterNode *pChildNode[MAXCLUSTER];	//ָ���������ڵ�ĺ��ӽڵ��ָ��

	int IsLeaf;						//�ڵ����ͣ��Ƿ�ΪҶ�ӽڵ㣬�����Ƿ�������ֹ
									//0����Ҷ�ӽڵ�
									//1��Ҷ�ӽڵ��Ҿ�����������
									//2��Ҷ�ӽڵ��Ҿ������������
};

class ClusterTree
{
public:
	//���캯��
	ClusterTree() {
		pRootNode = new ClusterNode();
		pRootNode->pParentNode = NULL;
		pRootNode->strPath = "0";
	}
	~ClusterTree() { delete pRootNode; }

	//����ڵ�
	void InsertNode(ClusterNode *pParent, ClusterNode *pNode, int i);

	//�ҵ��������¼��������ľ���ڵ�
	ClusterNode* FindNearestCluster(strMyRecord *pRecord){ return  pRootNode->GetNearestCluster(pRecord); }

	//��ø��ڵ�
	ClusterNode* GetRootNode() { return pRootNode; }

	//���������������־�ļ���
	void PrintLog();

private:
	ClusterNode* pRootNode;	//���ڵ�ָ��
};

#endif // !CLUSTER_TREE_H
