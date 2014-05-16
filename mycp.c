//
// main.c
// cp
//
// Created by Макс on 24.03.14.
// Copyright (c) 2014 Максим Никитин. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>

#define BUFF     64

void copyFile( char *path1, char *path2 )
{
	ssize_t read_bytes;
	char buffer[BUFF + 1];
	int fd1 = open( path1, O_RDONLY );
	int fd2 = open( path2, O_WRONLY | O_CREAT, 0777 );
    
	if ( fd1 < 0 || fd2 < 0 )
	{
		printf("Cannot open file\n");
		exit(1);
	}
    
	while ((read_bytes = read( fd1, &buffer, BUFF )) > 0)
	{
		if ( write( fd2, &buffer, read_bytes) < 0 )
		{
			printf ( "Cannot write\n");
			exit(1);
		}
	}
    
	close(fd1);
	close(fd2);
}

void copyDir( char *path1, char *path2 )
{
    DIR *dir1, *dir2;
    struct dirent *entry;
    struct stat inform;
    char newpath1[ PATH_MAX + 1];
    char newpath2[ PATH_MAX + 1];
    
    dir1 = opendir(path1);
    
    if ( !dir1 )
    {
        printf ("ERROR file '%s'\n", path1);
        exit(1);
    }
    
    if ( mkdir( path2, 0777 ) < 0 )
    {
        printf("ERROR DIR '%s'\n", path2);
        exit(1);
    }
    
    while ( ( entry = readdir( dir1 ) ) != NULL )
    {
        if ( entry->d_name == "." || entry->d_name == "..")
            continue;
        
        strncpy( newpath1, path1, PATH_MAX );
        strncpy( newpath2, path2, PATH_MAX);
        strncat( newpath1, "/", PATH_MAX );
        strncat( newpath2, "/", PATH_MAX );
        strncat( newpath1, entry->d_name, PATH_MAX );
        strncat( newpath2, entry->d_name, PATH_MAX );
        
        if (stat(newpath1, &inform ) < 0)
        {
            fprintf (stderr, "ls: Cannot open file\n");
            exit(1);
        }
        
        if (inform.st_mode & S_IFREG)
            copyFile( newpath1, newpath2 );
        
		if (inform.st_mode & S_IFDIR)
			copyDir(newpath1, newpath2);
    }
    
    closedir(dir1);
}


int main( int argc, char **argv) {
    
	struct stat inform;
    
	if (argc < 3)
	{
		printf("Too few arguments\n");
		exit (1);
	}
    
	if (stat( argv[1] , &inform ) < 0)
    {
        printf(" Cannot open file\n");
        exit(1);
    }
    
	if (inform.st_mode & S_IFREG)
		copyFile(argv[1], argv[2]);
	else
	{
		if (opendir(argv[2]))
		{
			printf("This name already exists '%s'\n", argv[2]);
			exit(1);
		}
        
		copyDir(argv[1], argv[2]);
	}
	exit(0);
}
