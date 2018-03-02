// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

/*#define BUFFER_SIZE 50
static char buffer[BUFFER_SIZE];*/

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
char history[HISTORY_DEPTH][COMMAND_LENGTH] ;//= { {'\0' } };;
int history_indices[HISTORY_DEPTH] = {-1};
int command_Counter = 0 ;

int ReceivedCtrlC = 0;



void overwrite_PreviousCommand(char myhistory[HISTORY_DEPTH][COMMAND_LENGTH], int myhistory_indices[HISTORY_DEPTH]){

	//strcpy(dest, src);
	strcpy(myhistory[(command_Counter -1)%10 ], myhistory[(command_Counter-2)%10]);
}

int overwrite_Command(char myhistory[HISTORY_DEPTH][COMMAND_LENGTH], int myhistory_indices[HISTORY_DEPTH], int index){

	//!10 means index will be 10 
	//if the array is full then the command to be replaced will be at the end of the array[HISTORY_DEPTH -1] or if the array is not full the command to be replaced will be at (index-1)

	// 6 commmands ->  !3 and command_Counter = 7 so command_Counter is always 1 ahead

	int j = 0;

	if(command_Counter > HISTORY_DEPTH){

		for(j = 0; j <= HISTORY_DEPTH -1 ; j ++){

			if(myhistory_indices[j] == (index)){
				strcpy(myhistory[HISTORY_DEPTH -1 ], myhistory[j]);
				break;
			}

		}



	}
	else{	//case where the array is not full yet, then we want to tokenize at array index j - 1

		//strcpy(dest, src);
		strcpy(myhistory[command_Counter -1 ], myhistory[ index - 1]);

		j = command_Counter - 1;

			/*for(int j = 0; j <= command_Counter; j++){

				if(myhistory_indices[j] == index - 1){

					strcpy(myhistory[(index - 1)%10],myhistory[j]);

				}
			}*/

	}
	//strcpy(myhistory[index - 1],myhistory[command_Counter -1 ]);

	return j;
}

void add_Command(char myhistory[HISTORY_DEPTH][COMMAND_LENGTH], char *buff, int myhistory_indices[HISTORY_DEPTH]){


	//add command is working but print history is slightly off!printing garbage

	int k = 0;

	while(buff[k] != '\n'){

		k++;
	}
	buff[k] = '\0';


	if(command_Counter>= HISTORY_DEPTH){


		for (int y = 0; y<= HISTORY_DEPTH-1; y++){
			//strcpy(dest, src);

			strcpy(myhistory[y],myhistory[y+1]);
			myhistory_indices[y] = myhistory_indices[y+1];


		}
		strcpy(myhistory[HISTORY_DEPTH - 1],buff);
		myhistory_indices[HISTORY_DEPTH - 1] = command_Counter+1;

	}
	else{

		strcpy(myhistory[command_Counter],buff);
		myhistory_indices[command_Counter] = command_Counter+1;

	}
}

void print_history(char myhistory[HISTORY_DEPTH][COMMAND_LENGTH], int myhistory_indices[HISTORY_DEPTH]){

	int end_Index = HISTORY_DEPTH - 1;

	//forloop for finding the end index
	for (int i = 0;i <= HISTORY_DEPTH - 1; i++){
	
			if(myhistory_indices[i] == 0){
				end_Index = i- 1;
				break;
			}

	}

	char output_String[200];
	for (int j = 0; j <= end_Index; j++){

		//char output_String[200];
		//output_String = "";
		//strcpy(output_String, "\0");//, strlen("\0"));
		sprintf(output_String, "%d\t%s\n", myhistory_indices[j], myhistory[j]);
		//sprintf(output_String, "%d", myhistory_indices[j]);

		//printf("%s\n",myhistory[j]);
		
		//write(STDOUT_FILENO, output_String, strlen(output_String));
		//strcpy(output_String, "\0");//, strlen("\0"));
		//sprintf(output_String, "\t%s", myhistory[j]);

		//sprintf(output_String, "%d\t%s\n", myhistory_indices[j], myhistory[j]);

		write(STDOUT_FILENO, output_String, strlen(output_String));

	}



}


/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	//copy the user input
	add_Command(history, buff, history_indices);
	command_Counter++;

