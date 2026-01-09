/* test3.c (OS/2 Startup Test 3)  */

#if !defined(QA_MAKE_EXE) && !defined(QA_MAKE_DLL)
#error You must define either of QA_MAKE_EXE or QA_MAKE_DLL
#endif
#if defined(QA_MAKE_EXE) && defined(QA_MAKE_DLL)
#error You can only define one of QA_MAKE_EXE or QA_MAKE_DLL
#endif

#include <stdio.h>
#include <process.h>
#include <string.h>
#include <dos.h>    // for sleep()

#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#include <os2.h>

#define NUM_THREADS     5
#if QA_MAKE_EXE
    #define RTL_TYPE    "EXE"
#else
    #define RTL_TYPE    "DLL"
#endif

/* Fish out the stack location from TIB, going directly through FS. */
extern  void *get_thread_stk_bot( void );
#pragma aux get_thread_stk_bot = "mov eax,fs:[4]" modify exact [eax] value [eax];
extern  void *get_thread_stk_top( void );
#pragma aux get_thread_stk_top = "mov eax,fs:[8]" modify exact [eax] value [eax];
extern  void *get_esp( void );
#pragma aux get_esp = "mov eax,esp" modify exact [eax] value [eax];
extern  unsigned get_tid( void );
#pragma aux get_tid = "mov eax,fs:[12]" "mov eax, [eax]" modify exact [eax] value [eax];

/*
 * The __GetThreadPtr function is an internal RTL routine that returns
 * a pointer to the current thread's thread struct. The struct layout
 * only matters to the multi-threaded support code, but the first item
 * is the __stklowP address/pointer which the stack checking routine
 * relies on.
 */
struct thread_data {
    void    *__stklowP;
};

extern struct thread_data *(* __GetThreadPtr)( void );

void query_stack( void )
{
    PVOID   stk_bot_tib, stk_top_tib;
    PVOID   stk_bot_fs,  stk_top_fs;
    PTIB    ptib;
    PPIB    ppib;
    PVOID   stklow;

    DosGetInfoBlocks( &ptib, &ppib );
    stk_bot_tib = ptib->tib_pstack;
    stk_top_tib = ptib->tib_pstacklimit;

    stk_bot_fs = get_thread_stk_bot();
    stk_top_fs = get_thread_stk_top();

    if( stk_bot_tib != stk_bot_fs )
        printf( "Stack bottom mismatch (%p TIB vs %p FS)!", stk_bot_tib, stk_bot_fs );

    if( stk_top_tib != stk_top_fs )
        printf( "Stack top mismatch (%p TIB vs %p FS)!", stk_top_tib, stk_top_fs );

    stklow = __GetThreadPtr()->__stklowP;

    printf( "%s RTL stack: %P:%P __stklowP %P ESP %P TID %u\n", RTL_TYPE, stk_bot_tib, stk_top_tib, stklow, get_esp(), get_tid() );
}

#if defined(QA_MAKE_DLL)

void dll_threadfunc( void* private_data )
{
    query_stack();
    /* Sleep a sec to let other threads start before this one exits. */
    sleep( 1 );
}

void do_start_threads( const char* const szMsg )
{
    int i;

    for( i = 0; i < NUM_THREADS; i++ ) {
        _beginthread( &dll_threadfunc, NULL, 8192, (void*)szMsg );
    }
}

static const char* const rgMsgs[2] =
{
    "Process initialization",
    "Process termination"
};

PVOID   entry_stk_bot, entry_stk_top;

