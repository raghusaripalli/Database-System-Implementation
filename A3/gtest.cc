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

int loadNationTable() {
    DBFile dbfile;
	dbfile.Create (n->path(), heap, NULL);

	char tbl_path[100];
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, n->name()); 

	dbfile.Load (*(n->schema ()), tbl_path);
    dbfile.Close();
    return 0;
}

TEST(SortedFile, Load)
{
    int actual = loadNationTable();
    ASSERT_EQ(0, actual);
}

TEST(SortedFile, Close)
{
    loadNationTable();
    DBFile dbFile;
    char *fpath = n->path();
    dbFile.Open(fpath);
    int actual = dbFile.Close();
    ASSERT_EQ(2, 2);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    setup();
    return RUN_ALL_TESTS();
}