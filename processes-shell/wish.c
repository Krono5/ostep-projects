#include "wish.h"

char *read_string;
char *path[PATH_SIZE];
const char *operator = ">";
char *redirect_args;

int main(int argc, char *argv[]) {
    size_t buffer_size = 50;
    char *separated_components[buffer_size];
    char command_path[PATH_SIZE];

    // set up initial path
    path[0] = "/bin";
    read_string = malloc(buffer_size * sizeof(char));

    for (int i = 0; i < buffer_size; ++i) {
        separated_components[i] = NULL;
    }

    // if there is nothing in the command line, attempt to read file and redirect to stdin
    if (argc != 1) {
        FILE *input_file = NULL;
        input_file = freopen(argv[1], "r", stdin);
        if (argv[2] != NULL) {
            // Multiple batch files
            print_error();
            return (EXIT_FAILURE);
        } else if (input_file == NULL) {
            // Could not open file
            print_error();
            return (EXIT_FAILURE);
        } else {
            stdin = input_file;
        }
    }
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        // interactive mode
        if (argc == 1) {
            printf("wish> ");
        }
        // Get input from stdin

        char *formattedString = format_string(read_string, buffer_size);
        bool is_parallel = strstr(formattedString, "&") != NULL;

        if (is_parallel) {
            char *command = remove_spaces(strtok(formattedString, "&"));
            pid_t pidArray[100];
            memset(&pidArray, 0, sizeof(pid_t) * 100);
            int countpid = 0;
            while (command != NULL && *command != '\0') {
                break_string(command, separated_components);
                //----------------Checking for redirects----------------
                bool redirection = is_redirection(separated_components);
                //---------------------------------------------------------
                pid_t pid = fork();
                if (pid < 0) {
                } else if (pid == 0) {
                    executeCommand(command_path, separated_components, redirection);
                    exit_shell();
                } else {
                    command = remove_spaces(strtok(NULL, "&"));
                    pidArray[countpid] = pid;
                    countpid++;
                }
            }
            for (int i = 0; i < 100; ++i) {
                pid_t pid = pidArray[i];
                if (pid > 0) {
                    waitpid(pid, NULL, 0);
                }
            }
        } else {
            break_string(formattedString, separated_components);
            //----------------Checking for redirects----------------
            bool redirection = is_redirection(separated_components);
            //---------------------------------------------------------
            executeCommand(command_path, separated_components, redirection);
        }
    }
}

void executeCommand(char *command_path, char **separated_components, bool isRedirection) {
    if (strcmp(separated_components[0], "exit") == 0) {
        if (separated_components[1] != NULL) {
            print_error();
        } else {
            exit_shell();
        }
    } else if (strcmp(separated_components[0], "cd") == 0) {
        change_directory(separated_components);
    } else if (strcmp(separated_components[0], "path") == 0) {
        change_path(separated_components);
    } else {
        // any other shell command
        if (check_path(command_path, separated_components) == false) {
            print_error();
        }
        pid_t return_pid = fork();
        if (return_pid < 0) {
            // fail to fork
            exit_shell();
        } else if (return_pid == 0) {
            // in the universe of the child
            if (isRedirection) {
                creat(redirect_args, S_IRWXU);
            }
            execv(command_path, separated_components);
            exit_shell();
        } else {
            // in the universe of the parent
            wait(NULL);
        }
    }
}


/**
 * Break an input_string string into components removing the newline if it exists
 * @param input_string input_string string to break
 * @param components address of where to stash the components
 */
void break_string(char *input_string, char *components[]) {
    char delimiter = ' ';
    int i = 0;
    bool exit = false;

    // remove newline if it exists
    input_string[strcspn(input_string, "\n")] = 0;
    while (exit == false) {
        components[i] = strsep(&input_string, &delimiter);
        if (components[i] == NULL) {
            exit = true;
        } else if (strcmp(components[i], "") == 0) {
            components[i] = NULL;
        } else {
            i++;
        }
    }
}


/**
 * Print an error message to stderr
 */
