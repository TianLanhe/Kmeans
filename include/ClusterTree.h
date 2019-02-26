#ifndef CLUSTER_TREE_H
#define CLUSTER_TREE_H

#include "Cluster.h"

#include <string>
#include <vector>

class ClusterNode
{
public:
	friend std::ostream& operator<<(std::ostream&, const ClusterNode&);
	friend class ClusterTree;

public:
	//���캯��
	ClusterNode() :pParentNode(NULL), IsClusterOK(false), IsLeaf(0) { }
	ClusterNode(const Cluster& c, bool isOk, int isLeaf) :m_cluster(c), IsClusterOK(isOk), IsLeaf(isLeaf) { m_cluster.GetRecordList().clear(); }
	~ClusterNode();

	//���������¼���ýڵ����ĵľ���
	double CalCenterDistance(strMyRecord* pRecord) const;

	//��ú���i��ָ��
	int GetChildCount() const { return pChildNode.size(); }
	ClusterNode* GetChildNode(int i) const { return pChildNode[i]; }

	//��ø��׽ڵ��ָ��
	ClusterNode* GetParentNode() const { return pParentNode; }

	//��ñ��ڵ�ľ����ǩ
	int GetClusterNodeLabel() const { return m_cluster.GetMainLabel(); }

	//�ݹ麯�������Ըýڵ�Ϊ���׵������У���������ݼ�¼��������ľ���ڵ�
	const ClusterNode* GetNearestCluster(strMyRecord* pRecord) const ;

private:
	Cluster m_cluster;				//������Ϣ
	bool IsClusterOK;				//�������Ƿ���ϱ�׼

	std::string strPath;			//����ڵ��·��

	ClusterNode *pParentNode;		//ָ���������ڵ�ĸ��ڵ��ָ��
	std::vector<ClusterNode*> pChildNode;

	int IsLeaf;						//�ڵ����ͣ��Ƿ�ΪҶ�ӽڵ㣬�����Ƿ�������ֹ
									//0����Ҷ�ӽڵ�
									//1��Ҷ�ӽڵ��Ҿ�����������
									//2��Ҷ�ӽڵ��Ҿ������������
};

class ClusterTree
{
public:
	friend std::ostream& operator<<(std::ostream&, const ClusterTree&);

public:
	//���캯��
	ClusterTree() {
		pRootNode = new ClusterNode();
		pRootNode->pParentNode = NULL;
		pRootNode->strPath = "0";
	}
	~ClusterTree() { delete pRootNode; }

	//����ڵ�
	void InsertNode(ClusterNode *pParent, ClusterNode *pNode);

	//�ҵ��������¼��������ľ���ڵ�
	const ClusterNode* FindNearestCluster(strMyRecord *pRecord) const { return  pRootNode->GetNearestCluster(pRecord); }

	//��ø��ڵ�
	ClusterNode* GetRootNode() const { return pRootNode; }

private:
	void _insertNode(ClusterNode *root, ClusterNode *parent, ClusterNode *node);

private:
	ClusterNode* pRootNode;	//���ڵ�ָ��
};

std::ostream& operator<<(std::ostream&, const ClusterNode&);
std::ostream& operator<<(std::ostream&, const ClusterTree&);

#endif // !CLUSTER_TREE_H
