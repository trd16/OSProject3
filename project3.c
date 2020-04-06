#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct{
	char** tokens;
	int numTokens;
} instruction;

pid_t child_pids[1000];
int child_nb = 0;

void addToken(instruction* instr_ptr, char* tok);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
void printTokens(instruction* instr_ptr);

char userinput[100];
char inputitems[10][10];

int numberOfDirectories(char* path);
char* expandPath(char* path);
char* resolvePath(char* path);
int isValidDir(char* path);
int isValidFile(char* path);
int isExecutable(char* file);

void execute(char** cmd);
void parse(instruction* instr);

int main(){
	char* command = "start";
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	do {
		printf("$ ");	
		clearInstruction(&instr);
		parse(&instr);
	
		if(strcmp(instr.tokens[0], "info") == 0){	//thomas
			printf("Info selected\n");
			
			//parse the boot sector. print the field name and corresponding values for each entry, one per line
				//bytes per sector
				//sectors per cluster
				//reserved sector count
				//number of FATs
				//total sectors 
				//FATsize
				//root cluster
		}

		else if(strcmp(instr.tokens[0], "size") == 0){	//thomas
			printf("Size selected\n");
			
			//check file name in inputitem[1] exists
				//print size of file in current working directory in bytes
			
			//if not print error
				//fprint("File does not exist.\n");

			
		}
	
		else if(strcmp(instr.tokens[0], "ls") == 0){	//thomas
			if (instr.numTokens > 1) {
				instr.tokens[1] = resolvePath(instr.tokens[1]);
			
				if (isValidDir(instr.tokens[0]))
					execute(instr.tokens);
				else
					printf("Invalid directory\n");
			}
			
			else 
				execute(instr.tokens);
		}
		
		else if(strcmp(instr.tokens[0], "cd") == 0){	//scott
			printf("cd selected\n");

			//check that dirname/inputitems[1] exists and is a directory
				//change current working directory to dirname
					//code will need to maintain current working directory state
			
			//if not print error
				//fprint("Directory does not exist.\n");
		}

		else if(strcmp(instr.tokens[0], "creat") == 0){	//taylor
			printf("creat selected\n");

			//check file does not already exist
				//creates a file in the current working directory with a size of 0 bytes and with a name of filename/inputitems[1]
			
			//if it does exist print error
				//fprint("File already exists.\n");
		}

		else if(strcmp(instr.tokens[0], "mkdir") == 0){	//taylor
			printf("mkdir selected\n");

			//check directory does not already exist
				//create a new directory in the current working directory with the name dirname/inputitems[1]
			
			//if it does exist print error
				//printf("Directory already exists.\n");
		}

		else if(strcmp(instr.tokens[0], "mv") == 0){	//scott
			printf("mv selected\n");


			//if both to/inputitems[2] and from/inputitems[1] exist and are files print error
				//printf("The name is already being used by another file.\n");
			
			//if to is a file and from is a directory print error
				//printf("Cannot move directory: invalid destinationa argument.\n");

			//otherwise if to exists and is a directory, the system will move the from entry to be inside the to directory

			//if to does not exist the system will rename the from entry to the name specified by to

		}

		else if(strcmp(instr.tokens[0],"open") == 0){	//taylor
			printf("open selected\n");

			//print error if file\inputitems[1] is already opened
				//printf("File is already opened.\n");

			//print eror if file does not exist
				//printf("File does not exist.\n");

			//print error if invalid mode is used
				//printf("Invalid mode.\n");

			//open a file named filename in the current working directory
				//file can only be read from or written to if it is opened first
				//need to maintain table of opened files and add filename to it when open is called
				//mode is a string and is only valid if it is one of the following
					//r - read-only
					//w - write-only
					//rw - read and write
					//wr - write and read

		}

		else if(strcmp(instr.tokens[0], "close") == 0){		//taylor
			printf("close selected\n");

			//print error if file is not opened/in the table
				//printf("File is not open.");
		
			//print error if file does not exist
				//printf("File does not exist./n");

			//close file named filename/inputitems[1]
				//need to remove file entry from the open file table
								
		
		}

		else if(strcmp(instr.tokens[0], "read") == 0){	//scott
			printf("read selected");

			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");

			//print error if filename is a directory
				//printf("Must be file to read.\n");

			//print error if file is not opened for reading
				//printf("File is not open.\n");

			//print error if offset\inputitems[2] is larger than the size of the file
				//printf("Offset is larger than the size of the file.\n");
		
			//start reading the data from the file in the current working directory with the name filename/inputitems[1]
				//read from the file at offset/inputitems[2] bytes and stop after reading size/inputitems[3] bytes
				//if offset + size is larger than the size of the file, just ready size - offset bytes starting at offset

		}

		else if(strcmp(instr.tokens[0], "write") == 0){	//thomas
			printf("write selected\n");
		
			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");

			//print error if filename is a directory
				//printf("Must be file to read.\n");

			//print error if file is not opened for reading
				//printf("File is not open.\n");

			//print error if offset\inputitems[2] is larger than the size of the file
				//printf("Offset is larger than the size of the file.\n");
			
			//write to a file in current working directory with name filename/inputitems[1]
				//start writing at offset/inputitems[2] bytes and stop after writing size/inputitems[3] bytes. you will write string at this position
				//if offset + size is larger than the size of the file you will need to extend the length of the file to at least hold the data being written
				//if string is larger than size write only the first size characters of string. if string is smaller than size write the remaining character as ascii 0(null characters)



		}

		else if(strcmp(instr.tokens[0], "rm") == 0){	//thomas
			printf("rm selected\n");

			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");

			//print error if filename is a directory
				//printf("Must be file to read.\n");
			
			//delete the file named filename from the current working directory
				//this means removing the entry in the directory as well as reclaiming the actual file data


		}

		else if(strcmp(instr.tokens[0], "cp") == 0){	//scott
			printf("cp selected\n");
			
			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");
			
			//copy the file specified by filename
				//if to/inputitems[2] is a directory, copy the file directly into a folder
				//if to is not a valid entry, create a copy of the file within the current working directory and assign it to the name given by to
			


		}

		else{
			printf("Error: Wrong Command Name\n");
		}
	
	} while (instr.tokens[0], "exit");

	//exit
	//free memory
	clearInstruction(&instr);

	return 0;
}

