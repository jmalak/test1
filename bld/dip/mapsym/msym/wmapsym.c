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
* Description:  Convert a WLINK .map file to a MAPSYM .sym file.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "bool.h"
#include "mapsym.h"

#define ALIGN_16(x)     (((x) + 15) & ~15)

/* A map file (and a module) can contain a mix of 16-bit and 32-bit segments.
 * The linker will write 32-bit offsets to the map file if anything 32-bit is
 * in sight. MAPSYM does not particularly care which is which, and appears to
 * only use 32-bit symbols if there is a 64K or larger offset within that
 * segment.
 */

/* Symbol information structure. */
typedef struct sym_info_s {
    char        *name;      /* Symbol name (null-terminated). */
    uint32_t    offset;     /* Symbol offset (16- or 32-bit). */
} sym_info;

/* Segment information structure. */
typedef struct seg_info_s {
    char        *name;      /* Segment name (null-terminated). */
    sym_info    *symbols;   /* Symbol table for this segment. */
    uint16_t    num_syms;   /* Number of symbols in table. */
    uint16_t    addr;       /* Segment address/number. */
    uint32_t    offs;       /* Offset for 32-bit segments. */
    bool        is_32bit;   /* Segment uses 32-bit offsets. */
} seg_info;


static bool     AddAlphaSorted = FALSE; /* Add alpha-sorted symbol tables.*/
static bool     Syms32bit = FALSE;      /* 32-bit flat segments and symbols. */
static int      LineNo = 0;

/* Old MS LINK maximum. MAPSYM clients are unlikely to handle more. */
#define MAX_SEGS    1000

/* Architectural limit on number of symbols per segment in a .SYM file. */
#define MAX_SYMS    16384

/* Information for all segments. */
seg_info        Segments[MAX_SEGS];
unsigned        NumSegs;

/* Name of this map (module). */
char            *MapName;

/* Maximum symbol name length. Used e.g. by SYMDEB to format output when
 * listing the symbols in a map.
 */
uint32_t        MaxSymLen;

/* We just brute force the symbol table creation. The number of symbols in each
 * segment turns out to be fairly limited; although there are 16-bit symbol
 * counters, the offsets to symbols within a segment are also 16-bit. Since
 * each symbol must take up at minimum 4 bytes (2-byte offset, 1-byte count,
 * 1 byte of symbol name), there can not possibly be more than 16,384 symbols
 * per segment, and there will be fewer because the offset table itself needs
 * some space within the same 64K segment.
 * NB: MS LINK almost certainly has a lower limit of symbols per segment, and
 * it supports at most 1,000 segments.
 */


seg_info *findSeg16( uint16_t seg_addr )
{
    unsigned    i;

    for( i = 0; i < NumSegs; ++i ) {
        if( Segments[i].addr == seg_addr )
            return( &Segments[i] );
    }
    return( NULL );
}


/* Read in segment information.
 * NB: We assume that the segments are sorted. However, symbol files are
 * unlikely to depend on the segment ordering anyway.
 */
static int readSegments( FILE *f )
{
    char            lbuf[1024];
    char            name_seg[256];
    char            name_cls[256];
    char            name_grp[256];
    uint32_t        seg_sz;
    uint16_t        seg_16;
    uint32_t        ofs_32;
    char            start_str[16];
    char            *fg_rc;

    /* Check for the expected underline. */
    fg_rc = fgets( lbuf, sizeof( lbuf ), f );
    ++LineNo;
    if( !fg_rc )
        return( -1 );

    if( strncmp( lbuf, "====", 4 ) ) {
        printf( "Unexpected format (no ==== on line %d\n)\n", LineNo );
        return( -1 );
    }

    /* Skip the following empty line. */
    fg_rc = fgets( lbuf, sizeof( lbuf ), f );
    ++LineNo;
    if( !fg_rc )
        return( -1 );

    for( ;; ) {
        fg_rc = fgets( lbuf, sizeof( lbuf ), f );
        ++LineNo;
        if( !fg_rc )
            break;

        /* An empty line indicates the end of section. */
        if( lbuf[0] == '\n' )
            break;

        /* Segment / Class / Group / Address / Size. */
        sscanf( lbuf, "%s %s %s %s %08x\n", name_seg, name_cls, name_grp, start_str, &seg_sz );
        seg_16 = 0;
        if( start_str[4] == ':' ) {
            if( isspace( start_str[9] ) )
                sscanf( start_str, "%04x:%04x", &seg_16, &ofs_32 );
            else
                sscanf( start_str, "%04x:%08x", &seg_16, &ofs_32 );
        } else
            sscanf( start_str, "%08x", &ofs_32 );

#ifdef DEBUG
        if( start_str[4] == ':' ) {
            if( isspace( start_str[9] ) )
                printf( "%04X:%04X ", seg_16, ofs_32 );
            else
                printf( "%04X:%08X ", seg_16, ofs_32 );
        } else
            printf( "%08X ", ofs_32 );
        printf( "size %08X: %s\n", seg_sz, name_seg );
#endif

        if( seg_16 && findSeg16( seg_16 ) ) {
            /* If we've seen this segment number, do nothing. The name will
             * be taken from the first segment seen.
             */
            ;
        } else {
            /* Allocate storage for symbols. */
            Segments[NumSegs].symbols = calloc( MAX_SYMS, sizeof( sym_info ) );

            if( !Segments[NumSegs].symbols ) {
                printf( "Error: Out of memory!\n" );
                return( -1 );
            }

            /* Fill in segment data. */
            Segments[NumSegs].name = strdup( name_seg );
            if( Syms32bit ) {
                Segments[NumSegs].addr = 0;
                Segments[NumSegs].offs = ofs_32;
            } else {
                Segments[NumSegs].addr = seg_16;
                Segments[NumSegs].offs = ofs_32;
            }

            /* One more segment done. */
            ++NumSegs;
        }
    }

    return( 0 );
}


