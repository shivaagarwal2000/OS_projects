#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>															//desc of process
#include <unistd.h>														
																	
typedef struct process{												
	char *name;
	int arrival;
	int cpu_burst;
	int turnaround;
	int wait;
	int start;
	int end;
	int remburst;
	struct process *link;

} process;

//typedef struct process process;
//typedef struct process process;

typedef struct node {
	int index;
	struct node *link;
}node;
																	//desc of ReadyQueue
typedef struct {
	process *rq;
	int size;
} RQueue;

typedef struct gentry{															//use to keep the info about how the process-time is designed
	char *name;
	int start;
	int turnaround;
	int wait;
	int end;
	struct gentry *link;
} gentry;

//typedef struct gentry 

void FCFS (RQueue *rq);
void RR (RQueue *rqueue, int quantum);
void printTable (process *arr, int size);
RQueue* readFile (char *filename);
void addgNode (process **pghead, process* tmp);
void startSch (RQueue *rq);
void printgchart (gentry *gchart, int size);
void printgchartl (process *gchart);
int isEmpty (process *head);
void RR (RQueue *rqueue, int quantum);
void printStatistics (RQueue *rqueue, gentry *gchart);
void SRBT (RQueue *rqueue);
void addqNode (process **pqhead, process* tmp);
process *qdequeue (process **pqhead);
process *jdequeue (process **pthead, char *name);
void printStatisticsl (RQueue *rqueue, process *gchart);
bool trans_arrived_process (process **pthead,process **prhead,int current_time) ;



int main (int argc, char *argv[]) {
	
	
	if (argc != 2) {
		fprintf (stderr, "err: not enough arguments for the scheduling to take place\n");
		exit (1);
	}
	
	int size;
	RQueue *rq;
	
	char *filename = argv[1];
	rq = readFile (filename);
																	//start the scheduling
	startSch (rq);
	
	
	return 0;
}


																	//main driver code containing the menu for the process scheduling
void startSch (RQueue *rq) {
	int ch,q;
	while (1) {
		
		printf ("\n\n>>>>>\t\tCPU Process Scheduling\t\t<<<<<\n\n1.First Come First Served\n2.Round Robin\n3.Shortest Remaining Burst First\n4.exit\n\nPress the Corresponding integer to continue\n");
		scanf ("%d",&ch);
		
		switch (ch) {
			case 1: printf ("\n>>>>>\tFirst Come First Served\t<<<<<\n\n");FCFS (rq);break;
			
			case 2: printf("enter the value of quantum: "); scanf("%d",&q); printf ("\n>>>>>\tRound Robin Scheduling\t<<<<<\n\n");RR(rq,q);break;
			
			case 3:  printf ("\n>>>>>\tShortest Remainning Burst First\t<<<<<\n\n");SRBT(rq); break;
			
			case 4: return;
			
			default: printf ("err: Invalid choice of option\nEnter again\n");
		
		}
	}

}

																	//read the file for the info about the processes
RQueue* readFile (char *filename) {
	int nlines = 0, counter = 0, size = 0, c;
	
	FILE *dmp, *fp;
	
	
	//if (filename[strlen(filename)] == '\0') printf ("null terminated\n");
	//printf ("%s %ld\n",filename,strlen(filename));
	
	if ((dmp = fopen (filename,"r")) == NULL) {
		fprintf (stderr,"err cant open\n");
	}
	if ((fp = fopen (filename,"r")) == NULL) {
		fprintf (stderr,"err cant open\n");
	}
	
																	//read the number of lines in the file
	while ((c = fgetc(dmp)) != EOF) {
		//printf ("hello\n");
		if (c == '\n') nlines++;
	}
	
	//printf("num of lines in the input file: %d\n",nlines);
	
	
	counter = size = nlines;
	process *arr = (process *)malloc (nlines * sizeof(process));
	
	RQueue *rqueue = (RQueue *)malloc(1 * sizeof(RQueue));
	rqueue->rq = arr;
	rqueue->size = size;
																	//read nlines into the array of the entity
	for (counter = 0 ;counter < size; counter++) {
		arr[counter].name = (char *)malloc (1000 *sizeof (char));
		fscanf (fp,"%s %d %d",arr[counter].name,&(arr[counter].arrival),&(arr[counter].cpu_burst));
		arr[counter].remburst = arr[counter].cpu_burst;
	}
	
																	//print the table
	//printTable (arr, size);
	//printTable (rqueue->rq, rqueue->size);
	
	
	return rqueue;
	
}

																	//print the info read in the table
