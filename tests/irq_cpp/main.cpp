/*
* Copyright (C) 2021 Jens Wetterich <jens@wetterich-net.de>
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Unit tests for the C++ IRQ wrapper irq.hpp
 *
 * @author     Jens Wetterich <jens@wetterich-net.de>
 */

#define FFF_ARG_HISTORY_LEN  1u
#define FFF_CALL_HISTORY_LEN 1u
#include "cppunit.hpp"
#include "fff.h"
#include "irq.h"
#include "irq.hpp"
DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(unsigned, irq_disable)
FAKE_VALUE_FUNC(unsigned, irq_enable)
FAKE_VALUE_FUNC(int, irq_is_enabled)
FAKE_VALUE_FUNC(int, irq_is_in)
FAKE_VOID_FUNC(irq_restore, unsigned)

class irq_suite : public riot::testing::test_suite {
public:
    void set_up() override {
        RESET_FAKE(irq_restore);
        RESET_FAKE(irq_disable);
        RESET_FAKE(irq_enable);
        RESET_FAKE(irq_is_enabled);
        RESET_FAKE(irq_is_in);
    }
};

TEST_SUITE_F(irq_suite, irq);

TEST(irq, is_isr) {
    // Setup test data
    irq_is_in_fake.return_val = 0;
    // Run test
    auto en = riot::irq_lock::is_isr();
    irq_is_in_fake.return_val = 1;
    auto en2 = riot::irq_lock::is_isr();
    // Assert results
    EXPECT_EQ(en, false, "Return Value");
    EXPECT_EQ(en2, true, "Return Value");
    EXPECT_FFF_CALL_COUNT(irq_is_in, 2);
}

TEST(irq, is_enabled) {
    // Setup test data
    irq_is_enabled_fake.return_val = 0;
    // Run test
    auto en = riot::irq_lock::is_locked();
    irq_is_enabled_fake.return_val = 1;
    auto en2 = riot::irq_lock::is_locked();
    // Assert results
    EXPECT_STREQ("s", "s", "");
    EXPECT_EQ(en, true, "Return Value");
    EXPECT_EQ(en2, false, "Return Value");
    EXPECT_FFF_CALL_COUNT(irq_is_enabled, 2);
}

TEST(irq, irq_disable_restore) {
    // Setup test data
    auto reg = 7u;
    irq_disable_fake.return_val = reg;
    // Run test
    {
        riot::irq_lock lock;
    }
    // Assert results
    EXPECT_FFF_CALL_COUNT(irq_disable, 1);
    EXPECT_FFF_CALL_COUNT(irq_restore, 1);
    EXPECT_FFF_CALL_PARAMS(irq_restore, reg);
    EXPECT_EQ(irq_restore_fake.arg0_val, reg, "Restore Value");
}

int main() {
    puts("Testing irq wrapper");
    RUN_SUITE(irq);
}
/** @} */
