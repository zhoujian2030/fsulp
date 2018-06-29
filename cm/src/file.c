/*
 * file.c
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// -----------------------------------
int FileOpen(const char* fileName, OpenMode openMode, AccessType accessType)
{
    int fd = -1;
    int oflag;

    if (fileName == 0) {
        return fd;
    }

    switch (accessType)
    {
        case FILE_READ_ONLY:
        {
            oflag = O_RDONLY;
            break;
        }
        case FILE_WRITE_ONLY:
        {
            oflag = O_WRONLY;
            break;
        }
        case FILE_READ_WRITE:
        {
            oflag = O_RDWR;
            break;
        }
        case FILE_READ_WRITE_APPEND:
        {
            oflag = O_RDWR | O_APPEND;
            break;
        }
        default:
        {
            // printf error
            return fd;
        }   
    }

    switch (openMode)
    {
        case FILE_OPEN:
        {
            // No additional OFLAGS here
            fd = open(fileName, oflag, 0);
            if (fd == -1) {
                // LOG_ERROR(CM_LOGGER_NAME, "fail to open file %s. errno = %d - %s", m_filename.c_str(), errno, strerror(errno));
            }
            break;
        }
        case FILE_CREATE:
        {
            // Create file if it does not exist.
            // Truncate it if it exists.
            if(accessType == FILE_READ_WRITE_APPEND) {
                // LOG_WARN(CM_LOGGER_NAME,  "Not support O_APPEND when create new file");
                oflag = O_RDWR;
            }
            fd = open(fileName, oflag | O_CREAT | O_TRUNC, ALL_READ_WRITE);
            if (fd == -1) {
                // LOG_ERROR(CM_LOGGER_NAME, "fail to create file %s. errno = %d - %s", m_filename.c_str(), errno, strerror(errno));
            }
            break;
        }
        case FILE_OPEN_CREATE:
        {
            // Create file if it does not exist.
            fd = open(fileName, oflag | O_CREAT, ALL_READ_WRITE);
            if (fd == -1) {
                // LOG_ERROR(CM_LOGGER_NAME, "fail to open/create file %s. errno = %d - %s", m_filename.c_str(), errno, strerror(errno));
            }
            break;
        }
        default:
        {
            // printf error
        }   
    }

    return fd;
}

// -----------------------------------
void FileClose(int fd)
{
    close(fd);
}

// -----------------------------------
int FileWrite(int fd, const char* theBuffer, int numOfBytesToWrite, int* numberOfBytesWritten)
{
    if (fd == -1 || theBuffer == 0 || numberOfBytesWritten == 0) {
        // LOG_ERROR(CM_LOGGER_NAME, "File not open");
        return FILE_ERR;
    }

    *numberOfBytesWritten = write(fd, theBuffer, numOfBytesToWrite);
    if (*numberOfBytesWritten == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // For non-blocking socket, it would return EAGAIN or EWOULDBLOCK 
            // when buffer is full
            // LOG_INFO(CM_LOGGER_NAME, "no data write to file now, fd = %d", m_fd);
            return FILE_WAIT;
        } else {
            // LOG_ERROR(CM_LOGGER_NAME, "fail to write data to file: %s. errno = %d - %s", m_filename.c_str(), errno, strerror(errno));
            return FILE_ERR;        
        }
    }

    return FILE_SUCC;
}

// -----------------------------------
int FileRead(int fd, char* theBuffer, int buffSize, int* numOfBytesRead)
{
    if (fd == -1 || theBuffer == 0 || numOfBytesRead == 0) {
        // LOG_ERROR(CM_LOGGER_NAME, "File not open");
        return FILE_ERR;
    }

    *numOfBytesRead = read(fd, theBuffer, buffSize);

    if (*numOfBytesRead == -1) {
        // LOG_ERROR(CM_LOGGER_NAME, "fail to read data from file: %s. errno = %d - %s", m_filename.c_str(), errno, strerror(errno));
        return FILE_ERR;  
    }

    return FILE_SUCC;
}

// -----------------------------------
long FileSeek(int fd, long thePos)
{
    if (fd == -1) {
        return -1L;
    }

    switch (thePos)
    {
        case F_SEEK_CURRENT:
        {
            return lseek(fd, 0L, SEEK_CUR);
        }
        case F_SEEK_END:
        {
            return lseek(fd, 0L, SEEK_END);
        }
        case F_SEEK_BEGIN:
        {
            return lseek(fd, 0L, SEEK_SET);
        }
    }

    if (thePos > 0)
    {
        return lseek(fd, thePos, SEEK_SET);
    }

    return -1L;
}

// -----------------------------------
void FileTruncate(int fd, int size)
{
    if (fd == -1) {
        return;
    }

    ftruncate(fd, size);
}

