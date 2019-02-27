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
	// 获取字段个数
	virtual int GetFieldNum() const = 0;
	// 获取第 i 个字段的值
	virtual int GetFieldValue(int index) const = 0;
	// 获取第 i 个字段的名称
	virtual std::string GetFieldName(int index) const = 0;
	// 获取这条记录的分类标签
	virtual int GetLabel() const = 0;

	// 设置第 i 个字段的值
	virtual void SetFieldValue(int index, int value) = 0;
	// 设置这条记录的分类标签
	virtual void SetLabel(int value) = 0;

	unsigned int GetID() const { return m_id; }

protected:
	unsigned int m_id; // 每条记录都有一个独立的ID
};

// 聚类对象的基类
class Object {
public:
	typedef std::vector<Record*> record_list;
	typedef record_list::iterator record_iterator;
	typedef record_list::const_iterator record_const_iterator;

	virtual ~Object() {}
};

#endif // !STR_MY_RECORD_H


