#ifndef CONFUSE_MATRIX_H
#define CONFUSE_MATRIX_H

#include "Common.h"

class ConfuseMatrix
{
public:
	ConfuseMatrix();

	// ��ֵ���º���
	void UpdateValue(int iTrueLabel, int iPreLabel);

	// ��ӡ��������
	void PrintMatrixToLog() const;

	// ��ȡ������
	int GetTotalNum() const { return m_total; }

	// ��ȡԤ����ȷ������
	int GetCorrectNum() const { return m_correct; }

private:
	int m_matrix[MAX_LABEL][MAX_LABEL];	//��������
	int m_total;							// ������
	int m_correct;							// Ԥ����ȷ������
};


#endif // !CONFUSE_MATRIX_H
