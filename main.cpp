#include "ClusterTree.h"
#include "Kmeans.h"
#include "confuse_matrix.h"
#include "Log.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <ctime>

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

	// 记录开始时间
	int start = time(NULL);

	// 创建 CKMeans 对象
	KOptions options;
	//options.Unique = true;
	//options.Consistency = false;
	CKMeans m_CKMeans(options);

	// 读取训练数据集中的数据
	if (!m_CKMeans.ReadTrainingRecords()) {
		KMeans::log << "Read trainging records \"" << TRAINING_FILE << "\" error" << endl; 
		return 1;
	}

	// 开始递归聚类过程，生成聚类树
	ClusterTree *clusterTree = m_CKMeans.RunKMeans(MAXCLUSTER);
	cout << (time(NULL) - start) << 's' << endl;

	// 将聚类结果打印到日志文件中
	clusterTree->PrintLog();

	KMeans::log << "Start classifying testing records ... " << endl;
	KMeans::out << "********************** Classification Result **************************" << endl;

	// 读取测试数据集中的数据
	vector<strMyRecord> testList(ReadTestFile());

	// 为测试数据寻找最近的聚类节点，并将该节点的类别作为这条记录的预测类别
	// 将数据记录的预测类型与正确类别比较，统计模型预测的准确率
	ConfuseMatrix matrix;
	for (vector<strMyRecord>::size_type i = 0; i < testList.size(); ++i) {
		ClusterNode* pNode = clusterTree->FindNearestCluster(&testList[i]);

		matrix.UpdateValue(testList[i].GetLabel() - 1, pNode->GetClusterNodeLabel() - 1);

		if ((i + 1) % 10000 == 0) {
			KMeans::log << "----------- " << i + 1 << "  records have been done ----------" << endl;
		}
		//KMeans::out << "True Label = " << getLabelName(testList[i].GetLabel()) << "   Pre Label = " << getLabelName(pNode->GetClusterNodeLabel()) << "   Cluster Path = " << pNode->strPath << endl;
	}
	KMeans::log << testList.size() << " records have been classified" << endl;

	matrix.PrintMatrixToLog();
	KMeans::out << "CLUSTER_PRECITION = " << CLUSTER_PRECITION << " DIMENSION = " << DIMENSION << " Unique = " << (options.Unique ? "true" : "false") << endl;
	KMeans::out << "FieldName: ";
	for (int i = 0; i < DIMENSION; ++i) {
		KMeans::out << testList[0].GetFieldName(i) << ' ';
	}
	KMeans::out << endl;

	delete clusterTree;
	return 0;
}