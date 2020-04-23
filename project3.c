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

void parse(instruction* instr);
int getOffset(int currentDir);

void listContents(char* directory);

void size(char* file);
void makeDir(char directory[]);
void creatFile(char file[]);
void writeFile(char* file, int offset, int size, char* buffer);

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
unsigned int curDirClusterAddr;
unsigned int rootDirClusterAddr;


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

				

int main(){
	int filelistspot = 0;

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
	
	//set current directory to root
	curDir = BPB_RootClus;
	offset = getOffset(curDir);

	char rootDir[512];
	char dirNames[16];

	//read root directory
	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);

	rootDirClusterAddr = BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec + (BPB_RsvdSecCnt * BPB_BytesPerSec);
	curDirClusterAddr = rootDirClusterAddr;
	
	//printf("Root Cluster Check: %x\n", curDirClusterAddr);
	//printf("offset: %x\n", offset);

	do {
		printf("$ ");	
		clearInstruction(&instr);
		parse(&instr);
	
		if(strcmp(instr.tokens[0], "info") == 0){	//thomas
			if (instr.numTokens != 1) {
				printf("Error: No arguments expected\n");
				continue;
			}
			
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
			if (instr.numTokens != 2) {
				printf("Error: Incorrect number of arguments\n");
				continue;
			}
			
			size(instr.tokens[1]);
		}
	
		//prints contents of curDir
		//if a directory is listed, cd directory, print directory, then change back to previous
		else if(strcmp(instr.tokens[0], "ls") == 0){	//thomas
			if (instr.numTokens > 2) {
				printf("Error: Incorrect number of arguments\n");
				continue;
			}
			
			if(instr.tokens[1] != NULL)
				listContents(instr.tokens[1]);
			else 
				listContents(NULL);
		}
		
		else if(strcmp(instr.tokens[0], "cd") == 0){	//scott
			//printf("cd selected\n");

			int dircheck = 0;
			char temp[12];
			int a;
			int b;
			for(b = 1; b < 16; b=b+2){
				for(a = 0; a < 12; a++){
					temp[a] = 0;
					if(dir[b].DIR_Name[a] == ' ')
						break;	
					else
						temp[a] = dir[b].DIR_Name[a];

				}
				if(strcmp(instr.tokens[1], temp) == 0 && dir[b].DIR_Attr == 16){
					dircheck = 1;
					break;
				}
			}	
			
			if(dircheck == 1)
			{
				//printf("Addr: %d", curDirClusterAddr);

				//curDirClusterAddr = (( (int32_t) dir[b].DIR_FstClusLO - 2) * BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt) + (BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec);

				printf("It's a directory\n");
			}
			else
			{
				printf("Directory does not exist.\n");
			}
		
			//////// ***** you'll be changing curDirClusterAddr and curDir to where the dir starts) ////////////////////////////////////

			//check that dirname/inputitems[1] exists and is a directory
				//change current working directory to dirname
					//code will need to maintain current working directory state

			
			//if not print error
				//fprint("Directory does not exist.\n");
		}

		else if(strcmp(instr.tokens[0], "creat") == 0){	//taylor
			if(instr.tokens[2] != NULL){
				printf("Too many arguments.\n");
				continue;
			}

			if(instr.tokens[1] == NULL){
				printf("Missing arguments.\n");
				continue;
			}
			
			creatFile(instr.tokens[1]);
		}

		else if(strcmp(instr.tokens[0], "mkdir") == 0){	//taylor
			if(instr.tokens[2] != NULL){
				printf("Too many arguments.\n");
				continue;
			}
			
			if(instr.tokens[1] == NULL){
				printf("Missing arguments.\n");
				continue;
			}
	
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
				printf("Missing arguments.\n");
				continue;
			}
			

			//print error if invalid mode is used
			if(strcmp(instr.tokens[2], "r") != 0 && strcmp(instr.tokens[2],"w") != 0 && strcmp(instr.tokens[2], "rw") != 0 && strcmp(instr.tokens[2], "wr") != 0){
				printf("Invalid mode.\n");
				continue;
			}

			int filecheck = 0;
			
			//print error if file\inputitems[1] is already opened
			//print error if it is a directory

			int i;
			for(i = 0; i < 100; i++){
				if(strcmp(instr.tokens[1], &openFileList[i]) == 0)
				{
					printf("File is already opened.\n");
					break;
				}
			
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
					if(strcmp(instr.tokens[1], temp) == 0){
						filereal = 1;
						
						//check if file entered and give error if not
						if(dir[j].DIR_Attr == 16)
						{
							printf("Enter a file, not directory.\n");
							break;
						}



						break;
					}							
			}

			

			//check if file exists
			
			if(filereal == 0)
				printf("File does not exist.\n");


			else{
				if(strcmp(instr.tokens[2],"r") == 0)
					dir[filereal].DIR_Attr = 1;
			}

		strcpy(&openFileList[filelistspot],instr.tokens[1]);
		filelistspot= filelistspot + 10;

		}

		else if(strcmp(instr.tokens[0], "close") == 0){		//taylor

			if( instr.tokens[1] == NULL){
				printf("Missing parameters.\n");
				continue;
			}

			int opencheck = 0;
			int i;
			for(i = 0; i < 100; i++){
				if(strcmp(instr.tokens[1], &openFileList[i]) == 0)
				{
					opencheck = 1;
					printf("File has been successfully closed.\n");
					strcpy(&openFileList[i],"0");
				}
			}

		
			if(opencheck == 0){
				printf("Error: File is not open.\n");
				continue;
			}

			//set attribute
			char temp[12];
			int p;
			int j;
			for(j = 1; j < 16; j=j+2){
				for(p=0; p < 12;p++){
					temp[p] = 0;
					if(dir[j].DIR_Name[p] == ' ')
						break;
					else
						temp[p] = dir[j].DIR_Name[p];
				}
					if(strcmp(instr.tokens[1], temp) == 0){
						dir[j].DIR_Attr = 32;
						break;
					}
			}
		
		}

		else if(strcmp(instr.tokens[0], "read") == 0){	//scott
			//check params are all there
			if( instr.tokens[3] == NULL || instr.tokens[2] == NULL || instr.tokens[1] == NULL){
				printf("Missing parameters.\n");
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
					if(strcmp(instr.tokens[1], temp) == 0){
						
						//check if file entered and give error if not
						if(dir[j].DIR_Attr == 16)
						{
							filereal = 2;
							printf("Enter a file, not directory.\n");
							break;
						}

						filereal = 1;
						break;
					}							
			}

			

			//check if file exists
			
			if(filereal == 0){
				printf("File does not exist.\n");
				continue;
			}

			if(filereal == 2)
				continue;

			//check that file is open
			int opencheck = 0;
			int i;
			for(i = 0; i < 100; i++){
				if(strcmp(instr.tokens[1], &openFileList[i]) == 0)
				{
					opencheck = 1;
					break;
				}
			}

		
			if(opencheck == 0){
				printf("Error: File is not open.\n");
				continue;
			}



			//print error if offset\inputitems[2] is larger than the size of the file
			if((inputitems[2] - '0') > (inputitems[3] - '0'))
				printf("Error: Offset is larger than the size of the file.\n");
		
			//start reading the data from the file in the current working directory with the name filename/inputitems[1]
				//read from the file at offset/inputitems[2] bytes and stop after reading size/inputitems[3] bytes
				//if offset + size is larger than the size of the file, just ready size - offset bytes starting at offset

		}

		else if(strcmp(instr.tokens[0], "write") == 0){	//thomas
			if (instr.numTokens != 5) {
				printf("Error: Incorrect number of arguments\n");
				continue;
			}
				
			//writeFile(&instr);
		
			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");

			//print error if filename is a directory
				//printf("Must be file to read.\n");

			//print error if file is not opened for writing
				//printf("File is not open.\n");

			//print error if offset\inputitems[2] is larger than the size of the file
				//printf("Offset is larger than the size of the file.\n");
			
			//write to a file in current working directory with name filename/inputitems[1]
				//start writing at offset/inputitems[2] bytes and stop after writing size/inputitems[3] bytes. you will write string at this position
				//if offset + size is larger than the size of the file you will need to extend the length of the file to at least hold the data being written
				//if string is larger than size write only the first size characters of string. if string is smaller than size write the remaining character as ascii 0(null characters)



		}

		else if(strcmp(instr.tokens[0], "rm") == 0){	//thomas
			if (instr.numTokens != 2) {
				printf("Error: Incorrect number of arguments\n");
				continue;
			}

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
	fclose(imagefile);
	imagefile = NULL;
	clearInstruction(&instr);

	return 0;
}


