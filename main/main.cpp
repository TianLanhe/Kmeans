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

int main() {
	cout << "Start K-means clustering proccess ..." << endl;

	// ���� CKMeans ����
	KOptions options;
	//options.Unique = true;
	//options.Consistency = false;
	options.ThreadNum = 1;
	//options.Print = false;
	options.LogFile = "";
	CKMeans m_CKMeans(options);

	// ��ȡѵ�����ݼ��е�����
	if (!m_CKMeans.ReadTrainingRecords()) {
		cout << "Read training records \"" << TRAINING_FILE << "\" error" << endl; 
		return 1;
	}

	// ��¼��ʼʱ��
	int start = time(NULL);

	// ��ʼ�ݹ������̣����ɾ�����
	ClusterTree *clusterTree = m_CKMeans.RunKMeans();
	int train_time = time(NULL) - start;

	// ����������ӡ����־�ļ���
	cout << *clusterTree;

	cout << "Start classifying testing records ... " << endl;
	out << "********************** Classification Result **************************" << endl;

	// ��ȡ�������ݼ��е�����
	vector<strMyRecord> testList(ReadTestFile());


	// Ϊ��������Ѱ������ľ���ڵ㣬�����ýڵ�������Ϊ������¼��Ԥ�����
	// �����ݼ�¼��Ԥ����������ȷ���Ƚϣ�ͳ��ģ��Ԥ���׼ȷ��
	ConfuseMatrix matrix(options.KValue,options.KValue);
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
	out << "CLUSTER_PRECITION = " << m_CKMeans.GetOptions().ClusterPrecision << " DIMENSION = " << options.Dimension << " Unique = " << (options.Unique ? "true" : "false") << endl;
	out << "FieldName: ";
	for (int i = 0; i < options.Dimension; ++i) {
		out << testList[0].GetFieldName(i) << ' ';
	}
	out << endl;

	delete clusterTree;
	return 0;
}