/*
  * Fresh v. 0.2
  * Based on Simple C shell:
  * Copyright (c) 2013 Juan Manuel Reyes
  * Copyright (c) 2015 Daniele Lacamera
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
// Shell pid, pgid, terminal modes
static pid_t GBSH_PID;
static pid_t GBSH_PGID;
static int GBSH_IS_INTERACTIVE;
static struct termios GBSH_TMODES;


struct sigaction act_child;
struct sigaction act_int;

int no_reprint_prmpt;

pid_t pid;

struct env {
    char *key;
    char *value;
    struct env *next;
};


struct env *Environment = NULL;

/**
 * SIGNAL HANDLERS
 */
// signal handler for SIGCHLD */
void signalHandler_child(int p);
// signal handler for SIGINT
void signalHandler_int(int p);
int changeDirectory(char * args[]);

#define LIMIT 16 // max number of tokens for a command
#define MAXLINE 256 // max number of characters from user input
static char currentDirectory[128];
static char lastcmd[128] = "";

int puts_r(struct _reent *r, const char *s)
{
    return strlen(s);
}


/**
 * Function used to initialize our shell. We used the approach explained in
 * http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
void shell_init(char *file){
    int stdin_fileno, stdout_fileno, stderr_fileno;

    if (file) {
        do {
            stdin_fileno = open(file, O_RDONLY, 0);
        } while (stdin_fileno < 0);

        do {
            stdout_fileno = open(file, O_WRONLY, 0);
        } while (stdout_fileno < 0);

        do {
            stderr_fileno = open(file, O_WRONLY, 0);
        } while (stderr_fileno < 0);
    }

    // See if we are running interactively
    GBSH_PID = getpid();
    // The shell is interactive if STDIN is the terminal
    GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);

    if (!GBSH_IS_INTERACTIVE) {
        printf("Warning: this shell is not a TTY.\r\n");
    }
}

/**
 * Method used to print the welcome screen of our shell
 */
void welcomeScreen(){
        printf("\r\n\t============================================\r\n");
        printf("\t          Frosted shell - aka \"Fresh\"         \r\n");
        printf("\t--------------------------------------------\r\n");
        printf("\t             Licensed under GPL             \r\n");
        printf("\t============================================\r\n");
        printf("\r\n\r\n");
}

/**
 * SIGNAL HANDLERS
 */

/**
 * signal handler for SIGCHLD
 */
void signalHandler_child(int p){
    /* Wait for all dead processes.
     * We use a non-blocking call (WNOHANG) to be sure this signal handler will not
     * block if a child was cleaned up in another part of the program. */
    while (waitpid(-1, NULL, WNOHANG) < 0) {
    }
    printf("\r\n");
}


/**
 *	Displays the prompt for the shell
 */
void shellPrompt(){
    // We print the prompt in the form "<user>@<host> <cwd> >"
    char prompt[256];
    char hostn[] = "frosted";
    snprintf(prompt, 255, "root@%s %s # ", hostn, getcwd(currentDirectory, 128));
    write(STDOUT_FILENO, prompt, strlen(prompt));
}

/**
 * Signal handler for SIGINT
 */

volatile static int interrupted = 0;
void signalHandler_int(int p){
    //shellPrompt();
    interrupted++;
}

/**
 * Method to set an environment variable
 */
static int _setenv(const char *key, const char *value)
{
    struct env *ee = Environment;

    while (ee) {
        if (strcmp(ee->key, key) == 0) {
            strcpy(ee->value, value);
            return 0;
        }
        ee = ee->next;
    }


    struct env *e = malloc(sizeof(struct env));
    if (!e)
        return -1;

    e->key = malloc(strlen(key) + 1);
    e->value = malloc(strlen(value) + 1);
    /* TODO: check each failing malloc... */

    strcpy(e->key, key);
    strcpy(e->value, value);
    e->next = Environment;
    Environment = e;

    return 0;
}

/**
 * Method to get an environment variable
 */
char *_getenv(const char *key)
{
    struct env *e = Environment;

    while (e) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
        e = e->next;
    }

    return NULL;
}

