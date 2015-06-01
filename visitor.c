/********************************************************************/
/********************************************************************/
/*Filename 	:  visitor.c                                        */
/*Description	:  This program implements the functionality of     */
/*                 synchronization using semaphores.                */
/*Input         :  Arg1 - input.txt (file with sequence of visitors)*/
/*                 Arg2 - time (Time that each visitor has to wait  */
/*                        in the house(critical section)            */ 
/*Output	:  Returns the order of execution of visitors.      */
/*Note:         :  This code handles deadlock, synchronization,     */
/*                 starvation.                                      */   
/*Refernces	: http://www.yolinux.com/TUTORIALS/                 */ 
/*                LinuxTutorialPosixThreads.html                    */
/*                https://computing.llnl.gov/tutorials/pthreads/    */
/*                                                                  */
/*                                                                  */ 
/********************************************************************/
/********************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

/********Initializing the queue size to 100*******************/
#define MAX_Q_SIZE 100          

/********Declaration of semaphore locks and variables*********/
sem_t GlobLock;

sem_t LeftSema;
int   LeftQ[MAX_Q_SIZE];
int   LeftQSize;

sem_t RightSema;
int   RightQ[MAX_Q_SIZE];
int   RightQSize;

sem_t HouseSema;

/*****************End of Declaration Section****************************************/

/****************Initialization of Global variables********************************/

int   HouseQ[3];
int   HouseQSize;
int   HouseDirection;
int   prevexitid=0;
int   h;
int   waitflag;
int   arr_time;

/**************End of declaration section of Global Variables************************/


/*****************Start of user defined functions************************************/


/*********Visitor waiting line function: In this section, the new arrived threads*******/
/********* or visitor will wait in this section until the before thread is processed***/

void WaitInLine(int direc, int id)
{

//Lock has been applied if any visitor waiting before to the newly arrived visitor
  
  sem_wait(&GlobLock);
  {
//Check the entry direction of the visitor is from the left door (here direc = -1 means left). 
    if (direc == -1)
    {
//Check if the current thread is at the start of the queue else apply lock until the previous thread has been entered the house.
	if(LeftQ[0]!=id)
	{
		printf("Visitor %d: Waiting...\n",id);
	}
      	while (LeftQ[0] != id)
      	{
        sem_post(&GlobLock);
        sem_wait(&GlobLock);
      	}
    }
//Check if the entry direction is from right door(here direc = 1 means right).
    else if (direc == +1)
    {
//Check if the current visitor is at the start the queue at right door else apply lock and wait until the previous visitor has entered. 
	if(RightQ[0]!=id)
	{
		printf("Visitor %d: Waiting...\n",id);
	}     
 	while (RightQ[0] != id)
      	{
        sem_post(&GlobLock);
        sem_wait(&GlobLock);
      	}    
    }
    else
      abort();
  }
  sem_post(&GlobLock);
/***********At this point we are at the head of the respective queue and ready for crossing/entering the house*************/
}


/*****************This is the Crossing or Entry section for the house***************************************/
/*****************This section handles the following scenarios *********************************************/
/***************** 1.The order of crossing of visitors should be same as the entry sequence*****************/
/***************** 2.Handled starvation.********************************************************************/
/***************** 3.Prevented Deadlock*********************************************************************/ 

void CrossHouse(int direc, int id)
{
waitflag=0;

/*******************************Intial lock for each newly arrived visitor***********************/
  sem_wait(&GlobLock);
  {
//Check if the house is full i.e., house size = 3 or if the visitor arrives in opposite direction, apply lock

	if(3 <= HouseQSize || HouseDirection == -direc)
	{
		printf("Visitor %d: Waiting...\n",id);
		h++;
	}
	if(h!=0)
	{
		if(id!=(prevexitid+1)|| HouseDirection == -direc)
		{
		printf("");
		//printf("In wait flag.....\n");
		waitflag=1;
		}
		else
		{
		waitflag==0;
		}
	}
	while(waitflag==1)
	{
	sem_post(&GlobLock);
	sem_wait(&GlobLock);
	}
	while((waitflag==0)&&(id!=(prevexitid+1)))
	{
	sem_post(&GlobLock);
	sem_wait(&GlobLock);
	}
	while (3 <= HouseQSize || HouseDirection == -direc)
	{
      	sem_post(&GlobLock);
      	sem_wait(&GlobLock);
    	}

//Once the lock is being released, update the previous house direction variable and previous thread id
    HouseDirection = direc;
    prevexitid=id;   
    HouseQ[HouseQSize++] = id;

//Check if the entry direction is left or right, then grant access to the visitor accordingly and print the output
   if(direc == -1)
	{
    	printf("Visitor %d: Cross house request granted(Current crossing: left to right, Number of visitors on house: %d)\n", id,HouseQSize );
	}
   else
	{
        printf("Visitor %d: Cross house request granted(Current crossing: right to left, Number of visitors on house: %d)\n", id,HouseQSize );
	}

//Once the visitor has entered the house,then update the appropriate queue by removing that visitor.

    if (direc == -1)
    {
      memmove(LeftQ, LeftQ + 1, --LeftQSize * sizeof(int));
      sem_post(&LeftSema);
    }
    else
    {
      memmove(RightQ, RightQ + 1, --RightQSize * sizeof(int));
      sem_post(&RightSema);
    }
  } 
//Using post to increment the semaphore lock
 sem_post(&GlobLock);

//Specify sleep time so that each visitor would stay in the house for that time.
  usleep(arr_time);    
}

