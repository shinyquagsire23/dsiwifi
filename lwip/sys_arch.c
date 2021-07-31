#include "sys.h"

static u32 val = 0;

void sys_now_inc(u32 amt)
{
    val += amt;
}

u32_t
sys_now(void)
{
    return val;
}