/**
 * Method to change directory
 */
int changeDirectory(char* args[]){
    // If we write no path (only 'cd'), then go to the home directory
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
    }
    // Else we change the directory to the one specified by the
    // argument, if possible
    else{
        if (chdir(args[1]) == -1) {
        	printf(" %s: no such directory\r\n", args[1]);
            return -1;
        }
    }
    return 0;
}

/**
 * Method used to manage the environment variables with different
 * options
 */
int manageEnviron(char * args[], int option){
    /*
    char **env_aux;
    switch(option){
        // Case 'environ': we print the environment variables along with
        // their values
        case 0:
        	for(env_aux = environ; *env_aux != 0; env_aux ++){
        		printf("%s\r\n", *env_aux);
        	}
        	break;
        // Case 'setenv': we set an environment variable to a value
        case 1:
        	if((args[1] == NULL) && args[2] == NULL){
        		printf("%s","Not enought input arguments\r\n");
        		return -1;
        	}

        	// We use different output for new and overwritten variables
        	if(getenv(args[1]) != NULL){
        		printf("%s", "The variable has been overwritten\r\n");
        	}else{
        		printf("%s", "The variable has been created\r\n");
        	}

        	// If we specify no value for the variable, we set it to ""
        	if (args[2] == NULL){
        		setenv(args[1], "", 1);
        	// We set the variable to the given value
        	}else{
        		setenv(args[1], args[2], 1);
        	}
        	break;
        // Case 'unsetenv': we delete an environment variable
        case 2:
        	if(args[1] == NULL){
        		printf("%s","Not enought input arguments\r\n");
        		return -1;
        	}
        	if(getenv(args[1]) != NULL){
        		unsetenv(args[1]);
        		printf("%s", "The variable has been erased\r\n");
        	}else{
        		printf("%s", "The variable does not exist\r\n");
        	}
        break;


    }
    */
    return 0;
}

/**
* Method for launching a program. It can be run in the background
* or in the foreground
*/
void launchProg(char **args, int background){
     int err = -1;
     struct stat st;
     char bin_arg0[60] = "/bin/";
     strcpy(bin_arg0 + 5, args[0]);

     /* Find executable command */
     if (stat(bin_arg0, &st) < 0) {
         printf("Command not found.\r\n");
         return;
     }

     pid = vfork();

     if(pid == -1) {
         printf("Child process could not be created\r\n");
         return;
     }
     // pid == 0 implies the following code is related to the child process
    if(pid==0){

        // We set parent=<pathname>/simple-c-shell as an environment variable
        // for the child
        //setenv("parent",getcwd(currentDirectory, 128),1);

        // If we launch non-existing commands we end the process
        if (execvp(bin_arg0,args)==err){
        	printf("Command not found");
        	kill(getpid(),SIGTERM);
        }
     }

     // The following will be executed by the parent

     // If the process is not requested to be in background, we wait for
     // the child to finish.
     if (background == 0){
        int status;
        char exit_status_str[16];
        while (waitpid(pid, &status, 0) != pid) {
        }
        sprintf(exit_status_str, "%i", status);
        _setenv("?", exit_status_str);
     }else{
         // In order to create a background process, the current process
         // should just skip the call to wait. The SIGCHILD handler
         // signalHandler_child will take care of the returning values
         // of the childs.
         printf("Process created with PID: %d\r\n",pid);
     }
}

