/*Includes two functions: parse() and main()*/

#include "p2.h"
#include "getword.h"
#include <stdlib.h>
#include <unistd.h>

/*GLOBAL*/
char *newargv[MAXITEM];

/*parse accepts parameter that points
 to a memory location */
int parse(){

	char w[STORAGE*MAXITEM];
	char *slot;
	int word_count = 0;
	int c;
	int word_size;
	int arg = 0;

	//address of slot = address of w
	slot = w;
	/*
	What does parse do?
	 - calls getword.c over and over until the
		whole input stream is done
	 - puts words in a buffer stream (done)
	 - puts the addresses to those words
	   in newargv[0], 1, 2 etc until there
	   are no more words
	*/ 
	/*pass memory location slots to getword */
	while((c = getword(slot)) != -10){
		if (c == 0) break;
		word_count++;
		word_size = abs(c);
		/*the value of newargv slot 0 is the address of 
		the first word*/

		//add the current words memory location
		// to newargv
		newargv[arg] = *slot;
		arg++;

		/*increment the slot
		that gets passed to getword*/
		slot = slot + word_size + 1;
	}


	return word_count;
}



int main(int argc, char *argv[]){
	/* a pointer to a location in memory that 
	stores chars */
	int words;
	/*a pointer to a location in memory 
	that stores POINTERS to chars 
	AN ARRAY OF ADDRESSES*/
	


	for(;;) {
		//
		words = parse(*newargv);
		execv(newargv[0], newargv);
		if (words == 0) break;

	}

	return 0;
}