/* Read in symbol information.
 * NB: The symbols in a map file are sorted by module and thus do not
 * have any particular sort order by address or symbol name.
 */
static int readSymbols( FILE *f )
{
    char            lbuf[1024];
    uint32_t        ofs_32;
    unsigned        i;
    uint16_t        seg_16;
    char            name[256];
    char            *fg_rc;
    seg_info        *seg;

    /* Check for the expected underline. */
    fg_rc = fgets( lbuf, sizeof( lbuf ), f );
    ++LineNo;
    if( !fg_rc )
        return( -1 );

    if( strncmp( lbuf, "====", 4 ) ) {
        printf( "Unexpected format (no ==== on line %d\n)\n", LineNo );
        return( -1 );
    }

    /* Skip the following empty line. */
    fg_rc = fgets( lbuf, sizeof( lbuf ), f );
    ++LineNo;
    if( !fg_rc )
        return( -1 );

    for( ;; ) {
        fg_rc = fgets( lbuf, sizeof( lbuf ), f );
        ++LineNo;
        if( !fg_rc )
            break;

        /* An empty line indicates the end of section. */
        if( lbuf[0] == '\n' )
            break;

        /* Skip "module" lines. */
        if( !strncmp( lbuf, "Module:", 7 ) )
            continue;

        /* Collect symbol information. */
        if( isxdigit( lbuf[0] ) && isxdigit( lbuf[1] ) && isxdigit( lbuf[2] ) && isxdigit( lbuf[3] ) ) {
            int     flg_chr;

            if( lbuf[4] == ':' ) {
                if( isspace( lbuf[9] ) ) {
                    sscanf( lbuf, "%04x:%04x%c %s\n", &seg_16, &ofs_32, &flg_chr, name );
#ifdef DEBUG
                    printf( "%04X:%04X %s\n", seg_16, ofs_32, name );
#endif
                } else {
                    sscanf( lbuf, "%04x:%08x%c %s\n", &seg_16, &ofs_32, &flg_chr, name );
#ifdef DEBUG
                    printf( "%04X:%04X %s\n", seg_16, ofs_32, name );
#endif
                }
            } else {
                sscanf( lbuf, "%08x%c %s\n", &ofs_32, &flg_chr, name );
#ifdef DEBUG
                printf( "%08X: %s\n", ofs_32, name );
#endif
            }
        }

        /* Store symbol data for later. */
        if( Syms32bit ) {
            /// @todo What do we do here exactly?
        } else {
            seg = findSeg16( seg_16 );

            if( seg ) {
                i = seg->num_syms++;
                seg->symbols[i].offset = ofs_32;
                seg->symbols[i].name   = strdup( name );
                if( ofs_32 >= UINT16_MAX )
                    seg->is_32bit = TRUE;
            }
            /// @todo Is it an error if findSeg16 fails?
        }
    }

    return( 0 );
}


/* Add dummy symbols if needed. */
void fakeSymbols( void )
{
    int         i;
    seg_info    *seg;

    /* Microsoft's debuggers don't like segments with no symbols (e.g. WDEB386
     * crashes). MAPSYM creates fake symbols to avoid the situation.
     */
    for( i = 0; i < NumSegs; ++i ) {
        seg = &Segments[i];
        if( !seg->num_syms ) {
            seg->num_syms++;
            seg->symbols[0].offset = 0;
            seg->symbols[0].name   = strdup( "__$dummy$" );
        }
    }
}