/**
* Method used to manage I/O redirection
*/
void fileIO(char * args[], char* inputFile, char* outputFile, int option)
{
    int err = -1;
    int fileDescriptor; // between 0 and 19, describing the output or input file

    if((pid=vfork())==-1){
        printf("Child process could not be created\r\n");
        return;
    }
    if(pid==0){
        // Option 0: output redirection
        if (option == 0){
            uint32_t flags;
        	// We open (create) the file truncating it at 0, for write only
            flags = O_CREAT;
            flags |= O_TRUNC | O_WRONLY;
        	fileDescriptor = open(outputFile, flags, 0600);
        	// We replace de standard output with the appropriate file
        	dup2(fileDescriptor, STDOUT_FILENO);
        	close(fileDescriptor);
        // Option 1: input and output redirection
        }else if (option == 1){
        	// We open file for read only (it's STDIN)
        	fileDescriptor = open(inputFile, O_RDONLY, 0600);
        	// We replace de standard input with the appropriate file
        	dup2(fileDescriptor, STDIN_FILENO);
        	close(fileDescriptor);
        	// Same as before for the output file
        	fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
        	dup2(fileDescriptor, STDOUT_FILENO);
        	close(fileDescriptor);
        }

        //setenv("parent",getcwd(currentDirectory, 128),1);

        if (execvp(args[0],args)==err){
        	printf("err");
        	kill(getpid(),SIGTERM);
        }
    }
    while (waitpid(-1, NULL, 0) < 0) {
    }
}

/**
* Method used to manage pipes.
*/
void pipeHandler(char * args[]){
    // File descriptors
    int filedes[2]; // pos. 0 output, pos. 1 input of the pipe
    int filedes2[2];

    int num_cmds = 0;

    char *command[LIMIT];


    pid_t pid;

    int err = -1;
    int end = 0;

    // Variables used for the different loops
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    // First we calculate the number of commands (they are separated
    // by '|')
    while (args[l] != NULL){
        if (strcmp(args[l],"|") == 0){
        	num_cmds++;
        }
        l++;
    }
    num_cmds++;

    // Main loop of this method. For each command between '|', the
    // pipes will be configured and standard input and/or output will
    // be replaced. Then it will be executed
    while (args[j] != NULL && end != 1){
        k = 0;
        // We use an auxiliary array of pointers to store the command
        // that will be executed on each iteration
        while (strcmp(args[j],"|") != 0){
        	command[k] = args[j];
        	j++;
        	if (args[j] == NULL){
        		// 'end' variable used to keep the program from entering
        		// again in the loop when no more arguments are found
        		end = 1;
        		k++;
        		break;
        	}
        	k++;
        }
        // Last position of the command will be NULL to indicate that
        // it is its end when we pass it to the exec function
        command[k] = NULL;
        j++;

        // Depending on whether we are in an iteration or another, we
        // will set different descriptors for the pipes inputs and
        // output. This way, a pipe will be shared between each two
        // iterations, enabling us to connect the inputs and outputs of
        // the two different commands.
        if (i % 2 != 0){
        	pipe(filedes); // for odd i
        }else{
        	pipe(filedes2); // for even i
        }

        pid=vfork();

        if(pid==-1){
        	if (i != num_cmds - 1){
        		if (i % 2 != 0){
        			close(filedes[1]); // for odd i
        		}else{
        			close(filedes2[1]); // for even i
        		}
        	}
        	printf("Child process could not be created\r\n");
        	return;
        }
        if(pid==0){
        	// If we are in the first command
        	if (i == 0){
        		dup2(filedes2[1], STDOUT_FILENO);
        	}
        	// If we are in the last command, depending on whether it
        	// is placed in an odd or even position, we will replace
        	// the standard input for one pipe or another. The standard
        	// output will be untouched because we want to see the
        	// output in the terminal
        	else if (i == num_cmds - 1){
        		if (num_cmds % 2 != 0){ // for odd number of commands
        			dup2(filedes[0],STDIN_FILENO);
        		}else{ // for even number of commands
        			dup2(filedes2[0],STDIN_FILENO);
        		}
        	// If we are in a command that is in the middle, we will
        	// have to use two pipes, one for input and another for
        	// output. The position is also important in order to choose
        	// which file descriptor corresponds to each input/output
        	}else{ // for odd i
        		if (i % 2 != 0){
        			dup2(filedes2[0],STDIN_FILENO);
        			dup2(filedes[1],STDOUT_FILENO);
        		}else{ // for even i
        			dup2(filedes[0],STDIN_FILENO);
        			dup2(filedes2[1],STDOUT_FILENO);
        		}
        	}

        	if (execvp(command[0],command)==err){
        		kill(getpid(),SIGTERM);
        	}
        }

        // CLOSING DESCRIPTORS ON PARENT
        if (i == 0){
        	close(filedes2[1]);
        }
        else if (i == num_cmds - 1){
        	if (num_cmds % 2 != 0){
        		close(filedes[0]);
        	}else{
        		close(filedes2[0]);
        	}
        }else{
        	if (i % 2 != 0){
        		close(filedes2[0]);
        		close(filedes[1]);
        	}else{
        		close(filedes[0]);
        		close(filedes2[1]);
        	}
        }

        while (waitpid(pid,NULL,0) < 0) { }

        i++;
    }
}

