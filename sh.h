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
//addUser: adds a user to a linked list of users to watch
void addUser (char * command, struct pathelement **userpath);
//printUser: prints all users in the linked list. Used for debugging purposes.
void printUsers (struct pathelement * userpath);
//deleteUser: removes a user from the linked list and stops it from being watched. 
void deleteUser (char * removeUser, struct pathelement **userpath);
//watchUser: starts the whole watchuser process. Adds a user to the list to be watched, or removes them from the whole being watched thing.
void watchUser(struct pathelement **userpath, char * user, char * off);
//collectLogin: the function called in the watch user thread. Used to print out which users are logged in from the watch user linked list. 
void *collectLogin(void * userpath);
//addFile: adds a file to watch file linked list. 
void addFile (char * command, struct pathelement **filepath);
//manageThread: used to switch on and off watch file threads. 
void manageThread(struct pathelement **filepath, char * file, char * off);
//watchFile: called from the watchfile command. Adds/turns off files and thier watched status. 
void watchFile(struct pathelement **filepath, char * file, char * off);
//collectSize: called in manageThread, and used to print out weather or not a file has changed. 
void *collectSize (void * usernode);

#define PROMPTMAX 32
#define MAXARGS 10
