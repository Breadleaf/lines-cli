#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define IF_ENV(envvar) if (getenv(envvar) != NULL)

void run_command(const char *, char *const[], int, int);

int main(int argc, char *argv[]) {
    IF_ENV("DEBUG") {
        for (size_t i = 1; i < argc; i++) {
            fprintf(stderr, "PATTERN[%lu] = \"%s\"\n", i, argv[i]);
        }
    }

    if (argc < 2) {
        fprintf(stdout, "usage: %s [glob 1] ... [glob N]\n", argv[0]);
        exit(1);
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe()");
        return 1;
    }

    int patterns = argc - 1;           // count of glob arguments
    int extras = patterns - 1;         // number of '-o'
    int find_argc = 4 + extras * 3 + 2 // +2 for "-type f"
                    + 1;               // +1 for NULL

    IF_ENV("IGNORE_GIT") {
        find_argc += 3; // add flags: -not -path "./.git*"
    }

    char **find_argv = malloc(find_argc * sizeof(char *));
    if (find_argv == NULL) {
        perror("malloc()");
        exit(1);
    }

    find_argv[0] = "find";
    find_argv[1] = ".";
    bool first_has_slash = strchr(argv[1], '/') != NULL;
    find_argv[2] = first_has_slash ? "-path" : "-name";
    find_argv[3] = argv[1];

    for (size_t i = 0; i < extras; i++) {
        size_t slot = 4 + i * 3;
	bool has_slash = strchr(argv[i + 2], '/') != NULL;
        find_argv[slot + 0] = "-o";
        find_argv[slot + 1] = has_slash ? "-path" : "-name";
        find_argv[slot + 2] = argv[i + 2];
    }

    IF_ENV("IGNORE_GIT") {
        find_argv[find_argc - 6] = "-not";
        find_argv[find_argc - 5] = "-path";
        find_argv[find_argc - 4] = "./.git*";
    }

    find_argv[find_argc - 3] = "-type";
    find_argv[find_argc - 2] = "f";
    find_argv[find_argc - 1] = NULL;

    IF_ENV("DEBUG") {
        printf("Running 'find' command:\n(");
        for (size_t i = 0; find_argv[i] != NULL; i++) {
            printf("%s", find_argv[i]);
            if (find_argv[i + 1] != NULL) {
                printf(" ");
            }
        }
        printf(")\n");
    }

    run_command("find", (char *const *)find_argv, STDIN_FILENO, pipefd[1]);
    close(pipefd[1]);
    free(find_argv);

    char *const xargs_argv[] = {"xargs", "wc", "-l", NULL};

    IF_ENV("DEBUG") {
        printf("Running 'xargs' command:\n(");
        for (size_t i = 0; xargs_argv[i] != NULL; i++) {
            printf("%s", xargs_argv[i]);
            if (xargs_argv[i + 1] != NULL) {
                printf(" ");
            }
        }
        printf(")\n");
    }

    run_command("xargs", xargs_argv, pipefd[0], STDOUT_FILENO);
    close(pipefd[0]);

    for (int i = 0; i < 2; i++) {
        int status;
        wait(&status);
    }

    return 0;
}

void run_command(const char *file, char *const argv[], int in_fd, int out_fd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }

    // child
    if (pid == 0) {
        if (in_fd != STDIN_FILENO) {
            if (dup2(in_fd, STDIN_FILENO) < 0) {
                perror("dup2() - stdin");
                _exit(1);
            }
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("dup2() - stdout");
                _exit(1);
            }
            close(out_fd);
        }
        execvp(file, argv);
        perror("execvp()");
        _exit(1);
    } else {
        // parent: leave closing of in_fd / out_fd to caller
    }
}
