#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	int c;
	
	while ((c = getchar()) != EOF) {
	
		if ((c >= 'a') && (c <= 'z')) putchar (c - 32);
		else if ((c >= 'A') && (c <= 'Z')) putchar (c + 32);
		else putchar (c);
		
	}
	
	exit(0);
}
