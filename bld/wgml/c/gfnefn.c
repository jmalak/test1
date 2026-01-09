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
* Description:  tags FN and eFN
*
* Note: FNREF is defined in gxxref.c
*
****************************************************************************/


#include "wgml.h"

static  bool            concat_save;            // for ProcFlags.concat
static  group_type      sav_group_type;         // save prior group type
static  ju_enum         justify_save;           // for ProcFlags.justify

/***************************************************************************/
/*      :FN  [id=’id-name’].                                               */
/*           <paragraph elements>                                          */
/*           <basic document elements>                                     */
/* The footnote tag causes a note to be placed at the bottom of the page.  */
/* The footnote text is preceded by a footnote number which is generated   */
/* by the WATCOM Script/GML processor. Footnotes may be used where a basic */
/* document element is permitted to appear, with the exception of a        */
/* figure, footnote, or example. The :efn tag terminates a footnote.       */
/***************************************************************************/

void gml_fn( const gmltag * entry )
{
    char            buffer[11];
    char            id[ID_LEN];
    char        *   p;
    char        *   pa;
    ref_entry   *   cur_ref;

    memset( &AttrFlags, 0, sizeof( AttrFlags ) );   // clear all attribute flags
    start_doc_sect();
    scr_process_break();
    scan_err = false;

    g_keep_nest( "Footnote" );          // catch nesting errors

    fn_count++;                         // get current FN number
    id[0] = '\0';                       // initialize to no id

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
            if( !strnicmp( "id", p, 2 ) ) {
                p += 2;
                p = get_id_value( p, id );
                if( AttrFlags.id ) {
                    if( g_att_val.val_quoted ) {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len + 1 );
                    } else {
                        xx_line_err_ci( err_att_dup, g_att_val.att_start, 
                            g_att_val.val_start - g_att_val.att_start + g_att_val.val_len );
                    }
                }
                AttrFlags.id = true;
                if( ProcFlags.no_value_found ) {
                    xx_line_err_c( err_att_val_missing, p );
                }
                if( ProcFlags.no_equal_sign ) {
                    xx_line_err_c( err_eq_missing, p );
                }
                if( g_att_val.val_start == NULL ) {
                    break;
                }
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

    g_curr_font = layout_work.fn.font;
    init_nest_cb();
    nest_cb->p_stack = copy_to_nest_stack();

    nest_cb->left_indent = conv_hor_unit( &layout_work.fn.line_indent, g_curr_font );
    nest_cb->font = g_curr_font;
    nest_cb->c_tag = t_FN;

    t_page.cur_left += nest_cb->left_indent;
    t_page.cur_width = t_page.cur_left;
    ProcFlags.keep_left_margin = true;      // keep special indent

    g_text_spacing = layout_work.fn.spacing;

    if( t_page.last_pane->cols->footnote == NULL ) {   // pre_lines affects first FN on page only
        set_skip_vars( NULL, &layout_work.fn.pre_lines, NULL, g_text_spacing, g_curr_font );
    } else {
        set_skip_vars( NULL, NULL, NULL, g_text_spacing, g_curr_font );
    }

    sav_group_type = cur_group_type;
    cur_group_type = gt_fn;
    cur_doc_el_group = alloc_doc_el_group( gt_fn );
    cur_doc_el_group->next = t_doc_el_group;
    t_doc_el_group = cur_doc_el_group;
    cur_doc_el_group = NULL;

    concat_save = ProcFlags.concat;
    ProcFlags.concat = false;
    justify_save = ProcFlags.justify;
    ProcFlags.justify = ju_off;         // TBD

    /* Only create the entry on the first pass */

    if( pass == 1 ) {                   // add this FN to the fn_list
        fn_entry = init_ffh_entry( fn_list );
        fn_entry->flags = ffh_fn;       // mark as FN
        fn_entry->number = fn_count;    // add number of this FN
        if( fn_list == NULL ) {         // first entry
            fn_list = fn_entry;
        }
        if( id[0] != '\0' ) {           // add this entry to fn_ref_dict
            cur_ref = find_refid( fn_ref_dict, id );
            if( cur_ref == NULL ) {     // new entry
                cur_ref = mem_alloc( sizeof( ref_entry ) );
                init_ref_entry( cur_ref, id );
                cur_ref->flags = rf_ffh;
                cur_ref->u.ffh.entry = fn_entry;
                add_ref_entry( &fn_ref_dict, cur_ref );
            } else {
                dup_id_err( cur_ref->id, "footnote" );
            }
        }
    } else {
        if( (page + 1) != fn_entry->pageno ) {  // page number changed
            fn_entry->pageno = page + 1;
            if( GlobalFlags.lastpass ) {        // last pass only
                if( strlen( id ) > 0 ) {        // FN id exists
                    fn_fwd_refs = init_fwd_ref( fn_fwd_refs, id );
                }
            }
        }
    }

/// these will need to be used ... eventually
//skip
//frame

    format_num( fn_entry->number, buffer, sizeof( buffer ),
                                                        layout_work.fn.number_style );
    process_text( buffer, layout_work.fn.number_font ); // FN prefix
    t_page.cur_left = nest_cb->lm + conv_hor_unit( &layout_work.fn.align, g_curr_font );
    t_page.cur_width = t_page.cur_left;
    ProcFlags.keep_left_margin = true;  // keep special indent

    if( !ProcFlags.reprocess_line && *p != '\0' ) {
        SkipDot( p );                       // possible tag end
        SkipSpaces( p );                    // skip preceding spaces
        ProcFlags.concat = true;            // concatenation on
        if( *p != '\0' ) {
            process_text( p, g_curr_font);  // if text follows
        }
    }
    scan_start = scan_stop + 1;
    return;
}


/***************************************************************************/
/*      :eFN.                                                              */
/* This tag signals the end of a footnote. A corresponding :fn tag must be */
/* previously specified for each :efn tag.                                 */
/***************************************************************************/

void gml_efn( const gmltag * entry )
{
    char    *   p;
    tag_cb  *   wk;

    scr_process_break();
    rs_loc = 0;

    if( cur_group_type != gt_fn ) {         // no preceding :FN tag
        g_err_tag_prec( "FN" );
        scan_start = scan_stop + 1;
        return;
    }

    ProcFlags.concat = concat_save;
    ProcFlags.justify = justify_save;
    t_page.cur_left = nest_cb->lm;
    t_page.max_width = nest_cb->rm;

    wk = nest_cb;
    nest_cb = nest_cb->prev;
    add_tag_cb_to_pool( wk );

    g_curr_font = nest_cb->font;

    /* Submit the FN/eFN block */

    cur_group_type = sav_group_type;
    if( t_doc_el_group != NULL) {
        cur_doc_el_group = t_doc_el_group;      // detach current element group
        t_doc_el_group = t_doc_el_group->next;  // processed doc_elements go to next group, if any
        cur_doc_el_group->next = NULL;
        insert_col_fn( cur_doc_el_group );
    }

    t_page.cur_width = t_page.cur_left;

    scan_err = false;
    p = scan_start;
    SkipDot( p );                       // possible tag end
    if( *p != '\0' ) {
        process_text( p, g_curr_font);  // if text follows
    }
    if( pass > 1 ) {                    // not on first pass
        fn_entry = fn_entry->next;      // get to next FN
    }
    scan_start = scan_stop + 1;
    return;
}

