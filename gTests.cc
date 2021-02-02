#include <gtest/gtest.h>
#include "DBFile.h"

TEST(DBFile, CreateNegative) {
    DBFile dbFile;
    ASSERT_EQ(dbFile.Create(NULL, heap, NULL), 0);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}