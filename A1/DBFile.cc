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
    db = NULL;
}

DBFile::~DBFile () {

}

int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    // If f_path is NULL
    if (f_path == NULL) {
        return 0;
    }
    // throw error if file is already opened
    FATALIF(db!=NULL, "File already opened.");
    
    createFile(f_type);
    
    // convert const char to char
    char *fPath = strdup(f_path);

    // Open output file if present else create and open
    db->file.Open(0, fPath);

    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) {
    
    // throw error if loadpath is null
    FATALIF(loadpath==NULL, "File already opened.");
    
    // change to write mode
    mode = write;

    // Open input file
    FILE* input_file = fopen(loadpath, "r");

    // throw error if unable to open input file
    FATALIF(input_file==NULL, loadpath);
    
    // Iterate through records and add each record to output file
    Record nextRecord;
    db->page.EmptyItOut();
    while (nextRecord.SuckNextRecord(&f_schema, input_file)) {
        Add(nextRecord);
    }

    // close the input_file
    fclose(input_file);
}

int DBFile::Open (const char* fpath) {
    // If f_path is NULL
    if (fpath == NULL) {
        return 0;
    }

    // throw error if file is already opened
    FATALIF(db!=NULL, "File already opened.");

    // use heap file
    int ftype = heap;

    // append file name at the beginning
    ifstream ifs((db->parseTableName(fpath)).c_str());
    if (ifs) {
        ifs >> ftype;
        ifs.close();
    }
    createFile(static_cast<fType>(ftype));

    // convert const char to char
    char *fPath = strdup(fpath);

    // open file
    db->file.Open(1, fPath);

    return 1;
}

void DBFile::MoveFirst () {
    db->MoveFirst();
}

int DBFile::Close () {
    // close the file and return the value
    return db->Close();
}

void DBFile::Add (Record &rec) {
    db->Add(rec);
}

int DBFile::GetNext (Record &fetchme) {
    // fetch next record
    while (!db->page.GetFirst(&fetchme)) {
        if(++db->pageIdx > db->file.lastIndex()) {
            // no records
            return 0;
        }
        db->file.GetPage(&db->page, db->pageIdx);
    }
    // record fetched!
    return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    return db->GetNext(fetchme, cnf, literal);
}

/**
 *  Creates file by the file type
 */
void DBFile::createFile(fType ftype) {
    switch (ftype) {
        case heap: db = new Heap(); break;
        default: db = NULL;
    }
    // throw error upon invalid file type
    FATALIF(db==NULL, "File Type is Invalid.");
}