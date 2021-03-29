#include <stdio.h>

int main (int argc, char *argv[]) {
	int i, flag = 0;
	
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			printf ("%s ",argv[i]);
			flag = 1;
		}
		
	}
	if (flag == 1) {
			printf ("\n");
	}
	return 0;
}
