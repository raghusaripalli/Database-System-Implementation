#ifndef A1_TEST_H
#define A1_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "DBFile.h"
#include "Record.h"
#include "Schema.h"
using namespace std;

extern "C" {
	int yyparse(void);
}

extern struct AndList *final;

class relation {

private:
	char *rname;
	char *prefix;
	char rpath[100]; 
	Schema *rschema;
public:
	relation (char *_name, Schema *_schema, char *_prefix) :
		rname (_name), rschema (_schema), prefix (_prefix) {
		sprintf (rpath, "%s%s.bin", prefix, rname);
	}
	const char* name () { return rname; }
	const char* path () { return rpath; }
	Schema* schema () { return rschema;}
	void info () {
		cout << " relation info\n";
		cout << "\t name: " << name () << endl;
		cout << "\t path: " << path () << endl;
	}

	void get_cnf (CNF &cnf_pred, Record &literal) {
		cout << " Enter CNF predicate (when done press ctrl-D):\n\t";
  		if (yyparse() != 0) {
			std::cout << "Can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
	}
};

char *supplier = "supplier";
char *partsupp = "partsupp";
char *part = "part";
char *nation = "nation";
char *customer = "customer";
char *orders = "orders";
char *region = "region";
char *lineitem = "lineitem";

relation *s, *p, *ps, *n, *li, *r, *o, *c;

void setup (const char *catalog_path, const char *dbfile_dir, const char *tpch_dir) {
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";
	char *dfd = strdup(dbfile_dir);
	char* cp = strdup(catalog_path);
	s = new relation (supplier, new Schema (cp, supplier), dfd);
	ps = new relation (partsupp, new Schema (cp, partsupp), dfd);
	p = new relation (part, new Schema (cp, part), dfd);
	n = new relation (nation, new Schema (cp, nation), dfd);
	li = new relation (lineitem, new Schema (cp, lineitem), dfd);
	r = new relation (region, new Schema (cp, region), dfd);
	o = new relation (orders, new Schema (cp, orders), dfd);
	c = new relation (customer, new Schema (cp, customer), dfd);
}

void cleanup () {
	delete s, p, ps, n, li, r, o, c;
}

#endif
