#ifndef KMEANS_H
#define KMEANS_H

#include "Cluster.h"
#include "Record.h"

#include <vector>

class ClusterTree;
class ClusterNode;
class Log;

// 算法参数选项
struct KOptions
{
	KOptions() :Dimension(18), Print(true), Consistency(true), ClusterPrecision(0.1),
		MaxLevel(10), ThreadNum(4), PrecisionIncreaseLevel(3), KValue(5),LogFile("Log.txt") {}

	int Dimension;		// 数据维数，默认 18
	bool Print;			// 是否打印日志，默认 true
	bool Consistency;	// 结果是否需要强一致(多线程并发的随机性可能导致训练结果的不确定性)，默认 true
	double ClusterPrecision;	// 聚类精度阈值，默认 0.1
	int MaxLevel;				// 聚类树最大层数，默认 10
	int ThreadNum;				// 并发线程数量，默认 4
	int PrecisionIncreaseLevel;	// 聚类精度递增的层数阈值，默认 3
	int KValue;			// K值，要分成几个聚类，默认 5
	std::string LogFile;		// 日志文件，默认为 "Log.txt"，若为空，则不写入文件
};

class KMeans : public Object
{
public:
	KMeans(KOptions options = KOptions());

	~KMeans();

	// 运行 KMeans 算法，返回训练后的聚类树，可用于预测结果
	template < typename Iter >
	ClusterTree* RunKMeans(Iter begin, Iter end) {
		m_RecordsList.clear();
		while (begin != end) {
			m_RecordsList.push_back(&(*begin));
			++begin;
		}

		return _runKMeans();
	}

	// 获取算法的参数配置
	KOptions GetOptions() const { return m_options; }

private:
	KMeans(ClusterNode *pSelf, ClusterTree *pTree, int KmeansID, int Level, const record_list& pDataList, KOptions options,Log* l);

	//判断该条记录与之前的聚类中心是否完全相同
	bool isSameAsCluster(Record *pRecord) const;

	//K-means算法的第一步：从n个数据对象任意选择k个对象作为初始聚类中心
	void InitClusters(unsigned int NumClusters);

	//K-means算法的第二步：把每个对象分配给与之距离最近的聚类
	void DistributeSamples();

	//K-means算法的第三步：重新计算每个聚类的中心
	bool CalcNewClustCenters();

	//K-means算法的总体过程
	ClusterTree* _runKMeans();

	//找到离给定数据对象最近的一个聚类
	int FindClosestCluster(Record *pRecord) const;

	//检查聚类后一类中的分类是否合理
	bool IsClusterOK(int i) const;

	//获得本对象中子类i包含不同的Label个数
	int GetDiffLabelofCluster(int i) const;

	//判断是否结束递归过程
	bool IsStopRecursion(int i) const { return m_ClusterLevel >= m_options.MaxLevel; }

	//创建聚类树节点
	void CreatClusterTreeNode();

	// 随机数相关函数
	int initRandSeed();
	int nrand(int n);
	// 获取 [start,start+lenth) 之间的随机整数
	int getRandomNumByLength(int start, int length);
	// 获取 [start,end) 之间的随机整数
	int getRandomNumByRange(int start, int end);

private:
	record_list m_RecordsList;		// 数据记录链表
	std::vector<Cluster> m_Cluster;	// 子类数组
	int m_ClusterLevel;				// 聚类对象所处的层次
	int m_KmeansID;					// KMeans对象的ID号
	ClusterTree *pClusterTree;		// 聚类树的指针
	ClusterNode *pSelfClusterNode;	// 与本KMeans对象相关的聚类节点的指针

	KOptions m_options;				// 聚类算法相关参数选项

	Log *m_log;
};

#endif // !KMEANS_H
