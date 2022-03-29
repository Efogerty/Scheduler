//Evan Fogerty and Josh DeWitt
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <wait.h>

typedef struct task{
  int task_id,    /* alphabetic tid can be obtained as 'A'+(task_counter++) */
    arrival_time,
    service_time,
    remaining_time,
    completion_time,
    response_time,
    wait_time;
  struct task *next;
}task_t;

int main(int argc, char *argv[]){
  //check that args are correct
  if(argc != 4){
    printf("Incorrect number of command line arguments.\n");
    return 0;
  }
  //set our args to variables
  char* function;
  FILE* infile;
  FILE* outfile;
  function = (char*)malloc(sizeof(char*));
  strcpy(function,argv[1]);
  if(!(infile = fopen(argv[2],"r"))){
    printf("infile does not exist.\n");
    return 0;
  }
  outfile = fopen(argv[3],"w");
//  int numTasks = 0;
//  char* line;
  task_t* head_task = (task_t*)malloc(sizeof(task_t));
//  int i = 0;
  while(!feof(infile)){
    task_t* current_task = head_task;
    while(current_task->next != NULL){
      current_task = current_task->next;
    }
    task_t* new_task = (task_t*)malloc(sizeof(task_t));
    current_task->next = new_task;
    fscanf(infile, "%d %d", &((*new_task).arrival_time), &((*new_task).service_time));
    new_task->next = NULL;
  }

  /*
  //find the number of tasks in the file first and allocate
  while(fgets(line, 100,infile)){
    numTasks++;
  }
  struct task *taskList = malloc(numTasks*sizeof(struct task));
  //fill in the linked list
  for(int i=0;i<numTasks;i++){
    fscanf(infile, "%d", &taskList->arrival_time);
    fscanf(infile, "%d", &taskList->service_time);
    //printf("%d %d\n",taskList->arrival_time,taskList->service_time);
  }*/
}
