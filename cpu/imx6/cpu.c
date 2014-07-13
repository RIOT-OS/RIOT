#include "irq.h"

inline void dINT(void)
{
    disableIRQ();
}
inline void eINT(void)
{
    enableIRQ();
}