/* Calculate the size of symbols within a segment. */
uint32_t calcSegSymSize( seg_info *seg )
{
    uint32_t    map_size = 0;
    uint32_t    sym_size;
    uint32_t    name_len;
    unsigned    i;

    /* Figure out the size of fixed per-symbol storage. */
    sym_size = ((Syms32bit || seg->is_32bit) ? SYM_SYMDEF_32_FIXSIZE : SYM_SYMDEF_FIXSIZE) + 1;

    /* Now add it together with storage needed by symbol names. */
    for( i = 0; i < seg->num_syms; ++i ) {
        name_len = strlen( seg->symbols[i].name );
        if( name_len > MaxSymLen )
            MaxSymLen = name_len;
        map_size += sym_size + name_len;
    }

    return( map_size );
}


/* Calculate the size in bytes of one segment in a .sym file.
 * For each symbol, we need to store:
 * - Either 16-bit or 32-bit symbol definition
 * - Symbol name and length
 * - One 16-bit offset
 * - If alpha-sorted table is present, another 16-bit offset
 */
uint32_t calcSegSize( seg_info *seg )
{
    uint32_t    map_size;
    uint32_t    sym_size;

    /* Start with the segment header. */
    map_size = SYM_SEGDEF_FIXSIZE + 1 + strlen( seg->name );

    /* Now add the storage needed by symbol entries including names. */
    map_size += calcSegSymSize( seg );

    /* Add the size of offset table(s). */
    sym_size = sizeof( uint16_t );
    if( AddAlphaSorted )
        sym_size += sizeof( uint16_t );

    map_size += sym_size * seg->num_syms;

    return( map_size );
}


uint32_t calcHdrSize( void )
{
    uint32_t    map_size;

    /* Note that although there's no null terminator
     * stored in the .sym file, a length byte is stored instead.
     */
    map_size = SYM_MAPDEF_FIXSIZE + 1 + strlen( MapName );

    /// @todo Add size of absolute symbols

    return( map_size );
}


/* Calculate size in bytes of one map (module). Does not include trailer. */
uint32_t calcMapSize( void )
{
    uint32_t    map_size;
    uint32_t    seg_size;
    unsigned    i;

    /* First the map header and absolute symbols, if any. */
    map_size = calcHdrSize();
    map_size = ALIGN_16( map_size );

    /* Now add all the segments. */
    for( i = 0; i < NumSegs; ++i ) {
        seg_size = ALIGN_16( calcSegSize( &Segments[i] ) );
        if( seg_size > 65535 ) {
            printf( "Error: Segment %s: Symbol information > 64K!\n", Segments[i].name );
            return( 0 );
        }
        map_size += seg_size;
    }

    printf( "map_size: %u\n", map_size );

    /* NB: Result will always be paragraph aligned. */
    return( map_size );
}


/* Comparison function for sorting by address. */
int cmp_addr( const void *op1, const void *op2 )
{
    const sym_info  *p1 = op1;
    const sym_info  *p2 = op2;

    return( p1->offset - p2->offset );
}


/* Comparison function for alphanumeric sorting. */
int cmp_alpha( const void *op1, const void *op2 )
{
    const sym_info  *p1 = op1;
    const sym_info  *p2 = op2;

    return( strcmp( p1->name, p2->name ) );
}


void sortSegment( seg_info *seg, bool alpha_sort )
{
    if( alpha_sort ) {
        qsort( seg->symbols, seg->num_syms, sizeof( seg->symbols[0] ), cmp_alpha );
    } else {
        qsort( seg->symbols, seg->num_syms, sizeof( seg->symbols[0] ), cmp_addr );
    }
}


/* Ensure the next bit of file will start at a paragraph boundary. */
int padSection( FILE *f )
{
    long        pos;
    int         n;
    unsigned    pad;
    uint8_t     zeros[16];

    pos = ftell( f );

    if( pos & 15 ) {
        memset( zeros, 0, sizeof( zeros ) );
        pad = 16 - (pos & 15);
        n = fwrite( zeros, 1, pad, f );
    }
    return( 0 );
}


