#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <wordexp.h>
#include "sh.h"


int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *promptPrefix = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  char **argsEx = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;
  struct pathelement *historylist;
  int background = 0;
  
  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
  promptPrefix = " ";   
 
  
  
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  snprintf(prompt, PROMPTMAX, "%s%s%s%s", promptPrefix , "[", pwd, "]>");

  /* Put PATH into a linked list */
  pathlist = get_path();
  sigignore(SIGINT);
  sigignore(SIGTERM);
  sigignore(SIGTSTP);

  while ( go )
  {
 /* print your prompt */

   printf("%s",prompt);
   
  
    /* get command line and process */

   while(fgets(commandline, MAX_CANON, stdin) != NULL) {
        background = 0;
        commandline[strlen(commandline)-1] = '\0';
        arg = calloc(MAX_CANON, sizeof(char));
        command = calloc(MAX_CANON, sizeof(char));
        
        arg = strtok(commandline, " ");
        strcpy(command, arg); 
        arg = strtok(NULL, " ");
        addHistory(command, &historylist);
        int count = 0;
        while (argsEx[count] != NULL) {
           argsEx[count] = NULL;
           count++;
        }
        count = 0;
        while (args[count] != NULL) {
             args[count] = NULL;
             count++;
        }
        count = 0;
        wordexp_t wildcard;
        printf("%s", arg);
        memset(&wildcard, 0, sizeof(wildcard));
        while (arg != NULL) {
           if (count == 0) {
               if (strcmp(arg, "&") == 0) {
                  printf("here");
                  background = 1;
                  wordexp("", &wildcard, 0);
               }
               else {
                     printf("Not here");
                     wordexp(arg, &wildcard, 0);
               }
           }
           else {
               if (strcmp(arg, "&") == 0) {
                   printf("here again");
                   background = 1;
                   wordexp("", &wildcard, WRDE_APPEND);
               }
               else {
                    printf("Not here again");
                    wordexp(arg, &wildcard, WRDE_APPEND);
               }
           }
           count++;
           arg = strtok(NULL, " ");
      }

      if (count > 0) {
      args = wildcard.we_wordv;
      }

    printf("%s", prompt);
    /* check for each built in command and implement */
    if (strcmp(command, "exit") == 0) {
      printf("\nRunning exit\n");
      printf("That was terrible. I'll be back in 10 minutes.\n");
      exit(0);
    }
    else if (strcmp(command, "which") == 0) {
       
       printf("\nRunning built in which\n");
       count = 0;
       while (args[count] != NULL) {
          which(args[count], pathlist);
          count++;
       }
       printf(prompt);
    }
    else if (strcmp(command, "where") == 0) {
       printf("\nRunning built in where\n");
       count = 0;
       while (args[count] != NULL) {
          where(args[count], pathlist);
          count++;
       }
       printf(prompt);
    }
    else if (strcmp(command, "list") == 0) {
       count = 0;
       if (args[count] == NULL) {
           printf("\nPrinting files in current directory.");
           list(args[count]);
        }
       while (args[count] != NULL) {
          printf("\nPrinting files in %s\n", args[count]);
          list(args[count]);
          count++;
       }
        printf(prompt);
    }
    else if (strcmp(command, "cd") == 0) {
        printf("\nRunning built in command cd \n");
        if (args[0] == NULL) {
           chdir("..");
        }
        else {
           chdir(args[0]);
        }
        if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
        {
           perror("getcwd");
           exit(2);
        }
        snprintf(prompt, PROMPTMAX, "%s%s%s%s", promptPrefix , "[", pwd, "]>");
        printf(prompt);
    }
    else if (strcmp(command, "pwd") == 0) {
        printf("\nRunning built in command pwd \n");
        printWorking(pwd);
        printf(prompt);
    }
    else if (strcmp(command, "pid") == 0) {
        getPID();
        printf("\n");
        printf(prompt);
    }
    else if (strcmp(command, "kill") == 0) {
       printf("\nRunning built in command kill\n");
       killPID(args[0], args[1]);
       printf(prompt);
    }
    else if (strcmp(command, "prompt") == 0) {
        printf("\nRunning built in command prompt\n");
        promptPrefix = promptUser(prompt, promptPrefix, pwd, args[0]);
        printf("\n");
        printf(prompt);
    }
    else if (strcmp(command, "printenv") == 0) {
        printf("\nRunning built in command printenv\n");
        printEnvironment(pathlist, args[0], args[1]);
        printf("\n");
        printf(prompt);
    }  //set the environment
	else if (strcmp(command, "setenv") == 0) {
	printf("\nRunning built in command setenv");
	if(args[1] == NULL) {
		printEnvironment(pathlist, args[0], args[1]);
	}
	else if((args[1]!=NULL)&&(args[2]==NULL)) {
		setenv(args[1], "", 1);
	}
	else {
		setenv(args[1], args[2], 1);
		
		if(strcmp(args[1], "HOME") == 0) {
			homedir = getenv("HOME");
		}
		else if(strcmp(args[2], "PATH") == 0) {
			pathlist = get_path();
		}
	}
        printf("\n");
        printf(prompt);
}
    else if (strcmp(command, "history") == 0) {
        printf("\nRunning built in command history\n");
        printHistory(historylist, args[0]);
        printf("\n");
        printf(prompt);
    }
    else if (*command == NULL) {
        printf("\n");
        printf(prompt);
    }

    /*  else  program to exec */
     /* do fork(), execve() and waitpid() */
    else {
    //check to see if the command entered is a file name
    if(access(command, F_OK) == 0) {
      pid_t pid = fork();
     /* find it */
     if(pid == -1) {
         printf("Your child is doa. You should take better care of them next itme.\n");
        return;
     } 
     else if (pid == 0) {
          if(execve(command, args, envp) < 0) {
                 printf("Could not execute command.\n");
          }
          exit(0);
     }
     else {
        waitpid(-1, NULL, 0);
        printf(prompt);
    }
}



     else {
     pid_t pid = fork();
     /* find it */
       char * pathline = calloc(MAX_CANON, sizeof(char));
       pathline = which(command, pathlist); 
       argsEx[0] = pathline;
       count = 0;
        while (args[count] != NULL) {
           argsEx[count+1] = args[count];
           count++;
      }
     if(pid == -1) {
         printf("Your child is doa. You should take better care of them next itme.\n");
        return;
     } 
     else if (pid == 0) {
          //if & is there, then background the process
          if (background == 1) {
               printf("Backgrounding");
               setpgid(0,0);
          }
          if(execve(pathline, argsEx, envp) < 0) {
                 printf("Could not execute command.\n");
          }
          exit(0);
     }
     else  if (background == 1){
        printf("Background \n");
        printf(prompt);
    }
    else {
        waitpid (-1, &status, WNOHANG);
        printf(prompt);
    }
    
}
 
    free(arg);
    free(command);
  }
}
}
  return 0;
} 
/* sh() */