void printTable (process *arr, int size) {
	for (int i = 0; i < size; i ++) {
		printf ("%s %d %d\n",arr[i].name,arr[i].arrival,arr[i].cpu_burst);
		arr[i].remburst = arr[i].cpu_burst;
	}

}

void printStatistics (RQueue *rqueue, gentry *gchart) {
	int size = rqueue->size;
	process *pr = rqueue->rq;
	int turnaround_times = 0;
	int wait_times = 0; 
	int avgtat = 0;
	int avgwt = 0;
	printf ("\nProc_Name\tTurnaround_Times\tWait_times\n");
	for (int i = 0; i  < size; i++) {
		turnaround_times = 0;
		wait_times = 0;
		for (int j = 0; j < size; j++) {
			if (gchart[j].name == pr[i].name) {
				turnaround_times = turnaround_times + gchart[j].turnaround;
				wait_times = wait_times + gchart[j].wait;
			}
		}
		avgtat += turnaround_times;
		avgwt += wait_times;
		printf ("  %s\t\t\t %d\t\t   %d\n",pr[i].name,turnaround_times,wait_times);
	}
	printf("\nAverage Turnaround Time: %.2f\nAverage Wait Time: %.2f\n",(float)avgtat/size,(float)avgwt/size);
	
}

void printStatisticsl (RQueue *rqueue, process *gchart) {
	int size = rqueue->size;
	process *pr = rqueue->rq;
	int turnaround_times = 0;
	int wait_times = 0; 
	int avgtat = 0;
	int start = INT_MAX;
	int end = 0;
	int avgwt = 0;
	printf ("\nProc_Name\tTurnaround_Times\tWait_times\n");
	process *tmp;
	for (int i = 0; i  < size; i++) {
		turnaround_times = 0;
		wait_times = 0;
		start = INT_MAX;
		end = 0;
		tmp = gchart;
		
		while (tmp != NULL) {
			//printf ("%s arrival: %d end: %d start: %d end: %d\n",tmp->name,tmp->arrival,tmp->end,start,end);
			if (strcmp(tmp->name,pr[i].name) == 0) {
				if (tmp->arrival < start) {
					start = tmp->arrival;
				}
				if (tmp->end > end) {
					end = tmp->end;
				}
				wait_times = wait_times + tmp->wait;
			}
			//printf ("%s arrival: %d end: %d start: %d end: %d\n",tmp->name,tmp->arrival,tmp->end,start,end);
			tmp = tmp->link;
		}
		
		/*
		for (int j = 0; j < size; j++) {
			if (gchart[j].name == pr[i].name) {
				turnaround_times = turnaround_times + gchart[j].turnaround;
				wait_times = wait_times + gchart[j].wait;
			}
		}
		*/
		
		turnaround_times = end - start;
		
		avgtat += turnaround_times;
		avgwt += wait_times;
		
		printf ("  %s\t\t\t %d\t\t   %d\n",pr[i].name,turnaround_times,wait_times);
	}
	printf("\nAverage Turnaround Time: %.2f\nAverage Wait Time: %.2f\n",(float)avgtat/size,(float)avgwt/size);
}


void printgchart (gentry *gchart, int size) {												//print the gantt chart which contains the info about the scheduling of the processes
	
	for (int i = 0; i < size; i ++) {
		printf ("[%d-%d] %s running\n", gchart[i].start, gchart[i].end, gchart[i].name);
	}
	
}

