#ifndef CLUSTER_TREE_H
#define CLUSTER_TREE_H

#include "Common.h"
#include "cluster.h"
#include <string>

class ClusterNode
{
public:
	//构造函数
	ClusterNode() :pParentNode(NULL), IsClusterOK(false), IsLeaf(0) { for (int i = 0; i < MAXCLUSTER; ++i) pChildNode[i] = NULL; }
	~ClusterNode();

	//计算该条记录到该节点中心的距离
	double CalCenterDistance(strMyRecord* pRecord);

	//获得孩子i的指针
	ClusterNode* GetChildNode(int i) { return pChildNode[i]; }

	//获得本节点的聚类标签
	int GetClusterNodeLabel() { return m_cluster.GetMainLabel(); }

	//递归函数，在以该节点为父亲的子树中，获得与数据记录距离最近的聚类节点
	ClusterNode* GetNearestCluster(strMyRecord* pRecord);

	//将聚类树输出到日志文件中
	void PrintLog();

public:
	Cluster m_cluster;				//聚类信息
	bool IsClusterOK;				//聚类结果是否符合标准

	std::string strPath;			//聚类节点的路径

	ClusterNode *pParentNode;		//指向这个聚类节点的父节点的指针
	ClusterNode *pChildNode[MAXCLUSTER];	//指向这个聚类节点的孩子节点的指针

	int IsLeaf;						//节点类型，是否为叶子节点，聚类是否正常终止
									//0：非叶子节点
									//1：叶子节点且聚类正常结束
									//2：叶子节点且聚类非正常结束
};

class ClusterTree
{
public:
	//构造函数
	ClusterTree() {
		pRootNode = new ClusterNode();
		pRootNode->pParentNode = NULL;
		pRootNode->strPath = "0";
	}
	~ClusterTree() { delete pRootNode; }

	//插入节点
	void InsertNode(ClusterNode *pParent, ClusterNode *pNode, int i);

	//找到与给定记录距离最近的聚类节点
	ClusterNode* FindNearestCluster(strMyRecord *pRecord){ return  pRootNode->GetNearestCluster(pRecord); }

	//获得根节点
	ClusterNode* GetRootNode() { return pRootNode; }

	//将聚类树输出到日志文件中
	void PrintLog();

private:
	ClusterNode* pRootNode;	//根节点指针
};

#endif // !CLUSTER_TREE_H
