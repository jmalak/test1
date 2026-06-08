/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2008 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML tags :P, :PC and :NOTE processing
*
****************************************************************************/

#include    "wgml.h"

static  bool    in_fig_xmp_block;       // set to true if in FIG or XMP


/***************************************************************************/
/*  Setup for both proc_p_pc() and do_force_pc()                           */
/***************************************************************************/

static void p_pc_setup( p_lay_tag * p_pc )
{
    /****************************************************************/
    /* While this was once much more straightforward, FIG, LQ, and  */
    /* XMP turned out to have an additonal effect on P and PC       */
    /****************************************************************/

    in_fig_xmp_block = false;
    switch( nest_cb->c_tag ) {
    case t_FIG:
    case t_XMP:
        in_fig_xmp_block = true;
    case t_LQ:
        g_curr_font = layout_work.defaults.font;
    case t_DL:
    case t_GL:
    case t_OL:
    case t_SL:
    case t_UL:
        ProcFlags.lp_p_pc_in_block = true;
    }

    ProcFlags.keep_left_margin = true;  // special Note indent
    if( ProcFlags.overprint && ProcFlags.cc_cp_done ) {
        ProcFlags.overprint = false;    // cancel overprint
    }
    ProcFlags.cc_cp_done = false;       // cancel CC/CP notification
    start_doc_sect();                   // if not already done

    if( g_line_indent == 0 ) {
        ProcFlags.para_starting = false;    // clear for this tag's break
    }
    scr_process_break();

    /* If ProcFlags.para_starting is false, scr_process_break() will not process ProcFlags.block_starting */

    if( !ProcFlags.para_starting && ProcFlags.block_starting ) {
        if( ProcFlags.wh_device ) {             // may apply to other devices as well, but not PS
            if( g_top_skip > 0 ) {              // SP rather than SK
                g_post_skip +=g_subs_skip;      // this appears to be correct so far
                ProcFlags.block_starting = false;
            }
        } else {                                // for PS and perhaps other devices, but not for WHELP
            g_post_skip +=g_subs_skip;          // this appears to be correct so far
            ProcFlags.block_starting = false;
        }
    }
    g_line_indent = conv_hor_unit( &(p_pc->line_indent), g_curr_font );

    t_page.cur_width = t_page.cur_left + g_line_indent; // possibly indent first line

    g_cur_threshold = layout_work.widow.threshold; // standard threshold

    if( ProcFlags.force_pc && ProcFlags.in_done ) {
        // placeholder
    } else {
        set_skip_vars( &(p_pc->pre_skip), NULL, &(p_pc->post_skip), g_text_spacing, g_curr_font );
    }

    if( ProcFlags.wh_device ) {             // for WHELP and perhaps other devices, but not for PS
        // placeholder
    } else {                                // for PS and perhaps other devices, but not for WHELP
        g_subs_skip += g_top_skip;          // add value from SP
    }

    ProcFlags.para_starting = true;     // for next break, not this tag's break

    post_space = 0;

    return;
}

/***************************************************************************/
/*  :P. :PC common routine                                                 */
/***************************************************************************/

static void proc_p_pc( p_lay_tag * p_pc, e_tags t )
{
    char    *   p;

    p_pc_setup( p_pc );

    if( nest_cb->c_tag != t_LQ ) {
        ProcFlags.block_starting = true;    // to catch empty paragraphs
    }
    scan_err = false;
    p = scan_start;

    SkipDot( p );                       // over '.'
    if( *p != '\0' ) {
        if( in_fig_xmp_block ) {        // in FIG or XMP
            // placeholder
        } else if( (t == t_P) && !ProcFlags.concat ) {
            if( input_cbs->fmflags & II_tag ) {
                g_post_skip = 0;
            } else {
                g_subs_skip = g_post_skip;
                g_post_skip = 0;
            }
        }
        process_text( p, g_curr_font );
    } else if( !ProcFlags.concat ) {
        g_post_skip = 0;
    }

    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  :P.perhaps paragraph elements                                          */
/***************************************************************************/

extern void gml_p( const gmltag * entry )
{
    proc_p_pc( &layout_work.p, t_P );
}

/***************************************************************************/
/*  :PC.perhaps paragraph elements                                         */
/***************************************************************************/

extern void gml_pc( const gmltag * entry )
{
    if( g_top_skip > 0 ) {                  // SP was used
        if( g_subs_skip > g_top_skip ) {    // merge the values for PC
            g_top_skip = g_subs_skip;
        }
    }

    proc_p_pc( &layout_work.pc, t_PC );
}

/***************************************************************************/
/*  :NOTE.perhaps paragraph elements                                       */
/***************************************************************************/

extern void gml_note( const gmltag * entry )
{
    char        *   p;
    font_number     font_save;
    text_chars  *   marker;

    scan_err = false;
    p = scan_start;

    start_doc_sect();                   // if not already done

    scr_process_break();

    note_lm = t_page.cur_left;
    font_save = g_curr_font;
    set_skip_vars( &layout_work.note.pre_skip, NULL, NULL,
                    g_text_spacing, layout_work.note.font );

    t_page.cur_left += conv_hor_unit( &layout_work.note.left_indent, layout_work.note.font );
    t_page.cur_width = t_page.cur_left;
    ju_x_start = t_page.cur_width;
    ProcFlags.keep_left_margin = true;  // keep special Note indent

    if( strlen(layout_work.note.string) > 0 ) {
        process_text( layout_work.note.text, layout_work.note.font );
    }
    insert_hard_spaces( layout_work.note.spaces, strlen(layout_work.note.spaces), FONT0 );
    t_page.cur_left = t_page.cur_width; // set indent for following text
    ProcFlags.note_starting = true;
    ProcFlags.zsp = true;

    g_text_spacing = layout_work.note.spacing;
    g_curr_font = layout_work.defaults.font;

    set_skip_vars( NULL, NULL, &layout_work.note.post_skip, g_text_spacing, g_curr_font );
    SkipDot( p );                       // over '.'
    SkipSpaces( p );                    // skip initial space
    if( *p != '\0' ) {                  // if text follows
        post_space = 0;
        process_text( p, g_curr_font );
    } else if( !ProcFlags.concat && ProcFlags.has_aa_block &&
               (t_line != NULL) && (post_space > 0) ) {

        /* only create marker if line not empty,                            */
        /* :NOTE note_string is not nullstring and ends in at least 1 space */

        marker = alloc_text_chars( NULL, 0, font_save );
        marker->x_address = t_page.cur_width;
        if( t_line->first == NULL ) {
            t_line->first = marker;
            t_line->last = t_line->first;
        } else {
            marker->prev = t_line->last;
            t_line->last->next = marker;
            t_line->last = t_line->last->next;
        }
        post_space = 0;
    }

    ProcFlags.block_starting = true;    // to catch empty paragraphs
    g_curr_font = font_save;
    scan_start = scan_stop + 1;
    return;
}

/***************************************************************************/
/*  Force PC on text line following certain blocks                         */
/*  Note: only called with text, so ProcFlags.block_starting is not set    */ 
/***************************************************************************/

extern void do_force_pc( char * p )
{
    if( g_top_skip > 0 ) {                  // SP was used
        if( g_subs_skip > g_top_skip ) {    // merge the values for PC
            g_top_skip = g_subs_skip;
        }
    }

    p_pc_setup( &layout_work.pc );

    /* Inline tags use NULL because the text font is different from the font needed by PC */
    if( (p != NULL) && (*p != '\0') ) {
        process_text( p, g_curr_font );
    }

    return;
}
