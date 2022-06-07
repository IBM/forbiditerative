#ifndef TASKS_ROOT_TASK_H
#define TASKS_ROOT_TASK_H

#include "../abstract_task.h"

namespace tasks {
static const int PRE_FILE_VERSION = 3;
extern std::shared_ptr<AbstractTask> g_root_task;
extern void read_root_task(std::istream &in);
}
#endif