/**
* Method used to handle the commands entered via the standard input
*/
int commandHandler(char * args[]){
    int i = 0;
    int j = 0;

    int fileDescriptor;
    int standardOut;

    int aux;
    int background = 0;

    char *args_aux[8] = { NULL };


    // We look for the special characters and separate the command itself
    // in a new array for the arguments
    while ( args[j] != NULL){
        if ( (strcmp(args[j],">") == 0) || (strcmp(args[j],"<") == 0) || (strcmp(args[j],"&") == 0)){
        	break;
        }
        args_aux[j] = args[j];
        j++;
    }

    // 'exit' command quits the shell
    if(strcmp(args[0],"exit") == 0) exit(0);
    // 'pwd' command prints the current directory
 	else if (strcmp(args[0],"pwd") == 0){
        if (args[j] != NULL){
        	// If we want file output
        	if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
        		fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
        		// We replace de standard output with the appropriate file
        		standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
        											// because we'll want it back
        		dup2(fileDescriptor, STDOUT_FILENO);
        		close(fileDescriptor);
        		printf("%s\r\n", getcwd(currentDirectory, 128));
        		dup2(standardOut, STDOUT_FILENO);
        	}
        }else{
        	printf("%s\r\n", getcwd(currentDirectory, 128));
        }
    }
    // 'cd' command to change directory
    else if (strcmp(args[0],"cd") == 0) changeDirectory(args);
    // 'environ' command to list the environment variables
    else if (strcmp(args[0],"environ") == 0){
        if (args[j] != NULL){
        	// If we want file output
        	if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
        		fileDescriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
        		// We replace de standard output with the appropriate file
        		standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
        											// because we'll want it back
        		dup2(fileDescriptor, STDOUT_FILENO);
        		close(fileDescriptor);
        		manageEnviron(args,0);
        		dup2(standardOut, STDOUT_FILENO);
        	}
        }else{
        	manageEnviron(args,0);
        }
    }
    // 'setenv' command to set environment variables
    else if (strcmp(args[0],"setenv") == 0) {
        _setenv(args[1], args[2]);
    }
    else if (strcmp(args[0],"getenv") == 0) {
        char *value;
        value = _getenv(args[1]);
        printf("%s\r\n", *value);
    }
    // 'unsetenv' command to undefine environment variables
    else if (strcmp(args[0],"unsetenv") == 0) manageEnviron(args,2);
    else{
        // If none of the preceding commands were used, we invoke the
        // specified program. We have to detect if I/O redirection,
        // piped execution or background execution were solicited
        while (args[i] != NULL && background == 0){
        	// If background execution was solicited (last argument '&')
        	// we exit the loop
        	if (strcmp(args[i],"&") == 0){
        		background = 1;
        	// If '|' is detected, piping was solicited, and we call
        	// the appropriate method that will handle the different
        	// executions
        	}else if (strcmp(args[i],"|") == 0){
        		pipeHandler(args);
        		return 1;
        	// If '<' is detected, we have Input and Output redirection.
        	// First we check if the structure given is the correct one,
        	// and if that is the case we call the appropriate method
        	}else if (strcmp(args[i],"<") == 0){
        		aux = i+1;
        		if (args[aux] == NULL || args[aux+1] == NULL || args[aux+2] == NULL ){
        			printf("Not enough input arguments\r\n");
        			return -1;
        		}else{
        			if (strcmp(args[aux+1],">") != 0){
        				printf("Usage: Expected '>' and found %s\r\n",args[aux+1]);
        				return -2;
        			}
        		}
        		fileIO(args_aux,args[i+1],args[i+3],1);
        		return 1;
        	}
        	// If '>' is detected, we have output redirection.
        	// First we check if the structure given is the correct one,
        	// and if that is the case we call the appropriate method
        	else if (strcmp(args[i],">") == 0){
        		if (args[i+1] == NULL){
        			printf("Not enough input arguments\r\n");
        			return -1;
        		}
        		fileIO(args_aux,NULL,args[i+1],0);
        		return 1;
        	}
        	i++;
        }
        // We launch the program with our method, indicating if we
        // want background execution or not
        launchProg(args_aux,background);

    }
