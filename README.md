# easy-bughunting

I'm thinking about an easy-to-use approach to debugging of (fault-tolerant) distributed algorithms.
In the approach, the user (the person who debugs an algorithm) writes a sequential C program that simulates the behavior of the algorithm under debugging. Ordinary testing/debugging tools for C progrmas can be used to write a correct program that faithfully represents the algorithm. Once the user obtains the confidence that the program is correct (in the sense that it correctly represents the algorithm), symbolic testing techniques, such as bounded model checking or symbolic execution, are used to find bugs in the algorithm. 
