//Evan Fogerty and Josh DeWitt
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <wait.h>
#include <stdbool.h>

int task_counter;
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

void FIFO(task_t* temp, FILE* out){
  task_t* current = temp;
  int clock_tick = 0;
  bool allFinished = false;
  fprintf(out, "FIFO scheduling results\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
  current = temp->next;
  int total_time = 0;
  int  arrival_times[task_counter];
  int j = 0;
  while(current->next!=NULL){
    arrival_times[j] = current->arrival_time;
    current->remaining_time = current->service_time;
    current->completion_time = (current->arrival_time+current->service_time);
    current->response_time = current->service_time;
    current->wait_time = 0;
    current = current->next;  
    j++;
  }
  /*goes through the list and accumulates the project wait times for each based on shared 
      arrival time and service time*/
  //still needs to be checked over;
  for(int i = 0; i <task_counter-1; i++){
    int accumulated_wait = 0;
    for(int g = 0; g<i;g++){
      if(arrival_times[i]==arrival_times[g]){
        current = temp->next;
        for(int q = 0; (q<g)&&(current !=NULL) ;q++){
          current = current->next;
        }
        accumulated_wait += current->service_time;
        //printf("current serv is from %c going to %c\n", current->task_id,current->next->task_id);
        current = current->next;
        current->wait_time = accumulated_wait;
      }
    }
  }
  current = temp;
  while(current->next!=NULL){
    current = current->next;
    printf("task_id %c, task arrival %d, task service %d, task wait %d\n",current->task_id,current->arrival_time,current->service_time,current->wait_time);
  }
  
  //current = temp->next;
  //while(allFinished == false){
    //fprintf(out, "%d\t", clock_tick);
    /*while(current->next != NULL/*look through entire task list){
      current->remaining_time = current->service_time;
      current->completion_time = (current->arrival_time+current->service_time);
      current->response_time = current->service_time;
      current->wait_time = 0;
      current = current->next;  
    }*/
    //clock_tick++;
  //}
}

void SJF(task_t* temp, FILE* out){
  fprintf(out, "SJF(preemptive) scheduling results\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
}

void RR(task_t* temp, FILE* out){
  fprintf(out, "RR scheduling results (time slice is 1)\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
}
void assign_letter(task_t* head){
  task_t* current = head;
  //int counter = 0;
  /*while(current->next != NULL){
    current = current->next;
    counter++;
  }*/
  //current = head;
  //for(int i = 'A'; current !=NULL; i++){}
}

int main(int argc, char *argv[]){
  task_counter=0;
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
    new_task->task_id='A'+task_counter;
    task_counter++;
    current_task->next = new_task;
    fscanf(infile, "%d %d", &((*new_task).arrival_time), &((*new_task).service_time));
    printf("created task %c\n",new_task->task_id);
    new_task->next = NULL;
  }
  //there will now be a head task with no values as well as an end task with no values
  //we will need to make sure not ot print the end task
  //or otherwise figure out a way to get rid of it
  /*task_t* temp = head_task;
  while(1){
    if(temp->next->next == NULL){
      temp->next->
    }
  }*/
  //make the CL function uppercase to identify all kinds of capitalizations
  for(int i = 0; i <strlen(function);i++){
    function[i] = toupper(function[i]);
  }

  if(!(strcmp(function,"-FIFO"))){//function.toupper() == -FIFO){
    FIFO(head_task, outfile);
  }else if(!(strcmp(function,"-SJF"))){
    SJF(head_task, outfile);
  }else if(!(strcmp(function,"-RR"))){
    RR(head_task, outfile);
  }else{
    printf("Incorrect scheduling policy name\n");
    return 0;
  }
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