return 1;
}

void pointer_shift(int *a, int s, int n) {
   int i;
   for (i = n; i > s - 1; i--) {
      *(a+i+1) = *(a+i);
   }
}

char *readline_tty(char *input, int size)
{

    while (2>1)
    {
        int len = 0, pos = 0;
        int out = STDOUT_FILENO;
        char got[5];
        int i, j;
        int repeat = 0;

        while(len < size)
        {
            const char del = 0x08;
            const char space = 0x20;
            int ret = read(STDIN_FILENO, got, 4);

            /* arrows */
            if ((ret == 3) && (got[0] == 0x1b)) {
                char dir = got[2];
                if (dir == 'A') {
                    if ((strlen(lastcmd) == 0) || repeat) {
                        continue;
                    }
                    repeat++;

                    while (len > 0) {
                        write(STDOUT_FILENO, &del, 1);
                        write(STDOUT_FILENO, &space, 1);
                        write(STDOUT_FILENO, &del, 1);
                        len--;
                    }
                    len = strlen(lastcmd);
                    lastcmd[len] = 0x00;
                    len--;
                    lastcmd[len] = 0x00;
                    pos = len;
                    printf( "%s", lastcmd);
                    fflush(stdout);
                    strcpy(input, lastcmd);
                    continue;
                } else if (dir == 'B') {
                } else if (dir == 'C') {
                    if (pos < len) {
                        printf("%c", input[pos++]);
                        fflush(stdout);
                    }
                } else if (dir == 'D') {
                    write(STDOUT_FILENO, &del, 1);
                    pos--;
                    continue;
                }
            }

            if (ret > 3) {
            	if ((got[0] == 0x1B) && (got[2] == 0x33) && (got[3] == 0x7E)) {
                	if (pos < len) {
	                    //write(STDOUT_FILENO, &del, 1);
        	            //printf( " ");
                	    //write(STDOUT_FILENO, &del, 1);
                	    pos--;
        	            len--;
                	    if (pos < len) {
                    	for ( i = pos+1; i < len; i++) {
	                	input[i] = input[i+1];
                        write(STDOUT_FILENO, &input[i], 1);
                	}
                    write(STDOUT_FILENO, &space, 1);
	                i = len - pos;
        	        while (i > 0) {
                		write(STDOUT_FILENO, &del, 1);
                    		i--;
                    	}

	                    } else {
	                    input[pos] = 0x00;
	                    pos--;
	                    len--;
	            }

                    continue;
                }
            	}
                continue;
            }
            if ((ret > 0) && (got[0] >= 0x20) && (got[0] <= 0x7e)) {
                for (i = 0; i < ret; i++) {
                    /* Echo to terminal */
                    if (got[i] >= 0x20 && got[i] <= 0x7e)
                        write(STDOUT_FILENO, &got[i], 1);
                	if (pos < len) {
                		for (j = len + 1; j > pos; j--) {
                			input[j] = input[j-1];
                		}
                		input[pos] = got[i];
                    	for ( j = pos + 1; j < len +1; j++) {
                            write(STDOUT_FILENO, &input[j], 1);
                    	}
                        write(STDOUT_FILENO, &input[i], 1);
                    	j = len - pos + 1;
                    	while (j > 0) {
                    		write(STDOUT_FILENO, &del, 1);
                    		j--;
                    	}
                	} else {
                		input[pos] = got[i];
                	}

                    len++;
                    pos++;
                }
            }

            if ((got[0] == 0x0D)) {
            	input[len] = 0x0D;
                input[len + 1] = '\0';
                printf( "\r\n");
                fflush(stdout);
                if (len > 0)
                    strncpy(lastcmd, input, 128);
                return input; /* CR (\r\n) */
            }

            if ((got[0] == 0x4)) {
                printf( "\r\n");
                fflush(stdout);
                len = 0;
                pos = 0;
                break;
            }

            /* tab */
            if ((got[0] == 0x09)) {
                input[len] = 0;
                printf("\r\n");
                printf("Built-in commands: \r\n");
                printf("\t pwd cd ");
                printf("\r\n");
                shellPrompt();
                printf("%s", input);
                fflush(stdout);
                continue;
            }

            /* backspace */
            if ((got[0] == 0x7F) || (got[0] == 0x08)) {
                if (pos > 0) {
                    write(STDOUT_FILENO, &del, 1);
                    write(STDOUT_FILENO, &space, 1);
                    write(STDOUT_FILENO, &del, 1);
                    pos--;
                    len--;
                    if (pos < len) {
                    	for ( i = pos; i < len; i++) {
                    		input[i] = input[i+1];
                            write(STDOUT_FILENO, &input[i], 1);
                    	}
                        write(STDOUT_FILENO, &space, 1);
                    	i = len - pos + 1;
                    	while (i > 0) {
                    		write(STDOUT_FILENO, &del, 1);
                    		i--;
                    	}

                    } else {
	                    input[pos] = 0x00;
	            }

                    continue;
                }
            }
        }
        printf("\r\n");
        fflush(stdout);
        if (len < 0)
            return NULL;

        input[len + 1] = '\0';
    }
    return input;
}

