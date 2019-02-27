#include "include/ClusterTree.h"
#include "include/Kmeans.h"
#include "include/ConfuseMatrix.h"

#include "strmyrecord/strmyrecord.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

// 训练集文件
#define TRAINING_FILE "data/kddcup.data_10_percent_corrected_datatreat"
// 测试集文件
#define TESTING_FILE "data/corrected_datatreat"

using namespace std;

// 读取测试数据
vector<strMyRecord> ReadTestFile();
// 读取训练数据
vector<strMyRecord> ReadTrainingFile();

vector<strMyRecord> ReadTestFile() {
	vector<strMyRecord> ret;

	cout << "Start reading testing records from " << TESTING_FILE << " ..." << endl;

	ifstream in(TESTING_FILE);
	if (in) {
		strMyRecord record;
		while (in >> record) {
			ret.push_back(record);

			if (ret.size() % 10000 == 0) {
				cout << "----------- " << ret.size() << "  lines have read ----------" << endl;
			}
		}
		in.close();

		cout << ret.size() << " records have read from testing file" << endl;
	}
	return ret;
}

vector<strMyRecord> ReadTrainingFile() {
	vector<strMyRecord> ret;

	ifstream in(TRAINING_FILE);
	if (!in)
		return ret;

	cout << "Start reading training records from " << TRAINING_FILE << " ..." << endl;

	strMyRecord record;
	while (in >> record) {
		ret.push_back(record);

		if (ret.size() % 10000 == 0) {
			cout << "----------- " << ret.size() << "  lines have read ----------" << endl;
		}
	}
	cout << ret.size() << "  lines have read" << endl;
	in.close();

	return ret;
}

int main() {
	cout << "Start K-means clustering proccess ..." << endl;

	// 创建 CKMeans 对象
	KOptions options;
	//options.Consistency = false;
	options.ThreadNum = 1;
	//options.Print = false;
	options.LogFile = "";
	KMeans m_CKMeans(options);

	// 读取训练数据集中的数据
	vector<strMyRecord> trainingRecords(ReadTrainingFile());

	// 记录开始时间
	int start = time(NULL);

	// 开始递归聚类过程，生成聚类树
	ClusterTree *clusterTree = m_CKMeans.RunKMeans(trainingRecords.begin(),trainingRecords.end());

	// 记录结束时间
	int train_time = time(NULL) - start;

	// 打印聚类结果
	cout << *clusterTree;

	cout << "Start classifying testing records ... " << endl;
	cout << "********************** Classification Result **************************" << endl;

	// 读取测试数据集中的数据
	vector<strMyRecord> testList(ReadTestFile());

	// 为测试数据寻找最近的聚类节点，并将该节点的类别作为这条记录的预测类别
	// 将数据记录的预测类型与正确类别比较，统计模型预测的准确率
	ConfuseMatrix matrix(options.KValue, options.KValue);
	start = time(NULL);
	for (vector<strMyRecord>::size_type i = 0; i < testList.size(); ++i) {
		const ClusterNode* pNode = clusterTree->FindNearestCluster(&testList[i]);

		matrix.UpdateValue(testList[i].GetLabel() - 1, pNode->GetClusterNodeLabel() - 1);

		if ((i + 1) % 10000 == 0) {
			cout << "----------- " << i + 1 << "  records have been done ----------" << endl;
		}
		//cout << "True Label = " << getLabelName(testList[i].GetLabel()) << "   Pre Label = " << getLabelName(pNode->GetClusterNodeLabel()) << "   Cluster Path = " << pNode->strPath << endl;
	}
	cout << testList.size() << " records have been classified" << endl;

	// 记录分类时间
	int classfy_time = time(NULL) - start;

	// 打印训练和分类时间
	cout << "Training cost: " << train_time << " s" << endl;
	cout << "Classifing cost: " << classfy_time << " s" << endl;

	// 打印分类结果
	cout << matrix;
	cout << "Cluster_Percition = " << m_CKMeans.GetOptions().ClusterPrecision << " Dimension = " << options.Dimension << " Consistency = " << options.Consistency << " Thread_Number = " << options.ThreadNum << endl;
	cout << "FieldName: ";
	for (int i = 0; i < options.Dimension; ++i) {
		cout << testList[0].GetFieldName(i) << ' ';
	}
	cout << endl;

	delete clusterTree;
	return 0;
}
