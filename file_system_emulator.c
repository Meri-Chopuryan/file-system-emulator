#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    uint32_t inode;
    uint32_t parentInode;
    char type;
    char name[32];
} Inode;

Inode inodeList[1024];
size_t inodeCount = 0;
uint32_t currentInode = 0;

void loadInodeList(const char *path) {
//read the inodeList from the file path to your inodeList array
//update the inodeCount
//remember that your directory will have only one inodeList with a given number of inodes in the list   
    char fullpath[100];
    strcpy(fullpath, path);
    strcat(fullpath, "/inodes_list");
    
    FILE *fp = fopen(fullpath, "rb");
    if(fp == NULL) {
        printf("File couldn't be opened. Empty fs. Starting a new directory..\n");
        inodeList[0].inode = 0;
        inodeList[0].parentInode = 0;
        inodeList[0].type = 'd';
        strcpy(inodeList[0].name, "/");
        inodeCount = 1;
        return;
    }
    
    size_t count = fread(&inodeList, sizeof(Inode), 1024, fp);
    fclose(fp);
    inodeCount = count;
    printf("Loaded %zu inodes\n", count);
}


void saveInodeList(const char *path) {
//write the content of the inodeList to the file path
   
//    printf("Saving %zu inodes (0-%zu)\n", inodeCount, inodeCount-1);
    FILE *fp = fopen("inodes_list", "wb");
    if (fp == NULL) {
        printf("Error: Could not write inodes_list\n");
        return;
    }

    fwrite(inodeList, sizeof(Inode), inodeCount, fp);
    fclose(fp);  
}



void changeDirectory(const char *name) {
//iterate through the inodeList to search whether a directory with the name already exists
//if yes, then set the currentInode value to the index of the list with that name
//otherwise print an error message to indicate that the directory is not found
    if(name == NULL || name[0] == '\0') {
	currentInode = 0;
	return;
    }

    if(strcmp(name, "..") == 0 && currentInode !=0) {
	currentInode = inodeList[currentInode].parentInode;
	return;
    }
    else if(strcmp(name,"..") == 0 && currentInode == 0) {
	return;
    }

    for (size_t i = 0; i < inodeCount; i++) {
        if (strcmp(inodeList[i].name, name) == 0 && inodeList[i].type == 'd' && inodeList[i].parentInode == currentInode) {
            currentInode = inodeList[i].inode;
            printf("Changed to directory: %s\n", name);
            return;
        }
    }
    printf("Error: Directory %s not found\n", name);
}

void listContents() {
//iterate through inodeList and print the members of those elements in the list for which the parentInode is the currentInode
    for (size_t i = 0; i < inodeCount; i++) {
        if (inodeList[i].parentInode == currentInode) {
            printf("inode: %u, type: %c, name: %s\n", inodeList[i].inode, inodeList[i].type, inodeList[i].name);
        }
    }
}

void createDirectory(const char *name) {
//iterate through the inodeList to see if the name of the directory exists; if yes, exit
//check to see if the inodeCount is 1024; if yes, exit
//create a new inode with new inodeCount, set its parentInode and type, and also its name
//create a file with its name as the inode number
//write into this file its . and .. inode values
    for (size_t i = 0; i < inodeCount; i++) {
        if (inodeList[i].parentInode == currentInode && strcmp(inodeList[i].name, name) == 0) {
            printf("Error: %s already exists\n", name);
            return;
        }
    }
    
    if (inodeCount >= 1024) {
        printf("Error: Max inodes (1024) reached\n");
        return;
    }
    
    uint32_t newInode = inodeCount;
    inodeList[inodeCount].inode = newInode;
    inodeList[inodeCount].parentInode = currentInode;
    inodeList[inodeCount].type = 'd';
    strcpy(inodeList[inodeCount].name, name);
    
    char filename[50];
    sprintf(filename, "%u", newInode);// convert inode number to string
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error: Could not create file\n");
        return;
    }
    
    fwrite(".", 1, 1, fp);
    fwrite(&newInode, sizeof(uint32_t), 1, fp);
    fwrite("..", 2, 1, fp);
    fwrite(&currentInode, sizeof(uint32_t), 1, fp);
    fclose(fp);
    
    inodeCount++;
}

void createFile(const char *name) {
//iterate through inodeList to see if filename exists; if yes, exit
//check if the inodeCount is 1024; if yes, exit
//create a new entry for an inode in inodeList
//create a file with its name as the inode number
//write into this file its filename

    for(size_t i = 0; i < inodeCount; i++) {
        if (inodeList[i].parentInode == currentInode && strcmp(inodeList[i].name, name) == 0) {
            printf("Error: %s already exists\n", name);
            return;
        }
    }
    
    if(inodeCount >= 1024) {
        printf("Error: Max inodes (1024) reached\n");
        return;
    }
    
    uint32_t newInode = inodeCount;
    inodeList[inodeCount].inode = newInode;
    inodeList[inodeCount].parentInode = currentInode;
    inodeList[inodeCount].type = 'f';
    strcpy(inodeList[inodeCount].name, name);
    
    char filename[50];
    sprintf(filename, "%u", newInode); // convert inode number to string
    FILE *fp = fopen(filename, "wb");
    if(fp == NULL) {
        printf("Error: Could not create file\n");
        return;
    }
    
    fwrite(name, 1, strlen(name) + 1, fp);// +1 for null terminator
    fclose(fp);
    
    inodeCount++;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Error: not enough arguments\n");
        return 1;
    }
    printf("Program started with directory: %s\n", argv[1]);

    loadInodeList(argv[1]);
    chdir(argv[1]);
    currentInode = 0;
    
    char line[100], cmd[50], arg[50];
    int i, j;
    
    while(1) {
        printf("> ");
        if(!fgets(line, sizeof(line), stdin)) break;
        
        line[strlen(line)-1] = '\0';
        
        i = 0;
        while(line[i] == ' ') i++;
        j = 0;
        while(line[i] && line[i] != ' ') cmd[j++] = line[i++];
        cmd[j] = '\0';
        
        while(line[i] == ' ') i++;
        j = 0;
        while(line[i]) arg[j++] = line[i++];
        arg[j] = '\0';
        
        if(strcmp(cmd, "exit") == 0) {
            break;
        }
        else if(strcmp(cmd, "ls") == 0) {
            listContents();
        }
        else if(strcmp(cmd, "cd") == 0) {
            changeDirectory(arg);
        }
        else if(strcmp(cmd, "mkdir") == 0) {
            if(arg[0] == '\0') {
                printf("Error: mkdir requires a directory name\n");
            } else {
                if(strlen(arg) > 31) {
                    arg[31] = '\0';
                    printf("Warning: Name truncated to 31 chars\n");
                }
                createDirectory(arg);
            }
	}
        else if(strcmp(cmd, "touch") == 0) {
            if(arg[0] == '\0') {
                printf("Error: touch requires a file name\n");
            } else {
                if(strlen(arg) > 31) {
                    arg[31] = '\0';
                    printf("Warning: Name truncated to 31 chars\n");
                }
                createFile(arg);
            }
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    
    saveInodeList(argv[1]);
    return 0;
}
