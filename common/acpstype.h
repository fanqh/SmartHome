/////////////////////////////////////////////////////////////////////////////
//
// Header file: 	acpstype.h
//
// Main include file for  Win32 Static Library project.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _AEPSTYPE_HFILES_H_
#define _AEPSTYPE_HFILES_H_

// Generic defines used by all builds

#ifndef _TOOAN_PLATFORM_DEFINE_
#define _TOOAN_PLATFORM_DEFINE_
typedef char				int8  ;						// 8 bits
typedef short				int16 ;						// 16 bits
typedef int				int32 ;						// 32 bit integer
typedef long long			int64;

typedef unsigned char		uint8  ;					// unsigned 8 bits
typedef unsigned short		uint16 ;					// unsigned 16 bits
typedef unsigned int		uint32 ;					// unsigned 32 bit integer
typedef unsigned long long	uint64;

typedef signed int			indx   ;					// native integer type
typedef signed int          mindx  ;					// native integer type

typedef unsigned int        uindx  ;                    // (unsigned version)
typedef unsigned int	    umindx ;					// (unsigned version)

#define bool32              int32                       // 32-bit boolean
#define bool16              int16                       // 32-bit boolean
#define bool8               int8                        // 16-bit boolean
#define uchar		    uint8
#endif

#ifndef UINT16_MIN 
#define UINT16_MIN 			0
#endif
#ifndef UINT16_MAX
#define UINT16_MAX 			(0xffff)
#endif
#define UINT16_BITS     	16


#ifndef BOOL
typedef unsigned int            BOOL;						// BOOL used by matcher/extractor. 
#endif

// Define a text character type that will be atleat 8 bits.
#ifndef tchar
#define tchar TCHAR
#endif

#define char8	char		    /* This will typically be signed, but we let
                                   it be whatever the compiler default is to
                                   avoid compiler warnings when using string
                                   constants. Care must be taken to use this
                                   only for characters and not for general
                                   byte variables. */

/*
 *   The following define (SIZEOFCHAR) is very important.  It must be set to
 *   the size in bits of a "char" (int8 and uint8).  The define is tested in
 *   a few places in the AuthenTec Matcher to perform optimizations that work
 *   on processors that support 8-Bit operations.  For processors such as DSPs
 *   that may not support an 8-Bit char and use 16 bits or 32 bits instead to
 *   represent items of type "char", the symbol below must be set to the size
 *   used for char.
 */

#define SIZEOFCHAR 8                  /* Size of a "char" on this system. */

/*
 *   The following is used to support FAR pointers on x86 machines.
 *   Define FAR to nothing for machines that do not require FAR pointers.
 *   Defime MEMCPY and MEMSET to their normal lower case names for
 *   systems that don't require far pointers.
 */
#ifndef FAR
#define FAR
#endif

/*
 *   The following allow the implementor to implement custom memory
 *   support.  Thes are normally defined to be the AE platform-
 *   specific equivalents. If using the AuthenTec heap code for
 *   heap management, extended heap functions can be used to record
 *   the file name and line number of the calling code into the
 *   heap itself.  This is useful for identifying the source of a
 *   memory leak.
 */

#define MALLOC								malloc
#define FREE(a)								{free(a); a=NULL;}
#define MEMCPY								memcpy
#define MEMSET								memset
#define MEMMOVE								memmove

#define ALLOC(count, size)					malloc(count * size)
#define CALLOC(a,b)							MEMSET(MALLOC((a)*(b)), 0, (a)*(b))			// calloc(count, size)

#define COPYMEM(dst, src, count, size)		memcpy(dst, src, count * size)
#define MOVEMEM(dst, src, count, size)		memmove(dst, src, count * size)
#define FILLMEM(data, c, count)				memset(data, c, count)

#define AllocByteRow(length)				(uint8 *)(ALLOC(length, sizeof(uint8)))
#define CAllocByteRow(length)				(uint8 *)(CALLOC(length, sizeof(uint8)))
#define CopyByteRow(dstRow, row, length)	COPYMEM(dstRow, row, length, sizeof(uint8))

#define AllocInt16Row(length)				(int16 *)(ALLOC(length, sizeof(int16)))
#define CAllocInt16Row(length)				(int16 *)(CALLOC(length, sizeof(int16)))
#define CopyInt16Row(dstRow, row, length)	 COPYMEM(dstRow, row, length, sizeof(int16))

#define AllocUInt16Row(length)				(uint16 *)(ALLOC(length, sizeof(uint16)))
#define CAllocUInt16Row(length)				(uint16 *)(CALLOC(length, sizeof(uint16)))
#define CopyUInt16Row(dstRow, row, length)	 COPYMEM(dstRow, row, length, sizeof(uint16))

#define AllocInt32Row(length)				(int32 *)(ALLOC(length, sizeof(int32)))
#define CAllocInt32Row(length)				(int32 *)(CALLOC(length, sizeof(int32)))
#define CopyInt32Row(dstRow, row, length)	 COPYMEM(dstRow, row, length, sizeof(int32))

#define AllocUInt32Row(length)				(uint32 *)(ALLOC(length, sizeof(uint32)))
#define CAllocUInt32Row(length)				(uint32 *)(CALLOC(length, sizeof(uint32)))
#define CopyUInt32Row(dstRow, row, length)	 COPYMEM(dstRow, row, length, sizeof(uint32))

#define FreeRow(row)						free((void *)(row))

#define LITTLE_ENDIAN						 TRUE
#define MAX_INT32							0x7fffffff


#endif /* #ifndef _AEPSTYPE_HFILES_H_ */




