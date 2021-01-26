#ifndef HEAP_H_
#define HEAP_H_

#include "DBFile.h"

class Heap: public DBFile {
public:
	Heap();
	~Heap();

	int Close ();
	void Add (Record& me);
};

#endif