#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void handler_usr (int signum) {
	fprintf (stderr, "count: invalid -n argument was passed, i am exiting\n");
	exit(2);
}

void handler1 (int signum) {
	fprintf (stderr, "i've been killed\n");
	exit(1);
}

int main (int argc, char *argv[]) {
	int c, count = 0, tmp, tmp_count = 1;
	signal (SIGTERM, handler1);
	signal (SIGUSR1, handler_usr); 
	
	//if the argument -n the n is negative then do as per the requirement
	if (argc == 1) {
		while ((c = getchar()) != EOF) {
	
		if (((c < 'a') || (c > 'z')) && ((c < 'A') || (c > 'Z'))) {
			count++;
		}
		putchar(c);
		
		}
	}
	else if (argc == 2) {
		tmp = atoi (argv[1]);
		if (tmp >= 0) {
			raise (SIGUSR1);
		}
		tmp = -1 * tmp;
		//write at most n characters
		while (((c = getchar()) != EOF) && (tmp_count <= tmp)) {
	
			if (((c < 'a') || (c > 'z')) && ((c < 'A') || (c > 'Z'))) {
				count++;
			}
			tmp_count++;
		putchar(c);
		
		}
	}
	
	
	fprintf (stderr ,"final count = %d\n",count);
	exit(0);
}

