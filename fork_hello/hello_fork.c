/* CELEBF27

   This example creates a new child process.

 */

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

main() {
  pid_t pid;
  int status;

  if ((pid = fork()) < 0)
    perror("fork() error");
  else if (pid == 0) {
    puts("This is the child.");
    printf("Child's pid is %d and my parent's is %d\n",
           (int) getpid(), (int) getppid());
    sleep(5);
    exit(42);
  }
  else {
    puts("This is the parent.");
    printf("Parent's pid is %d and my child's is %d\n",
           (int) getpid(), (int) pid);
    puts("I'm waiting for my child to complete.");
    if (wait(&status) == -1)
      perror("wait() error");
    else if (WIFEXITED(status))
           printf("The child exited with status of %d\n",
                  WEXITSTATUS(status));
         else
           puts("The child did not exit successfully");
  }
}