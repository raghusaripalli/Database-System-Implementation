#ifndef HEAP_FILE_H_
#define HEAP_FILE_H_

#include "DBFile.h"

class HeapFile: protected GenericDBFile {
  friend class DBFile;
  using GenericDBFile::GetNext;
protected:
  HeapFile (); 
  ~HeapFile() {}

  int Close ();
  void Add (Record& me);

  void MoveFirst();
  int GetNext (Record& fetchme, CNF& cnf, Record& literal);

protected:
  void startWrite() { mode = WRITE; }
  void startRead();

private:
  void addtoNewPage(Record& rec) {
    curPage.EmptyItOut();
    curPage.Append(&rec);
  }

  HeapFile(const HeapFile&);
  HeapFile& operator=(const HeapFile&);
};

inline void HeapFile::startRead() {
  if (mode == READ) return;
  mode = READ;
  if (!curPage.empty()) theFile.addPage(&curPage);
}

#endif
