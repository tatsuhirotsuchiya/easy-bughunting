# easy bug hunting for distributed algorithms

I'm thinking about an easy-to-use approach to debugging of (fault-tolerant) distributed algorithms.
In the approach, the user (the person who debugs an algorithm) writes a sequential C program that simulates the behavior of the algorithm under debugging. Ordinary testing/debugging tools for C programs can be used to write a correct program that faithfully represents the algorithm. Once the user obtains the confidence that the program is correct (in the sense that it correctly represents the algorithm), the user can use symbolic testing techniques, such as bounded model checking or symbolic execution, to find bugs in the algorithm.

## How to apply CBMC on Windows

- Download CBMC binary from [CBMC homepage](https://www.cprover.org/cbmc/)
- Use Developer Command Prompt for VS to run the CBMC
 + ``cbmc -DCBMC -DNUM_PROC=5 -DMAX_ROUND=5 filename.c``
- CBMC uses Visual Studio's cl.exe as the compiler
- Use --trace option to show counterexample traces
