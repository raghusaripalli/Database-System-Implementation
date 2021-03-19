## COP6726 Project
==================
### Database System Implementation

**Members:**

Raghuveer Sharma Saripalli(UFID : 50946752)

Madhuri Uppu(UFID : 43029778)


**Objective:** The goal of this project is to implement Sorted File Database Implementation in part 2.

**Running the project:**
1.	Clone data generator from GitHub and compile it to generate 1GB TPCH data.
2.	Update the table path in test.cc
3.	Open the terminal in project folder and run the below commands :
    - make -f MakeFile.main
    - ./test.out
    - For gTests:
        - cmake .
        - make
        - ./GTest


**Functionalities implemented:**

1. Create a file
2. Load the TPCH data file into sorted file.
3. Scan the sorted file against CNF and fetch the records.
4. Implemented GTests for SortedFile methods.
