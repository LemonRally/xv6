#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#define NULL 0

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        write(1, "Too many arguements\n", 20);
        exit(1);
    }

    int curr_pipes[2];
    if (-1 == pipe(curr_pipes))
    {
        fprintf(2, "Error creating parent pipes\n");
        exit(1);
    }
    int read_pipe = curr_pipes[0];
    int write_pipe = curr_pipes[1];

    for (int i = 2; i <= 35; i++)
    {
        write(write_pipe, &i, 4);
    }
    close(write_pipe);

    int curr;
    pid_t pid = fork();

    if (pid > 0)
    {
        // master parent - needs to wait for all childs
        close(read_pipe);
        while (-1 != wait(NULL))
        {
        }
        exit(0);
    }
    else if (pid == 0)
    {
        int is_last = 0;
        while (!is_last)
        {
            // child
            int num = 0;
            int next_pipes[2];
            if (-1 == pipe(next_pipes))
            {
                fprintf(2, "Error creating parent pipes\n");
                exit(1);
            }
            write_pipe = next_pipes[1];

            int got_something = 0;
            while (0 < read(read_pipe, &curr, 4))
            {
                if (num == 0)
                {
                    num = curr;
                    fprintf(1, "prime %d\n", curr);
                    got_something = 1;
                }
                else
                {
                    if (curr % num != 0)
                    {
                        write(write_pipe, &curr, 4);
                    }
                }
            }
            if (!got_something)
            {
                is_last = 1;
            }
            close(read_pipe);
            read_pipe = next_pipes[0];
            close(write_pipe); // close write side
            pid_t child_pid = fork();
            if (child_pid > 0)
            {
                // parent
                close(read_pipe);
                wait(NULL);
                exit(0);
            }
        }
        close(read_pipe);
        exit(0);
    }

    exit(0);
}
