#ifndef INTERLOCK_GUARD_H
#define INTERLOCK_GUARD_H

#include "burner_types.h"

burner_fault_t interlock_guard_fault(burner_state_t state,
                                     const burner_input_t *input);

#endif