/*	if(command_Counter == 11){

			write (STDOUT_FILENO,"pause", strlen("pause"));
	}*/


	/*if (length < 0) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}*/
	if ( (length < 0) && (errno !=EINTR) ){
    perror("Unable to read command. Terminating.\n");
    exit(-1);  /* terminate with error */
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

/**
 * Main and Execute Commands
 */




/* Signal handler function */
void handle_SIGINT()
{
//write(STDOUT_FILENO, "buffer\n", strlen("buffer\n"));
//char *tokens[NUM_TOKENS];
//add history to history array
//increment commandcounter
//print the command
//tokenize most recent array in history
/*char myString[8];
//strcpy(dest, src);
strcpy(myString, "history");
myString[8] = '\0';
command_Counter++;
add_Command(history, myString, history_indices);
//tokens[0] = myString;
write(STDIN_FILENO, myString, strlen(myString)) ;
write(STDIN_FILENO, "\n", strlen("\n"));
//tokenize_command(history[command_Counter - 1], tokens);
print_history(history, history_indices);*/
write(STDIN_FILENO, "\n", strlen("\n"));
ReceivedCtrlC = 1;


}


int main(int argc, char* argv[])
{
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);

	char cwd[256];

	//int command_Counter = 0;//keep track of # of commands

	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

    int status;

	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().





		write(STDOUT_FILENO, strcat(getcwd(cwd, sizeof(cwd)),"> "), strlen(strcat(getcwd(cwd, sizeof(cwd)),"> ")));
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);
		
		
		//command_Counter++;
		//add_Command(history, tokens, history_indices, command_Counter);

		// DEBUG: Dump out arguments:
		/*for (int i = 0; tokens[i] != NULL; i++) {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
			//write(STDOUT_FILENO, "%i", sizeof(tokens));
		}*/

		if(ReceivedCtrlC == 1){
			tokens[0] = "history";
			ReceivedCtrlC = 0;
		}


		restartLoop : if (tokens[0] == NULL)
		{

			continue;

		}
		else if (strcmp(tokens[0], "!!")==0){

			 //write(STDIN_FILENO, "Command not found", strlen("Command not found"));

			//if (history[command_Counter-1] == 0){
			if(command_Counter == 1){
               write(STDIN_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
			   command_Counter--;
               continue;
            }
			else{

				overwrite_PreviousCommand(history, history_indices);

				write(STDIN_FILENO, history[command_Counter - 1], strlen(history[command_Counter - 1]));
            	write(STDIN_FILENO, "\n", strlen("\n"));
				tokenize_command(history[(command_Counter - 1)], tokens);
				goto restartLoop;


			}
			//else if (myhistory[command_Counter - 1])
		}
		else if (strchr(tokens[0], '!'))
		{
			//int line_Number = atoi(&tokens[0][1]);
			int line_Number;
			sscanf(tokens[0], "!%d", &line_Number);

			if(line_Number <= 0||line_Number < command_Counter - 9 || line_Number > command_Counter)	//invalid history number
			{
				write(STDIN_FILENO, "SHELL: Unknown history command.\n", strlen("SHELL: Unknown history command.\n"));
				 continue;
			}
			else{

				int IndexofNextTokens = overwrite_Command(history, history_indices,line_Number);

				write(STDIN_FILENO, history[(IndexofNextTokens) % 10], strlen(history[(IndexofNextTokens) % 10]) ) ;
            	write(STDIN_FILENO, "\n", strlen("\n"));
				tokenize_command(history[(IndexofNextTokens) % 10], tokens);
				goto restartLoop;






/*				//valid history number
				write(STDIN_FILENO, history[(line_Number-1) % 10], strlen(history[(line_Number-1) % 10]) ) ;
            	write(STDIN_FILENO, "\n", strlen("\n"));
				add_Command(history, history[(line_Number-1) % 10], history_indices);
				command_Counter++;
            	tokenize_command(history[(line_Number-1) % 10], tokens);
				goto restartLoop;*/
			}

		}
        else if(strcmp(tokens[0], "exit") == 0)
		{

            return 0;

        }
		else if (strcmp(tokens[0], "pwd") == 0)
		{

			//char cwd2[256];

			//write (STDOUT_FILENO,;
			getcwd(cwd, sizeof(cwd));
			//write (STDOUT_FILENO,cwd, sizeof(cwd));

		}
		else if (strcmp(tokens[0], "cd") == 0)
		{
			//prof said in the online discussion that we don't need to account for the case when the directory contains a space
			if (chdir(tokens[1]) <0 )
			{
				write(STDOUT_FILENO, "Error: Could not change directory.\n", strlen("Error: Could not change directory.\n"));
			}

		}
		else if (strcmp(tokens[0], "history") == 0)
		{
			print_history(history, history_indices);

		}


			pid_t pid;
			pid = fork();

				//Following the examplefrmo the proffesor's notes
			if (pid < 0 ){
				write (STDOUT_FILENO,"Fork Failed\n", strlen("Fork Failed\n"));
				exit(-1);
			}

			else if (pid == 0){  //child process will have pid = 0 

				execvp(tokens[0], tokens);  //call the command that is stored in tokens[0] and pass in the parameters ( if there are any ) which are in the following tokens
				exit(-1);
		
			}
			else { // the parent process will execute this case
		
				//the parent process does not wait for the child process
				if(in_background == false){

					while (waitpid(pid, &status, 0) > 0){; }

				}
			}

			// Cleanup any previously exited background child processes
			// (The zombies)
			while (waitpid(-1, NULL, WNOHANG) > 0)
				{;} // do nothing.

			

			if (in_background) {
				write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
			}

			/**
			* Steps For Basic Shell:
			* 1. Fork a child process
			* 2. Child process invokes execvp() using results in token array.
			* 3. If in_background is false, parent waits for
			*    child to finish. Otherwise, parent loops back to
			*    read_command() again immediately.
			*/


    }
    

    return 0;
}