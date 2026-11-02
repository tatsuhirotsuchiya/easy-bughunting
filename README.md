# easy bug hunting for distributed algorithms

I'm thinking about an easy-to-use approach to debugging (fault-tolerant) distributed algorithms. 
In this approach, the user (i.e., the person debugging the algorithm) writes a sequential C program that simulates the behavior of the algorithm being debugged. 
Standard C testing and debugging tools can then be used to ensure that the program correctly represents the algorithm. 
Once the user is confident that the program is a faithful representation of the algorithm, they can apply symbolic testing techniques, such as bounded model checking or symbolic execution, to find bugs in the algorithm.


## How to Run CBMC on Windows

- Download the CBMC binary from the [CBMC homepage](https://www.cprover.org/cbmc/).
- Open the **Developer Command Prompt for Visual Studio** and run CBMC using the following command:

  ```bash
  cbmc -DCBMC -DNUM_PROC=5 -DMAX_ROUND=5 filename.c
  ```

  - `-DNUM_PROC=5`: Sets the number of processes to 5.
  - `-DMAX_ROUND=5`: Sets the maximum number of rounds to 5.

- CBMC uses the C compiler `cl.exe` provided by Visual Studio.
- Use the `--trace` option to display counterexample traces.

## Citation

- Tatsuhiro Tsuchiya, "Sequential programming for distributed algorithm verification", 
in Proceedings of 2024 IEEE 29th Pacific Rim International Symposium on Dependable Computing (PRDC), 
2024, DOI: 10.1109/PRDC63035.2024.00042.
