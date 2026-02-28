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
void evaluate_expression(char** args);
void execute_shell_builtin(char** args);
void execute_command(char** args, bool foreground);

int main() {
    // register_child_signal(on_child_exit())
    // Note: You will bind SIGCHLD to on_child_exit here
    
    setup_environment();
    shell();
    
    return 0;
}

void on_child_exit(int sig) {
    // reap_child_zombie()
    // write_to_log_file("Child terminated")
}

void setup_environment() {
    // cd(Current_Working_Directory)
}

void shell() {
    bool command_is_not_exit = true;
    
    do {
        // read_input()
        // parse_input()
        // evaluate_expression()
        
        // switch(input_type):
        //     case shell_builtin:
        //         execute_shell_builtin();
        //     case executable_or_error:
        //         execute_command();

    } while (command_is_not_exit);
}

char* read_input() {
    // Get raw string from user
    return NULL; 
}

char** parse_input(char* input) {
    // Tokenize the string into an array of arguments
    return NULL;
}

void evaluate_expression(char** args) {
    // Determine if it's a builtin, executable, or background task (&)
}

void execute_shell_builtin(char** args) {
    // switch(command_type):
    //     case cd:
    //     case echo:
    //     case export:
}

void execute_command(char** args, bool foreground) {
    // child_id = fork()
    // if child:
    //     execvp(command parsed)
    //     print("Error")
    //     exit()
    // else if parent and foreground:
    //     waitpid(child)
}