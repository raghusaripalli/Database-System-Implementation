#ifndef DBFILE_H
#define DBFILE_H
#include <string>
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

typedef enum {heap, sorted, tree} fType;

class DBFile {

public:
	// constructor
	DBFile ();
	virtual ~DBFile();

	// creates the file
	virtual int Create (const char *fpath, fType file_type, void *startup);

	// Opens the file
	virtual int Open (const char *fpath);

	// Closes the file
	virtual int Close ();

	// load bulk data into files
	void Load (Schema &myschema, const char *loadpath);

	// moves file pointer to the first record
	virtual void MoveFirst ();

	// Adds records to the page
	virtual void Add (Record &addme);

	// Fetches next record and increments the pointer
	int GetNext (Record &fetchme);

	// Fetches next record as per CNF
	virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal);
protected:
	// parse table name from the file path
	std::string parseTableName(const char* fpath) {
		std::string path(fpath);
		size_t start = path.find_last_of('/'),
			end = path.find_last_of('.');
		return path.substr(start+1, end-start-1);
	}
	
	// decide in what mode the table is in
	enum Mode { read, write } mode;

	off_t pageIdx; // index pointer
	Page page; // page reference
	File file; // file reference
private:
	void createFile(fType ftype);
};
#endif