void FCFS (RQueue *rqueue) {
	int size = rqueue->size;
	process *pr = rqueue->rq;
	gentry *gchart = (gentry *)malloc(size*sizeof(gentry));
	
	
	for (int counter = 0; counter < size; counter++) {
		if (counter == 0) {
			//printf ("hello %d",counter);
			//addNode (&head,pr[counter].name, pr[counter].arrival, pr[counter].arrival+pr[counter].cpu_burst);
			//pr[counter].name = (char *)malloc (100 *sizeof (char));
			
			gchart[counter].name = pr[counter].name;
			gchart[counter].start = pr[counter].arrival;
			gchart[counter].wait = pr[counter].arrival;
			gchart[counter].turnaround = pr[counter].cpu_burst;
			gchart[counter].end = pr[counter].arrival + pr[counter].cpu_burst;
		}
		else {
			//printf ("hello %d",counter);
			//int x = (pr[counter].arrival >= pr[counter-1].cpu_burst
			//addNode (&head,pr[counter].name,
			//pr[counter].name = (char *)malloc (100 *sizeof (char));
			
			gchart[counter].name = pr[counter].name;
			gchart[counter].start = ((gchart[counter-1].end >= pr[counter].arrival) ? gchart[counter-1].end : pr[counter].arrival );
			gchart[counter].end = gchart[counter].start + pr[counter].cpu_burst;
			gchart[counter].wait = gchart[counter].start - pr[counter].arrival;
			gchart[counter].turnaround = gchart[counter].wait + pr[counter].cpu_burst;
		}
	}
	
	printgchart (gchart, size);
	printStatistics (rqueue, gchart);
	
}

/*
int addNode (gentry **phead,char *name, int start, int end) {									//represents the gantt chart in the form of linked list
	if (*phead == NULL) {
		(*phead) = (gentry*)malloc(sizeof(gentry));
		(*phead)->name = (char*)malloc(sizeof(1000));
		(*phead)->name = name;
		(*phead)->start = start;
		(*phead)->end = end;
		(*phead)->link = NULL;
		return end;
	}
	gentry *head = *phead;
	gentry *tmp = (gentry*)malloc(sizeof(gentry));
	tmp->name = (char*)malloc(sizeof(1000));
	tmp->name = name;
	tmp->start = start;
	tmp->end = end;
	tmp->link = NULL;
	
	while(head->link) {
		head = head->link;
	}
	head->link = tmp;
	
	return;
}
*/

int isEmpty (process *head) {
	return (head == NULL) ? 1:0;
}

void printgchartl (process *gchart) {
	if (!isEmpty(gchart)) {
		while (gchart != NULL) {
			printf ("[%d-%d] %s running\n", gchart->start, gchart->end, gchart->name);
			gchart = gchart -> link;
		}
	}
}

void addgNode (process **pghead, process* tmp) {
	if (isEmpty(*pghead)) {
		(*pghead) = tmp;
		return;
	}
	process *head = *pghead;
	while (head->link) {
		head = head->link;
	} 
	head->link = tmp;
	return;
	
}

void addqNode (process **pqhead, process* tmp) {
	if (isEmpty(*pqhead)) {
		(*pqhead) = tmp;
		return;
	}
	process *head = *pqhead;
	while (head->link) {
		head = head->link;
	} 
	head->link = tmp;
	return;
	
}

process *qdequeue (process **pqhead) {
	if ((*pqhead)->link) {
		process *tmp = (*pqhead);
		(*pqhead) = (*pqhead) -> link;
		return tmp;
	}
	
	process *tmp = (*pqhead);
	(*pqhead) = (*pqhead) -> link;
	return tmp;
	
}

bool trans_arrived_process (process **pthead,process **prhead,int current_time) {
	
	process *tmp = *pthead;
	process *dmp; 
	bool val = false;
	
	while (tmp != NULL) {
		//printf ("hrllo\n");
		if (tmp->arrival <= current_time) {
			dmp = jdequeue (pthead, tmp->name);
			dmp->link = NULL;
			addqNode (prhead,dmp);
			//printf("hello");
			val = true;
		}
		tmp = tmp->link;
	}
	return val;

}

process *jdequeue (process **pthead, char *name) {
	
	if ((*pthead)->link == NULL) {
		process *tmp = (*pthead);
		*pthead = NULL;
		return tmp;
	}
	
	process *tmp = *pthead, *prevtmp;
	//prevtmp = tmp;
	prevtmp = tmp;
	
	if ((strcmp(tmp->name,name) == 0)) {
		(*pthead) = (*pthead)->link;
		tmp->link = NULL;
		return tmp;
	}
	
	while ((!isEmpty(tmp)) && (strcmp(tmp->name,name) != 0)) {
		prevtmp = tmp;
		tmp = tmp->link;
		//printf("hello");
	}
	
	prevtmp->link = tmp->link;
	tmp->link = NULL;
	return tmp;
}

