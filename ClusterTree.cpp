#include "ClusterTree.h"

#include <iostream>
#include <cmath>
#include "Log.h"

using namespace std;

ClusterNode::~ClusterNode() {
	if (IsLeaf == 0) {
		for (int i = 0; i < MAXCLUSTER; ++i) {
			if (pChildNode[i] != NULL) {
				delete pChildNode[i];
			}
		}
	}
}

double ClusterNode::CalCenterDistance(strMyRecord* pRecord) {
	if (!pRecord)
		return 0;

	return m_cluster.CalcDistance(pRecord);
}

ClusterNode* ClusterNode::GetNearestCluster(strMyRecord *pRecord) {
	int i;
	double MinDistance, TmpDistance;		// 最短距离，临时距离
	ClusterNode* pNearestNode;			// 距离最近节点的指针

	pNearestNode = this;

	// 判断是否为叶子节点，如果不是，则递归求子类中的最近节点
	if (IsLeaf == 0) {
		// 计算本节点的聚类中心与记录的距离
		// MinDistance = CalCenterDistance(pRecord);
		MinDistance = -1;

		// 判断是否有孩子节点，如果有，调用递归函数，获得孩子节点所在的子树中
		// 离数据距离最近的节点指针
		for (i = 0; i < MAXCLUSTER; ++i) {
			if (pChildNode[i] != NULL) {
				ClusterNode* pTmpNode = pChildNode[i]->GetNearestCluster(pRecord);

				// 计算最近子节点与数据的距离
				TmpDistance = pTmpNode->CalCenterDistance(pRecord);

				if (fabs(MinDistance + 1) < 1e-5 || TmpDistance < MinDistance) {
					pNearestNode = pTmpNode;
					MinDistance = TmpDistance;
				}
			}
		}
		if (fabs(MinDistance + 1) < 1e-5) {
			cerr << "IsLeaf == 0 but has no child" << endl;
		}
	}
	return pNearestNode;
}

void ClusterNode::PrintLog() {
	KMeans::log << "Cluster " << strPath << ": " << m_cluster << endl;
	KMeans::log << "IsClusterOK: " << (IsClusterOK ? "true" : "false") << " IsLeaf:" << IsLeaf << " Center: " << m_cluster.GetCenterStr() << endl;

	for (int i = 0; i < MAX_LABEL; i++)
		if (pChildNode[i] != NULL)
			pChildNode[i]->PrintLog();
}

void insertNode(ClusterNode *root, ClusterNode *parent, ClusterNode *node, int index) {
	if (parent == root) {
		root->pChildNode[index] = node;
		root->IsLeaf = 0;
		node->pParentNode = root;
	}
	else {
		for (int i = 0; i < MAXCLUSTER; ++i)
			if (root->pChildNode[i])
				insertNode(root->pChildNode[i], parent, node, index);
	}
}

void ClusterTree::InsertNode(ClusterNode *pParent, ClusterNode *pNode, int index) {
	if (pParent && pNode && index >= 0)
		insertNode(pRootNode, pParent, pNode, index);
}

void ClusterTree::PrintLog() {
	for (int i = 0; i < MAXCLUSTER; ++i)
		if (pRootNode->pChildNode[i])
			pRootNode->pChildNode[i]->PrintLog();
}
