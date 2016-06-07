#ifndef __FFILE_H
#define __FFILE_H

//! @addtogroup ffile
//! @brief Module driver handling a mini filesystem on the data flash
//! @{

#define FILE_MAX_OPEN_FILES		4			//!< Maximum number of files that can be opened

//! @brief Seek position constants
enum FSEEK_Enum {
	FSEEK_SET,		//!< Set position relative to start of file
	FSEEK_CUR,		//!< Set position relative to current position
	FSEEK_END		//!< Set position relative to end of file
};

#define FAT_FILE_NAME_LEN		80		//!< Maximum file name length

//! @brief Working structure used to get the list of the files
typedef struct {
    uint8_t state;		//!< state of the current listing procedure (if any)
    char strdir[80+FAT_FILE_NAME_LEN];		//!< working buffer where storing the last file listed
    uint16_t page;		//!< physical dataflash page of the last file listed
} DIR;

//! @brief Working structure storing informations on an open file
typedef struct {
    char filename[FAT_FILE_NAME_LEN+1];		//!< filename of the opened file
    uint8_t fnlen;							//!< filename length
    uint16_t state;							//!< internal state flags
    long addr;								//!< global position inside the file
    uint16_t page;							//!< current dataflash page number
    uint16_t firstpage;						//!< first dataflash page number of the file
    uint16_t ind;							//!< offset inside the current dataflash page
    long size;								//!< size of the file
} FFILE;

/*!
 *  @brief Complete the refresh of the dataflash pages
 *  
 */
void ffsync(void);
//! @brief Periodic dataflash refresh
void ffrefresh(void);
/*!
 *  @brief Format the filesystem, any previous content is destroyed
 *  
 *  @return 0 if the operation is completed successfully
 *  
 */
int16_t fformat(void);
/*!
 *  @brief Open a file for read or write
 *  
 *  @param [in] filename name of the file to open
 *  @param [in] type     type of the open, 'r' read, 'w' write, 'a' append
 *  @return pointer to an internal structure describing the opened file, NULL in case of error
 *  
 */
FFILE *ffopen(const char *filename, char type);
/*!
 *  @brief Close an opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @return 0 if the operation is completed successfully
 *  
 */
uint16_t ffclose(FFILE *stream);
/*!
 *  @brief Close any opened file
 *  
 */
void ffcloseall(void);
/*!
 *  @brief Get a byte from an opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @return the byte read
 *  
 */
char ffget(FFILE *stream);
/*!
 *  @brief Write a byte in an opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] data   Byte to write
 *  
 */
void ffput(FFILE *stream, char data);
/*!
 *  @brief Delete a file
 *  
 *  @param [in] filename name of the file to delete 
 *  @return 0 if the operation is completed successfully
 *  
 */
uint16_t ffdelete(const char *filename);
/*!
 *  @brief Copy a file
 *  
 *  @param [in] fn Current name of the file to rename
 *  @param [in] cn file name of the copy to create
 *  @return 0 if the operation is completed successfully
 *  
 */
uint16_t ffcopy(const char *fn,const char *cn);
/*!
 *  @brief Rename a file
 *  
 *  @param [in] fn Current name of the file to rename
 *  @param [in] nn new name to assing to the file
 *  @return 0 if the operation is completed successfully
 *  
 */
uint16_t ffmove(const char *fn,const char *nn);
/*!
 *  @brief Read one or more bytes from an opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] pvoid  pointer to a buffer
 *  @param [in] size   number of bytes to read
 *  @return the number of bytes read
 *  
 */
uint16_t ffread(FFILE *stream, void *pvoid, uint16_t size);
/*!
 *  @brief Write one or more bytes in an opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] pvoid  pointer to a buffer
 *  @param [in] size   number of bytes to write
 *  @return the number of bytes written
 *  
 */
uint16_t ffwrite(FFILE *stream, const void *pvoid, uint16_t size);
/*!
 *  @brief Move the current position inside an opened file
 *
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] pos    relative position to set
 *  @param [in] whence enum FSEEK_Enum specifying the reference inside the file
 *  @return 0 if the operation is completed successfully
 *  
 */
int16_t ffseek(FFILE *stream,long pos,enum FSEEK_Enum whence);
/*!
 *  @brief Get the current position inside the opened file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @return the position inside the file
 *  
 */
long fftell(FFILE *stream);
/*!
 *  @brief Get the file size
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @return The size of the file
 *  
 */
long ffsize(FFILE *stream);
/*!
 *  @brief Write a null terminated string in a file
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] str    null terminated string to write
 *  
 */
void ffputs(FFILE *stream, const char *str);
/*!
 *  @brief End of file test on a file opened for read
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @return TRUE if the current position is at the end of the file, FALSE otherwise
 *  
 */
uint16_t ffeof(FFILE *stream);
/*!
 *  @brief Get the index of the first dataflash page of a file
 *  
 *  @param [in] filename name of the file to search
 *  @return a value >= 0 if the file has been found, -1 otherwise
 *  
 */
int16_t ffindFile(const char *filename);

/*!
 *  @brief Get the total amount of free space in the dataflash
 *  
 *  @return The number of bytes of free space in the dataflash
 *  
 */
long ffreeSpace(void);
/*!
 *  @brief Start a file list operation
 *  
 *  @return a pointer to the structure DIR
 *  
 */
DIR *opendir(void);
/*!
 *  @brief Get next file in list, compact format only the name
 *  
 *  @param [in] _dir pointer to structure DIR returned by opendir
 *  @return pointer to a null terminated string representing the name of a file, NULL if no other file has been found
 *  
 */
char *readdir(DIR *_dir);
/*!
 *  @brief Get next file in list, extended format, including name and size
 *  
 *  @param [in] _dir pointer to structure DIR returned by opendir
 *  @return pointer to a null terminated string representing the name and size of a file, NULL if no other file has been found
 *  
 */
char *readwdir(DIR *_dir);
/*!
 *  @brief Close the file list operation
 *  
 *  @param [in] _dir pointer to structure DIR returned by opendir
 *  @return 0 if the operation is completed successfully
 *  
 */
uint16_t closedir(DIR *_dir);
/**
 *  @brief Reset the file list operation to the first file
 *  
 *  @param [in] _dir pointer to structure DIR returned by opendir
 *  
 */
void rewinddir(DIR *_dir);
/*!
 *  @brief Read a line from a file opened for read, lines are delimited by CR or LF
 *  
 *  @param [in] stream pointer to a file descriptor
 *  @param [in] buffer pointer to a buffer where to store the line extracted from the file
 *  @param [in] size   size of the buffer in bytes
 *  @return number of bytes stored in buffer
 *  
 */
uint16_t ffreadln(FFILE *stream, char *buffer, uint16_t size);
/*!
 *  @brief Activation of filesystem service, must be called during initialization
 *  
 */
void ffstart(void);

/*!
 *  @brief Check whether the filesystem is ready to accept operation
 *  
 *  @param [in] tout timeout waiting for filesystem ready
 *  @return TRUE if the filesystem is ready, FALSE otherwise
 *  
 */
int ffready(uint32_t tout);

//! @brief Lock further accesses to file system
void fflock(void);

//! @}

#endif
