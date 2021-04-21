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

TEST(QueryPlan, CreateLeafNode)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = query.testCreateLeafNode();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(QueryPlan, FindLeastCostJoin)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = query.testFindLeastCostJoin();
    ASSERT_EQ(EXPECTED, actual);
}

TEST(QueryPlan, EstimateJoinPermutationCost)
{
    Statistics *s;
    QueryPlan query(s);
    int actual = query.testEstimateJoinPermutationCost();
    ASSERT_EQ(EXPECTED, actual);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}