#ifndef CLUSTER_H
#define CLUSTER_H

#include "strmyrecord.h"

#include <map>
#include <vector>

class Cluster : public Object
{
public:
	friend std::ostream& operator<<(std::ostream& out, const Cluster& cluster);

	typedef std::vector<double>::size_type center_size_type;

public:
	Cluster(int dimension = 0) :m_hasCalculated(false), m_mainLabel(0), m_clusterPrecition(1.0), m_center(dimension, 0.0) {
	}

	// 初始化一个聚类：清空所有数据，并添加一条记录，再更新中心为这条记录
	void Init(strMyRecord* record);

	// 把一条记录添加到类簇中
	void Add(strMyRecord* record);

	// 清空类簇中的所有记录
	void Clear();

	// 判断该聚类是否是空的
	bool Empty() const { return m_records.empty(); }

	// 更新类簇中心
	bool UpdateCenter();

	double CalcDistance(strMyRecord *record) const;

	// 判断聚类中心是否相等
	bool operator==(const Cluster&) const;
	bool operator!=(const Cluster& other) const { return !operator==(other); }

	// 判断聚类中心是否跟记录相等
	bool Equal(strMyRecord* record) const;

	// 获取属于该聚类的记录总数量
	int GetTotalNum() const { return m_records.size(); }

	// 获取属于主类别的记录数量
	int GetMainNum() const;

	// 获取不属于主类别的记录数量
	int GetIncorrectNum() const;

	// 获取该类簇的类别数量
	int GetLabelNum() const;

	// 获取该类簇的主类别
	int GetMainLabel() const;

	std::string GetCenterStr() const;

	// 获取该类簇的聚类精度
	double GetClusterPrecition() const;

	// 获取该类簇的记录列表
	record_list& GetRecordList() { return m_records; }
	const record_list& GetRecordList() const { return m_records; }

	// 计算上述数据
	void CalcInfo() { _calculateInfo(); }

private:
	bool _isCalculated() const { return m_hasCalculated; }

	void _calculateInfo() const;

private:
	std::vector<double> m_center; // 类簇中心
	record_list m_records;		// 属于该聚类的记录

	mutable std::map<int, int> m_labelMap;	// 每个类别的记录数量
	mutable int m_mainLabel;				// 主类别
	mutable double m_clusterPrecition;		// 聚类精度
	mutable bool m_hasCalculated;			// 是否已经计算过上述数据
};

std::ostream& operator<<(std::ostream& out, const Cluster& cluster);

#endif // !CLUSTER_H
