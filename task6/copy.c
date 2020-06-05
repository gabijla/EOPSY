#include<stdio.h>
#include<sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include<string.h>

#define SUCCESS 0
#define MAX_BUFFER_SIZE 100
#define FAIL 1

void copy_mmap(int fd_from, int fd_to);
void copy_read_write(int fd_from, int fd_to);
void help();

int main(int argc, char** argv)
{
    int arg;
    int access;
    char* file1;
    char* file2;
    short flag = 0;
    int fd;
    int fo;

    
    while ((arg = getopt(argc,argv,":m:h")) != -1)
    {
        switch (arg)
        {
        case 'h':
            help();
        case 'm':
            access = strlen(optarg) + 1; //getting the second file argument index
            file1 = optarg;          
            file2 = optarg + access; //we need this to access the second argument
            flag = 1; //flag to know m options is used

            //check if a second argument is given by the user
            if(strncmp(file2,"CLUTTER_IM_MODULE=xim",strlen(file2)) != 0)
            {
            //opening first file as read only    
                if((fd = open(file1, O_RDONLY)) == -1)
                {
                    printf("Error input file\n");
                    exit(FAIL);
                }
            //opening second file as write and using O_TRUNC to clean the content of the file
                if((fo = open(file2, O_WRONLY | O_TRUNC)) == -1)
                {
                    printf("Error output file\n");
                    exit(FAIL);
                }
            //copy_read_write function call   
                copy_read_write(fd,fo);
            }
            else
            {
                printf("No input on the second file");
                exit(FAIL);
            }
            break;
        case '?':
            printf("Wrong option %c \n", optopt);
            exit(FAIL);
        case ':':
            printf("This option needs a value \n");
            exit(FAIL);
        }
    }

    //copy_map use
    if(flag != 1)
    {
        //index of the next argumet
        file1 = argv[optind];  
        optind++;
        file2 = argv[optind];

	//opening first file as read only
        if((fd = open(file1,O_RDONLY)) == -1)
        {
            printf("Error input file\n");
            exit(FAIL);
        }
	//opening the second file, in this case we need both read and write permissions
        if((fo = open(file2,O_RDWR| O_TRUNC)) == -1)
        {
            printf("Error output file\n");
            exit(FAIL);
        }
       copy_mmap(fd,fo);
    }  
}

void copy_mmap(int fd_from, int fd_to)
{
    //this stuct stat retrieve information about the file pointed
    struct stat sf;
    char *in;
    char *out;

    //trying to get the file size
    if(fstat(fd_from,&sf) == -1)
    {
        printf("Error getting the file size \n");
        exit(FAIL);
    }

    //mmap return pointer to the first memory block of the input file, we will use read permissions, and private mapping so it will not be visible to other processes mapping the same file
    if((in = mmap(NULL,sf.st_size,PROT_READ, MAP_PRIVATE, fd_from,0)) == MAP_FAILED)
    {
        printf("Fail mapping input\n");
        exit(FAIL);
    }
    //extending output file size to input file size
    ftruncate(fd_to,sf.st_size);

    //in this case we must use write permissions so a copy is stored somewhere in the memory and shared flag so Updates to the mapping are visible to other processes mapping the same region 
    if((out = mmap(NULL,sf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to,0)) == MAP_FAILED)
    {
        printf("Fail mapping output\n");
        exit(FAIL);
    }
    //copy the memory space where our input file is located to the memory space where our output file is
    memcpy(out,in,sf.st_size);

    //deletes the mappings for both files
    munmap(in,sf.st_size);
    munmap(out,sf.st_size);

    //close both files in a secure way
    if((close(fd_from)) == -1 || close(fd_to) == -1)
    {
        exit(FAIL);
    }
}

void copy_read_write(int fd_from, int fd_to)
{
    
    char buffer[MAX_BUFFER_SIZE]; 
    ssize_t size;

    //returns the file size
    if((size = read(fd_from,&buffer,sizeof(buffer))) == -1)
    {
        printf("Reading buffer error\n");
        exit(FAIL);
    }
    //checking buffer size
    if(!(size < sizeof(buffer)))
	printf("Error, buffer size too big");
    else
    {
	//write to file from buffer
        if(write(fd_to,&buffer,size) == -1)
        {
            exit(FAIL);
        }

        //close both files in a secure way
        if((close(fd_from)) == -1 || close(fd_to) == -1)
        {
            exit(FAIL);
        }
    }
}

void help()
{
    printf("\n****HELP****\n\nUse: copy [-m] <file_name> <new_file_name>   copy [-h]\nWithout option -m use read() and write() functions to copy file contents. If the option -m is given, do not use neither read() nor write() but map files to memory regions with mmap() and copy the file with memcpy() instead.\nIf the option -h is given or the program is called without arguments print out some help information.\n");
    exit(FAIL);
}

