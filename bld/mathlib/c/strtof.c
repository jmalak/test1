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
* Description:  String to float conversion.
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
#include "watcom.h"
#include "xfloat.h"


_WMRTLINK float __F_NAME(strtof,wcstof)( const CHAR_TYPE *bufptr, CHAR_TYPE **endptr )
/************************************************************************************/
{
    double  x;
    float   f;

    /* strtold() is fat enough already and we'd rather not replicate it... */
    f = x = __F_NAME(strtod,wcstod)( bufptr, endptr );

    /* However, we have to handle inputs which are within -DBL_MAX : DBL_MAX
     * range yet outside of -FLT_MAX : FLT_MAX range.
     * To match other implementations, the result is converted to float and
     * ERANGE only reported if the conversion overflows (i.e. the float value
     * is infinity). Note that there is a set of doubles which are > FLT_MAX
     * but after rounding and conversion to float, result in FLT_MAX.
     * These are not flagged as errors.
     * Similarly there are inputs within the -DBL_MIN : DBL_MIN range that
     * fall outside of the -FLT_MIN : FLT_MIN range. We recognize these by
     * checking for a case where the double is non-zero but the converted
     * float is zero, and likewise report ERANGE.
     */
    if( isfinite( x ) ) {
        if( isinf( f ) )
            __set_ERANGE();
        else if( (fpclassify( f ) == FP_ZERO) && (fpclassify( x ) != FP_ZERO) )
            __set_ERANGE();
    }

    return( f );
}

