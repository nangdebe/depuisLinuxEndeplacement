#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <utime.h>
#define buffer_size 4096 //max allocation
 
//copies a single file to new target file
void CopyFile(char *fsource, char *ftarget){
    //opens the source file, returns the file's description
    int file1 = open(fsource, O_RDONLY);
    if (file1 == -1){
        printf("failed to open file!\n");
    }
    //file2 is the target file
    int file2;
    struct stat statbuf;         //retrieve information about the file pointed to by pathname
    struct utimbuf timeby;       //retrieve time information
    char BUFFER[buffer_size];
    int wordbit;                 //holds the size of the file
    //put the information of source file into statbuf
    stat(fsource,&statbuf);
    //create a new file as target file, return its description
    file2 = creat(ftarget,statbuf.st_mode);
    if (file2 == -1){    //but to copy we have to check first if it is the target file exist
        printf("failed to create new file!\n");  //....or it been sussesfully created 
    }
    //read and write files
    while((wordbit = read(file1,BUFFER,buffer_size)) > 0){
        //if the bit number written is different from source file
        if (write(file2,BUFFER,wordbit) != wordbit)
            printf("failed to write in target file!\n");
    }
    //modify the time attributions
    timeby.actime = statbuf.st_atime;
    timeby.modtime = statbuf.st_mtime;
    utime(ftarget,&timeby);
    //close them up at the end
    close(file1);
    close(file2);
}
 
//copy the source folder to target folder
void mycp(char *fsource,char *ftarget){
    char source[buffer_size];
    char target[buffer_size];
    struct stat statbuf;
    struct utimbuf timeby;
    struct dirent *entry;
    strcpy(source,fsource);
    strcpy(target,ftarget);
    DIR *dir;
    //open the source folder
    dir = opendir(source);
    /**************************************
       read contents in source folder
       return the next accessible file entrance node, 
       if reached the end of the dir dictionay return null
    **************************************************/
    while((entry = readdir(dir)) != NULL){
        //the file name is neither a dot "." nor double dot ".."
        if (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0){
            continue;
        }
        //if the current item is a folder
        if (entry->d_type == 4){
            strcat(source,"/"); //strcat:"Appends a copy of the source string to the destination string."
            strcat(source,entry->d_name);
            strcat(target,"/");
            strcat(target,entry->d_name);
            //get the state in source folder and fill it into statbuf
            stat(source,&statbuf);
            //create target folder
            mkdir(target,statbuf.st_mode);
            //set the last modification time
            timeby.actime = statbuf.st_atime;
            timeby.modtime = statbuf.st_mtime;
            //apply the modification
			utime(target,&timeby);
            //recursive call mycp() in current folder
            mycp(source,target);
            strcpy(source,fsource);
            strcpy(target,ftarget);
        }
        //if current item is not a folder but a file, copy that file directly
        else{
            strcat(source,"/");
            strcat(source,entry->d_name);
            strcat(target,"/");
            strcat(target,entry->d_name);
            CopyFile(source,target);
            strcpy(source,fsource);
            strcpy(target,ftarget);
        }
    }
}
 
int main(int argc,char *argv[]){
    //the state struct marks the state of file
	struct stat statbuf;
    //the utimbuf struct marks the time information of file
	struct utimbuf timeby;
	DIR *dir;
	if (argc != 3){
        printf("parameter number is not 3!\n");
	}
	else{
        //if the source folder does not exist
        if ((dir = opendir(argv[1])) == NULL){
            printf("error occurs in opening files!\n");
        }
        //if the target folder does not exist
        if ((dir = opendir(argv[2])) == NULL){
            //get the file state in argv[1] and fill in the statbuf
            stat(argv[1],&statbuf);
            //create the target folder using statbuf
            mkdir(argv[2],statbuf.st_mode);
			//set the last access time
            timeby.actime = statbuf.st_atime;
            //set the last modification time
            timeby.modtime = statbuf.st_mtime;
            //apply the time setting
            utime(argv[2],&timeby);
        }
        mycp(argv[1],argv[2]);//recurssive call of the function
	}
	printf("copied successfully!\n");
	return 0;
}
