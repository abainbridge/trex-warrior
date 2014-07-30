#ifndef INCLUDED_FILESYS_UTILS_H
#define INCLUDED_FILESYS_UTILS_H


//*****************************************************************************
// Misc directory and filename functions
//*****************************************************************************

bool DoesFileExist(char const *_fullPath);

char const *GetDirectoryPart(char const *_fullFilePath);
char const *GetFilenamePart(char const *_fullFilePath);
char const *GetExtensionPart(char const *_fileFilePath);


#endif
