#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	int c, count = 0;
	
	while ((c = getchar()) != EOF) {
	
		if (((c < 'a') || (c > 'z')) && ((c < 'A') || (c > 'Z'))) {
			count++;
		}
		putchar(c);
		
	}
	fprintf (stderr ,"final count = %d\n",count);
	exit(0);
}