/* Write out the symbol information for one segment. */
int writeSegment( FILE *f, seg_info *seg, uint16_t file_ofs )
{
    int         n;
    unsigned    i;
    unsigned    ofs_tbl_size;
    uint16_t    cur_ofs;
    uint16_t    *sym_ofs_tbl;
    sym_segdef  map_seg;

    memset( &map_seg, 0, sizeof( map_seg ) );

    map_seg.next_ptr    = file_ofs + ALIGN_16( calcSegSize( seg ) ) / 16;
    map_seg.num_syms    = seg->num_syms;
    map_seg.load_addr   = seg->addr;
    map_seg.name_len    = strlen( seg->name );
    map_seg.sym_tab_ofs = SYM_SEGDEF_FIXSIZE + 1 + map_seg.name_len + calcSegSymSize( seg );

    if( seg->is_32bit )
        map_seg.sym_type |= SYM_FLAG_32BIT;

    n = fwrite( &map_seg, 1, SYM_SEGDEF_FIXSIZE + 1, f );

    n = fwrite( seg->name, 1, map_seg.name_len, f );

#if 0
    typedef struct {
        unsigned_16     next_ptr;           /* next segdef, 0 if last; may be circular */
        unsigned_16     num_syms;           /* number of symbols in segment */
        unsigned_16     sym_tab_ofs;        /* offset of symbol table from segdef */
        unsigned_16     load_addr;          /* segment load address */
        unsigned_16     phys_0;             /* physical address 0 */
        unsigned_16     phys_1;             /* physical address 1 */
        unsigned_16     phys_2;             /* physical address 2 */
        unsigned_8      sym_type;           /* type of symbols (16/32bit) */
        unsigned_8      pad0;               /* pad byte */
        unsigned_16     linnum_ptr;         /* pointer to line numbers */
        unsigned_8      is_loaded;          /* segment loaded flag */
        unsigned_8      curr_inst;          /* current instance */
        unsigned_8      name_len;           /* length of symbol name */
        char            name[1];            /* segment name */
    } sym_segdef;
#endif

    /* Sort the symbols by address. */
    sortSegment( seg, FALSE );

    /* Now create a table of offsets to symbols. */
    ofs_tbl_size = seg->num_syms * sizeof( uint16_t );
    sym_ofs_tbl = malloc( ofs_tbl_size );
    if( !sym_ofs_tbl ) {
        printf( "Error: Failed to allocate symbol offset table for segment %s!\n", seg->name );
        return( -1 );
    }

    /* The first symbol will be stored right after the segment header/name. */
    cur_ofs = SYM_SEGDEF_FIXSIZE + 1 + map_seg.name_len;

    /* Write out the symbols and build offset table at the same time. */
    for( i = 0; i < seg->num_syms; ++i ) {
        sym_ofs_tbl[i] = cur_ofs;

        if( Syms32bit || seg->is_32bit ) {
            sym_symdef_32   sym32;

            sym32.offset = seg->symbols[i].offset;
            sym32.name_len = strlen( seg->symbols[i].name );

            /* Write out one symbol. */
            n = fwrite( &sym32, 1, SYM_SYMDEF_32_FIXSIZE + 1, f );

            n = fwrite( seg->symbols[i].name, 1, sym32.name_len, f );

            /* Advance the offset. */
            cur_ofs += SYM_SYMDEF_32_FIXSIZE + 1 + sym32.name_len;
        } else {
            sym_symdef  sym16;

            sym16.offset = (uint16_t)seg->symbols[i].offset;
            sym16.name_len = strlen( seg->symbols[i].name );

            /* Write out one symbol. */
            n = fwrite( &sym16, 1, SYM_SYMDEF_FIXSIZE + 1, f );

            n = fwrite( seg->symbols[i].name, 1, sym16.name_len, f );

            /* Advance the offset. */
            cur_ofs += SYM_SYMDEF_FIXSIZE + 1 + sym16.name_len;
        }
    }

    /* Now just dump out the whole offset table. */
    n = fwrite( sym_ofs_tbl, 1, ofs_tbl_size, f );

    free( sym_ofs_tbl );

    return( 0 );
}


