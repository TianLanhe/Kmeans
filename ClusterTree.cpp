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
	double MinDistance, TmpDistance;		// ��̾��룬��ʱ����
	ClusterNode* pNearestNode;			// ��������ڵ��ָ��

	pNearestNode = this;

	// �ж��Ƿ�ΪҶ�ӽڵ㣬������ǣ���ݹ��������е�����ڵ�
	if (IsLeaf == 0) {
		// ���㱾�ڵ�ľ����������¼�ľ���
		// MinDistance = CalCenterDistance(pRecord);
		MinDistance = -1;

		// �ж��Ƿ��к��ӽڵ㣬����У����õݹ麯������ú��ӽڵ����ڵ�������
		// �����ݾ�������Ľڵ�ָ��
		for (i = 0; i < MAXCLUSTER; ++i) {
			if (pChildNode[i] != NULL) {
				ClusterNode* pTmpNode = pChildNode[i]->GetNearestCluster(pRecord);

				// ��������ӽڵ������ݵľ���
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
