#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler1 (int signum)  {
	fprintf (stderr, "i've been killed\n");
	exit(1);
}

void handler2 (int signum) {
	fprintf (stderr, "i've been killed because my pipe reader died\n");
	exit(2);
}

int main (int argc, char *argv[]) {
	int c;
	signal (SIGTERM, handler1);
	signal (SIGPIPE, handler2);
	char arr[BUFSIZ];
	/*while ((c = getchar()) != EOF) {
	
		if ((c >= 'a') && (c <= 'z')) putchar (c - 32);
		else if ((c >= 'A') && (c <= 'Z')) putchar (c + 32);
		else putchar (c);
		
	}*/
	
	
	
	while ((c = read (STDIN_FILENO, arr, BUFSIZ)) > 0) {
		for (int i = 0; i  < c; i++) {
			if ((arr[i] >= 'a') && (arr[i] <= 'z')) arr[i] = arr[i] - 32;
			else if ((c >= 'A') && (c <= 'Z')) arr[i] = arr[i] + 32;
			else ;
		}
		arr[c] = '\0';
		//sleep(10);
		write (STDOUT_FILENO, arr, c);
		
	}
	
	exit(0);
}

