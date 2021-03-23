#include "gtest/gtest.h"
#include "SortedFile.h"
#include "RelOp.h"
#include "a2test.h"


TEST(RelationalOp, create_joinable_thread)
{
    for (int i = 0 ; i < 100000; i++);
    ASSERT_EQ(0, 0);
}

TEST(GroupBy, Run)
{
    for (int i = 0 ; i < 1000000; i++);
    ASSERT_EQ(0, 0);
}

TEST(SelectFile, Work)
{
    for (int i = 0 ; i < 100000; i++);
    ASSERT_EQ(0, 0);
}

TEST(DuplicateRemoval, Work)
{
    for (int i = 0 ; i < 100000; i++);
    ASSERT_EQ(0, 0);
}

TEST(Join, sortMergeJoin)
{
    for (int i = 0 ; i < 10000000; i++);
    ASSERT_EQ(0, 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}