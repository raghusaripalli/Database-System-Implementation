#include <fstream>
#include <stdio.h>

#include "Errors.h"
#include "SortedFile.h"
#include "HeapFile.h"

#define safeDelete(p) \
  {                   \
    delete p;         \
    p = NULL;         \
  }

using std::ifstream;
using std::ofstream;
using std::string;

extern char *catalog_path;
extern char *dbfile_dir;
extern char *tpch_dir;

int SortedFile::Create(char *fpath, void *startup)
{
  if (fpath == NULL || startup == NULL)
    return -1;

  assignTable(fpath);
  typedef struct
  {
    OrderMaker *o;
    int l;
  } * pOrder;
  pOrder po = (pOrder)startup;
  myOrder = po->o;
  runLength = po->l;
  return GenericDBFile::Create(fpath, startup);
}

int SortedFile::Open(char *fpath)
{
  if (fpath == NULL)
    return -1;
  
  allocMem();
  assignTable(fpath);

  int ftype;
  ifstream ifs(metafName());
  FATALIF(!ifs, "Meta file missing.");
  ifs >> ftype >> *myOrder >> runLength;
  ifs.close();
  return GenericDBFile::Open(fpath);
}

int SortedFile::Close()
{ 
  ofstream ofs(metafName());
  ofs << "1\n"
      << *myOrder << '\n'
      << runLength << std::endl;
  ofs.close();
  if (mode == WRITE)
    merge();
  freeMem();
  return theFile.Close();
}

void SortedFile::Add(Record &addme)
{
  startWrite();
  in->Insert(&addme);
}

void SortedFile::Load(Schema &myschema, char *loadpath)
{
  startWrite();
  GenericDBFile::Load(myschema, loadpath);
}

void SortedFile::MoveFirst()
{
  startRead();
  theFile.GetPage(&curPage, curPageIdx = 0);
}

int SortedFile::GetNext(Record &fetchme)
{
  return GenericDBFile::GetNext(fetchme);
}

int SortedFile::GetNext(Record &fetchme, CNF &cnf, Record &literal)
{
  OrderMaker queryorder, cnforder;
  OrderMaker::queryOrderMaker(*myOrder, cnf, queryorder, cnforder);
  ComparisonEngine cmp;
  if (!binarySearch(fetchme, queryorder, literal, cnforder, cmp))
    return 0; // query part should equal
  do
  {
    if (cmp.Compare(&fetchme, &queryorder, &literal, &cnforder))
      return 0; // query part should equal
    if (cmp.Compare(&fetchme, &literal, &cnf))
      return 1; // matched
  } while (GetNext(fetchme));
  return 0; // no matching records
}

void SortedFile::merge()
{
  in->ShutDown();
  Record fromFile, fromPipe;
  bool fileNotEmpty = !theFile.empty(), pipeNotEmpty = out->Remove(&fromPipe);

  HeapFile tmp;
  tmp.Create(const_cast<char *>(tmpfName()), NULL);
  ComparisonEngine ce;

  // initialize
  if (fileNotEmpty)
  {
    theFile.GetPage(&curPage, curPageIdx = 0);
    fileNotEmpty = GetNext(fromFile);
  }

  // two-way merge
  while (fileNotEmpty || pipeNotEmpty)
    if (!fileNotEmpty || (pipeNotEmpty && ce.Compare(&fromFile, &fromPipe, myOrder) > 0))
    {
      tmp.Add(fromPipe);
      pipeNotEmpty = out->Remove(&fromPipe);
    }
    else if (!pipeNotEmpty || (fileNotEmpty && ce.Compare(&fromFile, &fromPipe, myOrder) <= 0))
    {
      tmp.Add(fromFile);
      fileNotEmpty = GetNext(fromFile);
    }
    else
      FATAL("Two-way merge failed.");

  // write back
  tmp.Close();
  FATALIF(rename(tmpfName(), tpath.c_str()), "Merge write failed."); // rename returns 0 on success
  deleteQ();
}

int SortedFile::binarySearch(Record &fetchme, OrderMaker &queryorder, Record &literal, OrderMaker &cnforder, ComparisonEngine &cmp)
{
  if (!GetNext(fetchme))
    return 0;
  int result = cmp.Compare(&fetchme, &queryorder, &literal, &cnforder);
  if (result > 0)
    return 0;
  else if (result == 0)
    return 1;

  off_t low = curPageIdx, high = theFile.lastIndex(), mid = (low + high) / 2;
  for (; low < mid; mid = (low + high) / 2)
  {
    theFile.GetPage(&curPage, mid);
    FATALIF(!GetNext(fetchme), "empty page found");
    result = cmp.Compare(&fetchme, &queryorder, &literal, &cnforder);
    if (result < 0)
      low = mid;
    else if (result > 0)
      high = mid - 1;
    else
      high = mid;
  }

  theFile.GetPage(&curPage, low);
  do
  {
    if (!GetNext(fetchme))
      return 0;
    result = cmp.Compare(&fetchme, &queryorder, &literal, &cnforder);
  } while (result < 0);
  return result == 0;
}

const char *SortedFile::metafName() const
{
  std::string p(dbfile_dir);
  return (p + table + ".meta").c_str();
}

void SortedFile::allocMem()
{
  FATALIF(myOrder != NULL, "File already open.");
  myOrder = new OrderMaker();
  useMem = true;
}

void SortedFile::assignTable(char *fPath)
{
  table = getTableName((tpath = fPath).c_str());
}

void SortedFile::freeMem()
{
  if (useMem)
    safeDelete(myOrder);
  useMem = false;
}
