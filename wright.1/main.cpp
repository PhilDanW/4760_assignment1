//Philip Wright
//02/09/2020
//Assignment 1
//CS4760
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

extern int errno;
extern char ** environ;

//this is the first argument given that is not the file
int userArg;

//function declarations
int runCommand(int, char **, int); 
void printEnv(); 
void helpScreen();
void copyEnv();

int main(int argc, char * argv[]) {

    int opt;
    
    //output current environment if no arguments given.
    if ( argc == 1) {
        printEnv();
    }

    //check if argument given is -i, -h, or invalid argument
    while(( opt = getopt(argc, argv, "ih")) != -1) {

        switch(opt) {
            case 'i': {
		        //create an array of null terminated strings which will serve as empty environment	
                static char *nullArray[] = {NULL};
                environ = nullArray;
                  

                if (!runCommand(argc, argv, optind)){
                    printEnv();
                }
                else {
		            //index is set to first argument
                    int i = opt;
                    //iterate through all arguments
                    while(i < argc) {
                    //if there is a semicolon in the arg list this will ignore it
                        if(strcmp(argv[i], ";") == 0) i++;
                      
                        if ( execvp(argv[i], &argv[i])) {
                            printf("Error: %s\n",argv[i]);
                            perror("doenv: Failed to execute command\n");
                            exit(EXIT_FAILURE);
                        }
                    i++;
                    }
                  }
                  break;
                }
            case 'h':
                //Display the help screen
                helpScreen();
                break;
            case '?':
                // Display the help screen if invalid argument.
                helpScreen();         
                break;
        }	
    }
  return 0;
}

//executes any arguments given after the name=value pairs
int runCommand(int argc, char *argv[], int pos) {
    //variable to test if extra argument is given or not
    int args = 0;
    
    while(args < argc && strchr(argv[args], '=')) {

        if(putenv(argv[args++]) == -1 ) {
            char * varName = argv[args - 1];
            perror("doenv: The name argument is empty string or a string containing an '='.");
            printf("errno: %d\n", errno);
            printf("doenv: %s\n", varName);
            exit(EXIT_FAILURE);
        }
    }

    if (argc > args) {
        //Add where the command was found
        userArg = args;
        args = 1;
    }

  return args;
}

//Deletes the environment and replaces it will the current environment
void copyEnv() {

    char * firstEnv = * environ;
    int varLength  = strchr(firstEnv, '=') - firstEnv;
    char * varName = (char*)malloc(varLength * sizeof(char));

    //Iterate through the environment one variable at a time
    for(int i = 1; firstEnv; i++) {
        //must copy everything before the '=' for every name=value
        strncpy(varName, firstEnv, varLength);
        varName[varLength] = '\0';
    
        //unsetenv will delete the variable from the environment
        if (unsetenv(varName) == -1){
            perror("doenv: Failure trying to delete variable.");
            printf("errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    
        firstEnv = * ( environ + i );
    
        if(firstEnv != NULL){
            varLength = strchr(firstEnv, '=') - firstEnv;
            varName = (char*)realloc(varName, varLength * sizeof(char));
        }
    }
}

//Print the contents of current environment
void printEnv() {

    char ** envVar = environ;
    while(*envVar) {
        printf("%s\n", *envVar++);
    }
}

//Displays the help screen for usage information
void helpScreen() {
  
  printf("Help Screen");
  printf("\ndoenv [-h]: using h argument will display the help screen.");
  printf("\ndoenv [-i]: replace the current environment with [name=value] pairs to run commands in an empty environment.");
  printf("\ndoenv [no arg]: prints the current environment.\n");
}