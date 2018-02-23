/*
Project 2
Jake Bedard
Carroll
CS570
Due Date: February 28, 2018

This file contains 4 functions:
 - main() - implements below functions to create an interactive shell
 - parse() - runs getword over and over, adding words to buffer and correct arguments to newargv
 - openFile() - opens a file, runs dup2 to redirect stdin or stdout
 - startPipe() - performs necessary pipeline operations when '|' is encountered
  
*/
/*DEV BRANCH*/
/*Includes two functions: parse() and main()*/

#include "p2.h"


/*GLOBALS*/
/*an array of pointers (addresses for chars)*/
char *newargv[MAXITEM];
char w[STORAGE*MAXITEM];
int moveForward = 0;  //how many spots to move forward in buffer
int c; //will be the return val of getword() (size of word)

/*DECLARE FLAGS*/

/*unlike the other flags, pipe flag can
be changed to any integer, and will mark the index
of newargv that is after the pipe
*/
int flag_amp = 0;
int flag_pipe = 0;
int flag_out = 0; //will be switched to 1 if '>' encountered
char *outfile; //will eventually point to a position in our buffer if an outfile is established
int flag_in = 0; //will be switched to 1 if '<' encountered
int flag_hashtag = 0;
char *infile;//will eventually point to a position in our buffer if an infile is established
pid_t dead_child;


void myHandler(){}


