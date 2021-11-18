    

#define NO_OF_DRAWER 100
#define NO_OF_PRISONERS 100

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


//drawers with individual locks
typedef struct d
{
   int num;
   pthread_mutex_t mutex;
} drawer_t;

//initialize threads(prisoners)
pthread_t prisoner[NO_OF_PRISONERS];

//initialize drawers
drawer_t drawer[NO_OF_DRAWER];
  

//for my_rand() function
static unsigned long random_ = 1;
//to count number of successful searches in random strategy and no. of wins 
static unsigned long int success_r = 0, win_r = 0;
//to keep count of successful searches in sequential strategy and no. of wins
static unsigned long int success_s = 0, win_s = 0;

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;


//HELPER FUNCTIONS 
int my_rand(void);
void rand_gen(drawer_t draw[NO_OF_DRAWER]); //my random num generator
void my_srand(unsigned seed);
void random_drawer(int n, drawer_t draw[NO_OF_DRAWER]);
void* random_global(void * arg);
void sequential_drawer(int n, drawer_t draw[NO_OF_DRAWER]);
void* sequential_global(void * arg);
//RUN_THREADS FUNCTION
void run_threads(int n, void* (*proc) (void *));
//TIME CALCULATION FUNCTION
static double timeit(int n, void* (*proc)(void *) );


//<MAIN>
int main(int argc, char* argv[])
{
    int games = 0;
    char ch;

    //parsing command line argument
    while((ch = getopt(argc, argv, "n:s")) != EOF)
    {
        switch(ch) 
        {
            case 'n':
                games = atoi(optarg);
                break;
            case 's': 
                games = 100;
                my_srand(time(NULL)); //seed non-static value
                break;
            default:
                printf("Format of command is: ./prisoner [-s] [-n] [..ARGUMENT..] \n");
                return EXIT_FAILURE;
        }
    }

    if(argc == 1) //default
        games = 100;
    
//SIMULATE "games" NUMBER OF GAMES

    //simulate random strategy
    static double rand_time = 0;
    int s = 0;  //to keep track of successful searches per game

    for(int d = 1; d <= games; d++)
    {
        rand_gen(drawer);

        //initialize each drawer lock
        for(int i = 0; i < NO_OF_DRAWER; i++)
            pthread_mutex_init(& drawer[i].mutex, NULL); 

        rand_time += timeit(NO_OF_PRISONERS, random_global); 
       
        s = success_r - s;
        if(s == 100)  //if all prisoners find their number, the game is won
            win_r += 1;

        s = success_r;
    }

    //simulate sequential strategy
    static double seq_time = 0;
    int suc = 0;//to keep track of successful searches per game

    for(int d = 1; d <= games; d++)
    {
        rand_gen(drawer);

        //initialize each drawer lock
        for(int i = 0; i < NO_OF_DRAWER; i++)
            pthread_mutex_init(& drawer[i].mutex, NULL); 

        seq_time += timeit(NO_OF_PRISONERS, sequential_global);

        suc = success_s - suc;
        
        if(suc == 100)
            win_s ++;  
        
        suc = success_s;
    }

    printf("Method random_global:  wins = %ld/%d   time =  %lf ms\n", win_r, games, rand_time);
    printf("Method sequential_global:   wins = %ld/%d  time = %lf ms\n", win_s, games, seq_time);

    return EXIT_SUCCESS;
}

//END OF MAIN

int my_rand(void)
{
    random_ = random_ * 1103515245 + 12345;
    return((unsigned)(random_/65536) % 32768);
}

void my_srand(unsigned seed) 
{
    random_ = seed;
}


void rand_gen(drawer_t draw[NO_OF_DRAWER])
{
    int n = NO_OF_DRAWER;

    //fill in array from 1 to n
    for(int i=0; i<n; ++i)
        draw[i].num = i+1;

    //then permutation of 1..n
    for (int i = n-1; i >= 0; --i)
    {
        my_srand(time(NULL));
        //generate a random number between 0 and n-1
        int j = my_rand() % (i+1);

        //swap the last element with element at random index 'j'
        int temp = draw[i].num;
        draw[i].num = draw[j].num;
        draw[j].num = temp;
    }
    return;
}


void random_drawer(int n, drawer_t draw[NO_OF_DRAWER])
{
    int count = 1;
    while(count <= 50)  //prisoner can open at most 50 drawers
    {
        int idx = my_rand() % 100;  //number between 0 and 99
        
        pthread_mutex_lock(& draw[idx].mutex);
        int current = draw[idx].num;

        if(n+1 == current)
        {
            ++success_r ;
            pthread_mutex_unlock(& draw[idx].mutex);
            break;
        }

        count++;
        pthread_mutex_unlock(& draw[idx].mutex);
    }

    /*if (count == 51)
        printf("Prisoner %d: Unsuccessful Search\n", n);
    */
    return;
}


void* random_global(void * arg)
{
    int index = *(int *) arg;

    //a prisoner gets into the room full of drawers
    int rc = pthread_mutex_lock(&global_mutex);
    if(rc)
    {
        fprintf(stderr, "pthread_mutex_lock() failed\n");
        exit(EXIT_FAILURE);
    }

    //then the prisoner begins to search drawer by drawer
    random_drawer(index, drawer);

    pthread_mutex_unlock(&global_mutex);
    free(arg);

    return (void *)1;
}

void sequential_drawer(int n, drawer_t draw[NO_OF_DRAWER])
{
    int count = 1;
    int idx = n; 
    
    while (count <= 50)
    {
        pthread_mutex_lock(& draw[idx].mutex);
        int current = draw[idx].num;

        if(n+1 == current) //coz array indexing is from 0
        {                 // but numbers are from 1 to 100
            ++success_s;
            pthread_mutex_unlock(& draw[idx].mutex);
            break;
        }

        pthread_mutex_unlock(& draw[idx].mutex);

        idx = current-1;
        count++;    
    }
    
    /*if(count == 51)
        printf("Prisoner %d: Unsuccessful Search\n", n);
    */
    return;
}


void* sequential_global(void * arg)
{
    
    int index = *(int *) arg;

    int rc = pthread_mutex_lock(&global_mutex);
    if(rc)
    {
        fprintf(stderr, "pthread_mutex_lock() failed\n");
        exit(EXIT_FAILURE);
    }

    sequential_drawer(index, drawer);

    pthread_mutex_unlock(&global_mutex);
    free(arg);
    return (void *)1;
}

//RUN_THREADS FUNCTION
void run_threads(int n, void* (*proc) (void *))
{
    unsigned int i;
    for(i = 0; i < n; i++)
    {
        //variable 'a' to store the number of each prisoner
        int* a = malloc(sizeof(int));
        *a = i;

        int rc = pthread_create(&prisoner[i], NULL, proc, a);

        if (rc) 
        {
            fprintf(stderr, "Error in pthread_create() \n");
        }
    }

    //join threads
    for (i = 0; i < n; i++) 
    {
        if (prisoner[i]) 
        {
            int rc = pthread_join(prisoner[i], NULL);
            if (rc) 
            {
                fprintf(stderr, "Error in pthread_join() \n");
            }
        }
    }
}


//TIME CALCULATION FUNCTION
static double timeit(int n, void* (*proc)(void *))
{
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc);
    t2 = clock();
    return ((double) t2 - (double) t1) / CLOCKS_PER_SEC * 1000;
}