///parses user input
///places tokens inside of instruction pointer passed
void parse(instruction* instr) {
	char* token = NULL;
	char* temp = NULL;
	
	// loop reads character sequences separated by whitespace
	do {
		//scans for next token and allocates token var to size of scanned token
		scanf("%ms", &token);
		temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

		int i;
		int start = 0;
		for (i = 0; i < strlen(token); i++) {
			//pull out special characters and make them into a separate token in the instruction
			if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
				if (i-start > 0) {
					memcpy(temp, token + start, i - start);
					temp[i-start] = '\0';
					addToken(instr, temp);
				}

				char specialChar[2];
				specialChar[0] = token[i];
				specialChar[1] = '\0';

				addToken(instr, specialChar);

				start = i + 1;
			}	
		}
		
		if (start < strlen(token)) {
			memcpy(temp, token + start, strlen(token) - start);
			temp[i-start] = '\0';
			addToken(instr, temp);
		}

		//free and reset variables
		free(token);
		free(temp);

		token = NULL;
		temp = NULL;
	} while ('\n' != getchar());    //until end of line is reached
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
	instr_ptr->numTokens++;
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

int numberOfDirectories(char* path) {
	int i, num = 0;
	for (i = 0; i < strlen(path); ++i) {
		if (path[i] == '/')
			++num;
	}
	return num;
}

