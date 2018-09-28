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
  char*prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
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
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';
  strcpy(prompt, "🍆💦💦 >");

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
        arg = strtok(NULL, "");
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
     char * pathline = which(command, pathlist); 
     args[0] = pathline;
        count = 0;
        while (args[count] != NULL) {
           printf(args[count]);
           count++;
      }
     pid_t pid = fork();
     /* find it */
     if(pid == -1) {
         printf("Your child is doa. You should take better care of them next itme.\n");
        return;
     } 
     else if (pid == 0) {
          if(execve(pathline, args, envp) < 0) {
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
} /* list() */