/* loop through pathlist until finding command and return it.  Return
NULL when not found. */
char *which(char *command, struct pathelement *pathlist )
{
while (pathlist->next) {
    char *path = calloc(MAX_CANON, sizeof(char));
    snprintf(path, MAX_CANON, "%s%s%s", pathlist->element,"/",command);
    if (access(path, F_OK) == 0) {
        printf("%s\n", path);
        return path;
}
    pathlist = pathlist->next;
}
printf("Command not found in path\n");
return NULL;
  

} /* which() */
//where: prints all instances of the where the command is in the path 
char *where(char *command, struct pathelement *pathlist )
{
 while (pathlist->next) {
    char *path = calloc(MAX_CANON, sizeof(char));
    snprintf(path, MAX_CANON, "%s%s%s", pathlist->element,"/",command);
    if (access(path, F_OK) == 0) {
        printf("%s\n", path);
}
    pathlist = pathlist->next;
}
return NULL;
} /* where() */

void list ( char *dir )
{
DIR *directoryListing;
struct dirent *ent; 

if (dir == NULL) {
      if ((directoryListing= opendir(".")) == NULL) {
          printf("Could not open current directory");
          return 0;
  }
} 
else {
     if ((directoryListing= opendir(dir)) == NULL) {
         printf("Could not open current directory");
         return 0;
       }
}
while ((ent = readdir(directoryListing)) != NULL) {
    printf("%s\n", ent->d_name);
}
closedir(directoryListing);
} 


