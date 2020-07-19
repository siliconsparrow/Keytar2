#include <errno.h>
#include <unistd.h>
#include "Gui.h"

// write to disc or console.
extern "C" int _write(int fd, char *ptr, int len)
{
	if(fd == STDOUT_FILENO) {
		return Gui::Gui::instance()->consoleWrite(ptr, len);
	}
	else if(fd == STDERR_FILENO) {
		return Gui::Gui::instance()->consoleWrite(ptr, len, Gui::Gfx::kColourError);
	}
	errno = EBADF;
	return -1;
}

// Read from disc or console.
extern "C" int _read(int fd, char *buf, int count)
{
	return -1;
}

// Open a file.
extern "C" int _open(const char *name, int mode)
{
	return -1;
}

// Close a file. fd may be a file handle or it may be STDIN etc.
extern "C" int _close(int fd)
{
	return -1;
}

// Seek within an open file.
extern "C" int _lseek(int fd, int offset, int whence)
{
	return -1;
}

// Get a file's metadata.file_
extern "C" int _fstat(int fd, struct stat *st)
{
	return -1;
}

// Return nonzero if the given file descriptor is a pseudo-descriptor such as STDOUT.
extern "C" int _isatty(int fd)
{
	if(fd == STDOUT_FILENO
	|| fd == STDIN_FILENO
	|| fd == STDERR_FILENO)
		return 1;

	return 0;
}

// Rename a file on disc.
extern "C" int _rename(const char *oldpath, const char *newpath)
{
	return -1;
}

// Delete a file from disc.
extern "C" int _unlink(const char *pathname)
{
	return -1;
}

// Create a symlink on disc.
extern "C" int _link(const char *oldpath, const char *newpath)
{
	return -1; // We don't support links.
}

// Get our process ID. Irrelevant in a single-tasking system.
extern "C" pid_t _getpid()
{
	return (pid_t)1;
}

// Kill a process. Irrelevant on a single-tasking system.
extern "C" int _kill(pid_t pid, int sig)
{
	return -1;
}

// Get CPU time. Not supported.
extern "C" clock_t _times(struct tms *buf)
{
	return (clock_t)-1;
}

// Get the current time and date.
extern "C" int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
	return -1;
}

// exit() function - in the case of an embedded system, reboot.
extern "C" void _exit(int status)
{
//	NVIC_SystemReset();
	while(1)
		;
}