void SRBT (RQueue *rqueue) {
	int size = rqueue->size;
	process *pr = rqueue->rq;
	process *tmp;
	process *ghead = NULL; //gantt chart representing the scheduling info
	process *thead = NULL; //queue or tmp placement for the processes which contains all the processes even the ones which has not arrived
	process *rhead = NULL; //queue for the processes which has arrived and are available for execution
	int current_time = 0;
	process *selec;
	int lim = INT_MAX;
	bool isSelec = false;
	int lastburst;
	bool isExec = false;
	bool isTrans = false;
	bool canExec = false;
	process *prevselec;
	
	for (int next_counter = 0; next_counter < size; next_counter++) {  //insert the data from the table into the temp queue
	
		tmp = (process *)malloc(sizeof(process));
		tmp-> name = (char *)malloc(1000*sizeof(char));
		tmp->name = pr[next_counter].name;
		tmp->arrival = pr[next_counter].arrival;
		tmp->cpu_burst = pr[next_counter].cpu_burst;
		tmp->turnaround = 0;
		tmp->wait = 0;
		tmp->start = 0;
		tmp->end = 0;
		//tmp->wait = current_time - tmp->arrival;
		//tmp->start = current_time;
		tmp->remburst = tmp->cpu_burst;
		//tmp->end;
		//tmp->end = current_time + dmmydec;
		//dmmydec = ((pr[next_counter].remburst < quantum) ? pr[next_counter].remburst : quantum);
		//tmp->remburst = tmp->remburst - dmmydec;
		//tmp->turnaround += dmmydec;
		tmp->link = NULL;
		
		
		addqNode (&thead,tmp);
	}
	
	while ((!isEmpty(thead)) || (!isEmpty (rhead)) || (canExec == true)) { //if any of the queue has a process left to execute
		//printf("%d\n",current_time);
		
		isTrans = trans_arrived_process (&thead,&rhead,current_time);
		
		/*
		if (!isEmpty(rhead)) {
			printf ("rhead not empty\n");
		}
		*/
		
		if (current_time == 5) {
			//sleep (200);
		}
		
		//printf("%d\n",current_time);
		if (isTrans == true) { //some new process is available for the execution
			//printf ("hel\n");
			canExec = true;
			if (isSelec == true) { //a process was executing previously so check for context switch
				
				tmp = rhead;
				lim = INT_MAX;
				while (tmp) {
					if (tmp->cpu_burst < lim) {
						selec = tmp;
						lim = tmp->cpu_burst;
						
					}
					tmp = tmp->link;
				}
				lim = INT_MAX;
				tmp = ghead;
				
				while (tmp->link) {
					tmp = tmp->link;
				}
				
				if ((tmp->remburst <= selec->remburst) && (tmp->remburst > 0)) {//current proc has less or equal cpu_burst left than the avail ones
					tmp->remburst = tmp->remburst - 1;
					tmp->turnaround = tmp->turnaround + 1;
					
					
				}
				
				else { //switch context
					
					if (tmp->remburst == 0) {
						
						tmp->end = current_time;
						isSelec = false;
						
						
						selec = jdequeue(&rhead,selec->name);
					
						process *dmp = (process *)malloc (sizeof(process));
						dmp->name = selec->name;
						dmp->arrival = selec->arrival;
						dmp->cpu_burst = selec->cpu_burst;
						dmp->turnaround += 1;
						dmp->wait = current_time -selec->end;
						dmp->start = current_time;
						//dmp->end = current_time;
						dmp->remburst = selec->remburst-1;
						dmp->link = NULL;
						
						addgNode (&ghead,dmp);
						
						isSelec = true;
						
					}
					
					//dequeue from rhead and put the updated bookkeeping of last executing proc into rhead
					
					//create bookkeeping for the previous proc completely and insert into the gchart <- execute
					
					else {
						tmp->end = current_time;
						process *dmp = (process *)malloc (sizeof(process));
						dmp->name = tmp->name;
						dmp->arrival = tmp->arrival;
						dmp->cpu_burst = tmp->cpu_burst;
						dmp->turnaround = 0;
						dmp->wait = 0;
						dmp->start = 0;
						dmp->end = current_time;
						dmp->remburst = tmp->remburst;
						dmp->link = NULL;
					
						isSelec = true;
					
						addqNode (&rhead,dmp);
					
						selec = jdequeue(&rhead,selec->name);
					
						dmp = (process *)malloc (sizeof(process));
						dmp->name = selec->name;
						dmp->arrival = selec->arrival;
						dmp->cpu_burst = selec->cpu_burst;
						dmp->turnaround += 1;
						dmp->wait = current_time -selec->end;
						dmp->start = current_time;
						//dmp->end = current_time;
						dmp->remburst = selec->remburst-1;
						dmp->link = NULL;
						
						addgNode (&ghead,dmp);
					
					}
					
				}
				
			
			}
			else { //no process was executing prev just look for process with min cpu burst and execute
				tmp = rhead;
				lim = INT_MAX;
				while (tmp) {
					if (tmp->cpu_burst < lim) {
						selec = tmp;
						lim = tmp->cpu_burst;
						
					}		
					tmp = tmp->link;
				}
				lim = INT_MAX;
				//dequeue the proc given by selec
				selec = jdequeue (&rhead,selec->name);
				
				
				//create bookkeeping for the previous proc completely and insert into the gchart <- execute
				
				process *dmp = (process *)malloc (sizeof(process));
				dmp->name = selec->name;
				dmp->arrival = selec->arrival;
				dmp->cpu_burst = selec->cpu_burst;
				dmp->turnaround = selec->turnaround + 1;
				dmp->wait = current_time - selec->arrival;
				dmp->start = current_time;
				//dmp->end 
				dmp->remburst = selec->remburst -1;
				dmp->link = NULL;
				
				isSelec = true;
				
				addgNode(&ghead,dmp);
				
				if (dmp->remburst == 0) {
					dmp->end = current_time;
					isSelec = false;
				}
			
			}
		
		}
		
		else { //no new process is available for the execution
			
			if (isSelec == true) { //some process was executing previously
				
				
				tmp = ghead;
				
				while (tmp->link) {
					tmp = tmp->link;
				}
				
				if (tmp->remburst >0) {	//current proc can continue
					canExec = true;
					(tmp->remburst) = tmp->remburst - 1 ;
					tmp->turnaround = tmp->turnaround + 1;
					
					/*if (tmp->remburst == 0) {
						
						tmp->end = current_time;
						isSelec = false;
					}*/
				}
				
				else {		//current proc can not continue
					tmp->end = current_time;
					isSelec = false;
					canExec = false;
					if (!isEmpty(rhead)) { //if some process is available to execute then choose one with min cpu burst and execute
						canExec = true;
						tmp = rhead;
						lim = INT_MAX;
						while (tmp) {
							if (tmp->cpu_burst < lim) {
								selec = tmp;
								lim = tmp->cpu_burst;
							
							}		
							tmp = tmp->link;
						}
						lim = INT_MAX;
						
						
						selec = jdequeue(&rhead,selec->name);
					
						process *dmp = (process *)malloc (sizeof(process));
						dmp->name = selec->name;
						dmp->arrival = selec->arrival;
						dmp->cpu_burst = selec->cpu_burst;
						dmp->turnaround += 1;
						dmp->wait = current_time -selec->end;
						dmp->start = current_time;
						//dmp->end = current_time;
						dmp->remburst = selec->remburst-1;
						dmp->link = NULL;
						
						addgNode (&ghead,dmp);
						
						isSelec = true;
						
						if (dmp->remburst == 0) {
						
							dmp->end = current_time;
							isSelec = false;
						}
						
					}
					
					
					
					
				}
				
			}
			
			else 
				; //no process was executing previously
				
		}
		
		
		//printf("%d\n",current_time);
		//sleep(5);
		
		
		current_time++;
	
	} //while ends here
	
	printgchartl (ghead);
	printStatisticsl (rqueue,ghead);
	//printf("hello");
	
}


