#include "server.h"
#include "client.h"
#include "pipe.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {
   if (argc == 2 && strcmp(argv[1], "server") == 0) {
     run_server();
   } else {
     run_client();
     wait(NULL);
  }
  return 0;
 
   const pid_t pid = fork();

   if (pid < 0) {
      perror("ERROR: Incorrect PID from fork!");
      return EXIT_FAILURE;
   }

   if (pid == 0) {
      run_server();
   } else {
      run_client();
      wait(NULL);
	
      printf("Bye\n");
   }

   return 0;
}
