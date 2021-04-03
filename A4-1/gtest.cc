#include "gtest/gtest.h"
#include "SortedFile.h"
#include "RelOp.h"
#include "a2test.h"
#define EXPECTED 0


TEST(RelationalOp, create_joinable_thread)
{
    Project project;
    int actual = project.test();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(GroupBy, Run)
{
    GroupBy groupBy;
    int actual = groupBy.test();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(SelectFile, Work)
{
    SelectFile selectFile;
    int actual = selectFile.test();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(DuplicateRemoval, Work)
{
    DuplicateRemoval duplicateRemoval;
    int actual = duplicateRemoval.test();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(Join, sortMergeJoin)
{
    Join join;
    int actual = join.test();
    ASSERT_EQ(EXPECTED, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}