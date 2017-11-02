#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):             Vibhuthi Wickramage, Vihanga Silva
   Student number(s):   21498423, 21480143
   Date:
 */

// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_external(SHELLCMD *t){
  pid_t pid_execute = fork();
  if (pid_execute < 0) {
    /* Failed */
    perror("Fork Failed");
    exit(EXIT_FAILURE);
  } else if (pid_execute == 0) {
    /* Child */
    execv(t->argv[0], t->argv);
  }
    /*Parent*/
  wait(NULL);
  return EXIT_SUCCESS;
}

void execute_exit(SHELLCMD *t){ // Void as a return is not required as the program will exit
  if (t->argv[1] == NULL) {
    exit(EXIT_SUCCESS);
  } else {
    fprintf(stderr, "ERROR: Invaild arguments for exit\n");
    exit(EXIT_FAILURE);
  }
}

int execute_cd(SHELLCMD *t){ //Function for handling cd commands
  int exitstatus;
  //char cwd[200];

  if (t->argv[1] == NULL) {
    exitstatus = chdir(HOME);
    //getcwd(cwd, sizeof(cwd));
    //printf("%s\n", cwd);
  } else {
    exitstatus = chdir(t->argv[1]);

    if (exitstatus == -1){
      fprintf(stderr, "ERROR: The Directory: %s is invalid\n", t->argv[1]);
    } else {
      //getcwd(cwd, sizeof(cwd));
      //printf("%s\n", cwd);
    }
  }
return exitstatus;
}

int execute_time(SHELLCMD *t){
  int exitstatus;
  if (t->argv[1] != NULL){
    t->type = CMD_COMMAND;
    t->argc--;
    //t->argv++; This gives a core dump so the following was used.
    for (int i = 0; i < t->argc; i++) {
      t->argv[i] = t->argv[i+1];
    }
    t->argv[t->argc] = NULL;

    struct timeval start, stop;
    gettimeofday(&start, NULL);
    exitstatus = execute_cmdtype(t);
    gettimeofday(&stop, NULL);
    printf("Time taken to excute: %lu microseconds\n", stop.tv_usec - start.tv_usec);
  } else {
    printf("No command was provided.\nHence, time taken to excute: 0 microseconds\n");
    exitstatus = EXIT_FAILURE;
  }
  return exitstatus;
}

int execute_search(SHELLCMD *t) {
  int exitstatus;
  bool match = FALSE;
  char *matchdir;

  char spath[strlen(PATH)+1]; //This is to match use that the orignal PATH variable isn't destoried
  spath[strlen(PATH)] = '\0';
  strncpy(spath,PATH,sizeof(spath)-1);

  char *tok;
  tok = strtok(spath,":");
  while (tok != NULL) {
    //printf("%s\n", tok); //Just for Debugging
    char *dirn = tok;

    DIR *dirpp;
    struct dirent *dpp;

    dirpp = opendir(dirn);

    if (dirpp == NULL) {
      fprintf(stderr, "ERROR: Failed to open %s\n", dirn);
      exitstatus = EXIT_FAILURE;
    } else {
      while ((dpp = readdir(dirpp)) != NULL){
        if (strcmp(t->argv[0],dpp->d_name) == 0) {
          //printf("%s is found\n", dpp->d_name); //DEBUGGING
          match = TRUE;
          break;
        }
      }
      closedir(dirpp);
    }

    if (match == TRUE) {
      matchdir = dirn;
      break;
    }
    tok = strtok(NULL, ":");
  }

  if (match == TRUE) {
    strncat(matchdir,"/",1);
    strncat(matchdir,t->argv[0],strlen(t->argv[0]));
    strcpy(t->argv[0],matchdir);
    //printf("%s\n", t->argv[0]); //DEBUGGING
    exitstatus = execute_cmdtype(t);
  } else {
    fprintf(stderr, "ERROR: The command '%s' does not exist\n", t->argv[0]);
    exitstatus = EXIT_FAILURE;
  }
  return exitstatus;
}

int execute_cmdtype(SHELLCMD *t){
  int exitstatus;

  if (strchr(t->argv[0],'/') != NULL) {
    exitstatus = execute_external(t);
  } else {
    if (t == NULL) { //Then there is a error
      fprintf(stderr, "ERROR: Invaild Command!\n");
      exitstatus = EXIT_FAILURE;
    } else {
      //printf("%s\n", t->argv[0]); //DEBUGGING
      if (strcmp(t->argv[0],"exit") == 0) {
        execute_exit(t);
      } else if (strcmp(t->argv[0],"cd") == 0) {
        exitstatus = execute_cd(t);
      } else if (strcmp(t->argv[0],"time") == 0) {
        exitstatus = execute_time(t);
      } else {
        exitstatus = execute_search(t);
      }
    }
  }
  return exitstatus;
}

int execute_pipe(SHELLCMD *t){

  return 0;
}

int execute_shellcmd(SHELLCMD *t){
  int exitstatus;

  if (t == NULL) {
    fprintf(stderr, "<nullcmd> ");
    exitstatus = EXIT_FAILURE;
  }

  switch (t->type) {
    case CMD_COMMAND:
      exitstatus = execute_cmdtype(t);
      break;

    case CMD_SEMICOLON:
      //printf("Running first cmd\n"); //Debugging
      exitstatus = execute_shellcmd(t->left);
      //printf("running ssecond cmd\n"); //Debugging
      exitstatus = execute_shellcmd(t->right);
      break;

    case CMD_AND:
      if (execute_shellcmd(t->left) == EXIT_SUCCESS) {
        exitstatus = execute_shellcmd(t->right);
      } else {
        exitstatus = EXIT_FAILURE;
      }
      break;

    case CMD_OR:
      if (execute_shellcmd(t->left) == EXIT_FAILURE) {
        exitstatus = execute_shellcmd(t->right);
      } else {
        exitstatus = EXIT_SUCCESS;
      }
      break;

    case CMD_SUBSHELL:
      printf("Not Coded int yet\n");
      break;

    case CMD_PIPE:
      printf("Not Coded in yet\n");
      break;

    case CMD_BACKGROUND:
      printf("Not Coded in yet\n");
      break;

    default:
      fprintf(stderr, "%s: Invalid NODETYPE\n", argv0);
      exitstatus = EXIT_FAILURE;
      break;
  }
  return exitstatus;
}
