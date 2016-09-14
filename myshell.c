/****************************************************
Arron Solano
COP4610
Project1

This is a C program that mimics the UNIX command
environment. The program takes the initial input and
stores the data into an array. It will then use the first
input to determine the action which are separated by if
statements.


****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PATH_MAX 1024
#define MAX_ITEM 10
const char whitespace[11] = " \n\r\f\t\v";
const char *fSlash = "/";

struct data
{
  char *arrayItems[MAX_ITEM];
  char *userName;
  char *cwd;
  char *copyInput;
  char input[100];
  char buff[PATH_MAX + 1];
  char *modVal;
};

struct stack
{
  int top;
  char *stackArray[MAX_ITEM];	
};

int main()
{
  struct data d;
  struct stack s;
  struct stat statBuff;
  char *token;
  char *pathName;
  char *inFile;
  char *outFile;
  char input[100];
  int max = 0;
  int i = 1;
  int top = -1;
  int c;
  int flagOut;
  int flagIn;
  int fdin, fdout;
  
  //Prints the command line prompt
  d.userName = getenv("USER");
  d.cwd = getcwd(d.buff, PATH_MAX + 1);
  printf("%s@myshell:%s>", d.userName, d.cwd);
  //This line gets the input
  fgets(d.input, 100, stdin);
  //This line makes a copy of the inputted item
  
  //Sets the values to NULL
  for(c = 0; c < MAX_ITEM; ++c)
  {
	d.arrayItems[c] = NULL;
  }

  token = strtok(d.input, whitespace);
  //This is the main loop
  while(strcmp(token, "x")!=0)
  {
	//This loop gathers all the tokens into array
    while(token != NULL)
    {
      d.arrayItems[max] = token;
      token = strtok(NULL, whitespace);
	  //input
	  if(strcmp(token, "<")==0)
	  {
	    token = strtok(NULL, whitespace);
		inFile = token;
		token = strtok(NULL, whitespace);
		flagIn = 1;
	  }
	  //output
	  if(strcmp(token, ">")==0)
	  {
	    token = strtok(NULL, whitespace);
		outFile = token;
		token = strtok(NULL, whitespace);
		flagOut = 1;
	  }
      max = max + 1;
    }
	
	if(flagIn == 1)
	{
	  fdin = dup(0);
	  char iFile[strlen(inFile) + 1];
	  strcpy(iFile, inFile);
	  int inNum = open(iFile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	  
	  if(inNum != -1)
	  {
	    dup2(inNum, 1);
		close(inNum);
	  }
	}
	
	if(flagOut == 1)
	{
	  fdout = dup(1);
	  char oFile[strlen(outFile) + 1];
	  strcpy(oFile, outFile);
	  int x = open(oFile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	  
	  if(x != -1)
	  {
	    dup2(x, 1);
		close(x);
	  }
	}
	
	//This does the echo command
    if(strcmp(d.arrayItems[0], "echo")==0)
    {
	  int trav;
	  for(trav = 1; trav < max; trav++)
      {
        printf("%s ", d.arrayItems[trav]);
      }
      printf("\n");
    }//end echo
	//this processes the exit command
    else if(strcmp(d.arrayItems[0], "exit")==0)
    {
      break;
    }//end exit program
	//This runs the change directory command
    else if(strcmp(d.arrayItems[0], "cd")==0)
    {
      int limit = max - 1;
      if(limit > 1)
      {
        printf("There are too many entries");
      }
      else if(strchr(d.arrayItems[1], '$')!=NULL)
      {
        token = d.arrayItems[1];
        ++token;
        pathName = getenv(token);
        chdir(pathName);
      }
      else
      {
        token = d.arrayItems[1];
        if(chdir(token)==-1)
        {
          printf("Directory does not exist.\n");
        }
      }
    }// end change dir
	//This runs the built in Stack push
    else if(strcmp(d.arrayItems[0], "pushd")==0)
    {
      int limit = max - 1;
      //We first check to see the number of commands
	  if(limit > 1)
      {
        printf("There are too many entries");
      }
	  //this if checks to see if there are any $
	  else if(strchr(d.arrayItems[1], '$')!=NULL)
	  {
	    token = d.arrayItems[1];
		++token;
		pathName = getenv(token);
		if(chdir(pathName)== -1)
		{
		  printf("Directory does not exist.\n");
		}
		else
		{
		  top++;
		  s.stackArray[top] = pathName;
		  chdir(pathName);
		  
		  int trav;
		  for(trav = top; trav >= 0; trav--)
		  {
		    printf("%s\n", s.stackArray[trav]);
		  }
		}
	  }//end check $
	  else
	  {
	    token = d.arrayItems[1];

		if(chdir(token)== -1)
		{
		  printf("Directory does not exist.\n");
		}
		else
		{
		  //If there are no $ this process takes over
		  char *pushName;
		  char buffPush[PATH_MAX + 1];
		  
		  top++;
		  chdir(token);
		  pushName = getcwd(buffPush, PATH_MAX + 1);
		  char *pushCopy = malloc(strlen(pushName)+1);
		  strcpy(pushCopy, pushName);
		  s.stackArray[top] = pushCopy;
		  
		  int trav;
		  for(trav = top; trav > -1; trav--)
		  {
		    printf("%s\n", s.stackArray[trav]);
		  }
		}//end if for pushd
	  }
	
    }//end pushd
	//This process from the built Stack pops the top entry
    else if(strcmp(d.arrayItems[0], "popd")==0)
    {
      top--;	  
	  if(top == -1)
	  {
	    printf("popd: Stack empty.\n");
	  }
	  else
	  {
	    int trav;
	    for(trav = top; trav >= 0; trav--)
	    {
		  printf("%s\n", s.stackArray[trav]);
	    }
	  }
    }//end popd
	//This section gets the rest of the commands. Assumes they are external commands.
	else
	{
	  //This first if checks if there are any / if so it will process the command.
	  if(strchr(d.arrayItems[0], '/'))
	  {
		token = d.arrayItems[0];
		//This checks for a valid command
		if(stat(token, &statBuff) == -1)
		{
		  printf("%s: Command not found.", token);
		}
		else
		{
		  //This processes the valid command
		  pid_t pid = fork();
		  
		  if(pid < 0)
		  {
		    printf("Failed to fork.\n");
			return 0;
		  }
		  
		  if(pid == 0)
		  {
		    char pathName[strlen(token) + 1];
		    strcpy(pathName, token);
		    execv(token, d.arrayItems);
			return 0;
		  }
		  waitpid(pid, NULL, 0);
		  token = NULL;
		}
	  }
	  else
	  //If there are no / then we need to get the correct path by traversing through the PATH file and testing
	  {
		int errCode;
		char copyPath[strlen(getenv("PATH") + 1)];
		strcpy(copyPath, getenv("PATH"));
		token = strtok(copyPath, ":");
		
		while(token != NULL)
		{
		  char pathName[strlen(token) + strlen(fSlash) + strlen(d.arrayItems[0]) + 1];
		  strcpy(pathName, token);
		  strcat(pathName, fSlash);
		  strcat(pathName, d.arrayItems[0]);
		  
		  if(stat(pathName, &statBuff) == -1)
		  {
		    errCode = 0;
		  }
		  else
		  {
			pid_t pid = fork();
			
			if(pid < 0)
			{
			  printf("Failed to fork.\n");
			  return 0;
			}
			
			if(pid == 0)
			{
			  execv(pathName, d.arrayItems);
			  fprintf(stderr, "Command error on %s \n", d.arrayItems[0]);	
			  return 0;
			}
			waitpid(pid, NULL, 0);
		    errCode = 1;
		    break;
		  }
		  token = strtok(NULL, ":");
		}//end while for PATH check
		if(errCode == 0)
		{
		  printf("%s: Command not found.\n", d.arrayItems[0]);
		}
	  }
	}//ends check with
	
	//This code resets the setting if necessary
    if(fdin != -1)
	{
	  dup2(fdin, 0);
	}
	//This code resets the setting if necessary
	if(fdout != -1)
	{
	  dup2(fdout, 1);
	}
	
    max = 0;
    i = 1;
    d.userName = getenv("USER");
    d.cwd = getcwd(d.buff, PATH_MAX + 1);
    printf("%s@myshell:%s>", d.userName, d.cwd);
    //This line gets the input
    fgets(d.input, 100, stdin);
    //This line makes a copy of the inputted item
    
    token = strtok(d.input, whitespace);
  }//end while	
  printf("Good Bye\n");
}//end main
