#include "gtest/gtest.h"
#include <iostream>
#include <cstring>
#include <climits>
#include <string>
#include <algorithm>

#include "Errors.h"
#include "Execute.h"
#include "ParseTree.h"
#include "Queryops.h"
#include "Statistics.h"
#include "QueryPlan.h"

using std::cout;

char* catalog_path = "catalog";
char* dbfile_dir = "bin/";
char* tpch_dir = "../DATA/1G";

extern "C" {
  int yyparse(void);     // defined in y.tab.c
  struct YY_BUFFER_STATE* yy_scan_string(const char*);
}

extern struct FuncOperator *finalFunction; // the aggregate function (NULL if no agg)
extern struct TableList *tables; // the list of tables and aliases in the query
extern struct AndList *boolean; // the predicate in the WHERE clause
extern struct NameList *groupingAtts; // grouping atts (NULL if no grouping)
extern struct NameList *attsToSelect; // the set of attributes in the SELECT (NULL if no such atts)
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query
extern int distinctFunc;  // 1 if there is a DISTINCT in an aggregate query
extern char* newtable;
extern char* oldtable;
extern char* newfile;
extern char* deoutput;
extern struct AttrList *newattrs; //Use this to build Attribute* array and record schema


TEST (CREATETEST, CREATETABLEPASS) {

	char *fileName = "Statistics.txt";
	Statistics s;
	Queryops d;
	QueryPlan plan(&s);

	char* cnf1 = "CREATE TABLE table1(id INTEGER, name STRING) AS HEAP;";
	yy_scan_string(cnf1);
	yyparse();
	
	s.Read(fileName);
	
	ASSERT_EQ(true, d.createTable());

	char* cnf2 = "DROP TABLE table1;";
	yy_scan_string(cnf2);
	yyparse();

	s.Read(fileName);

	d.dropTable();
}

TEST (CREATETEST, CREATETABLEFAIL) {

	char *fileName = "Statistics.txt";
	Statistics s;
	Queryops d;
	QueryPlan plan(&s);

	char* cnf1 = "CREATE TABLE table2(id INTEGER, name STRING) AS HEAP;";
	yy_scan_string(cnf1);
	yyparse();

	s.Read(fileName);

	d.createTable();

	char* cnf2 = "CREATE TABLE table2(id INTEGER, name STRING) AS HEAP;";
	yy_scan_string(cnf2);
	yyparse();

	s.Read(fileName);

	ASSERT_EQ(false, d.createTable());

	char* cnf3 = "DROP TABLE table2;";
	yy_scan_string(cnf3);
	yyparse();

	s.Read(fileName);

	d.dropTable();
}

TEST (INSERTTEST, INSERTINTO) {

	char *fileName = "Statistics.txt";
	Statistics s;
	Queryops d;
	QueryPlan plan(&s);

	char* cnf1 = "CREATE TABLE table1(id INTEGER, name STRING) AS HEAP;";
	yy_scan_string(cnf1);
	yyparse();

	s.Read(fileName);

	d.createTable();

	char* cnf2 = "INSERT 'data.txt' INTO table1;";
	yy_scan_string(cnf2);
	yyparse();

	s.Read(fileName);

	ASSERT_EQ(true, d.insertInto());

	char* cnf3 = "DROP TABLE table1;";
	yy_scan_string(cnf3);
	yyparse();

	s.Read(fileName);

	d.dropTable();
}

TEST (DROPTEST, DROPTABLEPASS) {

	char *fileName = "Statistics.txt";
	Statistics s;
	Queryops d;
	QueryPlan plan(&s);

	char* cnf1 = "CREATE TABLE table1(id INTEGER, name STRING) AS HEAP;";
	yy_scan_string(cnf1);
	yyparse();

	s.Read(fileName);

	d.createTable();

	char* cnf2 = "DROP TABLE table1;";
	yy_scan_string(cnf2);
	yyparse();

	s.Read(fileName);

	ASSERT_EQ(true, d.dropTable());
}

TEST (DROPTEST, DROPTABLEFAIL) {

	char *fileName = "Statistics.txt";
	Statistics s;
	Queryops d;
	QueryPlan plan(&s);

	char* cnf = "DROP TABLE table1;";
	yy_scan_string(cnf);
	yyparse();

	s.Read(fileName);

	ASSERT_EQ(false, d.dropTable());
}

int main(int argc, char *argv[]) {
	
	testing::InitGoogleTest(&argc, argv); 
	
	return RUN_ALL_TESTS ();
	
}
