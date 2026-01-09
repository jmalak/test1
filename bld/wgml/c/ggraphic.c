/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML tag :GRAPHIC processing
*
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :GRAPHIC tag                                                           */
/***************************************************************************/

void    gml_graphic( const gmltag * entry )
{
    char            file[FILENAME_MAX];
    char            rt_buff[MAX_FILE_ATTR];
    char        *   p;
    char        *   pa;
    char        *   pb;
    doc_element *   cur_el;
    inputcb     *   cb                      = input_cbs;
    su              cur_su;
    uint32_t        depth;
    uint32_t        scale                   = 100;
    uint32_t        width;
    int32_t         xoff                    = 0;
    int32_t         yoff                    = 0;

    if( (ProcFlags.doc_sect < doc_sect_gdoc) ) {
        if( (ProcFlags.doc_sect_nxt < doc_sect_gdoc) ) {
            xx_err_c( err_tag_before_gdoc, entry->tagname );
        }
    }

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    scr_process_break();                // flush existing text
    start_doc_sect();                   // if not already done

    file[0] = '\0';
    rt_buff[0] = '\0';
    width = t_page.max_width - t_page.cur_left;

    p = scan_start;
    if( *p == '.' ) {
        /* already at tag end */
    } else {
        for( ;; ) {
            pa = get_attribute( p );
            p = g_att_val.att_start;
            if( ProcFlags.reprocess_line ) {
                break;
            }
            if( !strnicmp( "file", p, 4 ) ) {
                p += 4;
                p = get_value( p );
                if( AttrFlags.file ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.file = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                memcpy_s( file, FILENAME_MAX, g_att_val.val_start, g_att_val.val_len );
                if( val_len < FILENAME_MAX ) {
                    file[g_att_val.val_len] = '\0';
                } else {
                    file[FILENAME_MAX - 1] = '\0';
                }
                split_attr_file( file, rt_buff, sizeof( rt_buff ) );
                if( (rt_buff[0] != '\0') ) {
                    xx_warn( wng_rec_type_graphic );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( !strnicmp( "depth", p, 5 ) ) {
                p += 5;
                p = get_value( p );
                if( AttrFlags.depth ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.depth = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                if( value_to_su( &cur_su, true ) ) {
                    break;
                }
                depth = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( depth == 0 ) {
                    xx_line_err_c( err_inv_depth_graphic_1, g_att_val.val_start );
                }
                if( depth > t_page.max_depth ) {
                    xx_line_err_c( err_inv_depth_graphic_2, g_att_val.val_start );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( !strnicmp( "width", p, 5 ) ) {
                p += 5;
                p = get_value( p );
                if( AttrFlags.width ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.width = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }

                /* GRAPHIC uses the current column width even if "page" is specified */

                if( !strnicmp( "page", g_att_val.val_start, 4 ) ) {
                    // default value is the correct value to use
                } else if( !strnicmp( "column", g_att_val.val_start, 6 ) ) {
                    // default value is the correct value to use
                } else {    // value actually specifies the width
                    if( value_to_su( &cur_su, true ) ) {
                        break;
                    }
                    width = conv_hor_unit( &cur_su, g_curr_font );
                    if( width == 0 ) {
                        xx_line_err_c( err_inv_width_graphic_1, g_att_val.val_start );
                    }
                    if( width > t_page.last_pane->col_width ) {
                        xx_line_err_c( err_inv_width_graphic_2, g_att_val.val_start );
                    }
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( !strnicmp( "scale", p, 5 ) ) {
                p += 5;
                p = get_value( p );
                if( AttrFlags.scale ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.scale = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                pb = g_att_val.val_start;
                if( (*pb == '+') || (*pb == '-') ) {    // signs not allowed
                    xx_line_err_c( err_num_too_large, g_att_val.val_start );
                }
                scale = 0;
                while( my_isdigit( *pb ) ) {            // convert to number
                    scale = (10 * scale) + (*pb - '0');
                    pb++;
                    if( (pb - g_att_val.val_start) > g_att_val.val_len ) {  // value end reached
                        break;
                    }
                }
                if( scale > 0x7fffffff ) {              // wgml 4.0 limit
                    xx_line_err_c( err_num_too_large, g_att_val.val_start );
                }
                if( (pb - g_att_val.val_start) < g_att_val.val_len ) {      // value continues on
                    xx_line_err_c( err_num_too_large, g_att_val.val_start );
                }
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( !strnicmp( "xoff", p, 4 ) ) {
                p += 4;
                p = get_value( p );
                if( AttrFlags.xoff ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.xoff = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                if( value_to_su( &cur_su, false ) ) {
                    break;
                }
                xoff = conv_hor_unit( &cur_su, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else if( !strnicmp( "yoff", p, 4 ) ) {
                p += 4;
                p = get_value( p );
                if( AttrFlags.yoff ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.yoff = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
                if( value_to_su( &cur_su, false ) ) {
                    break;
                }
                yoff = conv_vert_unit( &cur_su, g_text_spacing, g_curr_font );
                if( ProcFlags.tag_end_found ) {
                    break;
                }
            } else {    // no match = end-of-tag in wgml 4.0
                ProcFlags.tag_end_found = true;
                p = pa; // restore spaces before text
                break;
            }
        }
    }

    if( !AttrFlags.depth || !AttrFlags.file ) { // detect missing required attributes
        xx_err( err_att_missing );
    }

    if( !ProcFlags.ps_device ) {        // character devices ignore SK & post_skip
        g_skip = 0;
        g_post_skip = 0;
    }
    set_skip_vars( NULL, NULL, NULL, 1, g_curr_font );

   // only set up the doc_element if the file exists
    if( search_file_in_dirs( file, "", "", ds_doc_spec ) ) {
        cur_el = init_doc_el( el_graph, depth );
        cur_el->element.graph.cur_left = t_page.cur_left;
        cur_el->element.graph.depth = depth;
        cur_el->element.graph.scale = scale;
        cur_el->element.graph.width = width;
        cur_el->element.graph.xoff = xoff;
        cur_el->element.graph.yoff = yoff;
        cur_el->element.graph.fp = try_fp;
        try_fp = NULL;
        if( nest_cb->c_tag == t_NONE ){
            cur_el->element.graph.next_font = FONT0;
        } else {
            cur_el->element.graph.next_font = g_prev_font;
        }
        strncpy_s( cur_el->element.graph.short_name, FILENAME_MAX, file, FILENAME_MAX );
        strncpy_s( cur_el->element.graph.file, FILENAME_MAX, try_file_name, FILENAME_MAX );

        if( GlobalFlags.inclist ) {
            g_info_lm( inf_curr_file, cur_el->element.graph.file );
            while( cb->fmflags & II_macro ) {                 // find prior file
                 cb = cb->prev;
            }
            g_info_lm( inf_curr_file, cb->s.f->filename );
        }

        insert_col_main( cur_el );

    } else {
        xx_err_c( err_file_not_found, file );
    }

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        if( *p != '\0' ) {                  // only if text follows
            process_text( p, g_curr_font );
        }
    }

    scan_start = scan_stop + 1;
    return;
}


