#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include<wait.h>
#include<setjmp.h>
#include<fcntl.h>


jmp_buf env;
 void sig_handler (int signum) { //signal handler for the SIGALRM signal
	fprintf (stderr, "err: read timeout in second child, killing both children (alarm went off)\n");
	longjmp (env, 1);
	exit (1);
}


int main (int argc, char *argv[]) {
	
	int fd1, fd2, buf[2], status, k = 0, flag;
	pid_t pid1, pid2, pid;
	
	if (argc == 1) //do nothing let the thing go on as normal
		;
		
	else if ((argc == 2) && (argv[1][0] == '-')) {
		//if n > 0 then read at most n char
		
		flag = 1; 
	}
	
	else if ((argc == 2) && (argv[1][0] != '-')){ //only one infile is there
		if (fd1 = open (argv[1], O_RDONLY) < 0) {
			fprintf (stderr, "err: can't open the  file\n"); 
			exit (1);
		}
		close (0);
		if (dup (fd1) < 0) {
			fprintf (stderr, "error\n");
			exit (1);
		}
	}
	
	else if ((argc == 3) && (argv[1][0] != '-') && (argv[2][0] != '-')) { //two file arguments are there ,... handle same as the qd.c
	
		if ((fd1 = open (argv[1], O_RDONLY)) < 0) {
			fprintf (stderr, "err: can't open the  file\n"); 
			exit (1);
		}
		close (0);
		if (dup (fd1) < 0) {
			fprintf (stderr, "error\n");
			exit (1);
		}
		
		if ((fd2 = creat (argv[2], 0644)) < 0) {
		 	fprintf (stderr, "err: can't creat the file\n");
		 	exit(1);
		}
		close (1);
		if (dup(fd2) < 0) {
			fprintf (stderr, "error\n");
			exit (1);
		}
	}
	
	else if ((argc == 3) && (argv[1][0] == '-') && (argv[2][0] != '>')) { //one infile argument and -n argument
		
		flag = 2;
		
		
		if ((fd1 = open(argv[2], O_RDONLY)) < 0) {
		 	fprintf (stderr, "err:can't open the file\n");
		 	exit(1);
		}
		close (0);
		if (dup (fd1) < 0) {
		 	fprintf (stderr, "error\n");
		 	exit(1);
		}
		 
		 
	} 
	//doubtful case
	else if ((argc == 3) && (argv[1][0] == '-') && (argv[2][0] == '>')) { //one outfile argument and -n argument
		
		flag = 3;
		argv[2][0] = ' ';
		if ((fd2 = creat (argv[2], 0644)) < 0) {
			fprintf (stderr,"err: can't creat the file\n");
			exit(1);
		}
		close (1);
		if (dup(fd2) < 0) {
			fprintf (stderr,"error\n");
			exit(1);
		}
		
	}
	
	else {  //all the 3 arguments are there ,... -n file1 and file2
		
		
		flag = 4;
		if ((fd1 = open(argv[2], O_RDONLY)) < 0){
			fprintf (stderr, "err: can't open the file\n");
			exit (1);
		}
		close (0);
		if (dup (fd1) < 0) {
			fprintf (stderr,"error\n");
			exit(1);
		}
		
		if ((fd2 = creat (argv[3], 0644)) < 0) {
			fprintf (stderr, "err:can't creat the file\n");
			exit(1);
		}
		close(1);
		if (dup (fd2) < 0) {
			fprintf (stderr,"error\n");
			exit(1);
		}
		
	}
	pipe(buf);
	
	
	//forking starts 
	if ((pid1 = fork ()) > 0) {	//parent
		if ((pid2 = fork()) > 0) { //parent
		
			signal(SIGALRM, sig_handler); //register the handler for the SIGALRM
			
			
			if (setjmp (env) != 0) { //longjmp will bring here
				if (kill (pid1, SIGTERM) < 0) {
					fprintf (stderr, "first child already died\n");
				}
				if (kill (pid2, SIGTERM) < 0) { //second child already died due to the SIGPIPE signal
					fprintf (stderr, "second child already died\n");
				}
				while (( pid = wait (&status)) > 0) {
					fprintf (stderr, "child pid = %d reaped with exit status = %d\n", pid, WEXITSTATUS (status));
					
				}
				exit(1);
			}
			
			
			alarm (15);
			close (buf[0]);
			close (buf[1]);
			while ( (pid = wait(&status)) > 0) {
				fprintf (stderr, "child pid = %d reaped with exit status = %d\n",pid,WEXITSTATUS(status));
				if ( WEXITSTATUS(status) == 0) {
					k++;
				}

			}
			alarm(0);
			if (k == 2) {
				fprintf (stderr, "normal children exit\n");
			}
			else if (k < 2) {
				fprintf (stderr, "the child process terminated abnormally did not work out normally as was scheduled\n");
				exit(1);
			}
			
			
		}
		else {	//second child 
			fprintf (stderr, "child2 pid = %d\n", getpid());
			//execl convert with different cases
			
				close (STDOUT_FILENO);
				dup (buf[1]);
				close (buf[1]);
				close (buf[0]); //<<<<closing unnecessary file descrip
				execl ("convert1", "convert1", (char*)NULL);
			
		}
	}
	else {	//first child
		fprintf (stderr, "child1 pid = %d\n", getpid());
		//execl count with different cases
		
		if ((flag == 1) || (flag == 2) || (flag == 3) || (flag == 4)){
			close (STDIN_FILENO);
			dup(buf[0]);
			close (buf[1]);
			execl ("count1", "count1", argv[1], (char *)NULL);
		}
		
		else  { //if (flag == 2)
			close (STDIN_FILENO);
			dup (buf[0]);
			close (buf[1]);
			execl ("count1" ,"count1", (char *)NULL);
		}
		
	}
	
	exit (0);
}