/* Write out the .sym file. */
int writeSymFile( FILE *f )
{
    int         n;
    unsigned    i;
    sym_mapdef  sym_hdr;
    sym_endmap  sym_trailer;
    uint16_t    file_ofs;
    uint32_t    map_size;

    /* Calculate how big the symbol information will be so that the .sym file
     * header can correctly point at the next map (which will be the trailer).
     */
    map_size = calcMapSize();
    if( !map_size )
        return( -1 );

    memset( &sym_hdr, 0, sizeof( sym_hdr ) );
    sym_hdr.next_ptr      = map_size / 16;
    sym_hdr.abs_sym_count = 0;
    sym_hdr.abs_tab_ofs   = 0;
    sym_hdr.num_segs      = NumSegs;
    sym_hdr.seg_ptr       = ALIGN_16( calcHdrSize() ) / 16;
    sym_hdr.max_sym_len   = MaxSymLen;
    sym_hdr.name_len      = strlen( MapName );

    /* Write out the map header. */
    n = fwrite( &sym_hdr, 1, SYM_MAPDEF_FIXSIZE + 1, f );

    n = fwrite( MapName, 1, strlen( MapName ), f );

    padSection( f );

    file_ofs = ALIGN_16( calcHdrSize() ) / 16;

    /* Now all the segments. */
    for( i = 0; i < NumSegs; ++i ) {
        writeSegment( f, &Segments[i], file_ofs );
        padSection( f );
        file_ofs += ALIGN_16( calcSegSize( &Segments[i] ) ) / 16;
    }

    /* And finally the .sym file trailer. */
    sym_trailer.zero      = 0;  /* No further maps. */
    sym_trailer.minor_ver = 10; //SYM_VERSION_MINOR_OLD;
    sym_trailer.major_ver = 3;  //SYM_VERSION_MAJOR_OLD;
    n = fwrite( &sym_trailer, 1, sizeof( sym_trailer ), f );

    return( 0 );
}


/* Assume a map file in the following format:
 *
 * Groups
 *   - Header line starts with 'Group'
 * Segments
 *   - Header line starts with 'Segment' and includes 'Group'
 * Absolute Segments (optional)
 *   - Header line starts with 'Segment', does not include 'Group'
 * Memory Map
 *   - Starts with 'Address'
 *
 * Each section ends with an empty line (actually two). The sections are
 * always in the above order. Once the end of the Memory Map is reached,
 * data input is complete.
 *
 * Map files can be 16-bit or 32-bit; no attempt is made to deal with
 * mixed 16/32-bit maps.
 */
int main( int argc, char *argv[] )
/********************************/
{
    FILE                *fmap;
    FILE                *fsym;
    char                lbuf[1024];
    char                *fg_rc;

    if( argc < 3 ) {
        printf( "Usage:  wmapsym [-a] <mapfile> <symfile>\n" );
        printf( "Where <file> is a Watcom .map file\n" );
        printf( "   -a  add alphabetically sorted symbol tables\n" );
        return( 1 );
    }

    if( !strcmp( argv[1], "-a" ) ) {
        AddAlphaSorted = TRUE;
        ++argv;
    }

    /* See if we can open the input map file. */
    fmap = fopen( argv[1], "rt" );
    if( fmap == NULL ) {
        printf( "Error opening map file '%s'.\n", argv[1] );
        return( 2 );
    }

    /* There's something like a map file, so try creating the .sym file. */
    fsym = fopen( argv[2], "wb" );
    if( fmap == NULL ) {
        printf( "Error creating sym file '%s'.\n", argv[2] );
        return( 2 );
    }

    for( ;; ) {
        fg_rc = fgets( lbuf, sizeof( lbuf ), fmap );
        ++LineNo;
        if( !fg_rc )
            break;

        if( !strncmp( lbuf, "Executable", 10 ) ) {
            char    image_name[256];
            char    *s;
            char    *q;

            sscanf( lbuf, "Executable Image: %s\n", image_name );

            /* The image name may be a relative path. */
            q = image_name;
            do {
                s = strchr( q, '/' );
                if( !s )
                    s = strchr( q, '\\' );
                if( s )
                    q = s + 1;
            } while( s );

            s = MapName = strdup( q );
            /* Strip extension and convert to uppercase. */
            while( *s ) {
                if( *s == '.' ) {
                    *s = '\0';
                    break;
                }
                *s = toupper( *s );
                ++s;
            }
            printf( "Map (module) name: %s\n", MapName );
        } else if( !strncmp( lbuf, "Group", 5 ) ) {
        } else if( !strncmp( lbuf, "Segment", 7 ) ) {
            if( strstr( lbuf, "Group" ) ) {
                readSegments( fmap );
            } else {
                //readAbsSegments( fmap );
            }
        } else if( !strncmp( lbuf, "Address", 7 ) ) {
            readSymbols( fmap );
        }
    }

    if( fclose( fmap ) != 0 ) {
        printf( "Error closing map file.\n" );
        return( 2 );
    }

    if( !MapName ) {
        printf( "Could not determine module name.\n" );
        return( 3 );
    }

    /// @todo Bail if there are no segments

    fakeSymbols();

    /* We have collected all data; write out the .sym file. */
    writeSymFile( fsym );

    free( MapName );

    if( fclose( fsym ) != 0 ) {
        printf( "Error closing sym file.\n" );
        return( 2 );
    }

    return( 0 );
}
