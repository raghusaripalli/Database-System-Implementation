#ifndef HEAP_H_
#define HEAP_H_

#include "DBFile.h"

class Heap: public DBFile {
public:
	Heap();
	~Heap();

	int Close ();
	void Add (Record& me);
	void MoveFirst();
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

protected:
	void readMode();
};

inline void Heap::readMode() {
  	if (mode == read) 
		return;
	mode = read;
  	if (!page.is_empty())
	  	file.addPage(&page);
}

#endif