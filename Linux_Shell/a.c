#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/types.h>

#define SEMCOL 1
#define AMPERS -1
#define SPACE 2



typedef struct node {
	int data;
	struct node* link;
} node;


void cmdhelper ( char *);
void bash (char *);
void handler (int sig);
void enqueue(node **, int );
int dequeue (node **);
node *newNode (int );
int isEmpty (node *);


int main (int argc, char *argv[]) {
	//int
	char input[BUFSIZ];
	signal (SIGINT, handler);
	
	
	while (1) {
		printf ("CS232/Shell186$ ");
		fgets (input, BUFSIZ, stdin);
		bash (input);
	}
	
	exit(0);
}

void handler (int sig) {
	if (sig == SIGINT) { 
		signal (SIGINT, SIG_DFL);
	}
}

void bash (char *input) {
	signal (SIGINT, handler);
	char *ptr; //tokenise the string 
	char *ptr1;
	bool sctest = false;
	bool amtest = false;
	bool pitest = false;
	bool flag = false;
	int firstapp = 0;
	int cmbtest = 0;
	char *buf; //the buffer for the string tokeniser
	char *ptrbuf;
	char *wd ;
	pid_t pid2;
	
	int val;
	
	//for scanning for the sequential execution	
	
	for (int i = 0; i < strlen(input); i++ ) {
		if (input[i] == ';') {sctest = true; cmbtest++;flag = true;}
		else if (input[i] == '&') {amtest = true;cmbtest++;flag = true;}
		
		
		//else if (input[i] == '|') pitest = true;
		//if ((sctest == true) && (amtest == false)) firstapp = 1;
		//else if ((sctest == false) && (amtest == true)) firstapp = -1;
			
	}
	
	//make queue
	node *head = NULL;
	
	
	//insert the symbols into the queue
	for (int i = 0; i < strlen(input); i++ ) {
		if (input[i] == ';') {enqueue(&head,SEMCOL);flag = true;}
		else if (input[i] == '&') {enqueue(&head,AMPERS);flag = true;}
			
	}
	if (flag == true) {
			while ((!isEmpty(head)) || (buf != NULL)) { //process the queue till its empty
				if ( (!isEmpty(head)) ) { val = dequeue (&head); }//printf("%s %s\n",ptr,buf);}
				else val = 0;
				
				if (val == SEMCOL) {
					ptr = strtok_r (input, ";" ,&buf);
					//printf("1)%s <<>> 2) %s\n",ptr,buf);
					cmdhelper (ptr);
					printf("\n");
					input = NULL;
					/*
					
					while (ptr) { 
						cmdhelper (ptr);
						if ((ptr = strtok_r (NULL, ";" ,&buf))) printf("\n");
						//input = NULL;
					}
					
					
					*/
		 			
				}
				else if (val == AMPERS) {
					ptr = strtok_r (input, "&" ,&buf);
					if ((pid2 = fork()) == 0) { //child goes here and parent will not wait as the process is run in the background
						//printf ("[%d] %d\n",pid2,getpid());
						//printf ("\n%ld %s\n",strlen(ptr),ptr);
						
						cmdhelper(ptr);
						exit(0);
						
					}
					//printf ("hello");
					return;
				}
				else {
					//ptrbuf = strtok_r (input, " ", &buf);
					//if (ptrbuf) cmdhelper (ptrbuf);
					//printf (">>.%s\n",buf);	printf("here");
					//printf ("%d %ld",isEmpty(head),strlen(buf));
					cmdhelper (buf);
					
					buf = NULL;
				}
			}
			
			
	} else {
		cmdhelper (input);
	}
	
	
	
	
	
	//only semicolons are there .. parse and send piece by piece
	
	
	/*	
	if (sctest) {
		ptr = strtok_r (input, ";" ,&buf);
		while (ptr) { 
			cmdhelper (ptr);
			if ((ptr = strtok_r (NULL, ";" ,&buf))) printf("\n");
			//input = NULL;
		}
	}
	//only 1 ampersand came -> run the command in the background
	else if (amtest) {
	//assume that you have the command in the ptr
	
		if ((pid2 = fork()) == 0) { //child goes here and parent will not wait as the process is run in the background
			printf ("[%d]\n",pid2);
			cmdhelper(ptr);
			exit(0);
		}
	
	}
	else {
		cmdhelper (input);	
	}
	*/


}

void enqueue(node **phead, int elem) {
	node *tmp = newNode(elem);
	node *head = *phead;
	node **pphead = phead;
	if (!head) {
		*pphead = *phead = tmp;
		return;
	}
	while (head->link) {
		head = head->link;
	}
	head->link = tmp;
	return;
}