unsigned APIENTRY LibMain( unsigned hmod, unsigned termination )
{
    /* Record the stack limits for initialization/termination.
     * This differs significantly between OS/2 versions! All OS/2 versions
     * appear to run the DLL initialization on the main thread's stack if
     * the DLL is statically linked. However, if DosLoadModule is used,
     * most versions (including OS/2 2.0 and WSeB) use the calling thread's
     * stack, but some versions (including Warp GA and Warp 4 GA) use
     * a completely separate stack higher up in memory. Warp 3 FP40 and
     * Warp 4 FP12 are known to have reverted to the earlier behavior.
     *
     * NB: Different OS/2 versions do not report the same stack bottom
     * in the TIB. WSeB and later, as well as newer Warp 3/4 FixPacks take
     * the initial ESP value and subtract the stack size (both taken from
     * the EXE header). Older OS/2 versions round down the stack bottom
     * value, which means the stack bottom reported in the TIB is quite
     * possibly lower than it should be.
     */
    entry_stk_bot = get_thread_stk_bot();
    entry_stk_top = get_thread_stk_top();

    if( termination )
        printf( "%s: stack %P:%P\n", rgMsgs[1], entry_stk_bot, entry_stk_top );
    else
        printf( "%s: stack %P:%P\n", rgMsgs[0], entry_stk_bot, entry_stk_top );
    return( 1 );
}

/*
 * A worker function that starts its own threads and does all
 * kinds of stuff.
 */
void __export QA_func1( void )
{
    printf( "In DLL\n" );
    query_stack();
    do_start_threads( "QA_func1" );
    sleep( 1 /* second */); // Let'em die
}

/*
 * A simple callback function that just "happens" to live
 * in a DLL.
 */
void __export QA_func2( void )
{
    query_stack();
}

#endif /* QA_MAKE_DLL */

#if defined(QA_MAKE_EXE)

#if !defined(QA_DLL_LINK) && !defined(QA_DLL_LOAD)
#error You must define either of QA_DLL_LINK or QA_DLL_LOAD
#endif
#if defined(QA_DLL_LINK) && defined(QA_DLL_LOAD)
#error You can only define one of QA_DLL_LINK or QA_DLL_LOAD
#endif

#ifdef QA_DLL_LINK
extern void QA_func1( void );
extern void QA_func2( void );
#else
void (*QA_func1)( void );
void (*QA_func2)( void );
#endif

// check that threading works at all in the exe
void exe_threadfunc( void* private_data )
{
    if( private_data ) {
        QA_func2();
    } else {
        query_stack();
    }
}

void do_start_threads( int call_dll )
{
    int i;

    for( i = 0; i < NUM_THREADS; ++i ) {
        _beginthread( &exe_threadfunc, NULL, 8192, (void *)call_dll );
    }
}


int main( int argc, char **argv )
{
#ifdef QA_DLL_LOAD
    APIRET  rc;
    HMODULE hmod;
    char    load_err[260];
    char    dll_name[260];
    char    *last_bs;
#endif

    printf( "In EXE\n" );
    query_stack();
    /* Start threads from the main EXE. */
    do_start_threads( 0 );

#ifdef QA_DLL_LOAD
    /* Construct the full path from the EXE name, in case
     * the current directory isn't searched.
     */
    strcpy( dll_name, argv[0] );
    _strupr( dll_name );
    last_bs = strrchr( dll_name, '\\' );
    if( last_bs ) {
        last_bs++;
        strcpy( last_bs, "TDLL32_3.DLL" );
    }

    rc = DosLoadModule( load_err, sizeof( load_err ), dll_name, &hmod );
    if( rc ) {
        printf( "Failed to load DLL (rc=%d, reason: %s)!\n", rc, load_err );
        return( 1 );
    }

    rc = DosQueryProcAddr( hmod, 0, "QA_func1_", (PFN*)&QA_func1 );
    if( rc ) {
        printf( "Failed to query function 1 address (rc=%d)!\n", rc );
        return( 2 );
    }

    rc = DosQueryProcAddr( hmod, 0, "QA_func2_", (PFN*)&QA_func2 );
    if( rc ) {
        printf( "Failed to query function 2 address (rc=%d)!\n", rc );
        return( 2 );
    }
    printf( "QA_func1 at %P, QA_func2 at %P\n", QA_func1, QA_func2 );

#endif

    /* Start more threads from the EXE and call into DLL. */
    printf( "EXE calling into DLL\n" );
    QA_func2();
    do_start_threads( 1 );

    sleep( 1 /* second */); // Let'em die

    /* Start threads from the DLL. */
    QA_func1();

#ifdef QA_DLL_LOAD
    // Do *not* unload the DLL as it may still be executing
    // DosFreeModule( hmod );
#endif

    return( 0 );
}

#endif /* QA_MAKE_EXE */
