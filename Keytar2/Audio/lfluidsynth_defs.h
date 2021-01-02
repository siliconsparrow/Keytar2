#ifndef LFLUIDSYNTH_DEFS_H
#define LFLUIDSYNTH_DEFS_H

#include "ff_wrapper.h"
#include "arm_math.h"

#ifdef QSPI_SOUNDFONT_ROM
#include "qspi_wrapper.h"
#endif

#define FLUID_BUFSIZE       256

//#define FLUID_SAMPLE_MAX_MEM 6291456 // 6 MB
#define FLUID_SAMPLE_MAX_MEM 4194304 // 4 MB
//#define FLUID_SAMPLE_MAX_MEM 3145728 // 3 MB
//#define FLUID_SAMPLE_MAX_MEM 2097152 // 2 MB

//#define FLUID_SAMPLE_READ_CHUNK_SIZE 512
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 1024
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 2048
#define FLUID_SAMPLE_READ_CHUNK_SIZE 4096
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 8192
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 16384
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 32768
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 65536
//#define FLUID_SAMPLE_READ_CHUNK_SIZE 131072

//#define FLUID_CALC_INTERPOLATE_LINEAR

#define	FLUID_LOG(...) fluid_log( __VA_ARGS__)


//#define FLUID_MALLOC(_n)             malloc(_n)
//#define FLUID_REALLOC(_p,_n)         realloc(_p,_n)
//#define FLUID_NEW(_t)                (_t*)malloc(sizeof(_t))
//#define FLUID_ARRAY(_t,_n)           (_t*)malloc((_n)*sizeof(_t))
//#define FLUID_FREE(_p)               free(_p)

#include "chipAlloc.h"
#define FLUID_MALLOC(_n)             SDmalloc(_n)
#define FLUID_REALLOC(_p,_n)         SDrealloc(_p,_n)
#define FLUID_NEW(_t)                (_t*)SDmalloc(sizeof(_t))
#define FLUID_ARRAY(_t,_n)           (_t*)SDmalloc((_n)*sizeof(_t))
#define FLUID_FREE(_p)               SDfree(_p)

#ifdef QSPI_SOUNDFONT_ROM
typedef QSPI_FILE* fluid_file;

#define FLUID_FOPEN(_f,_m)       QSPI_fopen(_f,_m)
#define FLUID_FCLOSE(_f)             QSPI_fclose(_f)
#define FLUID_FREAD(_p,_s,_n,_f)     QSPI_fread(_p,_s,_n,_f)
#define FLUID_FSEEK(_f,_n,_set)      QSPI_fseek(_f,_n,_set)
#define FLUID_FTELL(_f)				 QSPI_ftell(_f)
#define FLUID_FEOF(_f)				 QSPI_feof(_f)
#define FLUID_REWIND(_f)			 QSPI_fseek(_f,0,SEEK_SET)
#define FLUID_MMAP(_p,_s,_f)				QSPI_mmap(_p,_s,_f)

#else
typedef FIL*  fluid_file;

#define FLUID_FOPEN(_f,_m)       ff_fopen(_f,_m)
#define FLUID_FCLOSE(_f)             ff_fclose(_f)
#define FLUID_FREAD(_p,_s,_n,_f)     ff_fread(_p,_s,_n,_f)
#define FLUID_FSEEK(_f,_n,_set)      ff_fseek(_f,_n,_set)
#define FLUID_FTELL(_f)				 f_tell(_f)
#define FLUID_FEOF(_f)				 f_eof(_f)
#define FLUID_REWIND(_f)			 ff_fseek(_f,0,SEEK_SET)
#define FLUID_MMAP(_i)
#endif

#define FLUID_MEMCPY(_dst,_src,_n)   memcpy(_dst,_src,_n)
#define FLUID_MEMSET(_s,_c,_n)       memset(_s,_c,_n)
#define FLUID_STRLEN(_s)             strlen(_s)
#define FLUID_STRCMP(_s,_t)          strcmp(_s,_t)
#define FLUID_STRNCMP(_s,_t,_n)      strncmp(_s,_t,_n)
#define FLUID_STRCPY(_dst,_src)      strcpy(_dst,_src)
#define FLUID_STRCHR(_s,_c)          strchr(_s,_c)

#define FLUID_SIN(_s)				arm_sin_f32(_s)
#define FLUID_COS(_s)				arm_cos_f32(_s)
#define FLUID_ABS(_s)				fabsf(_s)
#define FLUID_POW(_b,_e)			powf(_b,_e)
#define FLUID_SQRT(_s)				sqrtf(_s)
#define FLUID_MATH_LOG(_s)			logf(_s)

#endif
