cmake_minimum_required(VERSION 2.6)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


# a1 project - main
add_executable(GTest gtest.cc Schema.cc Record.cc Comparison.cc ComparisonEngine.cc File.cc DBFile.cc HeapFile.cc Pipe.cc BigQ.cc ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})
target_link_libraries(GTest ${GTEST_LIBRARIES} pthread)