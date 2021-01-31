#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "Errors.h"

#include "Heap.h"

Heap::Heap() {}

Heap::~Heap() {}


int Heap::Close () {
    // if mode is write and page have records
    if (mode == write && !page.is_empty()) 
        file.addPage(&page); // write full page to file
    return file.Close(); // close and return value
}

void Heap::Add (Record& record) {
    // set mode to write
    mode = write;
    // append records to page
    if(!page.Append(&record)) {
        // when page is full, write full page to file
        file.addPage(&page);
        page.EmptyItOut();
        // and then append the record to page
        page.Append(&record);
    }
}

void Heap::MoveFirst() {
    readMode();
    file.GetPage(&page, pageIdx=0);
}

int Heap::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    ComparisonEngine comp;
    while(DBFile::GetNext(fetchme)) {
        // return 1 if record matches to the CNF
        if(comp.Compare(&fetchme, &literal, &cnf)) {
            return 1;
        }
    }
    // record didn't match to the CNF
    return 0;
}
