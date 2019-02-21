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

	// ��������������
	ClusterTree clusterTree;

	// ���� CKMeans ����
	CKMeans m_CKMeans(&clusterTree);

	// ��ȡѵ�����ݼ��е�����
	if (!m_CKMeans.ReadTrainingRecords()) {
		KMeans::log << "Read trainging records \"" << TRAINING_FILE << "\" error" << endl;
		return 1;
	}

	// ��ʼ�ݹ������̣����ɾ�����
	m_CKMeans.RunKMeans(MAXCLUSTER);

	// ����������ӡ����־�ļ���
	clusterTree.PrintLog();

	// ��ȡ�������ݼ��е�����
	vector<strMyRecord> testList(ReadTestFile());

	KMeans::log << "Start classifying testing records ... " << endl;
	KMeans::out << "********************** Classification Result **************************" << endl;

	// Ϊ��������Ѱ������ľ���ڵ㣬�����ýڵ�������Ϊ������¼��Ԥ�����
	// �����ݼ�¼��Ԥ����������ȷ���Ƚϣ�ͳ��ģ��Ԥ���׼ȷ��
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