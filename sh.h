#include "get_path.h"

int pid;
int sh( int argc, char **argv, char **envp);
//which: determines first avaiable path to command
char *which(char *command, struct pathelement *pathlist);
//where: shows all instances of command
char *where(char *command, struct pathelement *pathlist);
//list: lists all files in either the users current working directory or the directory provided

void list ( char *dir );

//printWorking: prints working directory

void printWorking (char *pwd);
//getPID(): prints pid of shell
void getPID();
//killPID: kills pid when given one, or sends other command to it when given two commands
void killPID(char * pid, char * signalNumber);
//promptUser: allows user to change prompts
char *promptUser(char * currentPrompt, char * currentHeader, char * currentDIR, char * promptName);
//printEnvironement: prints entire user environement
void printEnvironment(struct pathelement *pathlist, char * firstEnv, char * secondEnv);
//addHistory: adds command user types to history
void addHistory (char * command, struct pathelement **historypath);
//printHistory: prints all users history commands
void printHistory (struct pathelement * historypath, int count);
//deleteHistory: deletes all history theoretically
void deleteHistory (struct pathelement ** historypath);
void *collectLogin(void * userpath);
void *collectSize (void * usernode);

#define PROMPTMAX 32
#define MAXARGS 10
