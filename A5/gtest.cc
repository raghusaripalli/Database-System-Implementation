#include "gtest/gtest.h"
#include "ParseTree.h"
#include "QueryPlan.h"
#define EXPECTED 0

char* catalog_path = "catalog";
char* dbfile_dir = "";
char* tpch_dir = "../../data/tpch-1gb";

FuncOperator* finalFunction;
TableList* tables;
AndList* boolean;
NameList* groupingAtts;
NameList* attsToSelect;
int distinctAtts;
int distinctFunc;

char* newtable;
char* oldtable;
char* newfile;
char* deoutput;
struct AttrList *newattrs;
struct NameList *sortattrs;

TEST(Queryops, createTable)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = 0;
    ASSERT_EQ(EXPECTED, actual);
}

TEST(Queryops, insertInto)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = 0;
    ASSERT_EQ(EXPECTED, actual);
}

TEST(Queryops, dropTable)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = 0;
    ASSERT_EQ(EXPECTED, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}