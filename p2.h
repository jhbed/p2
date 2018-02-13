#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "getword.h"

#define MAXITEM 100 /*max number of words per line*/
int parse();
int openFile(int nameLength, char *locOfWord);