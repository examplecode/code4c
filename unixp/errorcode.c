#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc ,char * argv[]) 
{
	// fprintf(stderr, "EACCES: %s\n", strerror(EACCES));
	errno = EACCES;
	perror(argv[0]);
	exit(0);
}