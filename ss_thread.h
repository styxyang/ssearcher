#ifndef SS_POOL_H_
#define SS_POOL_H_

#include "ss_config.h"

void *ss_dispatcher_thread(void *);
void *ss_worker_thread(void *arg);

#endif /* SS_POOL_H_ */