void makeDir(char directory[]){
//check directory does not already exist
	char temp[12];
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
		if(strcmp(directory, temp) == 0){
			if(dir[j].DIR_Attr == 16)
				printf("Directory already exists.\n");
			else
				printf("File already exists with same name.\n");
			break;
		}	
								
	}

			
//create a new directory in the current working directory with the name dirname/inputitems[1]
			


}

void creatFile(char file[]){
//check file does not already exist
	char temp[12];
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
		if(strcmp(file, temp) == 0){
			if(dir[j].DIR_Attr == 32)
				printf("File already exists.\n");
			else
				printf("Directory already exists with same name.\n");
			break;
		}	
								
	}
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

void clearInstruction(instruction* instr_ptr) {
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

//returns offset of current directory
int getOffset(int currentDir) {
	if (currentDir == 0)
		currentDir = 2;
	
	int dirOffset = currentDir - 2;
	dirOffset = dirOffset * BPB_BytesPerSec;
	dirOffset += BPB_BytesPerSec * BPB_RsvdSecCnt;
	dirOffset += BPB_NumFATs * BPB_FATSz32 * BPB_BytesPerSec;
	
	return dirOffset;
}

///prints size of file in bytes
void size(char* file) {
	//get offset and read from directory
	offset = getOffset(curDir);
	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);
	
	int dircheck = 0;
	char temp[12];
	int a;
	int b;
	//check for file
	for(b = 1; b < 16; b=b+2){
		for(a = 0; a < 12; a++){
			temp[a] = 0;
			if(dir[b].DIR_Name[a] == ' ')
				break;	
			else
				temp[a] = dir[b].DIR_Name[a];
		}
		//if file name matches check that it is not a directory
		if(strcmp(file, temp) == 0 && dir[b].DIR_Attr != 16){
			dircheck = 1;
			break;
		}
	}	
	//print error if directory or not exist
	if (!dircheck) {
		printf("File %s does not exist\n", file);
	}
	printf("%d bytes\n", dir[b].DIR_FileSize);
}

