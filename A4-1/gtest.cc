#include "gtest/gtest.h"
#include "SortedFile.h"
#include "RelOp.h"
#include "Statistics.h"
#include "a2test.h"
#define EXPECTED 0


TEST(Statistics, ParseRelation)
{
    Statistics statistics;
    int actual = statistics.testParseRelation();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(Statistics, AddRel)
{
    Statistics statistics;
    int actual = statistics.testAddRel();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(Statistics, AddAttr)
{
    Statistics statistics;
    int actual = statistics.testAddAttr();
    ASSERT_EQ(EXPECTED, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}