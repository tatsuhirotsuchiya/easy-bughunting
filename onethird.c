#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define NUM_PROC 4
#define MAX_ROUND 10
#define TWOTHIRD ((int) (2*NUM_PROC/3))

int main() {
    int round;
    //short state[NUM_PROC];
    int state[NUM_PROC] = {1, 2, 4, 3};
    for (round = 1; round <= MAX_ROUND; round++) {
        int chnl[NUM_PROC][NUM_PROC]; 
        int proc;
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
            int i, count = NUM_PROC;
            for (i = 0; i < NUM_PROC; i++) {
                if (rand() % NUM_PROC == 0) {
                    ch[i] = -1; count--;
                    if (count - 1 <= TWOTHIRD)
                        break;
                }
            }

            for (i = 0; i < NUM_PROC; i++) {
                printf("%d..", ch[i]);
            }
            printf("\n");
        }
        // receive
        for (proc = 0; proc < NUM_PROC; proc++) {
            int *ch = chnl[proc];
            // select smallest most often occurring value
            int max_count = 0, cand_val = 0, i;
            for (i = 0; i < NUM_PROC; i++) {
                int val = ch[i];
                int j, count = 1;
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
        }

    }
}