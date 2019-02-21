#ifndef TASK_H
#define TASK_H

#include "DataStructureBase.h"

class Task {
public:
	virtual Status Run() = 0;
	virtual ~Task() {}
};

#endif // !TASK_H
