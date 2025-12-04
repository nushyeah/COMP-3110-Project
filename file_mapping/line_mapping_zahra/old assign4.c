#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // for open(), O_WRONLY, O_CREAT, O_TRUNC
#include <sys/wait.h> // for wait()
#include <string.h>
#include <errno.h> 
#include <limits.h>

#define MAX_LEN 1024 // defining maximum size of input line
 
void childFunction(char *line) {  // prototype for child function
    const char *childMsg = "I am a child process working for my parent\n";
    write(STDOUT_FILENO, childMsg, strlen(childMsg));

    int len = strlen(line);// Getting the length of the input line

    // Remove newline because read() may include it
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len--;
    }

    // to check if the line is empty or contains only spaces
    int onlySpaces = 1;
    for (int i = 0; i < len; i++) {
        if (line[i] != ' ' && line[i] != '\t') {
            onlySpaces = 0;
            break;
        }
    }

    if (onlySpaces) { // if the line is empty or contains only spaces
        exit(55);
    }

    // Reversing the string as required
    char reversed[MAX_LEN];
    for (int i = 0; i < len; i++) {
        reversed[i] = line[len - 1 - i];
    }
    reversed[len] = '\0';

    // printing the reversed string to stdout
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        write(STDOUT_FILENO, "Writing to code.txt in: ", 24); // printing current working directory 
        write(STDOUT_FILENO, cwd, strlen(cwd)); 
        write(STDOUT_FILENO, "\n", 1);
    }

    // opening the file code.txt for writing
    int fd = open("code.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // 
    if (fd < 0) {
        perror("File open error (write)");
        exit(1);
    }

    write(fd, reversed, strlen(reversed)); // writing the reversed string to the file
    write(fd, "\n", 1);
    close(fd);
    exit(0);
}

int main() {
    char line[MAX_LEN]; // having a buffer to store the input line

    while (1) { // infinite loop to keep asking for input because the program should not terminate
        const char *prompt = "This program codes a sentence, e.g., Good day, my name is BigFoot\n"; // prompt message
        write(STDOUT_FILENO, prompt, strlen(prompt)); // writing the prompt to stdout

        int bytes = read(STDIN_FILENO, line, MAX_LEN); // reading input from stdin
        if (bytes < 0) {
            perror("Read error");
            exit(1); // handling read error
        }

        line[bytes] = '\0'; // terminating the string with null character

        pid_t pid = fork(); // creating a child process 

        if (pid < 0) { // checking if fork failed
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) { // if fork returns 0, we are in the child process
            childFunction(line); // calling the child function to process the input
        } else {
            const char *msg = "I just created a child to perform a task, waiting for my child...\n";
            write(STDOUT_FILENO, msg, strlen(msg));

            int status; // storing the status of the child process
            wait(&status);
            if (WIFEXITED(status)) { // checking if the child process exited normally
                int code = WEXITSTATUS(status); // getting the exit status of the child process
                if (code == 55) { // 
                    const char *err = "Error, sentence missing\n";
                    write(STDOUT_FILENO, err, strlen(err)); // printing error message if the child exited with code 55
                    continue;
                } else { 
                    int fd = open("code.txt", O_RDONLY); // opening the file code.txt for reading
                    if (fd < 0) { // handling file open error
                        perror("File open error (read)");
                        exit(1);
                    }

                    char buffer[MAX_LEN]; // another buffer to read from the file
                    int n; 
                    while ((n = read(fd, buffer, MAX_LEN)) > 0) { // reading from file to buffer
                        write(STDOUT_FILENO, buffer, n);
                    }
                    close(fd); // closing the file descripto
                }
            }
        }
    }

    return 0;
}