char *readline_notty(char *input, int len)
{
    int ret = read(STDIN_FILENO, input, len - 1);
    if (ret > 0) {
        input[ret - 1] = 0x0D;
        input[ret] = 0x00;
        return input;
    }
    return NULL;
}

static char *readline(char *input, int len)
{
    if (GBSH_IS_INTERACTIVE)
        return readline_tty(input, len);
    else
        return readline_notty(input, len);
}


/**
* Main method of our shell
*/
int main(int argc, char *argv[]) {
    char line[MAXLINE]; // buffer for the user input
    char * tokens[LIMIT]; // array for the different tokens in the command
    int numTokens;
    struct sigaction sigint_a = {};
    sigint_a.sa_handler = signalHandler_int;


    no_reprint_prmpt = 0; 	// to prevent the printing of the shell
        					// after certain methods
    pid = -10; // we initialize pid to an pid that is not possible

    // We call the method of initialization and the welcome screen
    if (argc > 1)
        shell_init(argv[1]);
    else
        shell_init(NULL);


    sigaction(SIGINT, &sigint_a, NULL);

    welcomeScreen();
    fprintf(stdout, "Current pid = %d\r\n", getpid());

    // We set our extern char** environ to the environment, so that
    // we can treat it later in other methods
    environ = NULL;

    // We set shell=<pathname>/simple-c-shell as an environment variable for
    // the child
    //setenv("shell",getcwd(currentDirectory, 128),1);

    // Main loop, where the user input will be read and the prompt
    // will be printed
    while(1){
        // We print the shell prompt if necessary
        if (no_reprint_prmpt == 0) shellPrompt();
        no_reprint_prmpt = 0;

        // We empty the line buffer
        memset ( line, '\0', MAXLINE );

        // We wait for user input
        /* fgets(line, MAXLINE, stdin); */
        while (readline(line, MAXLINE) == NULL);
        if (interrupted) {
            printf("^C\r\n");
            fflush(stderr);
            interrupted = 0;
            continue;
        }

        // If nothing is written, the loop is executed again
        if((tokens[0] = strtok(line," \r\n\t")) == NULL) continue;

        // We read all the tokens of the input and pass it to our
        // commandHandler as the argument
        numTokens = 1;
        while((tokens[numTokens] = strtok(NULL, " \r\n\t")) != NULL) numTokens++;

        commandHandler(tokens);

    }

    exit(0);
}
