#include "server.h"
#include "client.h"
#include "pipe.h"
#include "config.h"
#include "player.h"

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
 }
