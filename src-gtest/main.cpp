//
// Created by bf on 25-1-14.
//

#include "main.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

TEST(SampleTest, SimpleAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
