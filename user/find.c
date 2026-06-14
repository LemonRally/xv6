#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"

// unsused because i didn't understand the question
// 0 for no, 1 for yes
int is_substring(char *string, char *substring)
{
    int string_len = strlen(string);
    int substring_len = strlen(substring);

    if (substring_len > string_len)
    {
        return 0;
    }

    char *p = string;
    for (int i = 0; i <= string_len - substring_len + 1; i++)
    {
        char temp = *(p + i + substring_len);
        *(p + i + substring_len) = '\0'; // put terminator for strcmp
        if (strcmp(p + i, substring) == 0)
        {
            return 1;
        }
        *(p + i + substring_len) = temp; // revert
    }

    return 0;
}

void find(char *path, char *search_term)
{
    int fd;
    struct stat st;
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    char buf[512], *p;
    struct dirent de;

    if (st.type == T_DIR)
    {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            // printf("%s\n", p);
            if (stat(buf, &st) < 0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0) // skip these
            {
                continue;
            }
            if (st.type == T_DIR) // need to recurse
            {
                // printf("Going to recurse: %s\n", buf);
                find(buf, search_term);
            }
            if (st.type == T_FILE) // print
            {
                if (strcmp(p, search_term) == 0)
                {
                    printf("%s\n", buf);
                }
            }
        }
    }
    else
    {
        printf("Provided path isn't a directory\n");
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc > 3)
    {
        write(1, "Too many arguements\n", 20);
        exit(1);
    }

    else if (argc < 3)
    {
        write(1, "Too little arguements\n", 22);
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}
