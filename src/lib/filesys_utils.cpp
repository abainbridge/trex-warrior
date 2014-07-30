#include "lib/universal_include.h"
#include "lib/filesys_utils.h"

#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <stdio.h>
#include <string.h>

#include "lib/debug_utils.h"


bool DoesFileExist(char const *_fullPath)
{
	FILE *f = fopen(_fullPath, "r");
	if (f) 
	{
		fclose(f);
		return true;
	}

	return false;
}


#define FILE_PATH_BUFFER_SIZE 256
static char s_filePathBuffer[FILE_PATH_BUFFER_SIZE + 1];

char const *GetDirectoryPart(char const *_fullFilePath)
{
	strncpy(s_filePathBuffer, _fullFilePath, FILE_PATH_BUFFER_SIZE);
    
    char *finalSlash = strrchr(s_filePathBuffer, '/');
    if (finalSlash)
    {
        *(finalSlash+1) = '\x0';
	    return s_filePathBuffer;
    }

    return NULL;
}


char const *GetFilenamePart(char const *_fullFilePath)
{
    char const *filePart = strrchr(_fullFilePath, '/') + 1;
    if (filePart)
    {
	    strncpy(s_filePathBuffer, filePart, FILE_PATH_BUFFER_SIZE);
        return s_filePathBuffer;
    }
	return NULL;
}


char const *GetExtensionPart(char const *_fullFilePath)
{
	return strrchr(_fullFilePath, '.') + 1;
}