int main(){
	int link_result;
	int open_result;
	int words; //the amount of words returned from parse()
	pid_t kidpid; //variable used to designate the child
	char *home;
	int dev_null;
	
	home = getenv("HOME");

	



/*---------------------INIT GROUPPD & SIGNAL HANDLER---------------*/
	(void) setpgid(0, 0);
	(void) signal(SIGTERM, myHandler);
/*-----------------------------------------------------------------*/

	for(;;) {


        //ask carroll why these have to be in here - why can't these be declared outside loop...
		
		memset(w, 0, (STORAGE*MAXITEM));
		printf("p2: ");
		flag_in = 0;
		flag_out = 0;
		flag_pipe = 0;
		flag_amp = 0;
		flag_hashtag = 0;
		words = parse();	
	
			
		//pipe
		if (flag_pipe != 0) {
		    (void) startPipe(flag_pipe);
		    
		    continue; //SKIP FORK EXEC IN MAIN SINCE WE DO IT IN startPipe()
		}

		// if(flag_amp != 0){
		// 	//AMP LOGIC
		// 	//(void) fflush(stdout);
		// 	if(-1 == (kidpid = fork())){ //if fork returns -1 it failed
		// 		perror("& Fork unsuccessful");
		// 		exit(EXIT_FAILURE);

		// 	} else if(kidpid == 0) { //CHILD
		// 		//(void) fflush(stdout);
		// 		execvp(newargv[0], newargv);
		// 		perror("execvp error with & process: ");    execve() returns only on error 
		// 		exit(EXIT_FAILURE);

		// 	} else { //PARENT
		// 		//this is getting printed when it should not be getting printed
		// 		// for(;;){
					
		// 		// 	dead_child = wait(NULL);
		// 		// 	if (dead_child == kidpid)
		// 		// 	{
		// 		// 		break;
		// 		// 	}
		// 		// }
		// 		printf("%s [%d]\n", newargv[0], kidpid);


		// 	}
		// 	flag_amp = 0;
		// 	continue;
		// }


		//EOF and no words
		if(words == EOF){
			killpg(getpgrp(), SIGTERM);
			break;
		}

		//newline or hashtag
		if(flag_hashtag > 0 && words == 0)
			continue;

			

		//cd logic
		if(strcmp(newargv[0], "cd") == 0){
			if(newargv[1] == NULL){
				chdir(home);
			} else if(newargv[2] != NULL){
				printf("cd error: Too many arguments\n");
			} else {
				chdir(newargv[1]);
				newargv[1] = NULL;
			}

			continue;
		}
		//MV logic
		if(strcmp(newargv[0], "MV") == 0){
			if(newargv[1] == NULL || newargv[2] == NULL || newargv[3] != NULL){
				printf("MV error, must have 2 arguments for MV\n");
			} else {
				if((link_result = link(newargv[1], newargv[2])) < 0){
					perror("link ERROR");
					continue;
				}

				if((link_result = unlink(newargv[1])) < 0){
					perror("unlink ERROR");
				}
			}

			continue;
		}
/*----------------------END CHECK FOR FLAGS AND SPECIAL CASES----------------------*/

		
/*------------------NOTHING SPECIAL ENCOUNTERED - BEGIN FORK/EXEC PROCESS ----------------------*/
		if(flag_amp == 0){
			(void) fflush(stdout);
			(void) fflush(stderr);
		}
 		if(-1 == (kidpid = fork())){ //if fork returns -1 it failed
			perror("Fork unsuccessful");
			exit(EXIT_FAILURE);
		} else if (0 == kidpid) { // if fork returns 0 that means we are the child


			//check for infile/outfile flags set from parse so we know where to read/write
			//out flag
			if(flag_out == 1){
			
				if((open_result = openFile(outfile, 'o')) < 0){
					perror("openFile() function failed");
					//fflush(stdout);

					//continue;
				}
				
			}

			//in flag
			if(flag_in == 1){
				
				if((open_result = openFile(infile, 'i')) < 0){
					perror("openFile(infile, 'i') function failed");
					
				}
				
			}


			if(flag_in == 0){
				//dev_null = open("/dev/null", O_WRONLY);
				//(void) dup2(dev_null, STDIN_FILENO);
				//(void) close(dev_null);
			}
			
			execvp(newargv[0], newargv);
			perror("main execve failed in main child proc");   /* execve() returns only on error */
            exit(EXIT_FAILURE);

		} else { // WE ARE THE PARENT, we return the PID of the child we created...

			if(flag_amp == 0){
				for(;;){
					dead_child = wait(NULL);
					if (dead_child == kidpid)
					{
						break;
					}
				}
			} else{
				printf("%s [%d]\n", newargv[0], kidpid);
			}
			//exit(EXIT_SUCCESS);
		}
	}
	/*
	A break statement has been encountered, this means an EOF was encountered on a 0 word line
	In some odd cases (which should not be encountered) this also could mean there was an error, 
	where STDIN was never restored to the filedesc spot 0.  
	*/
	printf("p2 terminated.\n");
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


	//reset w and moveForward
	
	//memset(newargv, 0, (MAXITEM));
	moveForward = 0;

	
	

	/*pass memory location slots to getword */
	//if getword returns &, EOF, or newline we want to stop getting words and execute
	while((c = getword(w + moveForward)) != -10){

		if(*(w + moveForward) == '#' && word_count == 0){
			flag_hashtag = 1;
			moveForward+=2;
			continue;
		}

		
/* ----------- REDIRECT FILE LOGIC ------------------- */
		if(*(w + moveForward) == '>' || *(w + moveForward) == '<'){
			if(*(w + moveForward) == '>'){
				moveForward+=2;
				flag_out = 1;
				c = getword(w + moveForward); //get the next word
				outfile = (w + moveForward); // set outfile to location in buffer 
			} else{
				moveForward+=2;
				flag_in = 1;
				c = getword(w + moveForward); //get the next word
				infile = (w + moveForward); // set infile to location in buffer 
			}			
			//move along in the buffer without putting the word in newargv!!!
			word_size = abs(c);
			moveForward += word_size + 1;
			word_count++;
			continue;
		}
/* ----------- END REDIRECT FILE LOGIC ------------------- */

/* ------------------- PIPE LOGIC and amp logic ------------------------ */
		if(*(w + moveForward) == '|'){
			moveForward+=2;
			flag_pipe = index;
			word_count++;
			continue;
		}

		if(*(w + moveForward) == '&'){
			moveForward+=2;
			flag_amp = index;
			//newargv[index] = NULL;
			return word_count;
		}
/* ------------------- END PIPE LOGIC -------------------- */


		if(c == 0 && word_count == 0){
			return EOF;
		}

		newargv[index] = w + moveForward; //set newargv[index] = address of start of word
		index++; 
		newargv[index] = NULL;
		word_size = abs(c);
		moveForward += word_size + 1;
		word_count++;

	}

	newargv[index] = '\0'; 
	return word_count;
}
// locOfWord is the location of the name of the file
// inOrout = i if in, o if out
int openFile(char *locOfWord, char inOrOut){
	
	int dup_result;
	int close_result;
	int output_fd; //open() output - filedesc. array index

	int flags;
	int mode;
	char *filename = locOfWord;

	//mode is read write		

	



	if(inOrOut == 'o'){
		flags = O_CREAT | O_RDWR;
		mode = S_IRUSR | S_IRGRP | S_IROTH;
		if((output_fd=open(filename, flags, mode)) < 0){
			printf("File failed: %s\n", filename);
			perror("open failed: ");
			return output_fd;
			//exit(1);
		}
		if((dup_result = dup2(output_fd, STDOUT_FILENO)) < 0){
			perror("dup2 Error: ");
			//exit(1);
		}
	} else {
		flags = O_CREAT | O_RDONLY;
		mode = S_IRUSR | S_IRGRP | S_IROTH;
		if((output_fd=open(filename, flags, mode)) < 0){
			printf("File failed: %s\n", filename);
			perror("open failed");
			//exit(1);
			return output_fd;
		}
		if((dup_result = dup2(output_fd, STDIN_FILENO)) < 0){
			printf("%c\n", inOrOut);
			perror("OpenFile dup2 Error");
			//exit(1);
			return dup_result;
		}
	}

	if((close_result = close(output_fd)) < 0){
		perror("Close Error: ");
		//exit(1);
	} else {

		return close_result;
	}

}


