#include "xfa.h"
#include "xfatest.h"

XFA(xfatest_t, xfatest, 0) _xfatest1 = { .val = 1, .text = "xfatest1", .letter = 'a' };
XFA_CONST(xfatest_t, xfatest_const, 0) _xfatest_const1 = { .val = 123, .text = "xfatest_const1", .letter = 'a' };
