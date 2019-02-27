#ifndef STR_MY_RECORD_H
#define STR_MY_RECORD_H

#include <string>
#include <vector>

typedef unsigned char BYTE;

class strMyRecord {
public:
	friend std::istream& operator >> (std::istream &, strMyRecord &);
	friend std::ostream& operator<<(std::ostream &, const strMyRecord &);

	strMyRecord();
	strMyRecord(const strMyRecord&);
	strMyRecord(strMyRecord&&);

public:
	// 获取字段个数
	int GetFieldNum() const;
	// 获取第 i 个字段的值
	int GetFieldValue(int index) const;
	// 获取第 i 个字段的名称
	std::string GetFieldName(int index) const;
	// 获取这条记录的分类标签
	int GetLabel() const;

	// 设置第 i 个字段的值
	void SetFieldValue(int index, int value);
	// 设置这条记录的分类标签
	void SetLabel(int value);

	unsigned int GetID() const { return m_id; }

private:
	int _stringToInt(const std::string&);

private:
	BYTE iProtocolType;//02 协议类型 3类
	BYTE iService;		//03 服务类型 66类
	BYTE iStatusFlag;	//04 状态标志
	int iSrcBytes;		//05 源到目的字节数
	int iDestBytes;		//06 目的到源字节数
	int iFailedLogins;	//11 登录失败次数
	int iNumofRoot;		//16 root用户权限存取次数
	int iCount;			//23 2s内连接相同主机数目
	int iSvrCount;		//24 2s内连接相同端口数目
	BYTE iRerrorRate;	//27 "REJ"错误的连接数比例
	BYTE iSameSrvRate;	//29 连接到相同端口数比例
	BYTE iDiffSrvRate;	//30 连接到不同端口数比例
	int iDstHostSrvCount;	//33 相同目的地相同端口连接数
	BYTE iDstHostSameSrvRate;	//34 相同目的地相同端口连接数比例
	BYTE iDstHostDiffSrvRate;	//35 相同目的地不同端口连接数比例
	BYTE iDstHostSameSrcPortRate;	//36 相同目的地相同源端口连接数比例
	BYTE iDstHostSrvDiffHostRate;	//37 不同主机连接相同端口比例
	BYTE iDstHostSrvSerrorRate;	//39 连接当前主机有S0错误的比例
	BYTE iLabel;	//42 类型标签

	unsigned int m_id;
};

std::istream& operator >> (std::istream &, strMyRecord &);
std::ostream& operator<<(std::ostream &, const strMyRecord &);

// 将从文件读取到的字符串 ProtolType 转换为符号值
BYTE getProtolType(const std::string& str);
// 将从文件读取到的字符串 Service 转换为符号值
BYTE getService(const std::string& str);
// 将从文件读取到的字符串 StatusFlag 转换为符号值
BYTE getStatusFlag(const std::string& str);
// 将从文件读取到的字符串标签转换为符号值
BYTE getLabel(const std::string& str);
// 将符号值标签转换为字符串
std::string getLabelName(BYTE label);


// 聚类对象的基类
class Object {
public:
	typedef std::vector<strMyRecord*> record_list;
	typedef record_list::iterator record_iterator;
	typedef record_list::const_iterator record_const_iterator;

	virtual ~Object() {}
};

#endif // !STR_MY_RECORD_H


