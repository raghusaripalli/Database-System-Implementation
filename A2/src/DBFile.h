#ifndef DBFILE_H
#define DBFILE_H

#include <string>

#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

typedef enum {heap, sorted, btree} fType;

class GenericDBFile;

class DBFile {
public:
  DBFile();
  ~DBFile();

  int Create (char* fpath, fType ftype, void* startup);
  int Open (char* fpath);
  int Close ();

  void Add (Record& addme);
  void Load (Schema& myschema, char* loadpath);

  void MoveFirst();
  int GetNext (Record& fetchme);
  int GetNext (Record& fetchme, CNF& cnf, Record& literal);

private:
  GenericDBFile* db;
  
  void createFile(fType ftype);

  DBFile(const DBFile&);
  DBFile& operator=(const DBFile&);
};

// TODO: empty the current page before starting add.
// be careful when switching from read mode to write mode, or vice versa (in heap files)
class GenericDBFile {
  friend class DBFile;
protected:
  GenericDBFile(): mode(READ) {}
  virtual ~GenericDBFile() {};

  virtual int Create (char* fpath, void* startup);
  virtual int Open (char* fpath);
  virtual int Close() = 0;

  virtual void Add (Record& addme) = 0;
  virtual void Load (Schema& myschema, char* loadpath);

  // this function does not deal with spanned records
  virtual void MoveFirst () = 0;
  virtual int GetNext (Record& fetchme);
  virtual int GetNext (Record& fetchme, CNF& cnf, Record& literal) = 0;

  static std::string getTableName(const char* fpath) {
    std::string path(fpath);
    size_t start = path.find_last_of('/'),
           end   = path.find_last_of('.');
    return path.substr(start+1, end-start-1);
  }

  off_t curPageIdx;
  Page curPage;
  File theFile;

protected:
  enum Mode { READ, WRITE } mode;

  virtual void startWrite() = 0;
  virtual void startRead() = 0;

private:
  GenericDBFile(const GenericDBFile&);
  GenericDBFile& operator=(const GenericDBFile&);
};

#endif
