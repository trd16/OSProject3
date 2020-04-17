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
void openFile(char file[]);
void closeFile(char file[]);
void makeDir(char directory[]);
void creatFile(char file[]);

unsigned int BPB_BytesPerSec;
unsigned int BPB_SecPerClus;
unsigned int BPB_RsvdSecCnt;
unsigned int BPB_NumFATs;
unsigned int BPB_FATSz32;
unsigned int BPB_RootEntCnt;
unsigned int BPB_RootClus;
unsigned int BPB_TotSec32;
char BS_OEMName[8];

unsigned int curDir;
unsigned int FirstDataSector;
unsigned int FirstSectorofCluster;
unsigned int offset;

FILE *imagefile;
FILE *openfile;

struct __attribute__((__packed__)) DirectoryEntry{
	unsigned char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned char DIR_CrtTime[2];
	unsigned char DIR_CrtDate[2];
	unsigned char DIR_LstAccDate[2];
	unsigned char DIR_FstClusHI[2];
	unsigned char DIR_WrtTime[2];
	unsigned char DIR_WrtDate[2];
	unsigned char DIR_FstClusLO[2];
	unsigned char DIR_FileSize[4];

};

struct DirectoryEntry dir[16];

char openFileList[100];

				
int filelistspot = 0;

int main(){

	imagefile = fopen("fat32.img", "r");
	char* command = "start";
	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	
	fseek(imagefile, 3, SEEK_SET);
	fread(&BS_OEMName, 8, 1, imagefile);
	
	fseek(imagefile, 11, SEEK_SET);
	fread(&BPB_BytesPerSec, 2, 1, imagefile);
	fread(&BPB_SecPerClus, 1, 1, imagefile);
	fread(&BPB_RsvdSecCnt, 2, 1, imagefile);
	fread(&BPB_NumFATs, 1, 1, imagefile);
	fread(&BPB_RootEntCnt, 2, 1, imagefile);
	
	fseek(imagefile, 36, SEEK_SET);
	fread(&BPB_FATSz32, 4, 1, imagefile);

	fseek(imagefile, 44, SEEK_SET);
	fread(&BPB_RootClus, 4, 1, imagefile);

	
	FirstDataSector = (BPB_NumFATs * BPB_FATSz32) + BPB_RsvdSecCnt;
	FirstSectorofCluster = FirstDataSector + ((BPB_RootClus - 2) * BPB_SecPerClus);


	offset = BPB_RootClus - 2;
	offset = offset * BPB_BytesPerSec;
	offset+= BPB_BytesPerSec * BPB_RsvdSecCnt;
	offset+= BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec;

	char rootDir[512];
	char dirNames[16];

	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);
	
	do {
		printf("$ ");	
		clearInstruction(&instr);
		parse(&instr);
	
		if(strcmp(instr.tokens[0], "info") == 0){	//thomas
			printf("Info selected\n");
			
			//parse the boot sector. print the field name and corresponding values for each entry, one per line
				//bytes per sector
				printf("Bytes per Sector: %d\n", BPB_BytesPerSec);
				//sectors per cluster
				printf("Sectors Per Cluster: %d\n", BPB_SecPerClus);
				//reserved sector count
				printf("Reserved Sector Count: %d\n", BPB_RsvdSecCnt);
				//number of FATs
				printf("Number of FATs: %d\n", BPB_NumFATs);
				//total sectors 
				printf("Total Sectors: %d\n", BPB_TotSec32);
				//FATsize
				printf("FATsize: %d\n", BPB_FATSz32);
				//root cluster
				printf("Root Cluster: %d\n", BPB_RootClus);
		}

		else if(strcmp(instr.tokens[0], "size") == 0){	//thomas
			printf("Size selected\n");
			
			//check file name in inputitem[1] exists
				//print size of file in current working directory in bytes
			
			//if not print error
				//fprint("File does not exist.\n");

			
		}
	
		else if(strcmp(instr.tokens[0], "ls") == 0){	//thomas

			int p;
			int j;
			int q = 0;
			for(j = 1; j < 16; j=j+2){
				for(p = 0; p < 12; p++){
					printf("%c", dir[j].DIR_Name[p]);
				}
			printf("\n");	
	
			}



	/*		if (instr.numTokens > 1) {
				instr.tokens[1] = resolvePath(instr.tokens[1]);
			
				if (isValidDir(instr.tokens[1]))
					execute(instr.tokens);
				else
					printf("Invalid directory\n");
			}
			
			else 
				execute(instr.tokens);*/
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

			creatFile(instr.tokens[1]);


			}

		else if(strcmp(instr.tokens[0], "mkdir") == 0){	//taylor
			printf("mkdir selected\n");

			makeDir(instr.tokens[1]);	

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


			if( instr.tokens[1] == NULL || instr.tokens[2] == NULL){
				printf("Missing parameters.\n");
				continue;
			}
			
			//print error if invalid mode is used
			if(strcmp(instr.tokens[2], "r") != 0 && strcmp(instr.tokens[2],"w") != 0 && strcmp(instr.tokens[2], "rw") != 0 && strcmp(instr.tokens[2], "wr") != 0){
				printf("Invalid mode.\n");
				continue;
			}



			int filereal = 0;
			char temp[12];
			char directory[16];
			int p;
			int j;
			int z = 0;
			for(j = 1; j < 16; j=j+2){
				for(p = 0; p < 12; p++){
					temp[p] = 0;
					if(dir[j].DIR_Name[p] == ' ')
						break;	
					else
						temp[p] = dir[j].DIR_Name[p];

				}
					if(strcmp(instr.tokens[1], temp) == 0)
						filereal = 1;
								
			}

			

			//check if file exists
			
			if(filereal == 0)
				printf("File does not exist.\n");



			int filecheck = 0;
			
			//print error if file\inputitems[1] is already opened
				int i;
				for(i = 0; i < 100; i++){
					if(strcmp(instr.tokens[1],&openFileList[i]) == 0){
						filecheck = 1;
					}
				}


		//	if(filecheck == 1)
		//		printf("File is already opened.\n");
		//	else
		//		openFile(instr.tokens[1]);


		}

		else if(strcmp(instr.tokens[0], "close") == 0){		//taylor
			printf("close selected\n");

			if( instr.tokens[1] == NULL){
				printf("Missing parameters.\n");
				continue;
			}
			else
				closeFile(instr.tokens[1]);
		
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

		else if(strcmp(instr.tokens[0], "exit") == 0){
			//return 0;
			break;
		
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





void openFile(char file[]){


//open a file named filename in the current working directory
	openfile = fopen(file, "r");

//	if(openfile == NULL)
//	{
//		printf("Error: file does not exist.\n");
//		return;
//	}	
//	else{
		printf("Opened %s\n", file);
		openFileList[filelistspot] = *file;		
		filelistspot++;




}


void closeFile(char file[]){
//print error if file is not opened/in the table
	int filecheck = 0;
	int i;
	for(i = 0; i < 100; i++){
		if(strcmp(file,&openFileList[i]) == 0)
			filecheck = i;
	}

	if(filecheck == 0)
		printf("File is not open.");
		
	else{
	//close file named filename/inputitems[1]
	//need to remove file entry from the open file table
		strcpy(&openFileList[i], "");		
		fclose(openfile);								
	}		

}


void makeDir(char directory[]){
//check directory does not already exist
	//create a new directory in the current working directory with the name dirname/inputitems[1]
			
//if it does exist print error
	//printf("Directory already exists.\n");
	

}


void creatFile(char file[]){
//check file does not already exist
	//creates a file in the current working directory with a size of 0 bytes and with a name of filename/inputitems[1]
			
//if it does exist print error
	//fprint("File already exists.\n");



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


