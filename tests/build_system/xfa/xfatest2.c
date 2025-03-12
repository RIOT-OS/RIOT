#include "xfa.h"
#include "xfatest.h"

XFA(xfatest_t, xfatest, 1, _xfatest2) = { .val = 2, .text = "xfatest2", .letter = 'b' };
XFA(xfatest_t, xfatest, 3, _xfatest4) = { .val = 4, .text = "xfatest4", .letter = 'd' };
XFA(xfatest_t, xfatest, 2, _xfatest3) = { .val = 3, .text = "xfatest3", .letter = 'c' };

XFA_CONST(xfatest_t, xfatest_const, 1, _xfatest_const2) = { .val = 45, .text = "xfatest_const2", .letter = 'b' };
XFA_CONST(xfatest_t, xfatest_const, 3, _xfatest_const4) = { .val = 44, .text = "xfatest_const4", .letter = 'd' };
XFA_CONST(xfatest_t, xfatest_const, 2, _xfatest_const3) = { .val = 42, .text = "xfatest_const3", .letter = 'c' };
