#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h> /* wait */
#include <signal.h>
#include <strings.h>

// WARNING: manca la gestione degli ERRORI

int go = 1;
int msd; // master socket descriptor

void sighand(int sig)
{
  if (sig == SIGINT)
  {
    printf("you typed CTRL-C ... closing master socket descriptor.\n");
    close(msd);
  }
  else if (sig == SIGCHLD)
  {
    printf("received signal SIGCHLD.\n");
  }
}

int main()
{

  int csd; // client socket descriptor

  int status, ret;

  pid_t pid;

  char cmd[256], buf[256];

  struct sockaddr_in sa;

  signal(SIGINT, sighand);
  signal(SIGCHLD, sighand);

  // create master socket
  msd = socket(AF_INET, SOCK_STREAM, 0);

  // init address
  bzero(&sa, sizeof(struct sockaddr_in)); // clear structure

  sa.sin_family = AF_INET;
  sa.sin_port = htons(5193);
  sa.sin_addr.s_addr = inet_addr("127.0.0.1"); // automatically fill with my IP

  // bind socket to an address
  bind(msd, (struct sockaddr *)&sa, sizeof(sa));

  // create a request queue of 10 elements
  listen(msd, 10);

  while (go)
  {

    // II argomento puntatore indirizzo del client remoto
    csd = accept(msd, NULL, 0);

    if (csd < 0)
    {

      go = 0;
    }
    else
    {
      pid = fork();

      if (pid == 0)
      { // child process

        close(msd); // closing master socket

        // read command of maximum 1000 characters
        read(csd, buf, 256);

        sprintf(cmd, "/bin/%s", buf);

        // execute command
        fprintf(stderr, "executing commmand: %s\n", cmd);

        ret = 0;

        if (execl(cmd, cmd, NULL) < 0)
        {
          fprintf(stderr, "ERROR: executing %s command\n", cmd);
          ret = -1;
        }

        return (ret);
      }
      else
      { // father process

        // wait ends of command execution
        wait(&status);

        // send back result
        write(csd, &status, sizeof(status));

        // ends communication
        close(csd);
      }
    }
  }

  return 0;
}
