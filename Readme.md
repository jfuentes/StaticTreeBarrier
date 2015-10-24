# Static Tree Barrier
Author: Joel Fuentes - joel.fuentes@uci.edu

The implementation is based on the algorithm described in the book "The Art of Multiprocessor
Programming" plus some adjustments for C++11 with atomic types. I implemented the data structure following the same logic of the Java code presented in Chapter 17.5, this also means the solution is based
on classes and objects. The main changes to port this solution to C++11 were based
on the definition of atomic variables and their related operations.

## Files:
- statictreebarrier.h: definition and implementation of the data structure
- statictreebarrier.cc: Test case
- Makefile: Script to compile the program

## Before compiling:
- Download and build the CDSChecker (http://plrg.eecs.uci.edu/eecs221/lib/exe/fetch.php?media=model-checker.tgz).


## Instruction to compile and execute:
1. Put the source code (folder statictreebarrier) in the CDSChecker directory.
2. Go into the statictreebarrier directory
3. Compile:
   $make
4. Go back to the previous directory:
   $cd ..
5. Execute the CDSChecker with the new data structure:
   $./run.sh statictreebarrier/statictreebarrier -m 2 -f 10 --verbose

Important: To change the number of threads, the macro NUMREADERS needs to be changed in statictreebarrier.cc before compiling.
The implementation was made by considering only complete trees, this means all the nodes except the leaves have r children,
where r is the radix value.
The current number of threads is 3 with a radix of 2. Other values for number of threads are 7, 13, and so on.



## The introduced changes are:
- Definition of two classes StaticTreeBarrier and Node.
- Definition of atomic variables for those that are accessed and modified by different threads.
These are:
```
std::atomic<bool>  thread_sense_ // from the StaticTreeBarrier class
std::atomic<int> child_count_  // from the Node class
```
- For every operation for accessing or modifying the atomic variables, the corresponding
atomic operation is used. For example atomic_int, load, store, etc.
- The synchronizations are possible by the use of memory order options (relase-acquire).
- When a thread waits for passing the barrier a operation thrd_yield() is used in the "while"
structure.
