#include "ClusterTree.h"
#include "Kmeans.h"
#include "confuse_matrix.h"
#include "Log.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdio>

using namespace std;

vector<strMyRecord> ReadTestFile() {
	vector<strMyRecord> ret;

	KMeans::log << "Start reading testing records from " << TESTING_FILE << " ..." << endl;

	ifstream in(TESTING_FILE);
	if (in) {
		strMyRecord record;
		while (in >> record) {
			ret.push_back(record);
		
			if (ret.size() % 10000 == 0) {
				KMeans::log << "----------- " << ret.size() << "  lines have read ----------" << endl;
			}
		
		}
		in.close();

		KMeans::log << ret.size() << " records have read from training file" << endl;
	}
	return ret;
}

int main() {
	KMeans::log << "Start K-means clustering proccess ..." << endl;

	// 创建聚类树对象
	ClusterTree clusterTree;

	// 创建 CKMeans 对象
	CKMeans m_CKMeans(&clusterTree);

	// 读取训练数据集中的数据
	if (!m_CKMeans.ReadTrainingRecords()) {
		KMeans::log << "Read trainging records \"" << TRAINING_FILE << "\" error" << endl;
		return 1;
	}

	// 开始递归聚类过程，生成聚类树
	m_CKMeans.RunKMeans(MAXCLUSTER);

	// 将聚类结果打印到日志文件中
	clusterTree.PrintLog();

	// 读取测试数据集中的数据
	vector<strMyRecord> testList(ReadTestFile());

	KMeans::log << "Start classifying testing records ... " << endl;
	KMeans::out << "********************** Classification Result **************************" << endl;

	// 为测试数据寻找最近的聚类节点，并将该节点的类别作为这条记录的预测类别
	// 将数据记录的预测类型与正确类别比较，统计模型预测的准确率
	ConfuseMatrix matrix;
	for (vector<strMyRecord>::size_type i = 0; i < testList.size(); ++i) {
		ClusterNode* pNode = clusterTree.FindNearestCluster(&testList[i]);

		matrix.UpdateValue(testList[i].GetLabel() - 1, pNode->GetClusterNodeLabel() - 1);

		if ((i+1) % 10000 == 0) {
			KMeans::log << "----------- " << i + 1 << "  records have been done ----------" << endl;
		}

		//KMeans::out << "True Label = " << getLabelName(testList[i].GetLabel()) << "   Pre Label = " << getLabelName(pNode->GetClusterNodeLabel()) << "   Cluster Path = " << pNode->strPath << endl;
	}
	KMeans::log << testList.size() << " records have been classified" << endl;

	matrix.PrintMatrixToLog();

	return 0;
}