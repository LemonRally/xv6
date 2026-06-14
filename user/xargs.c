#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        write(1, "Too little arguements\n", 22);
        exit(1);
    }

    // for (int i = 0; i < MAXARG; i++)
    // {
    //     printf("%d: %s\n", i, argv[i]);
    // }

    char buf[512];
    char *start = buf;
    char *end = buf;
    while (0 < read(0, end, 1))
    {
        if (*end == '\n')
        {

            *end = '\0';
            char *new_argv[MAXARG];
            for (int i = 0; i < argc - 1; i++)
            {
                new_argv[i] = argv[i + 1];
            }
            // now append the new one
            new_argv[argc - 1] = start;

            // for (int i = 0; i < MAXARG; i++)
            // {
            //     printf("%d: %s\n", i, new_argv[i]);
            // }

            pid_t pid = fork();
            if (pid == 0)
            {
                // child
                exec(new_argv[0], new_argv); // second parameter should include new_argv[0]?
            }
            else
            {
                // parent
                wait(0);
            }

            start = end + 1;
        }

        if (end - buf == 512)
        {
            // can't increment, end will overflow
            fprintf(2, "Too much info on stdin");
            exit(1);
        }

        end++;
    }

    exit(0);
}