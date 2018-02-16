/*Includes two functions: parse() and main()*/

#include "p2.h"


/*GLOBAL*/
/*an array of pointers (addresses for chars)*/
char *newargv[MAXITEM];
char w[STORAGE*MAXITEM];
int moveForward = 0;  //how many spots to move forward in buffer
int c; //will be the return val of getword() (size of word)

/*DECLARE FLAGS*/
int flag_out = 0; //will be switched to 1 if '>' encountered
char *outfile; //will eventually point to a position in our buffer if an outfile is established
int flag_in = 0; //will be switched to 1 if '<' encountered
char *infile;//will eventually point to a position in our buffer if an infile is established


int main(){
	int words; //the amount of words returned from parse()
	pid_t kidpid; //variable used to designate the child
	int save_stdout;


/*---------------------INIT GROUPPD & SIGNAL HANDLER---------------*/



/*-----------------------------------------------------------------*/

	for(;;) {
		save_stdout = dup(STDOUT_FILENO);
		printf("p2: ");
		words = parse();	

		if(flag_out == 1){
		
			openFile(outfile);
			
		}
			


/*----------LOGIC FOR WHEN WE GET EOF && 0 WORDS FROM PARSE-----------------*/		
	// if(words == EOF){
	// 	//kill(grouppd, SIGTERM)
	//	break;
	// }


		//need to communicated EOF to main here, not just end at parse()
		
		//if (words == EOF) exit(0);
		if(-1 == (kidpid = fork())){ //if fork returns -1 it failed
			perror("Fork unsuccessful");
			exit(EXIT_FAILURE);
		} else if (0 == kidpid) { // if fork returns 0 that means we are the child
			
			//will this always be 
			execvp(newargv[0], newargv);
			perror("execve");   /* execve() returns only on error */
            exit(EXIT_FAILURE);

		} else { // WE ARE THE PARENT, we return the PID of the child we created...

			wait(NULL); // this should be in a loop see page 6

/*-------------REMOVE THE KILL STATEMENT BELOW EVENTUALLY------------------------*/
			kill((int) kidpid, SIGTERM); //I don't think this is what he wants, may have to adjust this logic!!
			dup2(save_stdout, 1);
			close(save_stdout);

			//exit(EXIT_SUCCESS);
		}
	}
	//kill(getpid(), SIGTERM);
	printf("made it to bottom of main\n");
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

	
	int word_size; //will be used to decide moveForward (seen below)

	int word_count = 0; 
	int index = 0; //what index we are on in newargv[]
	
	
	

	//address of slot = address of w
	//memory slot = slot w

	/*pass memory location slots to getword */
	//if getword returns &, EOF, or newline we want to stop getting words and execute
	while((c = getword(w + moveForward)) != -10){

		
/* ----------- REDIRECT FILE LOGIC ------------------- */
/*
CURRENT BUG: IN THE '>' LOGIC THE STREAMS ARE OVERWRITING 
EACH OTHER! TRY echo hi > test2 then echo > test3 hello my name is jake

*/
		if(*(w + moveForward) == '>'){
			flag_out = 1;
			moveForward+=2;
			//we want the next word we get to BE the name of the file
			c = getword(w + moveForward); //get the next word
			outfile = (w + moveForward); // set outfile to location in buffer

			//move along in the buffer without putting the word in newargv!!!
			word_size = abs(c);
			moveForward += word_size + 1;
			word_count++;
			continue;
		}

		if(c == 0 && word_count == 0){
			printf("\np2 Terminated\n");
			return EOF;
		}

		newargv[index] = w + moveForward; //set newargv[index] = address of start of word
		index++; 
		word_size = abs(c);
		moveForward += word_size + 1;
		word_count++;

	}

	newargv[index] = '\0'; 
	return word_count;
}

int openFile(char *locOfWord){
	
	int dup_result;
	int close_result;
	int output_fd; //open() output - filedesc. array index

	int flags;
	int mode;

	flags = O_CREAT | O_WRONLY; //if the file doesn't exist, create it
	mode = S_IRUSR | S_IWUSR; //mode is read write	

	char *filename = locOfWord;

	if((output_fd=open(filename, flags, mode)) < 0){
		printf("File failed: %s\n", filename);
		perror("open failed: ");
		exit(1);
	}

	if((dup_result = dup2(output_fd, STDOUT_FILENO)) < 0){
		perror("dup2 Error: ");
		exit(1);
	}

	//free(filename);

	if((close_result = close(output_fd)) < 0){
		perror("Close Error: ");
		exit(1);
	} else {
		return close_result;
	}

}

