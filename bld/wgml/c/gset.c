/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GML :SET processing
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/*   :SET symbol='symbol-name'                                             */
/*        value='character-string'                                         */
/*              delete.                                                    */
/*                                                                         */
/* This tag defines and assigns a value to a symbol name.  The symbol      */
/* attribute must be specified.  The value of this attribute is the name   */
/* of the symbol being defined, and cannot have a length greater than ten  */
/* characters.  The symbol name may only contain letters, numbers, and the */
/* characters @, #, $ and underscore(_).  The value attribute must be      */
/* specified.  The attribute value delete or a valid character string may  */
/* be assigned to the symbol name.  If the attribute value delete is used, */
/* the symbol referred to by the symbol name is deleted.                   */
/***************************************************************************/

extern  void    gml_set( const gmltag * entry )
{
    bool            symbol_found    = false;
    bool            value_found     = false;
    char        *   p;
    char        *   pa;
    int             rc;
    symvar          sym;
    sub_index       subscript;
    symdict     *   working_dict;

    subscript = no_subscript;           // not subscripted
    scan_err = false;
    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags

    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;;) {
            pa = get_attribute( p );
            p = g_att_val.att_start;
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( !strnicmp( "symbol", p, 6 ) ) {
                p += 6;

                /* both get_value() and scan_sym() must be used */

                p = get_value( p );
                if( AttrFlags.symbol ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                        }
                }
                AttrFlags.symbol = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                scan_sym( g_att_val.val_start, &sym, &subscript, NULL, false );
                if( scan_err ) {
                    break;
                }
                symbol_found = true;
            } else if( !strnicmp( "value", p, 5 ) ) {
                p += 5;
                p = get_value( p );
                if( AttrFlags.value ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                        }
                }
                AttrFlags.value = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                value_found = true;
                memcpy_s( token_buf, buf_size, g_att_val.val_start, g_att_val.val_len );
                if( g_att_val.val_len < buf_size ) {
                    token_buf[g_att_val.val_len] = '\0';
                } else {
                    token_buf[buf_size - 1] = '\0';
                }
            } else if( !strnicmp( token_buf, "delete", 6 ) ) {  // catches "value=delete" by using token_buf
                p += 6;
                sym.flags |= deleted;
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( symbol_found && value_found ) {   // both attributes
        if( sym.flags & local_var ) {
            working_dict = input_cbs->local_dict;
        } else {
            working_dict = global_dict;
        }
        rc = add_symvar( working_dict, sym.name, token_buf, subscript, sym.flags );
    } else {
        xx_err( err_att_missing );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {
            post_space = 0;
            ProcFlags.ct = true;
            process_text( p, g_curr_font);  // if text follows
        }
    }
    scan_start = scan_stop + 1;
    return;
}

