#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    write(1, "Too many arguements\n", 20);
    exit(1);
  }

  int parent_pipes[2];
  if (-1 == pipe(parent_pipes))
  {
    fprintf(2, "Error creating parent pipes\n");
    exit(1);
  }
  int child_pipes[2];
  if (-1 == pipe(child_pipes))
  {
    fprintf(2, "Error creating parent pipes\n");
    exit(1);
  }

  pid_t child_pid = fork();
  if (child_pid < 0)
  {
    fprintf(2, "Error forking\n");
  }
  // child
  if (child_pid == 0)
  {
    close(parent_pipes[1]); // close write side
    close(child_pipes[0]);  // close read side
    pid_t pid = getpid();
    char byte;
    int ret;
    do
    {
      ret = read(parent_pipes[0], &byte, 1);
    } while (1 != ret);
    close(parent_pipes[0]);

    printf("%d: received ping\n", pid);
    write(child_pipes[1], &byte, 1);
    close(child_pipes[1]);

    exit(0);
  }
  // parent
  close(parent_pipes[0]); // close read side
  close(child_pipes[1]);  // close write side
  pid_t pid = getpid();
  char byte = 'b';
  int ret;
  ret = write(parent_pipes[1], &byte, 1);
  close(parent_pipes[1]);

  do
  {
    ret = read(child_pipes[0], &byte, 1);
  } while (1 != ret);
  close(child_pipes[0]);

  printf("%d: received pong\n", pid);

  exit(0);
}