///ls
///prints contents of directory
void listContents(char* directory) {
	//create temp variable to revert back to if cd is called
	int tempCurDir = curDir;
	
	offset = getOffset(curDir);
	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);

	if(directory != NULL){
		//need to check if directory exists
		//if not give error

		int dircheck = 0;
		char temp[12];
		int a;
		int b;
		for(b = 1; b < 16; b=b+2){
			for(a = 0; a < 12; a++){
				temp[a] = 0;
				if(dir[b].DIR_Name[a] == ' ')
					break;	
				else
					temp[a] = dir[b].DIR_Name[a];

			}
			if(strcmp(directory, temp) == 0 && dir[b].DIR_Attr == 16){
				dircheck = 1;
				break;
			}
		}	
		
		//do nothing for current directory
		if (strcmp(directory, ".") == 0);
		//read parent directory
		else if (strcmp(directory, "..") == 0) {
			//cd(..)
		}
		//directory does not exist
		else if(dircheck == 0) {
			printf("Directory %s does not exist.\n", directory);
			return;
		}
		//directory found, cd 
		else {
			//cd(dir)
		}
	}

	int p;
	int j;
	int q = 0;
	for(j = 1; j < 16; j=j+2){
		if(dir[j].DIR_Attr == 1 || dir[j].DIR_Attr == 16 || dir[j].DIR_Attr == 32){
			for(p = 0; p < 12; p++){
				printf("%c", dir[j].DIR_Name[p]);
			}
		printf("\n");
		}
	}
	printf(".\n");
	if (curDir != BPB_RootClus)
		printf("..\n");
	//change back to current directory
	curDir = tempCurDir;
}

void removeEntry(char* file) {
	offset = getOffset(curDir);
	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);
	
	int dircheck = 0;
	char temp[12];
	int a;
	int b;
	for(b = 1; b < 16; b=b+2){
		for(a = 0; a < 12; a++){
			temp[a] = 0;
			if(dir[b].DIR_Name[a] == ' ')
				break;	
			else
				temp[a] = dir[b].DIR_Name[a];

		}
		if(strcmp(file, temp) == 0 && dir[b].DIR_Attr != 16){
			dircheck = 1;
			break;
		}
	}	
	
	if (!dircheck) {
		printf("Error: File %s does not exist");
		return;
	}
}

void writeFile(char* file, int fileOffset, int size, char* buffer) {
	offset = getOffset(curDir);
	fseek(imagefile, offset, SEEK_SET);
	fread(&dir[0], 32, 16, imagefile);
	
	int dircheck = 0;
	char temp[12];
	int a;
	int b;
	for(b = 1; b < 16; b=b+2){
		for(a = 0; a < 12; a++){
			temp[a] = 0;
			if(dir[b].DIR_Name[a] == ' ')
				break;	
			else
				temp[a] = dir[b].DIR_Name[a];

		}
		if(strcmp(file, temp) == 0 && dir[b].DIR_Attr != 16){
			dircheck = 1;
			break;
		}
	}	
	
	if (!dircheck) {
		printf("Error: File %s does not exist");
		return;
	}
	
	/*if (fileOffset + size > dir[b].DIR_FileSize) {
		
	}*/
}
