# External Sort

This is a simple templated header library with my implementation of [External sorting](https://en.wikipedia.org/wiki/External_sorting) procedure for large files containing multi-line objects (with fixed number of lines per object. [Fastq](https://en.wikipedia.org/wiki/FASTQ_format) files are the best example). Objects should be placed in file continuously, without header or empty lines (see sample datasets in test directory).

Since size of multi-line string-represented objects may vary, user is required to provide number of objects per sorting batch instead of raw memory limit. Intermediate results are stored in a folder with randomly-generated name that will be created in working directory and deleted at the end of all computations.

The main class template takes two template parameters: object type T and integer number of lines per object in a file (default 1).
Number of objects per sorting batch (unsigned long) is passed as constructor argument.

In order to be sorted, your objects shold satisfy several criteria (see src/test.cpp file for more concrete example):
* Implement a default constructor and a parameterized constructor;
* Be move-construclable and move-assignable;
* Implement comparison operators < and >;
* Implement operators reading (>>) and writing (<<) chevron operators with any class derived from std::basic_istream.

In sake of compatibility with primitive types like int of float, operator << should not put any trailing newline character ('\n') as it is already handled.

### Usage example
Just include header file in your project.
```
    // initialize sorter object
    // each object of class Record occupies 2 lines in a file
    int ObjsPerBatch = 4;
    ExternalSorter<Record, 2> recordSorter(ObjsPerBatch);

    // sort objects: call operator () with paths to input and output files
    recordSorter("test/records.txt", "test/records.sorted");
```

### Requirements
* g++ 11.3.0 or higher
* {fmt} library