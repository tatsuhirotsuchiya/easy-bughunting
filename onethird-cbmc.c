// OneThirdRule
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Number of processes
#if !defined (NUM_PROC) 
#define NUM_PROC 3
#endif
// Number of rounds
#if !defined (MAX_ROUND)
#define MAX_ROUND 10
#endif

#define TWOTHIRD ((int) (2*NUM_PROC/3)) 

#define CBMC

int decision[NUM_PROC];

int main(int argc, char **argv) {
    //short state[NUM_PROC];
    int state[NUM_PROC];

#if defined (CBMC)
    // do nothing
#else
    // Set random seed
    if (argc > 1) {
        srand(atoi(argv[1]));
    }
    else {
        srand((unsigned) time(NULL));
    }
#endif

    // Set proposed values randomly 
#if defined (CBMC)
    for (int proc = 0; proc < NUM_PROC; proc++) {
         __CPROVER_assume(0 < state[proc] && state[proc] <= NUM_PROC);
    }
#else
    for (int proc = 0; proc < NUM_PROC; proc++) {
        state[proc] = rand() % NUM_PROC + 1;
    }
#endif

    // Iterate rounds MAX_ROUND times
    for (int round = 1; round <= MAX_ROUND; round++) {
        // channels
        int chnl[NUM_PROC][NUM_PROC]; 
        // send
        for (int proc = 0; proc < NUM_PROC; proc++) {
            for (int peer = 0; peer < NUM_PROC; peer++) {
                chnl[peer][proc] = state[proc];
            } 
        }
        // HO
        for (int proc = 0; proc < NUM_PROC; proc++) {
            int *ch = chnl[proc];
            for (int i = 0; i < NUM_PROC; i++) {
#if defined (CBMC)
                if (nondet_int() % 2 == 0) {
                    ch[i] = -1; 
                }
#else
                if (rand() % 2 == 0) {
                    ch[i] = -1; 
                }
#endif
            }
            // print out
            for (int i = 0; i < NUM_PROC; i++) {
                printf("%d..", ch[i]);
            }
            printf("\n");
        }
        // receive
        for (int proc = 0; proc < NUM_PROC; proc++) {
            int *ch = chnl[proc];
            // check if more than 2n/3 messages are received
            int count = 0;
            for (int i = 0; i < NUM_PROC; i++) {
                if (ch[i] >= 0) {
                    count++;
                }
            }
            if (count > TWOTHIRD) {
                // select smallest most often occurring value
                int max_count = 0, cand_val = 0;
                for (int i = 0; i < NUM_PROC; i++) {
                    int val = ch[i];
                    int count = 1;
                    if (val == -1) {
                        continue;
                    }
                    for (int j = i + 1; j < NUM_PROC; j++) {
                        if (ch[j] == val)
                            count++;
                    }
                    if (max_count < count || (max_count == count && val < cand_val)) {
                        cand_val = val; max_count = count;
                    }
                }
                printf("proc: %d chooses %d\n.", proc, cand_val);
                state[proc] = cand_val;
                if (max_count > TWOTHIRD) {
                    decision[proc] = cand_val;
                    printf("proc: %d decides %d\n.", proc, decision[proc]);
                }
            }
        }
        // safety 
        {
            int val = 0;
            for (int i = 0; i < NUM_PROC; i++) {
                if (decision[i] == 0) {
                    continue;
                }
                else if (val == 0) {
                    val = decision[i];
                }
                else {
                    assert(val == decision[i]);
                }
            }
        }
    }
}