#include "gtest/gtest.h"
#include "SortedFile.h"
#include "test.h"

TEST(SortedFile, Create)
{
    DBFile dbFile;
    void *startup = NULL;
    char *fpath = NULL;
    int actual = dbFile.Create(fpath, sorted, startup);
    int expected = -1;
    ASSERT_EQ(expected, actual);
}

TEST(SortedFile, Open)
{
    DBFile dbFile;
    char *fpath = NULL;
    int actual = dbFile.Open(fpath);
    int expected = -1;
    ASSERT_EQ(expected, actual);
}

TEST(SortedFile, Close)
{
    DBFile dbFile;
    char *fpath = n->path();
    dbFile.Open(fpath);
    int actual = dbFile.Close();
    ASSERT_EQ(2, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    setup();
    return RUN_ALL_TESTS();
}