#include "confuse_matrix.h"
#include <iostream>
#include <iomanip>
#include "Log.h"

using namespace std;

//构造函数
ConfuseMatrix::ConfuseMatrix():m_total(0),m_correct(0)
{
	for (int i = 0; i < MAX_LABEL; i++)
		for (int j = 0; j < MAX_LABEL; j++)
			m_matrix[i][j] = 0;
};

//数值更新函数
void ConfuseMatrix::UpdateValue(int iTrueLabel, int iPreLabel)
{
	m_matrix[iTrueLabel][iPreLabel]++;
	++m_total;
	if (iTrueLabel == iPreLabel)
		++m_correct;
};

//打印混淆矩阵
void ConfuseMatrix::PrintMatrixToLog() const
{
	int i, j;

	KMeans::out << "================================== Classify Result ====================================" << endl;
	KMeans::out << "Total Test Records = " << m_total << "  Right_Label Records = " << m_correct << " Right Rate = " << (m_total == 0 ? 0 : m_correct * 1.0 / m_total) << endl;
	
	KMeans::out << "=================================  Confusion Matrix  ==================================" << endl;
	//打印第一行
	KMeans::out << setiosflags(ios::left) << setw(5) << "T/P";
	for (i = 0; i < MAX_LABEL; i++)
		KMeans::out << setiosflags(ios::left) << setw(7) << i;
	KMeans::out << endl;
	
	//打印矩阵内容
	for (i = 0; i < MAX_LABEL; i++){
		KMeans::out << setiosflags(ios::left) << setw(5) << i;
		for (j = 0; j < MAX_LABEL; j++)
			KMeans::out << setiosflags(ios::left) << setw(7) << m_matrix[i][j];
		KMeans::out << endl;
	}
};
