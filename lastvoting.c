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

#define HALF ((int)(NUM_PROC / 2))
#define TWOTHIRD ((int)(2 * NUM_PROC / 3))

int decision[NUM_PROC];

int main(int argc, char **argv)
{
    //short state[NUM_PROC];
    int x[NUM_PROC];
    int vote[NUM_PROC];
    int voteToSend[NUM_PROC];
    int ts[NUM_PROC];

    // Set random seed
    if (argc > 1)
    {
        srand(atoi(argv[1]));
    }
    else
    {
        srand((unsigned)time(NULL));
    }

    // Set proposed values randomly
    // 1, 2, ..., NUM_PROC
    for (int proc = 0; proc < NUM_PROC; proc++)
    {
        x[proc] = rand() % NUM_PROC + 1;
        vote[proc] = -1;
        voteToSend[proc] = 0;
        ts[proc] = 0;
    }

    // Iterate rounds MAX_ROUND times
    int round = 1;
    for (int phase = 1; round <= MAX_ROUND; phase++)
    {
        int coord[NUM_PROC];
        for (int proc = 0; proc < NUM_PROC; proc++)
        {
            coord[proc] = rand() % NUM_PROC;
        }
        // start round % 3 = 1
        {
            // channels
            struct _chnl
            {
                int x;
                int ts;
            }
            chnl[NUM_PROC][NUM_PROC];
            // send
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                for (int peer = 0; peer < NUM_PROC; peer++)
                {
                    if (peer == coord[proc])
                    {
                        chnl[peer][proc].x = x[proc];
                        chnl[peer][proc].ts = ts[proc];
                    }
                    else
                    {
                        chnl[peer][proc].x = -1;
                        chnl[peer][proc].ts = -1;
                    }
                }
            }
            // message faults
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                for (int i = 0; i < NUM_PROC; i++)
                {
                    if (rand() % 2 == 0)
                    {
                        ch[i].x = -1;
                        ch[i].ts = -1;
                    }
                }
                // print out
                for (int i = 0; i < NUM_PROC; i++)
                {
                    printf("{%d,%d}..", ch[i].x, ch[i].ts);
                }
                printf("\n");
            }
            // receive
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                // count messages
                int count = 0;
                for (int i = 0; i < NUM_PROC; i++)
                {
                    if (ch[i].x >= 0)
                    {
                        count++;
                    }
                }
                // check if more than n/2 messages are received
                if (proc == coord[proc] && count > HALF)
                {
                    // select x with largest ts
                    int max_ts = -1;
                    int cand_x = -1;
                    for (int i = 0; i < NUM_PROC; i++)
                    {
                        if (ch[i].ts > max_ts)
                        {
                            max_ts = ch[i].ts;
                            cand_x = ch[i].x;
                        }
                    }
                    // vote
                    vote[proc] = cand_x;
                    voteToSend[proc] = 1;
                }
            }
            round++;
        } // round end

        //  round % 3 = 2
        {
            // channels
            struct _chnl
            {
                int vote;
            }
            chnl[NUM_PROC][NUM_PROC];
            // send
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                for (int peer = 0; peer < NUM_PROC; peer++)
                {
                    if ((proc == coord[proc]) && (voteToSend[proc] == 1))
                    {
                        chnl[peer][proc].vote = vote[proc];
                    }
                    else
                    {
                        chnl[peer][proc].vote = -1;
                    }
                }
            }
            // message faults
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                for (int i = 0; i < NUM_PROC; i++)
                {
                    if (rand() % 2 == 0)
                    {
                        ch[i].vote = -1;
                    }
                }
                // print out
                for (int i = 0; i < NUM_PROC; i++)
                {
                    printf("{%d}..", ch[i].vote);
                }
                printf("\n");
            }
            // receive
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                if (ch[coord[proc]].vote != -1)
                {
                    x[proc] = ch[coord[proc]].vote;
                    ts[proc] = phase;
                }
            }
            round++;
        } // end round

        // start round % 3 = 0
        {
            // channels
            struct _chnl
            {
                int ack;
                int x;
            }
            chnl[NUM_PROC][NUM_PROC];
            // send
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                for (int peer = 0; peer < NUM_PROC; peer++)
                {
                    if (ts[proc] == phase)
                    {
                        chnl[peer][proc].ack = 1;
                        chnl[peer][proc].x = x[proc];
                    }
                    else
                    {
                        chnl[peer][proc].ack = -1;
                        chnl[peer][proc].x = -1;
                    }
                }
            }
            // message faults
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                for (int i = 0; i < NUM_PROC; i++)
                {
                    if (rand() % 2 == 0)
                    {
                        ch[i].ack = -1;
                        ch[i].x = -1;
                    }
                }
                // print out
                for (int i = 0; i < NUM_PROC; i++)
                {
                    printf("{%d,%d}..", ch[i].ack, ch[i].x);
                }
                printf("\n");
            }
            // receive
            for (int proc = 0; proc < NUM_PROC; proc++)
            {
                struct _chnl *ch = chnl[proc];
                // count messages
                int count = 0;
                for (int i = 0; i < NUM_PROC; i++)
                {
                    if (ch[i].x >= 0)
                    {
                        count++;
                    }
                }
                // select an x value received n/2 times
                for (int i = 0; i < NUM_PROC; i++)
                {
                    int val = ch[i].x;
                    int count = 1;
                    if (val == -1)
                    {
                        continue;
                    }
                    for (int j = i + 1; j < NUM_PROC; j++)
                    {
                        if (ch[j].x == val)
                            count++;
                    }
                    if (count > HALF)
                    {
                        decision[proc] = val;
                        printf("Pr%d decides %d\n", proc, val);
                        break;
                    }
                }
                voteToSend[proc] = 0;
            }
            round++;
        } // round end
        // safety
        {
            int val = 0;
            for (int i = 0; i < NUM_PROC; i++)
            {
                if (decision[i] == 0)
                {
                    continue;
                }
                else if (val == 0)
                {
                    val = decision[i];
                }
                else
                {
                    assert(val == decision[i]);
                }
            }
        }
    } // phase end
    return 0;
}