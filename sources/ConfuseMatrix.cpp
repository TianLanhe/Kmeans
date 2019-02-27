#include "include/ConfuseMatrix.h"

#include <ostream>
#include <iomanip>

using namespace std;

//构造函数
ConfuseMatrix::ConfuseMatrix(int row, int col) :m_total(0), m_correct(0), m_matrix(row, vector<int>(col, 0)) { }

//数值更新函数
void ConfuseMatrix::UpdateValue(int iTrueLabel, int iPreLabel)
{
	m_matrix[iTrueLabel][iPreLabel]++;
	++m_total;
	if (iTrueLabel == iPreLabel)
		++m_correct;
};

//打印混淆矩阵
ostream& operator<<(ostream& out, const ConfuseMatrix& matrix) {
	int i, j;
	if (out) {
		out << "================================== Classify Result ====================================" << endl;
		out << "Total Test Records = " << matrix.m_total << "  Right_Label Records = " << matrix.m_correct << " Right Rate = " << (matrix.m_total == 0 ? 0 : matrix.m_correct * 1.0 / matrix.m_total) << endl;

		out << "=================================  Confusion Matrix  ==================================" << endl;
		//打印第一行
		out << setiosflags(ios::left) << setw(5) << "T/P";
		for (i = 0; i < matrix.m_matrix[0].size(); i++)
			out << setiosflags(ios::left) << setw(7) << i;
		out << endl;

		//打印矩阵内容
		for (i = 0; i < matrix.m_matrix.size(); i++) {
			out << setiosflags(ios::left) << setw(5) << i;
			for (j = 0; j < matrix.m_matrix[i].size(); j++)
				out << setiosflags(ios::left) << setw(7) << matrix.m_matrix[i][j];
			out << endl;
		}
	}
	return out;
};