void RR (RQueue *rqueue, int quantum) {
	int size = rqueue->size;
	int start = 0, end = 0;
	int cur_index = 0, remproc = size;	
	int flag = 1;																//processes not in consideration
	int current_time = 0;
	int counter = 0;
	int next_counter = 0;
	process *pr = rqueue->rq;
	int dmmydec = 0;
	//table == pr == array
	//gantt chart == gchar == linked list
	//queue for preempted proc == qhead == linked list
	
	
	
	process *ghead = NULL;
	process *qhead = NULL;
	
	for (		;	1	;	current_time+=dmmydec) {
	
		//printf("%d\n",current_time);
		
		
		if ((next_counter < size) && (pr[next_counter].arrival <= current_time)) { 	//a new process is available for switch from the newly arrived processes...
			
			//if a process is already executing
			if (!isEmpty(ghead)) {
				process *tmp = ghead;
				
				while (tmp->link) {
					tmp = tmp->link;
				}
				
				if (tmp->remburst > 0) {
										//process has some part left to execute
					tmp->end = current_time;
					tmp->turnaround += current_time - tmp->start;
					
					//preempting the process
					
					//create bookkeeping for the process to insert into the queue
						process *dmp = (process *)malloc (sizeof(process));
						dmp-> name = (char *)malloc(100*sizeof(char));
						dmp->name = tmp-> name;
						dmp->arrival = tmp->arrival;
						dmp->cpu_burst = tmp->cpu_burst;
						dmp->turnaround = 0;
						dmp->wait = 0;
						//cur_index = next_counter;
						dmp->start = tmp->start;
						dmp->end = tmp->end;
						//dmmydec = ((pr[next_counter].remburst < quantum) ? pr[next_counter].remburst : quantum);
						dmp->remburst = tmp->remburst;
						dmp->link = NULL;
						
						//printf ("cpu_burst: %d remburst: %d name: %s -- i am going into queue\n",dmp->cpu_burst,dmp->remburst,dmp->name); //print info of the process packet
						
						//add the proc into the queue
						addqNode (&qhead, dmp);
						
					//}
					
				}
				
			}
			
			//create a bookkeeping for the process to be switched from the new arrival section
			process *tmp = (process *)malloc (sizeof(process));
			tmp-> name = (char *)malloc(1000*sizeof(char));
			tmp->name = pr[next_counter].name;
			tmp->arrival = pr[next_counter].arrival;
			tmp->cpu_burst = pr[next_counter].cpu_burst;
			tmp->turnaround = 0;
			tmp->wait = current_time - tmp->arrival;
			cur_index = next_counter;
			tmp->start = current_time;
			tmp->remburst = pr[next_counter].remburst;
			//tmp->end;
			tmp->end = current_time + dmmydec;
			dmmydec = ((pr[next_counter].remburst < quantum) ? pr[next_counter].remburst : quantum);
			tmp->remburst = tmp->remburst - dmmydec;
			tmp->turnaround += dmmydec;
			tmp->link = NULL;
			
			//printf ("cpu_burst: %d remburst: %d name: %s -- i am going into gchart\n",tmp->cpu_burst,tmp->remburst,tmp->name); 
			
			//context switched
			addgNode (&ghead,tmp);
			
			next_counter++;
			
		}
		else if (!isEmpty(qhead)) {	 //queue has some partially executed process to switch
		
			//take the info from the gchart of rem part of the process and put it in queue...
			
			//if a process is already executing
			if (!isEmpty(ghead)) {
				process *tmp = ghead;
				
				while (tmp->link) {
					tmp = tmp->link;
				}
				
				
				if (tmp->remburst > 0) {
				//if (tmp->start  + tmp->remburst >= current_time) {	process has some part left to execute
					tmp->end = current_time;
					tmp->turnaround += current_time - tmp->start;
					
					// {	//preempting the process
					
					//create bookkeeping for the process to insert into the queue
						process *dmp = (process *)malloc (sizeof(process));
						dmp-> name = (char *)malloc(100*sizeof(char));
						dmp->name = tmp-> name;
						dmp->arrival = tmp->arrival;
						dmp->cpu_burst = tmp->cpu_burst;
						dmp->turnaround = 0;
						dmp->wait = 0;
						//cur_index = next_counter;
						dmp->start = tmp->start;
						dmp->end = tmp->end;
						//dmmydec = ((pr[next_counter].remburst < quantum) ? pr[next_counter].remburst : quantum);
						dmp->remburst = tmp->remburst;
						dmp->link = NULL;
						
						//printf ("cpu_burst: %d remburst: %d name: %s -- i am going into queue\n",dmp->cpu_burst,dmp->remburst,dmp->name); 
						
						//add the proc into the queue
						addqNode (&qhead, dmp);
						
					//}
					
				}
				
			}
			
			//take out the process from queue and put in gchart -> dequeue from the queue(FIFO) and insert into the gchart
			process *src = qdequeue (&qhead);
			
			//create a new bookkeeping for the new process to be executed
			process *tmp = (process *)malloc (sizeof(process));
			tmp->name = (char *)malloc(1000*sizeof(char));
			tmp->name = src->name;
			tmp->arrival = src->arrival;
			tmp->cpu_burst = src->cpu_burst;
			tmp->turnaround = 0;
			tmp->wait = current_time - src->end;
			//cur_index = next_counter;
			tmp->start = current_time;
			//tmp->end;
			dmmydec = ((src->remburst < quantum) ? src->remburst : quantum);
			tmp->end = current_time + dmmydec;
			tmp->remburst = src->remburst - dmmydec;
			tmp->turnaround += (tmp->wait + dmmydec);
			tmp->link = NULL;
			
			//printf ("cpu_burst: %d remburst: %d name: %s -- i am going into gchart\n",tmp->cpu_burst,tmp->remburst,tmp->name); 
			
			addgNode (&ghead,tmp);
			//update the current_index;
			
		}
		else if (!isEmpty(ghead)) {	//if current_process can continue
			process *tmp = ghead;
			while(tmp->link) {
				tmp = tmp->link;
			}
			if (tmp->remburst > 0) {
				dmmydec = (tmp->remburst < quantum)? tmp->remburst : quantum;
				tmp->remburst = tmp->remburst - dmmydec;
				tmp->end = current_time + dmmydec;
				
				/*
				//create a new bookkeeping for the new process to be executed
				process *dmp = (process *)malloc (sizeof(process));
				dmp->name = (char *)malloc(1000*sizeof(char));
				dmp->name = src->name;
				dmp->arrival = src->arrival;
				dmp->cpu_burst = src->cpu_burst;
				dmp->turnaround = 0;
				dmp->wait = current_time - src->end;
				//cur_index = next_counter;
				dmp->start = current_time;
				//tmp->end;
				dmmydec = ((src->remburst < quantum) ? src->remburst : quantum);
				dmp->end = current_time + dmmydec;
				dmp->remburst = src->remburst - dmmydec;
				dmp->turnaround += dmmydec;
				dmp->link = NULL;
				
				addgNode (&ghead,tmp);
				*/
				
				//printf ("cpu_burst: %d remburst: %d name: %s -- i am going into gchart\n",tmp->cpu_burst,tmp->remburst,tmp->name); 
			
				
			}
			
			else {		//scheduling is complete... we are done
					//any updates required to be done...	
				break;
			}
		
		}	
		
	
	}
	
	printgchartl (ghead);
	printStatisticsl (rqueue,ghead);
	
	
}




	/*
	while (!isEmpty(thead)) { //collection of available proc from which we can choose is not empty
		
		tmp = thead;
		
		//search for the process with the minimum cpu burst and satisfies the conditions ...
		while (tmp != NULL) {
		
			if ((tmp->arrival <= current_time) && (tmp->cpu_burst < lim)) {
			
				if ((isSelec == true) && (!strcmp(selec->name,tmp->name))) ;
				
				else {
					selec = tmp;
					if (isSelec == false) prevselec = selec;
					lim = tmp->cpu_burst;
					
					
					
				}
				
			}
			tmp = tmp->link;
			
			
		}
		
		if (isSelec == true) {
			//a proc was executing previously
			//create bookkeeping for the previous proc completely and insert into the gchart
				
			process *dmp = (process *)malloc (sizeof(process));
			dmp->name = prevselec->name;
			dmp->arrival = prevselec->arrival;
			dmp->cpu_burst = prevselec->cpu_burst;
			dmp->turnaround = prevselec->turnaround;
			dmp->wait = prevselec-> wait;
			dmp->start = prevselec ->start;
			dmp->end = current_time;
			dmp->remburst = prevselec->remburst;
			dmp->link = NULL;
				
			addgNode (&ghead);
			
			if (prevselec->remburst == 0) { //dequeue the process from the list
				
				
			}
				
			//take the note of the no
				
					
		}
		
		selec->start = current_time;
		selec->wait = current_time -  selec->arrival; 												//doubt here
		selec->remburst = selec->remburst - 1;
		selec->turnaround += 1;
		isSelec = true;
		
		
		//execute for the selected period of time
		current_time++;
		
		
	}//scheduling is done
	*/
