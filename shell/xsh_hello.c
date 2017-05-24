/* xsh_hello.c - xsh_hello */

#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * xsh_hello - hello command simply work like echo and prints the input string on console in a welcome message
 *----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 */
shellcmd xsh_hello(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s <sring> \n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays the welcome message with input string:\n");
		printf("Hello <string>, Welcome to the world of Xinu!!\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 2) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}
	
	if (nargs < 2) {
		fprintf(stderr, "%s: missing <string> argument\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}

	printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
	//test clktime
	printf("Time Now:  %d \n",clktime);
	return 0;
}
