#ifndef CONFUSE_MATRIX_H
#define CONFUSE_MATRIX_H

#include "Common.h"

class ConfuseMatrix
{
public:
	ConfuseMatrix();

	// 数值更新函数
	void UpdateValue(int iTrueLabel, int iPreLabel);

	// 打印混淆矩阵
	void PrintMatrixToLog() const;

	// 获取总数量
	int GetTotalNum() const { return m_total; }

	// 获取预测正确的数量
	int GetCorrectNum() const { return m_correct; }

private:
	int m_matrix[MAX_LABEL][MAX_LABEL];	//混淆矩阵
	int m_total;							// 总数量
	int m_correct;							// 预测正确的数量
};


#endif // !CONFUSE_MATRIX_H
