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
	//构造函数
	ClusterNode() :pParentNode(NULL), IsClusterOK(false), IsLeaf(0) { }
	ClusterNode(const Cluster& c, bool isOk, int isLeaf) :m_cluster(c), IsClusterOK(isOk), IsLeaf(isLeaf) { m_cluster.GetRecordList().clear(); }
	~ClusterNode();

	//计算该条记录到该节点中心的距离
	double CalCenterDistance(strMyRecord* pRecord) const;

	//获得孩子i的指针
	int GetChildCount() const { return pChildNode.size(); }
	ClusterNode* GetChildNode(int i) const { return pChildNode[i]; }

	//获得父亲节点的指针
	ClusterNode* GetParentNode() const { return pParentNode; }

	//获得本节点的聚类标签
	int GetClusterNodeLabel() const { return m_cluster.GetMainLabel(); }

	//递归函数，在以该节点为父亲的子树中，获得与数据记录距离最近的聚类节点
	const ClusterNode* GetNearestCluster(strMyRecord* pRecord) const ;

private:
	Cluster m_cluster;				//聚类信息
	bool IsClusterOK;				//聚类结果是否符合标准

	std::string strPath;			//聚类节点的路径

	ClusterNode *pParentNode;		//指向这个聚类节点的父节点的指针
	std::vector<ClusterNode*> pChildNode;

	int IsLeaf;						//节点类型，是否为叶子节点，聚类是否正常终止
									//0：非叶子节点
									//1：叶子节点且聚类正常结束
									//2：叶子节点且聚类非正常结束
};

class ClusterTree
{
public:
	friend std::ostream& operator<<(std::ostream&, const ClusterTree&);

public:
	//构造函数
	ClusterTree() {
		pRootNode = new ClusterNode();
		pRootNode->pParentNode = NULL;
		pRootNode->strPath = "0";
	}
	~ClusterTree() { delete pRootNode; }

	//插入节点
	void InsertNode(ClusterNode *pParent, ClusterNode *pNode);

	//找到与给定记录距离最近的聚类节点
	const ClusterNode* FindNearestCluster(strMyRecord *pRecord) const { return  pRootNode->GetNearestCluster(pRecord); }

	//获得根节点
	ClusterNode* GetRootNode() const { return pRootNode; }

private:
	void _insertNode(ClusterNode *root, ClusterNode *parent, ClusterNode *node);

private:
	ClusterNode* pRootNode;	//根节点指针
};

std::ostream& operator<<(std::ostream&, const ClusterNode&);
std::ostream& operator<<(std::ostream&, const ClusterTree&);

#endif // !CLUSTER_TREE_H