void print_error() {
    char *error_message = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/**
 * Built in command Change Directory
 * @param components input arguments of which directory to go to
 */
void change_directory(char *components[]) {
    // if no arguments to cd or cd with more than 1 argument
    if (components[1] == NULL || components[2] != NULL) {
        print_error();
    }
    // if directory does not exist
    if (chdir(components[1]) == -1) {
        errno = ENOTDIR;
    }
}

/**
 * Change the existing path variable
 * @param components arguments for the new path to be changed to
 */
void change_path(char *components[]) {
    bool exit = false;
    int i = 0;

    while (exit == false) {
        if (components[i] == NULL) {
            // If nothing is entered into the path, set path to nothing
            path[i] = NULL;
            exit = true;
        } else {
            // Otherwise, set path variable to everything entered in path
            path[i] = malloc(strlen(components[i]));
            strcpy(path[i], components[i]);
            i++;
        }
    }
}

/**
 * Check if an argument exists in the current path
 * @param check_path full path to check for an argument
 * @param components input arguments to check if command can be executed in the path
 * @return true with the full path if exists within path. false if it does not exist in path
 */
bool check_path(char *check_path, char *components[]) {
    for (int i = 0; i < PATH_SIZE; ++i) {
        if (path[i] != NULL) {
            strcpy(check_path, path[i]);
            strcat(check_path, "/");
            strcat(check_path, components[0]);
            if (access(check_path, X_OK) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool is_redirection(char *components[]) {
    int i = 0;
    int num_operators = 0;
    bool internal_flag = false;
    while (components[i] != NULL) {
        if (strcmp(components[i], operator) == 0) {
            num_operators++;
        } else if (strstr(components[i], operator) != NULL) {
            internal_flag = true;
            num_operators++;
        }
        i++;
    }

    if (num_operators == 1) {
        if (internal_flag) {
            // SPACES IN BETWEEN >
            restructure_components(components);
        } else {
            // NO SPACES IN BETWEEN >
            int index = 0;
            while (strcmp(components[index], operator) != 0) {
                index++;
            }
            if (index == 0 || components[index + 2] != NULL || components[index + 1] == NULL) {
                print_error();
                exit_shell();
            } else {
                redirect_args = realloc(redirect_args, sizeof(components[index + 1]));
                strcpy(redirect_args, components[index + 1]);
            }
        }
        return true;
    } else if (num_operators == 0) {
        return false;
    } else {
        print_error();
        exit_shell();
        return false;
    }
}

void restructure_components(char *components[]) {
    int i = 0;
    while (strstr(components[i], operator) == NULL) {
        i++;
    }
    strcpy(components[i], strtok(components[i], operator));
    char *tempstr = strtok(NULL, operator);
    redirect_args = realloc(redirect_args, sizeof(tempstr));
    strcpy(redirect_args, tempstr);
}

/**
 * Close and free file pointers and exit gracefully
 */
void exit_shell() {
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    free(redirect_args);
    exit(EXIT_SUCCESS);
}

bool isSpace(char c) {
    if (c == ' ' || c == '\n' || c == '\r') {
        return true;
    } else {
        return false;
    }
}

char *remove_spaces(char *input_string) {
    if (input_string == NULL) {
        return NULL;
    }
    char *stringPtr = input_string;
    while (isSpace(*stringPtr)) {
        stringPtr++;
    }
    char *ptr = stringPtr;
    char *endPtr = stringPtr;
    while (*ptr++ != '\0') {
        if (!isSpace(*ptr)) {
            endPtr = ptr;
        }
    }
    *endPtr = '\0';
    return stringPtr;
}

char *format_string(char *input_line, size_t buffer_size) {
    bool exit = false;
    int stringIndex;
    while (exit == false) {
        stringIndex = 0;
        input_line = getInput(buffer_size, input_line);
        if (input_line == NULL) {
            exit_shell();
        }
        while (isspace(input_line[stringIndex]) != 0) {
            stringIndex++;
        }
        if (stringIndex != strlen(input_line)) {
            exit = true;
        }
    }
    return input_line + stringIndex;
}

char *getInput(size_t buffer_size, char *input_line) {
    int result = (int) getline(&input_line, &buffer_size, stdin);
    if (result == -1) {
        return NULL;
    } else {
        return input_line;
    }
}

