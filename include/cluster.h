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

	// ��ʼ��һ�����ࣺ����������ݣ������һ����¼���ٸ�������Ϊ������¼
	void Init(strMyRecord* record);

	// ��һ����¼��ӵ������
	void Add(strMyRecord* record);

	// �������е����м�¼
	void Clear();

	// �жϸþ����Ƿ��ǿյ�
	bool Empty() const { return m_records.empty(); }

	// �����������
	bool UpdateCenter();

	double CalcDistance(strMyRecord *record) const;

	// �жϾ��������Ƿ����
	bool operator==(const Cluster&) const;
	bool operator!=(const Cluster& other) const { return !operator==(other); }

	// �жϾ��������Ƿ����¼���
	bool Equal(strMyRecord* record) const;

	// ��ȡ���ڸþ���ļ�¼������
	int GetTotalNum() const { return m_records.size(); }

	// ��ȡ���������ļ�¼����
	int GetMainNum() const;

	// ��ȡ�����������ļ�¼����
	int GetIncorrectNum() const;

	// ��ȡ����ص��������
	int GetLabelNum() const;

	// ��ȡ����ص������
	int GetMainLabel() const;

	std::string GetCenterStr() const;

	// ��ȡ����صľ��ྫ��
	double GetClusterPrecition() const;

	// ��ȡ����صļ�¼�б�
	record_list& GetRecordList() { return m_records; }
	const record_list& GetRecordList() const { return m_records; }

	// ������������
	void CalcInfo() { _calculateInfo(); }

private:
	bool _isCalculated() const { return m_hasCalculated; }

	void _calculateInfo() const;

private:
	std::vector<double> m_center; // �������
	record_list m_records;		// ���ڸþ���ļ�¼

	mutable std::map<int, int> m_labelMap;	// ÿ�����ļ�¼����
	mutable int m_mainLabel;				// �����
	mutable double m_clusterPrecition;		// ���ྫ��
	mutable bool m_hasCalculated;			// �Ƿ��Ѿ��������������
};

std::ostream& operator<<(std::ostream& out, const Cluster& cluster);

#endif // !CLUSTER_H
