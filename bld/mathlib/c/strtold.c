/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  String to long double conversion.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "libsupp.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#ifdef __WIDECHAR__
    #include <wchar.h>
#endif

#if DBL_DIG != LDBL_DIG
/* Must check before xfloat.h potentially redefineds LDBL_DIG. */
#error strtold/wcstold needs true long double support!
#endif

#include "watcom.h"
#include "xfloat.h"


_WMRTLINK long double __F_NAME(strtold,wcstold)( const CHAR_TYPE *bufptr, CHAR_TYPE **endptr )
/********************************************************************************************/
{
    return( __F_NAME(strtod,wcstod)( bufptr, endptr ) );
}

