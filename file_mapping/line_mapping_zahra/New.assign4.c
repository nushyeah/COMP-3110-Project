#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>      // for open(), O_WRONLY, O_CREAT, O_TRUNC
#include <sys/wait.h>   // for wait()
#include <string.h>
#include <errno.h>
#include <limits.h>

#define BUF_SIZE 1024              // renamed constant & comment
#define EXIT_EMPTY_LINE 55         // explicit exit code for empty/space-only input
#define EXIT_CHILD_ERROR 1

// forward declarations
void processLine(char *line);
static void trimNewline(char *line, int *length);
static int isOnlyWhitespace(const char *line, int length);

void processLine(char *line) {  // renamed from childFunction
    const char *childMsg = "Child: processing the sentence for my parent...\n";
    write(STDOUT_FILENO, childMsg, strlen(childMsg)); // notify that we are in child process

    int length = strlen(line); // length of the input line
    trimNewline(line, &length); // remove trailing newline, if any

    // check if the line is empty or contains only spaces / tabs
    if (isOnlyWhitespace(line, length)) {
        const char *warn = "Child: received an empty or whitespace-only line.\n";
        write(STDOUT_FILENO, warn, strlen(warn));
        exit(EXIT_EMPTY_LINE);
    }

    // reverse the input string
    char reversed[BUF_SIZE];
    for (int pos = 0; pos < length; pos++) {
        reversed[pos] = line[length - 1 - pos];
    }
    reversed[length] = '\0';

    // show current directory to the user
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        write(STDOUT_FILENO, "Writing reversed sentence to code.txt in: ", 43);
        write(STDOUT_FILENO, cwd, strlen(cwd));
        write(STDOUT_FILENO, "\n", 1);
    }

    // open the file for writing
    int outFd = open("code.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outFd < 0) {
        perror("Child: file open error (write)");
        exit(EXIT_CHILD_ERROR);
    }

    // write result and close file
    write(outFd, reversed, strlen(reversed));
    write(outFd, "\n", 1);
    close(outFd);

    const char *doneMsg = "Child: successfully wrote coded sentence to file.\n";
    write(STDOUT_FILENO, doneMsg, strlen(doneMsg));

    exit(0);
}

int main(void) {
    char line[BUF_SIZE]; // buffer to store the input line

    for (;;) { // infinite loop using for(;;) instead of while(1)
        const char *prompt =
            "This program codes a sentence.\n"
            "Example: Good day, my name is BigFoot\n"
            "Please type your sentence and press Enter:\n";
        write(STDOUT_FILENO, prompt, strlen(prompt));

        int bytes = read(STDIN_FILENO, line, BUF_SIZE);
        if (bytes < 0) {
            perror("Parent: read error");
            exit(EXIT_CHILD_ERROR);
        } else if (bytes == 0) {
            const char *eofMsg = "Parent: detected EOF on stdin, exiting program.\n";
            write(STDOUT_FILENO, eofMsg, strlen(eofMsg));
            break; // exit the loop if user sends EOF (Ctrl+D)
        }

        line[bytes] = '\0'; // terminate the string

        pid_t pid = fork(); // create a child process

        if (pid < 0) {
            perror("Parent: fork failed");
            exit(EXIT_CHILD_ERROR);
        } else if (pid == 0) {
            // child
            processLine(line);
        } else {
            // parent
            const char *msg =
                "Parent: created a child process, waiting for it to finish...\n";
            write(STDOUT_FILENO, msg, strlen(msg));

            int status;
            wait(&status);

            if (WIFEXITED(status)) {
                int code = WEXITSTATUS(status);
                if (code == EXIT_EMPTY_LINE) {
                    const char *err = "Error: sentence missing or only spaces.\n";
                    write(STDOUT_FILENO, err, strlen(err));
                    continue; // ask again
                }

                // child completed normally, try to read from code.txt
                int inFd = open("code.txt", O_RDONLY);
                if (inFd < 0) {
                    perror("Parent: file open error (read)");
                    exit(EXIT_CHILD_ERROR);
                }

                char buffer[BUF_SIZE];
                int n;
                while ((n = read(inFd, buffer, BUF_SIZE)) > 0) {
                    write(STDOUT_FILENO, buffer, n);
                }
                close(inFd);

                const char *sep = "----------------------------------------\n";
                write(STDOUT_FILENO, sep, strlen(sep));
            }
        }
    }

    return 0;
}

// helper: remove trailing newline if present
static void trimNewline(char *line, int *length) {
    if (*length > 0 && line[*length - 1] == '\n') {
        line[*length - 1] = '\0';
        (*length)--;
    }
}

// helper: check if a line is only whitespace
static int isOnlyWhitespace(const char *line, int length) {
    for (int index = 0; index < length; index++) {
        if (line[index] != ' ' && line[index] != '\t') {
            return 0;
        }
    }
    return 1;
}
