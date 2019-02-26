#ifndef CONFUSE_MATRIX_H
#define CONFUSE_MATRIX_H

#include <vector>

class ConfuseMatrix
{
public:
	friend std::ostream& operator<<(std::ostream&, const ConfuseMatrix&);

public:
	ConfuseMatrix(int row, int col);

	// 数值更新函数
	void UpdateValue(int iTrueLabel, int iPreLabel);

	// 获取总数量
	int GetTotalNum() const { return m_total; }

	// 获取预测正确的数量
	int GetCorrectNum() const { return m_correct; }

private:
	std::vector<std::vector<int> > m_matrix;			// 混淆矩阵
	int m_total;							// 总数量
	int m_correct;							// 预测正确的数量
};

std::ostream& operator<<(std::ostream&, const ConfuseMatrix&);

#endif // !CONFUSE_MATRIX_H
