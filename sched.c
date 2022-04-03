//Evan Fogerty and Josh DeWitt
//2022-04-02
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
int task_counter_print;
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

void printTable(task_t*temp, FILE* out){
  fprintf(out,"\tarrival service completion response wait\n");
  fprintf(out,"tid \ttime\ttime\ttime\t   time\t    time\n");
  fprintf(out,"------------------------------------------------\n");
  task_t* current = temp->next;
  //traversing the linked list
  while(current!=NULL){
    //printing the first table present in the output file
    current->response_time = current->completion_time-current->arrival_time;
    fprintf(out,"%c\t%3d\t%3d\t%3d\t%3d\t%3d\n",current->task_id,current->arrival_time,current->service_time,current->completion_time,current->response_time,current->wait_time);
    current = current->next;
  }
  //printing the 2nd table in the output file
  fprintf(out, "service wait\n");
  fprintf(out, "time\ttime\n");
  fprintf(out,"------------------\n");
  current = temp->next;
  //putting all the wait and service times in arrays for easier handling
  int waitTimeList[task_counter_print];
  int servTimeList[task_counter_print];
  for(int i = 0; current!=NULL; i++){
    waitTimeList[i] = current->wait_time;
    servTimeList[i] = current->service_time;
    current = current->next;
  }
  //sorting the arrays by service time then wait time
  for(int i = 0; i<task_counter_print;i++){
    for(int j = 0; j<task_counter_print;j++){
      if(j!=i){
        if(servTimeList[i]>servTimeList[j]){
          int temp;
          temp = servTimeList[i];
            servTimeList[i] = servTimeList[j];
            servTimeList[j] = temp;
            temp = waitTimeList[i];
            waitTimeList[i] = waitTimeList[j];
            waitTimeList[j] = temp;
        }else if(servTimeList[i]==servTimeList[j]){
          if(waitTimeList[i]>waitTimeList[j]){
            int temp;
            temp = waitTimeList[i];
            waitTimeList[i] = waitTimeList[j];
            waitTimeList[j] = temp;
            temp = servTimeList[i];
            servTimeList[i] = servTimeList[j];
            servTimeList[j] = temp;
          }
          
        }
      }
    }
  }
  //printing them all out
  for(int i = task_counter_print-1; i>=0; i--){
    fprintf(out,"%3d\t%3d\n",servTimeList[i],waitTimeList[i]);
  }
}
void FIFO(task_t* temp, FILE* out){
  task_t* current = temp;
  int clock_tick = 0;
  fprintf(out, "FIFO scheduling results\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
  current = temp->next;
  //keeps going until all the tasks are finished
  while (task_counter>=0){
    if(current == NULL){
      break;
    }
    //only printing if something should be there at a given clock tick
    // or if the remaining time is >0
    if((current->arrival_time<=clock_tick) && (current->remaining_time>0)){
      //insert printing here
      int counter = 0;//will determine if we put a comma with the alt stuff in ready queue
      task_t* printNode = current;//the temp node for printing stuff
      //printing the stuff in time and CPU
      fprintf(out, "%3d\t%c%d\t",clock_tick,printNode->task_id, printNode->remaining_time);
      printNode = printNode->next;
      int printedReady = 0;//flag int for ready
      while(printNode!=NULL){
        if(clock_tick>=printNode->arrival_time){
          if(counter == 0){
            fprintf(out, "%c%d", printNode->task_id, printNode->remaining_time);
            counter++;
            printedReady = 1;
          }else{
            fprintf(out, ",%c%d", printNode->task_id, printNode->remaining_time);
            counter++;
          }
        }else{}
        printNode = printNode->next;
      }
      if(printedReady){
        fprintf(out,"\n");
      }else{
        fprintf(out,"-- \n");
      }
      //decremting remaining time since there's no print things left
      current->remaining_time--;
      if(current->remaining_time == 0){
        //the clock tick when complete
        current->completion_time = clock_tick+1;
        //wait time = completion - service - arrival
        //prepping for printTable();
        current->wait_time = current->completion_time-current->service_time-current->arrival_time;
        current = current->next;
        task_counter--;
      }
    }else{
      //since there wasn't a task to print:
      fprintf(out, "%3d\t%c\t%c%c\n",clock_tick, ' ','-','-');
    }
    clock_tick++;
  }
  printTable(temp,out);
}


void SJF(task_t* temp, FILE* out){
  fprintf(out, "SJF(preemptive) scheduling results\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
  int clock_tick = 0;
  task_t* smallest = temp->next;//respresents the smallest node
  while(task_counter>0){
    if(task_counter ==1){
      //a fix happened upon after 7 hours of trying to fix this function
      break;
    }
    task_t* header = temp->next;
    //if smallest had remaining time 0, go to the first that isn't zero
    //if there aren't any, then we need to end the function by setting NULL and task_0
    if(smallest->remaining_time==0){
      smallest = temp->next;
      while(smallest!=NULL&&(smallest->remaining_time==0)){
        smallest = smallest->next;
        if(smallest->next == NULL &&smallest->remaining_time == 0){
          task_counter = 0;
          smallest = NULL;
          header = NULL;
          break;
        }
      }
    }
    //itterates through to set smallest to the smallest time
    while((header !=NULL)){
      if(smallest == NULL&&header->arrival_time<=clock_tick&&header->remaining_time!=0){
        smallest = header;
      }else if(smallest!=NULL&&header->arrival_time<=clock_tick&&header->remaining_time==smallest->remaining_time&&
      header->remaining_time!=0&&header->arrival_time<smallest->arrival_time){
        smallest = header;
      }else if(smallest != NULL&& header->arrival_time<=clock_tick&&header->remaining_time<smallest->remaining_time&&
      header->remaining_time!=0){
        smallest = header;
      }
      header = header->next;
    }
    //printing out the things we need
    if(smallest!=NULL&&(smallest->arrival_time<=clock_tick)&&(smallest->remaining_time>0)){
      fprintf(out, "%3d\t%c%d\t",clock_tick,smallest->task_id, smallest->remaining_time);
      smallest->remaining_time--;
      int counter = 0;//same purpose as in fifo
      int printedReady = 0;
      task_t* printNode = temp->next;
      while(printNode!=NULL){
        if(printNode->task_id!=smallest->task_id){
          if(printNode->remaining_time!=0){
            if(clock_tick>=printNode->arrival_time){
              if(counter == 0){
                fprintf(out, "%c%d", printNode->task_id, printNode->remaining_time);
                counter++;
                printNode->completion_time = clock_tick+1;
                printedReady = 1;
              }else{
                fprintf(out, ",%c%d", printNode->task_id, printNode->remaining_time);
                counter++;//the clock tick when complete
                printNode->completion_time = clock_tick+1;
              }
            }else{}
          }
        }
        printNode = printNode->next;
      }
      if(printedReady){
        fprintf(out,"\n");
      }else{
        fprintf(out,"-- \n");
      }
      if(smallest->remaining_time==0){
        //the clock tick when complete
        smallest->completion_time = clock_tick+1;
        //wait time = completion - service - arrival
        smallest->wait_time = smallest->completion_time-smallest->service_time-smallest->arrival_time;
        smallest = smallest->next;
        task_counter--;
      }
    }else{
      if(smallest == NULL && header == NULL){
          //don't need to print anything since we're done
      }else{
      fprintf(out, "%3d\t%c\t%c%c\n",clock_tick, ' ','-','-');
      }
    }    
    clock_tick++;
    
  }
  //done when there's only 1 task left
  task_t* current = temp->next;
  while(current != NULL){
    if(current->remaining_time!=0){
      break;
    }
    current = current->next;
  }
  //printing out all the stuff left
  while(current->remaining_time>0){
    fprintf(out, "%3d\t%c%d\t --\n",clock_tick,current->task_id, current->remaining_time);
    current->remaining_time--;
    clock_tick++;
  }
  current->completion_time = clock_tick+1;
  //wait time = completion - service - arrival
  current->wait_time = current->completion_time-current->service_time-current->arrival_time;
  printTable(temp,out);
}
void RR(task_t* temp, FILE* out){
  fprintf(out, "RR scheduling results (time slice is 1)\n\ntime\tcpu\tready\tqueue\t(tid/rst)\n----------------------------\n");
  task_t* current = temp->next;
  int clock_tick = 0;
  int onCPU = 0;
  //timeslice is 1 clock tick
  //going until there's no tasks left
  while(task_counter>0){
    current = temp->next;
    bool somethingToPrint = false;
    onCPU = 0;
    while(current !=NULL){
      if((current->remaining_time>0)){
        if((current->arrival_time<=clock_tick)){
          somethingToPrint = true;
          onCPU = current->task_id;
          fprintf(out, "%3d\t%c%d\t",clock_tick,current->task_id, current->remaining_time);
          current->remaining_time--;
          int counter = 0;
          int printedReady = 0;
          task_t* printNode = temp->next;
          while(printNode!=NULL){
            if(printNode->task_id!=onCPU){
              if(printNode->remaining_time!=0){
                if(clock_tick>=printNode->arrival_time){
                  if(counter == 0){
                    fprintf(out, "%c%d", printNode->task_id, printNode->remaining_time);
                    counter++;
                    printNode->completion_time = clock_tick+1;
                    printedReady = 1;
                  }else{
                    fprintf(out, ",%c%d", printNode->task_id, printNode->remaining_time);
                    counter++;//the clock tick when complete
                    printNode->completion_time = clock_tick+1;
                    printedReady = 1;
                  }
                }
              }
            }
            printNode = printNode->next;
          }
          if(printedReady){
            fprintf(out,"\n");  
          }else{
            fprintf(out,"-- \n"); 
          }
          if(current->remaining_time==0){
            current->completion_time = clock_tick+1;
            current->wait_time=current->completion_time-current->service_time-current->arrival_time;
            task_counter--;
          }
          clock_tick++;
        }
      }
      if(current->next == NULL){
        if(!somethingToPrint){
          fprintf(out, "%3d\t%c\t%c%c\n",clock_tick, ' ','-','-');
          clock_tick++;
        }
      }
      current = current->next;
    }
  }
  printTable(temp, out);
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
  task_t* head_task = (task_t*)malloc(sizeof(task_t));
  //inserting tasks
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
    new_task->remaining_time = new_task->service_time;
    new_task->next = NULL;
  }
  task_t* tempTask = head_task;
  while(tempTask->next->next != NULL){
    tempTask = tempTask->next;
  }
  free(tempTask->next->next);
  tempTask->next = NULL;
  tempTask = head_task->next;
  task_counter--;
  while(tempTask!=NULL){
    tempTask = tempTask->next;
  }
  //there will now be a head task with no values as well as an end task with no values
  //we will need to make sure not ot print the end task
  //or otherwise figure out a way to get rid of it
  //make the CL function uppercase to identify all kinds of capitalizations
  for(int i = 0; i <strlen(function);i++){
    function[i] = toupper(function[i]);
  }
  task_counter_print = task_counter;
  if(!(strcmp(function,"-FIFO"))){
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
