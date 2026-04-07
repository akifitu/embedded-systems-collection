#include "obstruction_policy.h"

#define NUDGE_RETRY_THRESHOLD 2u

unsigned obstruction_policy_record_retry(unsigned current_retries) {
    return current_retries + 1u;
}

bool obstruction_policy_should_nudge(unsigned current_retries) {
    return current_retries >= NUDGE_RETRY_THRESHOLD;
}
