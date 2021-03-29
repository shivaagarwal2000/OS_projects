#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
int main (int argc, char *argv[]) {

	if (argc != 3) {
		fprintf (stderr,"not enough arguments\n");
		exit (1);
	}
	
	int fd1, fd2, buf[2], flag1 = 1, flag2 = 1, status;
	
	
	if ((fd1 = open (argv[1], O_RDONLY)) < 0) {
		fprintf (stderr,"err: can't open the file\n");
		exit(1);
	}
	
	if ((fd2 = creat (argv[2], 0644)) < 0) {
		fprintf (stderr,"err: can't create the file\n");
		exit(1);
	}
	
	close (0);
	if (dup (fd1) < 0) {
		fprintf (stderr,"error\n");
		exit(1);
	}
	
	close(1);
	if (dup (fd2) < 0) {
		fprintf (stderr,"error\n");
		exit(1);
	}
	
	pipe(buf);
	
	if (fork() != 0) {
		if (fork() != 0) {
			close (buf[0]);
			close (buf[1]); 
			wait (&status);
			wait (&status);		
		}
		else {
			close (1);
			dup (buf[1]);
			
			close (buf[1]); 
			//close (buf[0]); 
			
			(void)execl ("convert","convert",(char *)0);		
			//execl -> convert, print all the char to stdout
		}
	} else {
		close (0);
		dup (buf[0]);
		close (buf[1]);
		//close (buf[0]);
		
		execl ("count","count",(char *)0);	
		//execl -> count and print the count on stderr, all chars to stdout..
	}
	
	exit(0);
}


