///expands path to absolute path
///returns NULL if ".." goes out of bounds
char* expandPath(char* path) {
	char* newPath;
	
	//set ~ to home directory
	if (path[0] == '~') {
		int len = strlen(path) + strlen(getenv("HOME"));
		newPath = (char*)malloc(len * sizeof(char));
		strcpy(newPath, getenv("HOME"));
		
		//get rest of path
		int i, j = strlen(newPath);
		for (i = 1; i < strlen(path); ++i)
			newPath[j++] = path[i++];
		newPath[j] = '\0';
	}
	//relative path
	else if (path[0] != '/') {
		int len = strlen(getenv("PWD")) + strlen(path);
		newPath = (char*)malloc((len + 2) * sizeof(char));
		strcpy(newPath, getenv("PWD"));
		strcat(newPath, "/");
		strcat(newPath, path);
	}
	//absolute path
	else
		strcpy(newPath, path);

	int numDirs = numberOfDirectories(newPath);
	if (numDirs == 0) {
		free(newPath);
		return NULL;
	}
	
	char** pathArray = (char**)malloc(numDirs * sizeof(char*));
	int i;
	for (i = 0; i < numDirs; ++i)
		pathArray[i] = (char*)malloc(strlen(newPath) * sizeof(char));
	
	i = 0;
	//split path into array
	char* ptr = strtok(newPath, "/");
	while (ptr != NULL) {
		//decrement i on ".."
		if (strcmp(ptr, "..") == 0) {
			--i;
			//invalid path, deallocate memory and return NULL
			if (i < 0) {
				free(newPath);
				
				int j;
				for (j = 0; j < numDirs; ++j)
					free(pathArray[j]);
				free(pathArray);
				
				return NULL;
			}
		}
		//only add directory to array if not . or ..
		else if (strcmp(ptr, ".") != 0) 
			strcpy(pathArray[i++], ptr);
		
		ptr = strtok(NULL, "/");
	}
	
	strcpy(newPath, "/");
	int j;
	for (j = 0; j < numDirs; ++j) {
		if (j < i) {
			//add directory to newPath
			if (j != 0)
				strcat(newPath, "/");
			strcat(newPath, pathArray[j]);
		}
		free(pathArray[j]);
	}
	free(pathArray);
	
	return newPath;
}

///expands path of executables to absolute path
///searches in PWD and all of PATH
///return null if not found
char* resolvePath(char* path) {
	//absoulte path
	if ( path[0] == '/') {
		if (isValidFile(path))
			return path;
		return NULL;
	}
	
	char* newPath = (char*)malloc(256 * sizeof(char));
	
	//test for "./path" first
	strcpy(newPath, "./");
	strcat(newPath, path);
	
	char* ptr = expandPath(newPath);
	if (ptr == NULL) {
		free(ptr);
		free(newPath);
		return NULL;
	}
	strcpy(newPath, ptr);
	free(ptr);
	
	if (isValidFile(newPath))
		return newPath;
	
	char PATHenv[1024];
	strcpy(PATHenv, getenv("PATH"));
	
	//split PATH by ":" and test each variable
	ptr = strtok(PATHenv, ":");
	while (ptr != NULL) {
		strcpy(newPath, ptr);
		strcat(newPath, "/");
		strcat(newPath, path);
		
		if (isValidFile(newPath))
			return newPath;
		
		ptr = strtok(NULL, ":");
	}
	
	//not found
	free(newPath);
	return NULL;
}

int isValidDir(char* path) {
	DIR* dir = opendir(path);
	if (dir)
	{
		closedir(dir);
		return TRUE;
	}
	return FALSE;
}

int isValidFile(char* path) {
	if (access(path, F_OK) != -1)
		return TRUE;
	return FALSE;
}

int isExecutable(char* file) {
	if (access(file, X_OK) != -1)
		return TRUE;
	return FALSE;
}

void execute(char** cmd) {
	int status;
	pid_t pid = fork();
	if(pid == -1)
	{
		//error
		exit(1);
	}
	else if(pid == 0)
	{
		//child
		execv(resolvePath(cmd[0]), cmd);
		printf("Problem executing %s\n", cmd[0]);
		exit(1);
	}
	else
	{
		//parent
		child_pids[child_nb++] = pid;
		waitpid(pid,&status, 0);
	}
}