// pwd: gets current working directory

void printWorking (char *pwd) {
  getcwd(pwd, sizeof(pwd));
  printf("\nDir: %s\n", pwd);
} 

void getPID() 
{
     pid_t pid, ppid;
     gid_t gid;

	/* get the process id */
	if ((pid = getpid()) < 0) {
	  perror("unable to get pid");
	} else {
	  printf("The process id is %d\n", pid);
	}
}

void killPID(char * pid, char * signalNumber) {
     if (signalNumber == NULL ) {
    
         if (kill(atoi(pid), SIGTERM ) < 0) {
	     perror("unable to kill process");
         } else {
	     printf("The process killed \n");
	 }
     } 
     else {
          if (kill(atoi(signalNumber), atoi(pid)) < 0) {
	     perror("unable to kill process");
         } else {
	     printf("The process killed \n");
	 }
     }

}

char *promptUser(char * currentPrompt, char * currentHeader, char * currentDIR, char * promptName) {
      if (promptName == NULL) {
           printf("Please enter your new prompt header.\n");
           char *commandlinePrompt = calloc(MAX_CANON, sizeof(char));
           fgets(commandlinePrompt, MAX_CANON, stdin);
           commandlinePrompt[strlen(commandlinePrompt)-1] = '\0';
           currentHeader = commandlinePrompt;
           snprintf(currentPrompt, PROMPTMAX, "%s%s%s%s", currentHeader , " [", currentDIR, "]>");
           free(commandlinePrompt);
           return currentHeader;
      }
      else {
           currentHeader = promptName;
           snprintf(currentPrompt, PROMPTMAX, "%s%s%s%s", currentHeader, " [", currentDIR, "]>");
           return currentHeader;
      }
}

void printEnvironment(struct pathelement *pathlist, char * firstEnv, char * secondEnv) {
      if (firstEnv == NULL) {
              pathlist = pathlist->next;
              while (pathlist->next != NULL) {
              printf("%s\n", pathlist->element);
              pathlist = pathlist->next;
              }
      }
      else if (secondEnv != NULL) {
              perror("Too many arguments passed");
      }
      else {
              printf(getenv(firstEnv));
      }
}

void addHistory (char * command, struct pathelement **historypath) { 
      struct pathelement* newElement = (struct pathelement*)malloc(sizeof(struct pathelement));
      struct pathelement* lastElement = *historypath;
      newElement->element = malloc(strlen(command)+1);
      strcpy(newElement->element, command);
      if (*historypath == NULL ) {
         *historypath = newElement; 
         return;
      }
      else {
            while (lastElement->next != NULL) {
                lastElement = lastElement->next;
            }
            lastElement->next = newElement;
            return;
      }
}

void printHistory (struct pathelement * historypath, int count) {
     if (count == NULL) {
         count = 10; 
     }
     int loopCounter = 0;
          historypath = historypath ->next;
     while (historypath != NULL && loopCounter < count) {
          printf("%s\n", historypath->element);
          historypath = historypath->next;
          loopCounter++;
      }
}

void deleteHistory (struct pathelement ** historypath) {
      struct pathelement * nextElement;
      struct pathelement * currentElement = *historypath;
     
      while (currentElement != NULL) {
         nextElement = currentElement -> next; 
         free(currentElement->element);
         free(currentElement);
         currentElement = nextElement; 
        }
      *historypath = NULL;
}