int dequeue (node **phead) {
	node *tmp;
	node *head = *phead;
	node **pphead = phead;
	int dmp;
	if (!(head->link)) {
		dmp = head->data;
		*pphead = *phead = NULL;
		//printf ("inside the dequeue%p %d\n",*phead,dmp);
		return dmp;
	}
	dmp = head->data;
	*pphead = (*pphead) ->link;
	return dmp;
}

node *newNode (int elem) { //make a new node to be inserted and return the pointer to it
	node* tmp = (node*)malloc(sizeof(node));
	tmp->data = elem;
	tmp->link = NULL;
	return tmp;
}

int isEmpty (node *head) {
	//printf ("%p\n",head);
	return (head == NULL)? 1: 0;
}


//if it's an internal command... no fork and execs... use syscalls
void cmdhelper ( char *input) {
		signal (SIGINT, handler);
		//parse and check for the command type
		//for scanning the commands -> 
		int fd1;
		char *ptr;
		char *srcptr;
		char *buf;
		char srcbuf[BUFSIZ];
		char *srcbuff;
		int srccount = 0;
		int wc = 0;
		char *wd ;
		char dmp[BUFSIZ];
		char *dmp_buf;
		int status;
		int csrc;
		
		if (strchr (input, ' ')) ptr = strtok_r (input, " " ,&buf);
		else ptr = strtok_r (input, "\n" ,&buf);
	
		//ptr[strlen(ptr)-1] = '\0';
		//printf ("val %s %d %ld\n",ptr,strcmp(ptr,"echo"),strlen(ptr));
		
		
		pid_t pid1;
		if ( (!strcmp((char*)ptr,"exit")) || (!strcmp((char*)ptr,"cd")) || (!strcmp((char*)ptr,"echo")) || (!strcmp((char*)ptr,"pwd")) || (!strcmp((char*)ptr,"source")) ) {
			
			//printf ("%s %ld\n",input,strlen(input));
			
			
			if (!strcmp((char*)ptr,"exit")) {
				exit(0);
			}
			
			else if (!strcmp((char*)ptr,"cd")) { //first time use the chdir and then proceed via pwd;
				if (chdir(buf) < 0) {
					fprintf (stderr,"err: can't change the directory\n");
				}
			}
			
			else if (!strcmp(ptr,"echo")) {
				//buf = strtok_r (NULL, " ", &buf);
		 		printf ("%s",buf);
			}
			
			else if (!strcmp((char*)ptr,"pwd")) {
				wd = getcwd (wd, BUFSIZ);
				printf ("%s\n",(char*)wd);
			}
			
			else if (!strcmp((char*)ptr,"source")) {
				//open the file and put stdin to it and after doing all the tasks come back to the normal
				
				
				ptr = strtok_r (NULL, " " ,&buf);
				ptr[strlen(ptr)-1] = '\0';
				//printf ("%ld %s\n",strlen(ptr),ptr);
				
				if ((fd1 = open (ptr,O_RDONLY)) < 0) {
					fprintf (stderr,"err: can't open the file\n");
					return;
				}
				if ((srccount = read(fd1, srcbuf, BUFSIZ)) < 0) {
					fprintf (stderr,"error\n");
					return;
				}
				srcbuf[srccount+1] = '\0';
				srcptr = strtok_r (srcbuf, "\n", &srcbuff);
				bash(srcptr);
				while (srcptr = strtok_r (NULL, "\n", &srcbuff)) {
					bash(srcptr);
				}
			}
			
		}
		
		
		//handle diff cases of cat
		else if ((!strcmp((char*)ptr,"cat"))) {
		
		
		}
		
		
		//its name of other pgm, use fork, execs, wait and different cases need to be handled.
		//launch the pgm
		else {
			//count the number of words, args
			
			
			
			//printf ("%ld\n",strlen(ptr));
			//printf ("%s %ld\n",ptr,strlen(ptr));
			
			
			
			
			
			//ptr[strlen(ptr)-1] = '\0';
			
			
			
			int k = 1;
			if (buf != NULL) {
				strcpy(dmp,buf);
				if (strtok_r(dmp, " ", &dmp_buf))
					wc++;
				
				while (strtok_r(NULL, " ", &dmp_buf)) {
					wc++;
				}
				
				//printf("%d\n",wc);
				
				char *args[wc + 2];
				args[0] = ptr;
				
				
				while (args[k] = strtok_r(NULL, " ", &buf)){
					for (int l = 0; l < strlen(args[k]); l++ ) if (args[k][l] == '\n') { args[k][l] = '\0';break;}
					k++;
				}
			
				args[wc + 1] = NULL;
				if ((pid1 = fork()) != 0) { //parent 
					wait(&status);
				}
				else { //child
					//printf ("%s",ptr);
					execvp (ptr,args);
				}
			}		
					

			
		}
}
