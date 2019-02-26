#include "cluster.h"

#include <ostream>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

bool Cluster::operator==(const Cluster& other) const {
	for (center_size_type i = 0; i < m_center.size(); ++i)
		if (fabs(m_center[i] - other.m_center[i]) > 1e-5)
			return false;
	return true;
}

bool Cluster::Equal(strMyRecord* record) const {
	for (center_size_type i = 0; i < m_center.size(); ++i)
		if (fabs(m_center[i] - record->GetFieldValue(i)) > 1e-5)
			return false;
	return true;
}

void Cluster::_calculateInfo() const {
	if (m_hasCalculated)
		return;

	m_hasCalculated = true;

	// 统计每个类别的记录数量
	m_labelMap.clear();
	for (record_const_iterator cit = m_records.begin(); cit != m_records.end(); ++cit) {
		++m_labelMap[(*cit)->GetLabel()];
	}

	// 计算主类别，即找出数量最多的类别
	int maxNum = 0;
	for (map<int, int>::const_iterator cit = m_labelMap.begin(); cit != m_labelMap.end(); ++cit) {
		if (cit->second > maxNum) {
			m_mainLabel = cit->first;
			maxNum = cit->second;
		}
	}

	// 计算聚类精度 = (非主类别记录数量 / 主类别记录数量)
	m_clusterPrecition = (m_records.size() - m_labelMap[m_mainLabel])*1.0 / m_labelMap[m_mainLabel];
}

int Cluster::GetMainNum() const {
	if (!_isCalculated())
		_calculateInfo();
	return m_labelMap[m_mainLabel];
}
int Cluster::GetIncorrectNum() const {
	if (!_isCalculated())
		_calculateInfo();
	return m_records.size() - m_labelMap[m_mainLabel];
}

int Cluster::GetLabelNum() const {
	if (!_isCalculated())
		_calculateInfo();
	return m_labelMap.size();
}

int Cluster::GetMainLabel() const {
	if (!_isCalculated())
		_calculateInfo();
	return m_mainLabel;
}

double Cluster::GetClusterPrecition() const {
	if (!_isCalculated())
		_calculateInfo();
	return m_clusterPrecition;
}

std::string Cluster::GetCenterStr() const {
	string ret;
	char prefix = '[';
	for (int i = 0; i < 17; ++i) {
		string num = to_string(m_center[i]);
		if (num.find('.') != string::npos) {
			num = num.substr(0, num.find('.') + 3);
		}
		ret.append(1, prefix).append(num);
		prefix = ',';
	}
	ret.append(1, ']');
	return ret;
}

void Cluster::Add(strMyRecord* record) {
	if (_isCalculated())
		throw(string("can not add record to a centain cluster"));

	m_records.push_back(record);
}

void Cluster::Clear() {
	m_records.clear();
	m_hasCalculated = false;
}

void Cluster::Init(strMyRecord* record) {
	Clear();
	Add(record);
	for (center_size_type i = 0; i < m_center.size(); ++i)
		m_center[i] = record->GetFieldValue(i);
}

bool Cluster::UpdateCenter() {
	Cluster TempCenter(m_center.size());
	strMyRecord *pRecord;

	// 聚类中没有元素了，聚簇中心不存在
	if (m_records.empty())
		return true;

	// 所有记录的各个维度进行累加
	for (record_const_iterator cit = m_records.begin(); cit != m_records.end(); ++cit) {
		pRecord = *cit;
		for (center_size_type i = 0; i < m_center.size(); ++i)
			TempCenter.m_center[i] += pRecord->GetFieldValue(i);
	}

	// 每个维度求平均值
	for (center_size_type j = 0; j < m_center.size(); ++j) {
		TempCenter.m_center[j] /= m_records.size();
	}

	// 更新聚簇中心
	if (this->operator!=(TempCenter)) {
		for (center_size_type i = 0; i < m_center.size(); ++i)
			m_center[i] = TempCenter.m_center[i];
		return true;
	}

	return false;
}

double Cluster::CalcDistance(strMyRecord *record) const {
	double fDist = 0;

	for (center_size_type i = 0; i < m_center.size(); ++i)
		fDist += pow((record->GetFieldValue(i) - m_center[i]), 2);

	// 省去开方
	return fDist;
}

ostream& operator<<(ostream& out, const Cluster& cluster) {
	if (out) {
		streamsize precision = out.precision(3);

		out << "LabelNum:" << setw(2) << cluster.GetLabelNum() << ' ';
		out << "MainLabel:" << setw(2) << cluster.GetMainLabel() << ' ';
		out << "NumOfRecord:" << setw(6) << cluster.GetTotalNum() << ' ';
		out << "NumOfMainRecord:" << setw(6) << cluster.GetMainNum() << ' ';
		out << "ClusterPrecition:" << setiosflags(ios::fixed) << setprecision(3) << setw(5) << cluster.GetClusterPrecition() << ' ';

		map<int, int> labelMap = cluster.m_labelMap;
		out << "LabelCount: ";
		out << setiosflags(ios::left);
		for (map<int, int>::const_iterator cit = labelMap.begin(); cit != labelMap.end(); ++cit) {
			out << cit->first << "->" << setw(5) << cit->second << ' ';
		}

		out.precision(precision);
		out << resetiosflags(ios::left);
		/*out << "Center:" << GetCenterStr() << ' ';*/
	}
	return out;
}
