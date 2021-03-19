#include "gtest/gtest.h"
#include "BigQ.h"
#include "test.h"


TEST(BigQ, Initialize) {
    Pipe in(100), out(100);
    OrderMaker dummy;
    BigQ bq(in, out, dummy, 10);
    ASSERT_EQ(1, 1);
}


TEST(BigQ, SortHelper) {
    Pipe in(100), out(100);
    OrderMaker dummy;
    BigQ bq(in, out, dummy, 10);
    string rand1 = bq.tmpfName();
    string rand2 = bq.tmpfName();
    ASSERT_NE(rand1, rand2);
}

TEST(BigQ, TestBigQRandomFileFunction) {
    Pipe in(100), out(100);
    OrderMaker dummy;
    BigQ bq(in, out, dummy, 10);
    void *expected = bq.Working(NULL);
    void *actual = NULL;
    ASSERT_EQ(expected, actual);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}