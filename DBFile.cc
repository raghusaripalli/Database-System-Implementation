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
    printf("DBFile::LOAD\n");
    mode = write;
    FILE* input_file = fopen(loadpath, "r");
    FATALIF(input_file==NULL, loadpath);
    Record nextRecord;
    page.EmptyItOut();
    while (nextRecord.SuckNextRecord(&f_schema, input_file)) {
        Add(nextRecord);
    }
}

int DBFile::Open (const char* fpath) {
    printf("DBFile::OPEN\n");
    FATALIF(db!=NULL, "File already opened.");
    int ftype = heap;
    ifstream ifs((db->parseTableName(fpath)).c_str());
    if (ifs) {
        ifs >> ftype;
        ifs.close();
    }
    createFile(static_cast<fType>(ftype));
    char *fPath = strdup(fpath);
    file.Open(1, fPath);
    return 1;
}

void DBFile::MoveFirst () {
    printf("DBFile::MoveFirst\n");
}

int DBFile::Close () {
    printf("DBFile::Close\n");
    db->Close();
}

void DBFile::Add (Record &rec) {
    printf("DBFile::ADD\n");
    db->Add(rec);
}

int DBFile::GetNext (Record &fetchme) {
    printf("DBFile::GET_NEXT\n");
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    printf("DBFile::GET_NEXT__\n");
}

void DBFile::createFile(fType ftype) {
    printf("DBFile::createFILE\n");
    switch (ftype) {
        case heap: db = new Heap(); break;
        case sorted: break;
        default: db = NULL;
    }
    FATALIF(db==NULL, "File Type is Invalid.");
}