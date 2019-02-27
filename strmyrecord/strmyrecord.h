#ifndef STR_MY_RECORD_H
#define STR_MY_RECORD_H

#include "include/Record.h"

#include <string>
#include <vector>

typedef unsigned char BYTE;

class strMyRecord : public Record {
public:
	friend std::istream& operator >> (std::istream &, strMyRecord &);
	friend std::ostream& operator<<(std::ostream &, const strMyRecord &);

public:
	// ��ȡ�ֶθ���
	int GetFieldNum() const;
	// ��ȡ�� i ���ֶε�ֵ
	int GetFieldValue(int index) const;
	// ��ȡ�� i ���ֶε�����
	std::string GetFieldName(int index) const;
	// ��ȡ������¼�ķ����ǩ
	int GetLabel() const;

	// ���õ� i ���ֶε�ֵ
	void SetFieldValue(int index, int value);
	// ����������¼�ķ����ǩ
	void SetLabel(int value);

private:
	int _stringToInt(const std::string&);

private:
	BYTE iProtocolType;//02 Э������ 3��
	BYTE iService;		//03 �������� 66��
	BYTE iStatusFlag;	//04 ״̬��־
	int iSrcBytes;		//05 Դ��Ŀ���ֽ���
	int iDestBytes;		//06 Ŀ�ĵ�Դ�ֽ���
	int iFailedLogins;	//11 ��¼ʧ�ܴ���
	int iNumofRoot;		//16 root�û�Ȩ�޴�ȡ����
	int iCount;			//23 2s��������ͬ������Ŀ
	int iSvrCount;		//24 2s��������ͬ�˿���Ŀ
	BYTE iRerrorRate;	//27 "REJ"���������������
	BYTE iSameSrvRate;	//29 ���ӵ���ͬ�˿�������
	BYTE iDiffSrvRate;	//30 ���ӵ���ͬ�˿�������
	int iDstHostSrvCount;	//33 ��ͬĿ�ĵ���ͬ�˿�������
	BYTE iDstHostSameSrvRate;	//34 ��ͬĿ�ĵ���ͬ�˿�����������
	BYTE iDstHostDiffSrvRate;	//35 ��ͬĿ�ĵز�ͬ�˿�����������
	BYTE iDstHostSameSrcPortRate;	//36 ��ͬĿ�ĵ���ͬԴ�˿�����������
	BYTE iDstHostSrvDiffHostRate;	//37 ��ͬ����������ͬ�˿ڱ���
	BYTE iDstHostSrvSerrorRate;	//39 ���ӵ�ǰ������S0����ı���
	BYTE iLabel;	//42 ���ͱ�ǩ
};

std::istream& operator >> (std::istream &, strMyRecord &);
std::ostream& operator<<(std::ostream &, const strMyRecord &);

// �����ļ���ȡ�����ַ��� ProtolType ת��Ϊ����ֵ
BYTE getProtolType(const std::string& str);
// �����ļ���ȡ�����ַ��� Service ת��Ϊ����ֵ
BYTE getService(const std::string& str);
// �����ļ���ȡ�����ַ��� StatusFlag ת��Ϊ����ֵ
BYTE getStatusFlag(const std::string& str);
// �����ļ���ȡ�����ַ�����ǩת��Ϊ����ֵ
BYTE getLabel(const std::string& str);
// ������ֵ��ǩת��Ϊ�ַ���
std::string getLabelName(BYTE label);

#endif // !STR_MY_RECORD_H


