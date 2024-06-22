#if PROFILE

#include "profile.h"

#define PROFILE_BEGIN Profile profile{instructions};
#define PROFILE_INSTRUCTION profile.instruction(instruction);

#else

#define PROFILE_BEGIN
#define PROFILE_INSTRUCTION

#endif
