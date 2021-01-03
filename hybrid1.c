// Hybrid-1(R)
// a.exe R [random]
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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

int main(int argc, char **argv)
{
    //short state[NUM_PROC];
    int x[NUM_PROC];
    int vote[NUM_PROC];
    bool voteToSend[NUM_PROC];
    int ts[NUM_PROC];

    // set parameter R
    int _R;
#if defined(CBMC)
     __CPROVER_assume((0 <= _R) && (_R) <= NUM_PROC/4));
#else
    // Set random seed
    if (argc > 1)
    {
        _R = atoi(argv[1]);
    }
    else
    {
        _R = 0; // default
    }
#endif

#if defined(CBMC)
     // do nothing
#else
    // Set random seed
    if (argc > 2)
    {
        srand(atoi(argv[2]));
    }
    else
    {
        srand((unsigned)time(NULL));
    }
#endif

    printf("R=%d\n", _R);
     // Set proposed values randomly
     // 1, 2, ..., NUM_PROC
     for (int proc = 0; proc < NUM_PROC; proc++)
     {
         x[proc] = _randint(1, NUM_PROC);
         vote[proc] = -1;
         voteToSend[proc] = false;
         ts[proc] = 0;
     }

     // Iterate rounds MAX_ROUND times
     int round = 1;
     for (int phase = 1; round <= MAX_ROUND; phase++)
     {
         int coord[NUM_PROC];
         for (int proc = 0; proc < NUM_PROC; proc++)
         {
             coord[proc] = _randint(0, NUM_PROC - 1);
         }
         // start round % 3 = 1
         {
             // channels
             struct _chnl
             {
                 int x;
                 int ts;
             } chnl[NUM_PROC][NUM_PROC];
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
                     if (_randint(0, 1) == 0)
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
                 // early decision
                 if ((phase == 1) && (count >= NUM_PROC - _R))
                 {
                     // select a value occurring >= NUM_PROC - R times
                     for (int i = 0; i < NUM_PROC; i++)
                     {
                         if (ch[i].x == -1)
                         {
                             continue;
                         }
                         int val = ch[i].x;
                         int count = 1;
                         for (int j = i + 1; j < NUM_PROC; j++)
                         {
                             if (ch[j].x == val)
                                 count++;
                         }
                         if (count >= NUM_PROC - _R)
                         {
                             decision[proc] = val;
                             break;
                         }
                     }
                 }

                 // check if more than max(n/2, 2*_R) messages are received
                 if (proc == coord[proc] && count > HALF && count > 2 * _R)
                 {
                     // Are the messages received, except at most _R, equal to <v, 0>?
                     bool areEqual = false;
                     int val = 0;
                     for (int i = 0; i < NUM_PROC; i++)
                     {
                         if ((ch[i].x == -1) || (ch[i].ts != 0))
                         {
                             continue;
                         }
                         int tmpval = ch[i].x;
                         int count1 = 1;
                         for (int j = i + 1; j < NUM_PROC; j++)
                         {
                             if ((ch[j].x == tmpval) && (ch[j].ts == 0))
                                 count1++;
                         }
                         if (count1 >= count - _R)
                         {
                             val = tmpval;
                             break;
                         }
                     }

                     if (areEqual)
                     {
                         vote[proc] = val;
                     }
                     else
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
                     }
                     voteToSend[proc] = true;
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
             } chnl[NUM_PROC][NUM_PROC];
             // send
             for (int proc = 0; proc < NUM_PROC; proc++)
             {
                 for (int peer = 0; peer < NUM_PROC; peer++)
                 {
                     if ((proc == coord[proc]) && (voteToSend[proc] == true))
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
                     if (_randint(0, 1) == 0)
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
             } chnl[NUM_PROC][NUM_PROC];
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
                     if (_randint(0, 1) == 0)
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
                 voteToSend[proc] = false;
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
