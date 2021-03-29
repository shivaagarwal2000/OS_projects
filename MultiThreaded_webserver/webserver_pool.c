
/*
This is the main program for the simple webserver.
Your job is to modify it to take additional command line
arguments to specify the number of threads and scheduling mode,
and then, using a pthread monitor, add support for a thread pool.
All of your changes should be made to this file, with the possible
exception that you may add items to struct request in request.h
*/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "tcp.h"
#include "request.h"

request* dequeue (request **);

void enqueue (struct request **, request *) ;
void handle_trequest (void *);
void Consume (int *) ;													
void Produce (struct request *);
void sffenqueue (struct request **, request *) ;

int counter = 0;
int first = 1;								
int b_size = 0; 												//currently used buffer size as the global variable
#define b_MAX 10
#define b_MIN 0
pthread_mutex_t mutex;
pthread_cond_t cond;
														//flag = 0 -> FIFO ... flag = 1 -> SFF
int flag;
														// FIFO queue for the thread pool (FIFO scheduling)
struct request* head = NULL; 		

int main( int argc, char *argv[] )
{
	struct stat *f_info = (struct stat*)malloc(sizeof(struct stat));
	int res_stat = 0;
	pthread_t thread1;
	if(argc<4) {
		fprintf(stderr,"use: %s <port> <NTHREADS> <MODE>\n",argv[0]);
		return 1;
	}

	if(chdir("webdocs")!=0) {
		fprintf(stderr,"couldn't change to webdocs directory: %s\n",strerror(errno));
		return 1;
	}
	
	int num_thread = atoi(argv[2]);

	int port = atoi(argv[1]);
	char *mode = argv[3];
	//mode[strlen(mode)-1] = '\0';
	
	flag = ((strcmp("FIFO",mode) == 0)) ? 0: 1;
	if ((strcmp("SFF",mode)!=0) && (strcmp("FIFO",mode) != 0)) {
		fprintf(stderr,"use: valid <MODE>\n");
		return 1;
	}
	

	
	struct tcp *master = tcp_listen(port);
	if(!master) {
		fprintf(stderr,"couldn't listen on port %d: %s\n",port,strerror(errno));
		return 1;
	}

	printf("webserver: waiting for requests.. \n");
	
	//printf ("main - %ld\n",pthread_self());								//print the thread ID of the main
	
														//create N threads for pool
	
	for (int i = 1; i <= num_thread; i++) {
				
		pthread_create (&thread1,NULL,(void *)&Consume, (void*)&i);
		
	}
	
	
	while(1) {
		struct tcp *conn = tcp_accept(master,time(0)+300);
		if(conn) {
			printf("\nwebserver: got new connection.\n");
			struct request *req = request_create(conn);
			
			if(req) {	
														//if request struct was created successfully
				printf("webserver: got request for %s\n",req->filename);
				req->link = NULL;
				
				if ( (res_stat = stat(req->filename,f_info)) < 0) {
					fprintf (stderr,"err: can't get the file info\n");
				}
			
				/*
					proper handling of different scenario of
					errors.
				
				
				
				*/
				
				req->fsize = f_info->st_size;
				//printf("\n%ld is the Bytes used - %s\n\n",f_info->st_size,req->filename);
				
				Produce(req);
				
				/*										//code to be present in the worker thread to make it do the work ... start
				request_handle(req);
				printf("webserver: done sending %s\n",req->filename);
				request_delete(req);	
				*/
												
														//... end
				
				
			} else {
				tcp_close(conn);
			}
		} else {
			printf("webserver: shutting down because idle too long\n");
			break;
		}
	}

	return 0;
}
														//Main Routine or the Producer will produce
void Produce (struct request *req) {										
	
	pthread_mutex_lock (&mutex);
	
														//while the buffer is full ... wait for some consumer to eat
	while (b_size == b_MAX) {											
		pthread_cond_wait(&cond,&mutex);
	}
														//insert into the buffer and then release the lock
	if (flag == 0) { 
		enqueue (&head,req);
	}
	else {
		sffenqueue (&head,req);
	}

	b_size++;
														//signal only if buffer was initially empty
	if (b_size == 1) {
		pthread_cond_signal (&cond);	
	}		
									
	pthread_mutex_unlock (&mutex);
	
} 
														//Worker thread will consume in some order
void Consume (int *position) {													
	request *dummy = NULL;
												
	pthread_mutex_lock (&mutex);
	
	if (counter == 0) {
		printf ("S.NO.\tDesc.\t\t ThreadID\n");
	}
	counter++;
	printf ("%d - new worker thread - %ld\n",counter,pthread_self());

	
	pthread_mutex_unlock(&mutex);
	/*
	if (first == 1) {
		sleep (20);	
		first++;					
	}
	*/
	request *tmp = head;
	while (tmp != NULL) {
		printf ("%s ... ",tmp->filename);
		tmp = tmp->link;
	}
								
	while (1) {
	
		pthread_mutex_lock (&mutex);
														//if the buffer is empty ...  wait for producer to produce
		while (b_size == b_MIN) {											
			pthread_cond_wait (&cond,&mutex);							
		}
														//consume from the buffer according to some scheduling
		/*
		if then else construct with the FLAG for differenciating between the scheduling
		*/
		
		

		dummy = dequeue (&head);										
		b_size--;	
				
		//printf ("\n T_id: %ld -- worker thread going to service -- FILE_NAME - %s\n\n",pthread_self(),dummy->filename); 

		pthread_cond_signal (&cond);									
		pthread_mutex_unlock (&mutex);																			
														//handle the request
		handle_trequest (dummy);
		
	}

}
														//function for the newly created threads//function for the newly created threads
void handle_trequest (void *req) {										
	struct request *rstruct = (struct request *)req;
														//print the thread ID of the newly created thread
	//printf ("T_id: %ld worker thread \n\n",pthread_self());							
	request_handle(rstruct);
	printf("\nwebserver: done sending %s\n",rstruct->filename);
	request_delete(rstruct);
		
}


//for the FIFO operations
request* dequeue (request **head) {
	request *tmp = (*head);
	
	if ((*head)->link == NULL) {
		(*head) = NULL;
		return tmp;
	}
	
	(*head) = (*head)->link;
	return tmp;
	
}
void enqueue (struct request **head, request *req) {

	if (*head == NULL) {
		(*head) = req;
		
		return;
	}
	request *tmp = (*head);
	
	while (tmp->link != NULL) {
		tmp = tmp->link;
	}
	tmp->link = req;
	return;
}


void sffenqueue (struct request **head, request *req) {								//special enqueue operation for the SFF putting files always in sequential order
	
	if (*head == NULL) {
		(*head) = req;
		return;
	}
	
	request *tmp = (*head), *ptmp = tmp;
	if (tmp->link == NULL) {
		if (tmp->fsize > req->fsize) {
			req->link = tmp;
			(*head) = req;
		}
		else {
			tmp->link = req;
		}
		return;
	}
	
	if (tmp->fsize > req->fsize) {
		req->link = tmp;
		(*head) = req;
		return;
	}
	
	while ((tmp != NULL) && (tmp->fsize < req->fsize)) {
		ptmp = tmp;
		tmp = tmp->link;
	}
	
	//if last element
	if (tmp->link == NULL) {
		if (tmp->fsize > req->fsize) {
			req->link = ptmp->link;
			ptmp->link = req;
		}
		else {
			tmp->link = req;
		}
		return;
	}
	
	req->link = tmp;
	ptmp->link = req;
	
	return;

}

