/*Includes two functions: parse() and main()*/

#include "p2.h"


/*GLOBAL*/
/*an array of pointers (addresses for chars)*/
char *newargv[MAXITEM];
char w[STORAGE*MAXITEM];


// FUNCTIONAL MAIN - IF ALL ELSE FAILS REVERT BACK TO THIS

// int main(){
	
// 	int words;

// 	for(;;) {
// 		//this will exit the program because you don't start a child process!!!
// 		words = parse();
// 		if (words == 0) break;
// 		execvp(newargv[0], newargv);
// 	}
// 	return 0;
// }



int main(){
	
	int words;
	int kidpid; //variable used to designate the child

	for(;;) {
		//this will exit the program because you don't start a child process!!!
		printf("p2: ");
		words = parse();
		
		if (words == -10) break;
		// execvp(newargv[0], newargv);
		if(-1 == (kidpid = (int) fork())){ //if fork returns -1 it failed
			perror("Fork unsuccessful");
			exit(EXIT_FAILURE);
		} else if (0 == kidpid) { // if fork returns 0 that means we are the child
			
			// INSERT THINGS WE WANT THE CHILD TO DO
			execvp(newargv[0], newargv);
			perror("execve");   /* execve() returns only on error */
            exit(EXIT_FAILURE);

		} else { // WE ARE THE PARENT, we return the PID of the child we created...

			wait(NULL);
			
			//exit(EXIT_SUCCESS);
		}
	}
	kill(getpid(), SIGTERM);
	return 0;
}


/*
parse()
 - calls getword.c over and over until the
	whole input stream is done
 - puts words in a buffer
 - puts the addresses to those words
   in newargv[0], 1, 2 etc until there
   are no more words
*/ 
int parse(){

	int c; //will be the return val of getword() (size of word)
	int word_size; //will be used to decide moveForward (seen below)


	int word_count = 0; 
	int index = 0; //what index we are on in newargv[]
	int moveForward = 0; //how many spots to move forward in buffer
	
	

	//address of slot = address of w
	//memory slot = slot w

	/*pass memory location slots to getword */
	//if getword returns &, EOF, or newline we want to stop getting words and execute
	while((c = getword(w + moveForward)) > 0){

		newargv[index] = w + moveForward; //set newargv[index] = address of start of word
		index++; 
		word_size = abs(c);
		moveForward += word_size + 1;
		word_count++;

	}


	return word_count;
}


