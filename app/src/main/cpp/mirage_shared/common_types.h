#pragma once

#include "../openxr/openxr_platform_defines.h"

enum cts_instruction : uint64_t  {
    NONE = 0,
    POPULATE_SYSTEM_PROPERTIES = 1,
};