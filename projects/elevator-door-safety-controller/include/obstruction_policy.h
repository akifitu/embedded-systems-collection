#ifndef OBSTRUCTION_POLICY_H
#define OBSTRUCTION_POLICY_H

#include <stdbool.h>

unsigned obstruction_policy_record_retry(unsigned current_retries);
bool obstruction_policy_should_nudge(unsigned current_retries);

#endif
