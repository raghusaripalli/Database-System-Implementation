#include "Errors.h"
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "Heap.h"
#include <fstream>
#include <string.h>

DBFile *db;

DBFile::DBFile () {

}

DBFile::~DBFile () {

}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    printf("DBFile::Create\n");
    FATALIF(db!=NULL, "File already opened.");
    createFile(f_type);
    char *fPath = strdup(f_path);
    file.Open(0, fPath);
    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
}

int DBFile::Open (const char* fpath) {
    printf("DBFile::Open\n");
    FATALIF(db!=NULL, "File already opened.");
    int ftype = heap;
    ifstream ifs((db->getTableName(fpath)+".meta").c_str());
    if (ifs) {
        ifs >> ftype;
        ifs.close();
    }
    createFile(static_cast<fType>(ftype));
    return db->Open(fpath);
}

void DBFile::MoveFirst () {
}

int DBFile::Close () {
}

void DBFile::Add (Record &rec) {
}

int DBFile::GetNext (Record &fetchme) {
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
}

void DBFile::createFile(fType ftype) {
  switch (ftype) {
    case heap: db = new Heap(); break;
    case sorted: break;
    default: db = NULL;
  }
  FATALIF(db==NULL, "File Type is Invalid.");
}