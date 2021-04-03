#ifndef SORTED_FILE_H_
#define SORTED_FILE_H_

#include <string>

#include "DBFile.h"
#include "Pipe.h"
#include "BigQ.h"
#include "Defs.h"

#define safeDelete(p) \
  {                   \
    delete p;         \
    p = NULL;         \
  }

class SortedFile : protected GenericDBFile
{
  static const size_t PIPE_BUFFER_SIZE = PIPE_SIZE;
  friend class DBFile;
  friend class HeapFile;
  using GenericDBFile::GetNext;

protected:
  SortedFile() : myOrder(NULL), runLength(0),
                 in(NULL), out(NULL), biq(NULL), useMem(false) {}
  ~SortedFile() {}

  int Create(char *fpath, void *startup);
  int Open(char *fpath);
  int Close();

  void Add(Record &addme);
  void Load(Schema &myschema, char *loadpath);

  void MoveFirst();
  int GetNext(Record &fetchme);
  int GetNext(Record &fetchme, CNF &cnf, Record &literal);

protected:
  void startWrite();
  void startRead();

private:
  OrderMaker *myOrder;
  int runLength;

  std::string tpath;
  std::string table;

  Pipe *in, *out;
  BigQ *biq;

  const char *metafName();
  inline const char *tmpfName() const;

  void merge();
  int binarySearch(Record &fetchme, OrderMaker &queryorder, Record &literal, OrderMaker &cnforder, ComparisonEngine &cmp);

  bool useMem;
  void allocMem();
  void freeMem();
  void assignTable(char *fPath);
  void createHelper(char *fpath, void *startup);
  void openHelper(char *fpath);
  void closeHelper();

  void createQ()
  {
    in = new Pipe(PIPE_BUFFER_SIZE), out = new Pipe(PIPE_BUFFER_SIZE);
    biq = new BigQ(*in, *out, *myOrder, runLength);
  }

  void deleteQ()
  {
    delete biq;
    delete in;
    delete out;
    biq = NULL, in = out = NULL;
  }

  SortedFile(const SortedFile &);
  SortedFile &operator=(const SortedFile &);
};

const char *SortedFile::tmpfName() const
{
  return (tpath + ".tmp").c_str();
}

inline void SortedFile::startRead()
{
  if (mode != READ)
  {
    mode = READ;
    merge();
  }
}

inline void SortedFile::startWrite()
{
  if (mode != WRITE)
  {
    mode = WRITE;
    createQ();
  }
}

#endif
