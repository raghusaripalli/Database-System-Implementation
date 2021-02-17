#include <gtest/gtest.h>
#include "DBFile.h"
#include "test.h"

using namespace std;


const char *dbfile_dir = "";
const char *tpch_dir ="tables/";
const char *catalog_path = "catalog";
DBFile dbFile;

TEST(DBFile, TestCreate) {
    ASSERT_EQ(dbFile.Create(NULL, heap, NULL), 0);
}

TEST(DBFile, TestOpen) {
    ASSERT_EQ(dbFile.Open(NULL), 0);
}

TEST(DBFile, TestClose) {
    ASSERT_EQ(dbFile.Close(), 0);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    setup (catalog_path, dbfile_dir, tpch_dir);
    return RUN_ALL_TESTS();
}