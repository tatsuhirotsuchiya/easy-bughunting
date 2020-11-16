// OneThirdRule

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define NUM_PROC 4
#define MAX_ROUND 10
#define TWOTHIRD ((int) (2*NUM_PROC/3)) 

int decision[NUM_PROC];

int main(int argc, char **argv) {
    int round = 1;
    //short state[NUM_PROC];
    int state[NUM_PROC];
    // propose
    int proc = 0; 

    if (argc > 1) {
        srand(atoi(argv[1]));
    }
    for (proc = 0; proc < NUM_PROC; proc++) {
        state[proc] = rand() % NUM_PROC + 1;
    }

    for (round = 1; round <= MAX_ROUND; round++) {
        int chnl[NUM_PROC][NUM_PROC]; 
        // send
        for (proc = 0; proc < NUM_PROC; proc++) {
            int peer = 0;
            for (peer = 0; peer < NUM_PROC; peer++) {
                chnl[peer][proc] = state[proc];
            } 
        }
        // HO
        for (proc = 0; proc < NUM_PROC; proc++) {
            int *ch = chnl[proc];
            int i = 0;
            for (i = 0; i < NUM_PROC; i++) {
                if (rand() % 2 == 0) {
                    ch[i] = -1; 
                }
            }
            // print out
            for (i = 0; i < NUM_PROC; i++) {
                printf("%d..", ch[i]);
            }
            printf("\n");
        }
        // receive
        for (proc = 0; proc < NUM_PROC; proc++) {
            int *ch = chnl[proc];
            // check if more than 2n/3 messages are received
            int i = 0, count = 0;
            for (i = 0; i < NUM_PROC; i++) {
                if (ch[i] >= 0) {
                    count++;
                }
            }
            if (count > TWOTHIRD) {
                // select smallest most often occurring value
                int max_count = 0, cand_val = 0;
                for (i = 0; i < NUM_PROC; i++) {
                    int val = ch[i];
                    int j, count = 1;
                    if (val == -1) {
                        continue;
                    }
                    for (j = i + 1; j < NUM_PROC; j++) {
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
            int i = 0;
            int val = 0;
            for (i = 0; i < NUM_PROC; i++) {
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