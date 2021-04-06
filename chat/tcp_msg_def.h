#pragma once

#include "intdef.h"

struct msg_head
{
    uint32_t type;
    uint32_t size;
    uint32_t chksum;
};

#define MAX_BODY_SIZE 1024