/*
startPipe(int index)
 - recieves an index, which is the index in newargv that contains
   the word after the '|'
 - this function also forks a child and a grandchild and runs the 
 pipe() system call.
 
*/
int startPipe(int index){ //index designates the arg after the '|'
	int open_result;
    int pipe_result;
    int filedes[2];
    pid_t child;
    pid_t gc; //grand child
    int dup2_result;
    char *arg_loc;
    (void) fflush(stderr);
    (void) fflush(stdout);
    if((child = fork()) < 0){

    	perror("1st Fork Failure: ");
    } else if(child == 0) { //child


    	//check for the in/outfile flags here. we may want to do this in
    	//the grandchild instead


    	if(flag_out == 1){
    	
    		if((open_result = openFile(outfile, 'o')) < 0){
    			perror("openFile() function failed");
    			//fflush(stdout);

    			//continue;
    		}
    		
    	}

    	//in flag
    	if(flag_in == 1){
    		(void) fflush(stderr);
   			(void) fflush(stdout);
    		if((open_result = openFile(infile, 'i')) < 0){
    			perror("openFile(infile, 'i') function failed");
    			
    		}
    		
    	}




        //start pipe
        //filedes[1] is write to pipe
        //filedes[0] is read from pipe (think like reading from stdin)
        if ((pipe_result = pipe(filedes)) < 0) {
            perror("Pipe Failed: ");
            exit(2);
        } 

       

        if((gc = fork()) == 0){ //I am the grandchild
        	//writes to pipe
            //grandchild WRITES TO PIPE
            dup2(filedes[1], STDOUT_FILENO); //put STDOUT IN WRITE side of pipe
            //child does not want crap sent by fd[1], only stuff coming from pipe
            close(filedes[0]);
            close(filedes[1]);
           // printf("Executing 1st, should be sort: %s\n", newargv[0]);
            newargv[index] = NULL;
            execvp(newargv[0], newargv);

            perror("pipe exec 1");

        } else { // parent - do not wait
            //parent is reading from PIPE
            dup2(filedes[0], STDIN_FILENO); //PUT STDIN IN READ side of pipe
            //parent doesn't want to have a conflict on the back end
            close(filedes[0]);
            close(filedes[1]);
            //execute the thing that will be reading from pipe
            //printf("Executing 2nd, should be tr: %s\n", newargv[2]);

            
            //printf("executing 2nd, should be tr %s\n", newargv[index]);

            execvp(newargv[index], newargv+index);
            perror("pipe exec 2");
            //execvp(filedes[1])
            //doing something with the WRITE side of pipe

        }


    } else { //grandparent

        
        for (;;) {
            pid_t dead_c;
            if ((dead_c = wait(NULL)) == child) {
                break;
            }
        }

    }
    //restore stdin and stdout to original spots in fd array
    return 0;
}



