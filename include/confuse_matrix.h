#ifndef CONFUSE_MATRIX_H
#define CONFUSE_MATRIX_H

#include <vector>

class ConfuseMatrix
{
public:
	friend std::ostream& operator<<(std::ostream&, const ConfuseMatrix&);

public:
	ConfuseMatrix(int row, int col);

	// ��ֵ���º���
	void UpdateValue(int iTrueLabel, int iPreLabel);

	// ��ȡ������
	int GetTotalNum() const { return m_total; }

	// ��ȡԤ����ȷ������
	int GetCorrectNum() const { return m_correct; }

private:
	std::vector<std::vector<int> > m_matrix;			// ��������
	int m_total;							// ������
	int m_correct;							// Ԥ����ȷ������
};

std::ostream& operator<<(std::ostream&, const ConfuseMatrix&);

#endif // !CONFUSE_MATRIX_H
