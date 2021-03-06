// OneThirdRule
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Number of processes
#if !defined(NUM_PROC)
#define NUM_PROC 3
#endif
// Number of rounds
#if !defined(MAX_ROUND)
#define MAX_ROUND 10
#endif

#define TWOTHIRD ((int)(2 * NUM_PROC / 3))

int _randint(int first, int last)
{
    int tmp;
#if defined(CBMC)
    __CPROVER_assume((first <= tmp) && (tmp <= last));
#else
    tmp = rand() % (last + 1 - first) + first;
#endif
    return tmp;
}

int decision[NUM_PROC];

int firstdecision = 0;
void decide(int proc, int value) 
{
    printf("proc: %d decides %d\n.", proc, value);
    decision[proc] = value;
    if (firstdecision == 0)
    {
        firstdecision = value;
    }
    else
    {
        assert(firstdecision == value);
    }
}

int main(int argc, char **argv)
{
    //short state[NUM_PROC];
    int state[NUM_PROC];

#if defined(CBMC)
    // do nothing
#else
    // Set random seed
    if (argc > 1)
    {
        srand(atoi(argv[1]));
    }
    else
    {
        srand((unsigned)time(NULL));
    }
#endif

    // Set proposed values randomly
    for (int proc = 0; proc < NUM_PROC; proc++)
    {
        state[proc] = _randint(1, NUM_PROC);
    }

    // Iterate rounds MAX_ROUND times
    for (int round = 1; round <= MAX_ROUND; round++)
    {
        // channels
        int chnl[NUM_PROC][NUM_PROC];
        // send
        for (int proc = 0; proc < NUM_PROC; proc++)
        {
            for (int peer = 0; peer < NUM_PROC; peer++)
            {
                chnl[peer][proc] = state[proc];
            }
        }
        // HO
        for (int proc = 0; proc < NUM_PROC; proc++)
        {
            int *ch = chnl[proc];
            for (int i = 0; i < NUM_PROC; i++)
            {
                if (_randint(0, 1) == 0)
                {
                    ch[i] = -1;
                }
            }
            // print out
            for (int i = 0; i < NUM_PROC; i++)
            {
                printf("%d..", ch[i]);
            }
            printf("\n");
        }
        // receive
        for (int proc = 0; proc < NUM_PROC; proc++)
        {
            int *ch = chnl[proc];
            // check if more than 2n/3 messages are received
            int count = 0;
            for (int i = 0; i < NUM_PROC; i++)
            {
                if (ch[i] >= 0)
                {
                    count++;
                }
            }
            if (count > TWOTHIRD)
            {
                // select smallest most often occurring value
                int max_count = 0, cand_val = 0;
                for (int i = 0; i < NUM_PROC; i++)
                {
                    int val = ch[i];
                    int count = 1;
                    if (val == -1)
                    {
                        continue;
                    }
                    for (int j = i + 1; j < NUM_PROC; j++)
                    {
                        if (ch[j] == val)
                            count++;
                    }
                    if (max_count < count || (max_count == count && val < cand_val))
                    {
                        cand_val = val;
                        max_count = count;
                    }
                }
                printf("proc: %d chooses %d\n.", proc, cand_val);
                state[proc] = cand_val;
                if (max_count > TWOTHIRD)
                {
                    decide(proc, cand_val);
                }
            }
        }
    }
}