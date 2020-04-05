/*
 * FileSystem.h
 *
 *  Created on: 4 Apr 2020
 *      Author: adam
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "ff.h"
#include <stdint.h>

class FileSystem {
public:

	class DirEntry
	{
	public:
		bool         isDirectory() const { return 0 != (_f.fattrib & AM_DIR); }
		const TCHAR *getName()     const { return _f.fname; }
		unsigned     getSize()     const { return _f.fsize; }

	private:
		friend class FileSystem;

		FILINFO _f;
	};

	class File
	{
	public:
		File();
		~File();

		bool     open(const TCHAR *filename, uint8_t mode = FA_READ);
		int      read(uint8_t *buf, unsigned bufMax);
		bool     seek(unsigned offset);
		unsigned getSize() const;

	private:
		friend class FileSystem;

		FIL _f;
	};


	static FileSystem *instance();

	void suspend();
	void resume();

	void readDir(const TCHAR* path,	const TCHAR* pattern, void (*delegate)(DirEntry &entry));

private:
	enum {
		kMaxFiles = 8
	};

	bool   _suspended;
    char   _sdPath[4];
    FATFS  _sdFatFs;  // File system object for SD card logical drive
    File  *_file[kMaxFiles];

	FileSystem();

	bool mount();
	bool unmount();

	bool registerFile(File *f);
	void unregisterFile(File *f);

	void dirTest();
};

#endif /* FILESYSTEM_H_ */
