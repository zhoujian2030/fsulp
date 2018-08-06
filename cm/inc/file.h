/*
 * file.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef enum {
    FILE_SUCC,
    FILE_ERR,
    FILE_WAIT
} FileErrCode;

typedef enum
{
    FILE_OPEN,			// Open, fail if does not exist.
    FILE_CREATE,		// Create, overwrite existing.
    FILE_OPEN_CREATE  	// Attempt to open, if does not exist create it.
} OpenMode;

// Seek modes
typedef enum
{
    F_SEEK_CURRENT = -2,
    F_SEEK_END     = -1,
    F_SEEK_BEGIN   = 0
} FileSeekType;

typedef enum
{
    FILE_READ_ONLY,
    FILE_WRITE_ONLY,
    FILE_READ_WRITE,
    FILE_READ_WRITE_APPEND
} AccessType;

typedef enum
{
    OWNER_READ                  = S_IRUSR,
    OWNER_READ_WRITE            = S_IRUSR | S_IWUSR,
    OWNER_EXECUTE               = S_IXUSR,
    GROUP_READ                  = S_IRGRP,
    GROUP_READ_WRITE            = S_IRGRP | S_IWGRP,
    GROUP_EXECUTE               = S_IXGRP,
    OWNER_AND_GROUP_READ_WRITE  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
    OTHER_READ                  = S_IROTH,
    OTHER_READ_WRITE            = S_IROTH | S_IWOTH,
    OTHER_EXECUTE               = S_IXOTH,
    ALL_READ                    = S_IRUSR | S_IRGRP | S_IROTH,
    ALL_READ_WRITE              = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH,
    ALL_EXECUTE                 = S_IXUSR | S_IXGRP | S_IXOTH
} FileMode;

int FileOpen(const char* fileName, OpenMode openMode, AccessType accessType);
void FileClose(int fd);
int FileWrite(int fd, const char* theBuffer, int numOfBytesToWrite, int* numberOfBytesWritten);
int FileRead(int fd, char* theBuffer, int buffSize, int* numOfBytesRead);
long FileSeek(int fd, long thePos);
void FileTruncate(int fd, int size);

#ifdef __cplusplus
}
#endif

#endif 
