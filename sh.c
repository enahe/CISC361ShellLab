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

  while ( go )
  {
 /* print your prompt */

   printf("%s",prompt);
   
  
    /* get command line and process */

   while(fgets(commandline, MAX_CANON, stdin) != NULL) {
        commandline[strlen(commandline)-1] = '\0';
        arg = calloc(MAX_CANON, sizeof(char));
        command = calloc(MAX_CANON, sizeof(char));
        arg = strtok(commandline, " ");
        strcpy(command, arg); 
        arg = strtok(NULL, " ");
        printf(" %s\n", command);
        int count = 0;
        while (arg != NULL) {
           args[count] = arg;
           count++;
           arg = strtok(NULL, " ");
      }
   
    printf("%s", prompt);
    /* check for each built in command and implement */
    if (strcmp(command, "exit") == 0) {
      printf("Running exit\n");
      printf("That was terrible. I'll be back in 10 minutes.\n");
      exit(0);
    }
    else if (strcmp(command, "which") == 0) {
       printf("Running which\n");
       which(args[0], pathlist);
       printf(prompt);
    }
    else if (strcmp(command, "where") == 0) {
       printf("Running where\n");
       where(args[0], pathlist);
       printf(prompt);
    }
    else if (strcmp(command, "list") == 0) {
        printf("Running list \n");
        list(args[0]);
        printf(prompt);
    }
    else if (strcmp(command, "cd") == 0) {
        printf("Running cd \n");
        if (args[0] == NULL) {
           chdir("-");
        }
        else {
           chdir(args[0]);
        }
        if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
        {
           perror("getcwd");
           exit(2);
        }
        snprintf(prompt, PROMPTMAX, "%s%s%s%s", " " , "[", pwd, "]>");
        printf(prompt);
    }
    else if (strcmp(command, "pwd") == 0) {
        printf("Running pwd \n");
        printWorking(pwd);
        printf(prompt);
    }
    else if (strcmp(command, "pid") == 0) {
        getPID();
        printf("\n");
        printf(prompt);
    }
    else if (strcmp(command, "kill") == 0) {
       killPID(args[0], args[1]);
       printf(prompt);
    }
    else if (strcmp(command, "prompt") == 0) {
        promptUser(prompt, pwd, args[0]);
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
       //error here, when I do this, the rest of args is lost.
        count = 0;
        while (args[count] != NULL) {
           printf("Fixing command arguments");
           argsEx[count+1] = args[count];
           printf(argsEx[count+1]);
           count++;
      }
     if(pid == -1) {
         printf("Your child is doa. You should take better care of them next itme.\n");
        return;
     } 
     else if (pid == 0) {
          if(execve(pathline, argsEx, envp) < 0) {
                 printf("Could not execute command.\n");
          }
          exit(0);
     }
     else {
        waitpid(-1, NULL, 0);
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

void promptUser(char * currentPrompt, char * currentDIR, char * promptName) {
      if (promptName == NULL) {
           printf("Please enter your new prompt header.\n");
           char *commandlinePrompt = calloc(MAX_CANON, sizeof(char));
           fgets(commandlinePrompt, MAX_CANON, stdin);
           commandlinePrompt[strlen(commandlinePrompt)-1] = '\0';
           snprintf(currentPrompt, PROMPTMAX, "%s%s%s%s", commandlinePrompt , " [", currentDIR, "]>");
           free(commandlinePrompt);
      }
      else {
           snprintf(currentPrompt, PROMPTMAX, "%s%s%s%s", promptName, " [", currentDIR, "]>");
      }
      
}

