#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char userinput[100];
char inputitems[10][10];



void parse(void){
int i,j,k;
j = 0;
k = 0;
fgets(userinput, sizeof userinput, stdin);

for(i = 0; i <= (strlen(userinput)); i++){
	if(userinput[i] == ' ' || userinput[i] == '\0')
	{
		inputitems[j][k] = '\0';
		j++;
		k = 0;
	}
	else
	{
		inputitems[j][k] = userinput[i];
		k++;
	}
}

/*	for(i = 0; i < j; i++)
		printf("%s", inputitems[i]);
*/
}


int main(){

	char* command = "start";

	while(command != "exit"){
		printf("$ ");	
		parse();
		

	
		if(strcmp(inputitems[0],"exit\n") == 0){
			printf("Exit selected\n");

			//free any allocated resources

			return 0;			
		}

		else if(strcmp(inputitems[0], "info\n") == 0){
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

		else if(strcmp(inputitems[0], "size") == 0){
			printf("Size selected\n");
			
			//check file name in inputitem[1] exists
				//print size of file in current working directory in bytes
			
			//if not print error
				//fprint("File does not exist.\n");

			
		}
	
		else if(strcmp(inputitems[0], "ls") == 0){
			printf("ls selected\n");
	
			//check that dirname/inputitems[1] exists and is a directory
				//print the name field for directories within conents of dirname including . and .. directories
					//print each direcotry entry on separate lines

			//if not print error
				//fprint("Directory does not exist.\n");
		}
		
		else if(strcmp(inputitems[0], "cd") == 0){
			printf("cd selected\n");

			//check that dirname/inputitems[1] exists and is a directory
				//change current working directory to dirname
					//code will need to maintain current working directory state
			
			//if not print error
				//fprint("Directory does not exist.\n");
		}

		else if(strcmp(inputitems[0], "creat") == 0){
			printf("creat selected\n");

			//check file does not already exist
				//creates a file in the current working directory with a size of 0 bytes and with a name of filename/inputitems[1]
			
			//if it does exist print error
				//fprint("File already exists.\n");
		}

		else if(strcmp(inputitems[0], "mkdir") == 0){
			printf("mkdir selected\n");

			//check directory does not already exist
				//create a new directory in the current working directory with the name dirname/inputitems[1]
			
			//if it does exist print error
				//printf("Directory already exists.\n");
		}

		else if(strcmp(inputitems[0], "mv") == 0){
			printf("mv selected\n");


			//if both to/inputitems[2] and from/inputitems[1] exist and are files print error
				//printf("The name is already being used by another file.\n");
			
			//if to is a file and from is a directory print error
				//printf("Cannot move directory: invalid destinationa argument.\n");

			//otherwise if to exists and is a directory, the system will move the from entry to be inside the to directory

			//if to does not exist the system will rename the from entry to the name specified by to

		}

		else if(strcmp(inputitems[0],"open") == 0){
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

		else if(strcmp(inputitems[0], "close") == 0){
			printf("close selected\n");

			//print error if file is not opened/in the table
				//printf("File is not open.");
		
			//print error if file does not exist
				//printf("File does not exist./n");

			//close file named filename/inputitems[1]
				//need to remove file entry from the open file table
								
		
		}

		else if(strcmp(inputitems[0], "read") == 0){
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

		else if(strcmp(inputitems[0], "write") == 0){
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

		else if(strcmp(inputitems[0], "rm") == 0){
			printf("rm selected\n");

			//print error if filename/inputitems[1] does not exist
				//printf("File does not exist.\n");

			//print error if filename is a directory
				//printf("Must be file to read.\n");
			
			//delete the file named filename from the current working directory
				//this means removing the entry in the directory as well as reclaiming the actual file data


		}

		else if(strcmp(inputitems[0], "cp") == 0){
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
	
		


	}







return 0;
}


