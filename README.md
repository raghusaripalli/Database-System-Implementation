## Database-System-Implementation

Implementation of a Database System in C++. Divided into 5 parts last part contains the whole project.

### Instructions to Compile

1. Update catalog_path, dbfile_dir and tpch_dir in main.cc 
2. Open the terminal in project folder (A5) and run the below commands:
  - make -f MakeFile.main clean
  - make -f MakeFile.main
  - ./a5.out
  - Enter the CNF to see the desired output.
3. For gTests:
   - cmake .
   - make
   - ./GTest
### Methods: 
- A new table is created if a table does not exist already.
- The file type is taken as Sorted if the attributes are sorted or else heap type is considered.
- Attributes are iterated and their names and types are stored in an output file.
- Attributes are copied into a new list and a new schema is generated.
- A new table is created which is sorted based on the attributes list given in the input.
- In insertInfo(), method the table is opened and the input file is stored into the table.
- dropTable() function deletes the table from the system and its corresponding binary file.

