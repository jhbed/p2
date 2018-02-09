/*
Project 1
Jake Bedard
Carroll
CS570
Due Date: ??

This function's purpose is to read from standard input and place one word
from the input stream into the memory location w, which is taken as an
argument. This function then returns the number of characters in that word.

Possible return values:
 -10 if the word is a newline ('\n') and nothing else
 -1 if the word is '#' and nothing else
 0 if the word is EOF and nothing else
 else, an integer equating to the number of chars in the word at memory location w
  
*/

#include "getword.h"
#include <stdbool.h>

int getword(char *w){
	int next;
	int input;
	int word_size = 0;
	bool isMeta = true;

	/*
	While there are still chars in the input stream,
	loop through the input and get the next char	
	*/
	while((input = getchar()) != EOF){		


		/*Backslash will make metacharacters treated as normal*/
		if(input == '\\' && isMeta == true){
			isMeta = false;
			continue;
		}

		if(input == '#' && word_size == 0 && isMeta == true){
			*w = input;
			w++;
			*w = '\0';
			return -1;
		}
		
		
		/*META CHARACTER LOGIC*/
		if(input == '>' || input == '<' || input == '&' || input == '|'){
			if(isMeta == true){
				if(word_size != 0){
					*w = '\0';
					ungetc(input, stdin);
					return word_size;
				}

				


				/*Peak ahead to see what the next char is*/
				/*if the next char is '&' we want to process it along with '|' in one word*/
				if((next = getchar()) == '&' && input == '|'){
					*w = input;
					w++;
					*w = next;
					w++;
					*w = '\0';
					return -2;
				/*If there is nothing special about the next character, put it back and process the meta as normal*/
				} else{
					ungetc(next, stdin);
					*w = input;
					w++;
					*w = '\0';
					return -1;
				}				
			}
							
		}
		/*END META CHARACTER LOGIC*/
	


		/*
		Checking to see if char is a spacebar	
		*/
		if(input == ' '){
			/*if spacebar isn't starting a word, end the word*/
			if(word_size != 0 && isMeta == true){
				*w = '\0';
				return word_size;
			} else if(isMeta == false){
				*w = input;
				w++;
				word_size++;
				*w = '\0';
			}
		/*only enter below conditions if not a spacebar
		  if it's a newline, replace newline with end word.		
		*/
		}else if(input == '\n'){
			*w = '\0';
			/*Check to see if \n is alone (not ending a word)*/
			if(word_size == 0)
				return -10;
			/*if ending a word, put \n back on the input stream
			  and return the word size. \n needs to be put back 
			  on the input stream because in this program it needs
			  to be treated as a word and return -10.
			*/
			else{	
				ungetc('\n',stdin);
				return word_size;
			}
		/*If this case is reached, that means it is a normal char.
		  add the char to the string.
		*/		
		}else {

			/*This logic is currently disfunctional*/
			if (word_size == STORAGE-2){
				*w = input;
				w++;
				*w = '\0';
				return word_size;
			}
			*w = input;
			w++;
			word_size++;
			*w = '\0';
			/*reset isMeta as we don't know if the next char will be meta or not*/
			isMeta = true;
		}


	}
/*EOF has been reached*/
	*w = '\0';
	return word_size;
}
