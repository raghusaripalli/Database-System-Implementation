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
	DBFile ();
	virtual ~DBFile();
	virtual int Create (const char *fpath, fType file_type, void *startup);
	virtual int Open (const char *fpath);
	virtual int Close ();
	void Load (Schema &myschema, const char *loadpath);
	virtual void MoveFirst ();
	virtual void Add (Record &addme);
	int GetNext (Record &fetchme);
	virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal);
protected:
	std::string parseTableName(const char* fpath) {
		std::string path(fpath);
		size_t start = path.find_last_of('/'),
			end = path.find_last_of('.');
		return path.substr(start+1, end-start-1);
	}
	enum Mode { read, write } mode;
	off_t pageIdx;
	Page page;
	File file;
private:
	void createFile(fType ftype);
};
#endif
