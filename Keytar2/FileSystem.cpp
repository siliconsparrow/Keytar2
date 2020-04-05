/*
 * FileSystem.cpp
 *
 *  Created on: 4 Apr 2020
 *      Author: adam
 */

#include "FileSystem.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include <stdio.h>
#include <string.h>

// Return the singleton instance
FileSystem *FileSystem::instance()
{
	static FileSystem *g_fs = 0;

	if(g_fs == 0) {
		g_fs = new FileSystem();
	}

	return g_fs;
}

FileSystem::FileSystem()
	: _suspended(false)
{
	memset(_file, 0, sizeof(_file));

	mount();
}

// Close all files and unmount the card so the USB system
// can use it.
void FileSystem::suspend()
{
	// TODO: Close all files, flush buffers etc.

	unmount();
	_suspended = true;
}

// Re-mount the card after a previous suspend operation.
void FileSystem::resume()
{
	_suspended = false;
	mount();
}

bool FileSystem::unmount()
{
	if(FR_OK != FATFS_UnLinkDriverEx(_sdPath, 0)) {
		printf("Filesystem unmount failed.\n");
		return false;
	}

	printf("Filesystem unmounted OK.\n");
	return true;
}

bool FileSystem::mount()
{
    if(FATFS_LinkDriver(&SD_Driver, _sdPath) != 0) {
    	printf("SD no more LUNs.\n");
    	return false;
    }

    if(f_mount(&_sdFatFs, (TCHAR const*)_sdPath, 0) != FR_OK) {
    		printf("Filesystem mount failed.\n");
    		return false;
    }

    printf("Filesystem mounted ok.\n");
    return true;
}

void FileSystem::readDir(const TCHAR* path, const TCHAR* pattern, void (*delegate)(DirEntry &entry))
{
	DIR dj;         /* Directory object */
	DirEntry entry;

	FILINFO *fi = &entry._f;
	FRESULT fr = f_findfirst(&dj, fi, path, pattern);
	while(fr == FR_OK && 0 != entry._f.fname[0]) {
		delegate(entry);
		fr = f_findnext(&dj, fi);               /* Search for next item */
	}
	f_closedir(&dj);
}

// Keep track of all open files.
bool FileSystem::registerFile(File *f)
{
	for(int i = 0; i < kMaxFiles; i++) {
		if(_file[i] == 0) {
			_file[i] = f;
			return true;
		}
	}

	return false; // Max files reached.
}

void FileSystem::unregisterFile(File *f)
{
	for(int i = 0; i < kMaxFiles; i++) {
		if(_file[i] == f) {
			_file[i] = 0;
			break;
		}
	}
}


//void dirTestDelegate(FileSystem::DirEntry &entry)
//{
//	if(entry.isDirectory()) {
//		printf("%s/\n", entry.getName());
//	} else {
//		printf("%s (%d bytes)\n", entry.getName(), entry.getSize());
//	}
//}
//
//void FileSystem::dirTest()
//{
//	readDir("", "*", dirTestDelegate);
//}

FileSystem::File::File()
{
	memset(&_f, 0, sizeof(_f));
}

FileSystem::File::~File()
{
	// Close any open file.
	if(0 != _f.obj.fs)
		f_close(&_f);

	// Release file handle.
	FileSystem::instance()->unregisterFile(this);
}

// Open a file.
bool FileSystem::File::open(const TCHAR *filename, uint8_t mode)
{
	if(!FileSystem::instance()->registerFile(this)) {
		return false; // No file handles left.
	}

	if(FR_OK != f_open(&_f, filename, mode)) {
		return false;
	}

	return true;
}

// Read some bytes from the file.
int FileSystem::File::read(uint8_t *buf, unsigned bufMax)
{
	unsigned bytesRead;
	if(FR_OK != f_read(&_f, buf, bufMax, &bytesRead)) {
		return -1;
	}

	return (int)bytesRead;
}

bool FileSystem::File::seek(unsigned offset)
{
	return FR_OK == f_lseek(&_f, offset);
}

unsigned FileSystem::File::getSize() const
{
	if(_f.obj.sclust == 0) {
		return 0;
	}

	return _f.obj.objsize;
}