/**************************End of crossing the house section*************************************************/

/*************************Start of visitor exiting the house section*****************************************/
/*************************Here the visitor entered in one direction should leave in the opposite direction***/
/*************************If a visitor enters from left, should exit from right and visitor at right should**/
/*************************should wait until left visitor in the house exits**********************************/

void ExitHouse(int direc, int id)
{
  sem_wait(&GlobLock);
  {
//Check the order of entrance is preserved and should exit the house in the same order,else lock

    while (HouseQ[0] != id)
    {
      sem_post(&GlobLock);
      sem_wait(&GlobLock);
    }

//Once the visitor exits the house, update the housesize so that next visitor could enter

    memmove(HouseQ, HouseQ + 1, --HouseQSize * sizeof(int));

   if(direc == -1)
	{
 	printf("Visitor %d: Exit house (Current crossing: left to right, Number of visitors in the house: %d)\n",id,HouseQSize);
	}
   else
	{
 	printf("Visitor %d: Exit house (Current crossing: right to left, Number of visitors in the house: %d)\n",id,HouseQSize);
	}
   if (0 == HouseQSize)
	{      
	HouseDirection = 0;
	waitflag=0;
	}
  
  }
  sem_post(&GlobLock);
}
/*********************End of exit house section*******************************************************/

/***************Start of controller function**********************************************************/
void *OneVisitor(void *id_and_direc_vptr)
{
  int id    = ((int*) id_and_direc_vptr)[0];
  int direc = ((int*) id_and_direc_vptr)[1];

  free(id_and_direc_vptr);
  WaitInLine(direc, id);
  CrossHouse(direc, id);
  ExitHouse(direc, id);

  return NULL;
}

/*********************End of Controller section******************************************************/

/**********************Start of Main Function*******************************************************/
int main(int argc, char **argv)
{

int  time=atoi(argv[2]);
int  visitorCnter = 0;
char seq[256];
int  m,k,j=0,c;
int  n=0;

FILE *input;

if(argc==3)
{
//printf("the first section**********\n");
/**********Converting from seconds to microseconds as we have used usleep()************************/
arr_time=time*1000000;
input=fopen(argv[1],"r");

/*******************Initializing semaphore locks**************************************************/
//printf("****************enterd into main section****************\n");
  sem_init(&GlobLock, 0, 1);            
  sem_init(&LeftSema, 0, MAX_Q_SIZE);
  sem_init(&RightSema, 0, MAX_Q_SIZE);
  sem_init(&HouseSema, 0, 3);

/*******************Processing the input file and storing the sequence of visitors in array******/
while((c=fgetc(input))!=EOF)
{
if((char)c !=',')  
seq[n++]=(char)c;
}

//pthread_t pth[n-1]

//Initialize the thread variable

pthread_t pth;

//Repeat the process for all the sequence of visitors in the file.
//printf("Before for loop\n");
for (int i = 0; i <= (n-1); ++i)
    {
      if (seq[i] != 'L' && seq[i] != 'R')
        continue;
//dynamically allocating the memory for array to store the visitor id and direction
    
      int *id_and_direc = malloc(2 * sizeof(int));
//Check if the memory is being allocated properly or not
      if (NULL == id_and_direc)
      {
        perror("malloc failed!");
        return 1;
      }
//For each input visitor , assign a visitor id in the order of arrival

      id_and_direc[0] = ++visitorCnter;

      if (seq[i] == 'L')
      {
//If the direction of visitor is from left, assign the direction value as '-1'.
        id_and_direc[1] = -1;

        sem_wait(&LeftSema);
        sem_wait(&GlobLock);
        {
//Update the leftq if the direction of the visitor is from left.
          LeftQ[LeftQSize++] = id_and_direc[0];
        }
        sem_post(&GlobLock);
 printf("Visitor %d: Request to cross the house (left to right)\n",id_and_direc[0]);
     
 }
      else
      {
//If the direction is from right, then update the direction value as '+1'.
        id_and_direc[1] = +1;

        sem_wait(&RightSema);
        sem_wait(&GlobLock);
        {
//Update the rightq with the newly arrived visitor id
          RightQ[RightQSize++] = id_and_direc[0];
        }
        sem_post(&GlobLock);
 printf("Visitor %d: Request to cross the house(right to left)\n",id_and_direc[0]);

      }

// Create threads for each visitor to process further

      if (pthread_create(&pth, NULL, OneVisitor, id_and_direc) || 
          pthread_detach(pth))
      {
//Check if the thread has been created properly or not
        perror("thread creation failed!");
        return 1;
      }
usleep((unsigned long)(1e6*rand()/RAND_MAX));
    }
/************************Wait until all the threads execution is being completed and then exit the main function**************/
//for(int j=0; j<=(n-1);j++)
//{
//pthread_join(pth[j],NULL);  
//}
usleep(120000000);
  printf("End of Main Program !\n");
return 0;
}
/******************Error handling section if the number of arguments is specified incorrectly*********************************/
else
{
printf("Invalid number of arguments,please provide input file name and time as inputs\n");
}
}
/**************************************End of the program********************************************************************/
