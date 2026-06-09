#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  if (argc > 2)
  {
    write(1, "Too many arguements\n", 20);
    exit(1);
  }
  else if (argc < 2)
  {
    write(1, "Too little arguements\n", 22);
    exit(1);
  }
  int ticks = atoi(argv[1]);
  sleep(ticks);

  exit(0);
}
