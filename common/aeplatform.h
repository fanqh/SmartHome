 /* /=======================================================================\
  * |                    TooAn Embedded (TAE) Software						|
  * |                                                                       |
  * |        THIS CODE IS PROVIDED AS PART OF AN TOOAN CORPORATION			|
  * |                    EMBEDDED DEVELOPMENT KIT (EDK).                    |
  * |                                                                       |
  * |        Copyright (C) 2008-2009, TooAnTec, Inc. - All Rights Reserved. |
  * |                                                                       |
  * |  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF    |
  * |  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO  |
  * |  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A       |
  * |  PARTICULAR PURPOSE.                                                  |
  * \=======================================================================/
  */

/* *************************************************************************

   File: 			AEPlatform.h

   Description:		This file contains all of the header files that are
					unique to a given platform (such as Win32 or WinCE).

					It is required to pull in all of the OS specific files
					that are required to build the AE libraries for a
					given platform.

					This file is unique to each platform. This will be the
					first file that is included for each library build.

   Author:			Zhilong Yang

   ************************************************************************* */

#ifndef _AEPLATFORM_H_
#define _AEPLATFORM_H_   

/*
** Include standard Windows files for both C and C++
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/* platform specific header files */
#include "acpstype.h"

#ifndef TRUE
#define  TRUE				1
#endif
#ifndef FALSE
#define  FALSE				0
#endif

#endif                      /* _AEPLATFORM_H_ } */


