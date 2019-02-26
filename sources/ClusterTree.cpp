#include "ClusterTree.h"
#include "Log.h"

#include <cmath>

using namespace std;

ClusterNode::~ClusterNode() {
	if (IsLeaf == 0)
		for (int i = 0; i < pChildNode.size(); ++i)
			if (pChildNode[i] != NULL)
				delete pChildNode[i];
}

double ClusterNode::CalCenterDistance(strMyRecord* pRecord) const {
	return pRecord ? m_cluster.CalcDistance(pRecord) : 0;
}

const ClusterNode* ClusterNode::GetNearestCluster(strMyRecord *pRecord) const {
	int i;
	double MinDistance, TmpDistance;		// 最短距离，临时距离
	const ClusterNode* pNearestNode;		// 距离最近节点的指针

	pNearestNode = this;

	// 判断是否为叶子节点，如果不是，则递归求子类中的最近节点
	if (IsLeaf == 0) {
		// 计算本节点的聚类中心与记录的距离
		// MinDistance = CalCenterDistance(pRecord);
		MinDistance = -1;

		// 判断是否有孩子节点，如果有，调用递归函数，获得孩子节点所在的子树中
		// 离数据距离最近的节点指针
		for (i = 0; i < pChildNode.size(); ++i) {
			if (pChildNode[i] != NULL) {
				const ClusterNode* pTmpNode = pChildNode[i]->GetNearestCluster(pRecord);

				// 计算最近子节点与数据的距离
				TmpDistance = pTmpNode->CalCenterDistance(pRecord);

				if (fabs(MinDistance + 1) < 1e-5 || TmpDistance < MinDistance) {
					pNearestNode = pTmpNode;
					MinDistance = TmpDistance;
				}
			}
		}
		if (fabs(MinDistance + 1) < 1e-5) {
			throw(string("IsLeaf == 0 but has no child"));
		}
	}
	return pNearestNode;
}

void ClusterTree::_insertNode(ClusterNode *root, ClusterNode *parent, ClusterNode *node) {
	if (parent == root) {
		int size = root->pChildNode.size();

		root->pChildNode.push_back(node);
		root->IsLeaf = 0;

		node->pParentNode = root;
		node->strPath = root->strPath + '.' + to_string(size);
	}
	else {
		for (int i = 0; i < root->pChildNode.size(); ++i)
			if (root->pChildNode[i])
				_insertNode(root->pChildNode[i], parent, node);
	}
}

void ClusterTree::InsertNode(ClusterNode *pParent, ClusterNode *pNode) {
	if (pParent && pNode)
		_insertNode(pRootNode, pParent, pNode);
}

ostream& operator<<(ostream& out, const ClusterNode& node) {
	if (out) {
		out << "Cluster " << node.strPath << ": " << node.m_cluster << endl;
		out << "IsClusterOK: " << (node.IsClusterOK ? "true" : "false") << " IsLeaf:" << node.IsLeaf << " Center: " << node.m_cluster.GetCenterStr() << endl;

		for (int i = 0; i < node.pChildNode.size(); i++)
			if (node.pChildNode[i] != NULL)
				out << *node.pChildNode[i];
	}
	return out;
}

ostream& operator<<(ostream& out, const ClusterTree& tree) {
	if (out) {
		// 根节点不存储聚类信息
		for (int i = 0; i < tree.pRootNode->GetChildCount(); ++i)
			if (tree.pRootNode->GetChildNode(i))
				out << *tree.pRootNode->GetChildNode(i);
	}
	return out;
}
