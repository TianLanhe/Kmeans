#ifndef COMMON_H
#define COMMON_H

#include "strmyrecord.h"

// 最大维数
#define DIMENSION 18
// 最大子类数
#define MAXCLUSTER 5
// 最大子类数
#define MAX_LABEL 5
// 训练集文件
#define TRAINING_FILE "data/kddcup.data_10_percent_corrected_datatreat"
// 测试集文件
#define TESTING_FILE "data/corrected_datatreat"
// 日志文件
#define	LOG_FILE "Log.txt"
// 结果文件
#define RESULT_FILE "Result.txt"
// 聚类精度阈值
#define CLUSTER_PRECITION 0.1
// 聚类树层数
#define INTERLEVEL 3
// 最大迭代次数
#define MAX_ITERATION 10

#endif // !COMMON_H
