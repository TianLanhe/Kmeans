#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>

class Record {
public:
	Record();
	Record(const Record&);
	Record(Record&&);

public:
	// ��ȡ�ֶθ���
	virtual int GetFieldNum() const = 0;
	// ��ȡ�� i ���ֶε�ֵ
	virtual int GetFieldValue(int index) const = 0;
	// ��ȡ�� i ���ֶε�����
	virtual std::string GetFieldName(int index) const = 0;
	// ��ȡ������¼�ķ����ǩ
	virtual int GetLabel() const = 0;

	// ���õ� i ���ֶε�ֵ
	virtual void SetFieldValue(int index, int value) = 0;
	// ����������¼�ķ����ǩ
	virtual void SetLabel(int value) = 0;

	unsigned int GetID() const { return m_id; }

protected:
	unsigned int m_id; // ÿ����¼����һ��������ID
};

// �������Ļ���
class Object {
public:
	typedef std::vector<Record*> record_list;
	typedef record_list::iterator record_iterator;
	typedef record_list::const_iterator record_const_iterator;

	virtual ~Object() {}
};

#endif // !STR_MY_RECORD_H


