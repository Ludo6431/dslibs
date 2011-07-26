#ifndef _PROCSPROCS_H
#define _PROCSPROCS_H

#include <dstk.h>

#if 0
// 65536-(BUS_CLOCK>>10)/59,8261
#define TIMEOUT_VBL ((u16)64989)
// period
#define TIMEOUT_MS(p) ((u16)-((BUS_CLOCK>>10)*((p)/100)))
// frequence
#define TIMEOUT_HZ(f) ((u16)TIMER_FREQ_1024(f))
#endif

#endif

