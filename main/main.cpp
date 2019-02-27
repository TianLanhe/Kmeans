#include "ClusterTree.h"
#include "Kmeans.h"
#include "ConfuseMatrix.h"
#include "Log.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <ctime>

// ѵ�����ļ�
#define TRAINING_FILE "data/kddcup.data_10_percent_corrected_datatreat"
// ���Լ��ļ�
#define TESTING_FILE "data/corrected_datatreat"
// ����ļ�
#define RESULT_FILE "Result.txt"

using namespace std;

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

	// ���� CKMeans ����
	KOptions options;
	//options.Consistency = false;
	options.ThreadNum = 8;
	options.Print = false;
	options.LogFile = "";
	CKMeans m_CKMeans(options);

	// ��ȡѵ�����ݼ��е�����
	vector<strMyRecord> trainingRecords(ReadTrainingFile());

	// ��¼��ʼʱ��
	int start = time(NULL);

	// ��ʼ�ݹ������̣����ɾ�����
	ClusterTree *clusterTree = m_CKMeans.RunKMeans(trainingRecords.begin(),trainingRecords.end());
	int train_time = time(NULL) - start;

	// ����������ӡ����־�ļ���
	cout << *clusterTree;

	Log out;
	out.add(&cout);
	out.add(new ofstream(RESULT_FILE));

	cout << "Start classifying testing records ... " << endl;
	out << "********************** Classification Result **************************" << endl;

	// ��ȡ�������ݼ��е�����
	vector<strMyRecord> testList(ReadTestFile());


	// Ϊ��������Ѱ������ľ���ڵ㣬�����ýڵ�������Ϊ������¼��Ԥ�����
	// �����ݼ�¼��Ԥ����������ȷ���Ƚϣ�ͳ��ģ��Ԥ���׼ȷ��
	ConfuseMatrix matrix(options.KValue, options.KValue);
	start = time(NULL);
	for (vector<strMyRecord>::size_type i = 0; i < testList.size(); ++i) {
		const ClusterNode* pNode = clusterTree->FindNearestCluster(&testList[i]);

		matrix.UpdateValue(testList[i].GetLabel() - 1, pNode->GetClusterNodeLabel() - 1);

		if ((i + 1) % 10000 == 0) {
			cout << "----------- " << i + 1 << "  records have been done ----------" << endl;
		}
		//out << "True Label = " << getLabelName(testList[i].GetLabel()) << "   Pre Label = " << getLabelName(pNode->GetClusterNodeLabel()) << "   Cluster Path = " << pNode->strPath << endl;
	}
	cout << testList.size() << " records have been classified" << endl;

	int classfy_time = time(NULL) - start;
	cout << "Training time: " << train_time << " s" << endl;
	cout << "Classifing cost: " << classfy_time << " s" << endl;

	out << matrix;
	out << "Cluster_Percition = " << m_CKMeans.GetOptions().ClusterPrecision << " Dimension = " << options.Dimension << " Consistency = " << options.Consistency << " Thread_Number = " << options.ThreadNum << endl;
	out << "FieldName: ";
	for (int i = 0; i < options.Dimension; ++i) {
		out << testList[0].GetFieldName(i) << ' ';
	}
	out << endl;

	delete clusterTree;
	return 0;
}