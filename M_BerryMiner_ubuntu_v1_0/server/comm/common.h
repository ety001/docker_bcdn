//============================================================
// common.h : common includes files, type definitions, etc
//                          
// Created: 2006-10-21
//============================================================

#ifndef _COMMON_HEADER_H_20061021_16
#define  _COMMON_HEADER_H_20061021_16

/**  common include files **/
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>


/** typedefs  **/
typedef uint64_t _u64;
typedef uint32_t _u32;
typedef uint16_t _u16;
typedef uint8_t _u8;
typedef int64_t _i64;
typedef int32_t _i32;
typedef int16_t _i16;
typedef int8_t _i8;

typedef int64_t __int64;
typedef int __int32;
typedef uint8_t byte;
typedef _u32 _stat_type;

#define DIST_SERVER_VERSION	"1.0.0.20"
#define COLL_SERVER_VERSION	"1.0.0.20"
#define DIST_SERVER_VER_MAJOR	1
#define DIST_SERVER_VER_BUILD	20
#define DIST_SERVER_ID 2

const int REPORT_FROM_XUNLEI = 0;
const int REPORT_FROM_PARTER = 1;

/**  macros **/
#define MAX_CID_LEN		20
#define MAX_PATH_LEN	512
#define MAX_PEERID_LEN	24
#define MAX_FILE_BLOCKS	128
#define COMMON_FILE_MODE	S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH

/** file type **/
#define FILE_INDEX 			10001
#define FILE_PARTS			10002

/** ver **/
#define HEAD_VER_10001 		10001
#define HEAD_VER_10002 		10002


#define PART_FILE_MAGIC	0x1327B5C4

#define NORMALFILE			-2
#define HAVE_PART_FD		1234567890

#define MAX_UDP_PACKET_SIZE  2048

#ifdef __LOG_OPEN__
#define LOG_OPEN(filename,avg...) true ? ({ \
	int fd = open(filename, avg); \
	(int)fd; \
}) : 0
#else
#define LOG_OPEN(filename,avg...) open(filename, avg)
#endif

#ifdef __LOG_CLOSE__
#define LOG_CLOSE(fd) true ? ({ \
	int ret = close(fd); \
	(int)ret; \
}) : 0
#else
#define LOG_CLOSE(fd) close(fd)
#endif

#endif // #ifndef _COMMON_HEADER_H_20061021_16

