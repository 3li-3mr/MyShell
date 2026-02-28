#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

void on_child_exit(int sig);
void setup_environment();
void shell();
char* read_input();
char** parse_input(char* input);
int evaluate_expression(char** args, bool* foreground);
void execute_shell_builtin(char** args);
void execute_command(char** args, bool foreground);

char abs_path[1024];
struct Map{
    char key[64];
    char value[256];
};
struct Map vars[100];
int var_count = 0;

int main() {
    signal(SIGCHLD, on_child_exit);
    setup_environment();
    shell();
    
    return 0;
}

void on_child_exit(int sig) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void setup_environment() {
    if(getcwd(abs_path, sizeof(abs_path)) != NULL){
        chdir(abs_path);
    }
    else{
        perror("error");
    }
}

void shell() {
    while(1){
        printf("MyShell:%s> ", abs_path);
        char* input = read_input();
        char** args = parse_input(input);
        bool foreground;
        int type = evaluate_expression(args, &foreground);
        if(type == 1){
            execute_shell_builtin(args);
        }
        else if(type == 2){
            execute_command(args, foreground);
        }
        free(input);
        free(args);
    }
}

char* read_input() {
    char* input = malloc(1024 * sizeof(char));
    if(fgets(input, 1024, stdin) == NULL){
        free(input);
        exit(0);
    }

    return input; 
}

char** parse_input(char* input) {
    char** args = malloc(64 * sizeof(char*));
    int i = 0;
    char* token = strtok(input, " \n");
    if(token != NULL && strcmp(token, "export") == 0){
        args[0] = token;
        char* exp = strtok(NULL, "\n");
        if(exp != NULL){
            args[1] = exp;
            args[2] = NULL;
        }
        else{
            args[1] = NULL;
        }
        return args;
    }
    while(token != NULL){
        args[i] = token;
        i++;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;
    return args;
}

int evaluate_expression(char** args, bool* foreground) {
    if(args[0] == NULL){
        return 0;
    }
    int i = 0;
    while(args[i] != NULL){
        if(args[i][0] == '$'){
            char* key = args[i];
            key++;
            bool found = false;
            for(int j = 0; j < var_count; j++){
                if(strcmp(key, vars[j].key) == 0){
                    found = true;
                    args[i] = vars[j].value;
                    break;
                }
            }
            if(!found){
                char* env = getenv(key);
                if(env != NULL){
                    args[i] = env;
                }
                else{
                    args[i] = "";
                }
            }
        }
        i++;
    }
    if(i > 0 && strcmp(args[i-1], "&") == 0){
        *foreground = false;
        args[i-1] = NULL;
    }
    else{
        *foreground = true;
    }

    if(strcmp(args[0], "cd") == 0 || strcmp(args[0], "echo") == 0 || strcmp(args[0], "export") == 0 || strcmp(args[0], "exit") == 0) return 1;

    else{
        return 2;
    }
}

void execute_shell_builtin(char** args) {
    if(strcmp(args[0], "exit") == 0){
        exit(0);
    }
    else if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL || strcmp(args[1], "~") == 0){
            chdir(getenv("HOME"));
        }
        else{
            chdir(args[1]);
        }
        getcwd(abs_path, sizeof(abs_path));
    }
    else if(strcmp(args[0], "export") == 0){
        if(args[1] != NULL){
            char* key = strtok(args[1], "=");
            char* value = strtok(NULL, "=");
            if(key != NULL && value != NULL){
                if(value[0] == '"') value++;
                if(value[strlen(value) - 1] == '"') value[strlen(value) - 1] = '\0';
                if(var_count < 100){
                    strcpy(vars[var_count].key, key);
                    strcpy(vars[var_count].value, value);
                    var_count++;
                } 
            }
        }
    }
    else if(strcmp(args[0], "echo") == 0){
        int i = 1;
        while(args[i] != NULL){
            char* exp = args[i];
            if(exp[0] == '"') exp++;
            if(exp[strlen(exp)-1] == '"') exp[strlen(exp)-1] = '\0';
            printf("%s ", exp);
            i++;
        }
        printf("\n");
    }

}

void execute_command(char** args, bool foreground) {
    pid_t pid = fork();
    if(pid == 0){
        execvp(args[0], args);
        perror("error");
        exit(0);
    }
    else if(pid > 0 && foreground){
        waitpid(pid, NULL, 0);
    }
}