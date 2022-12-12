/*
 * lua interpreter
 *
 * public domain code
 */

#include "common.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define LUX_VERSION "0.0.1.0"

static const char * progname = NULL ;

static void cannot ( const char * pname, const char * msg, const int e )
{
  if ( pname && * pname ) {
    ; /* ok */
  } else {
    pname = "runlua" ;
  }

  (void) fprintf ( stderr, "%s:\tcannot %s:\n", pname, msg ) ;

  if ( 0 < e ) {
    (void) fprintf ( stderr, "\t%s\n", strerror ( e ) ) ;
  }

  (void) fflush ( NULL ) ;
  exit ( 111 ) ;
}

static int clear_stack ( lua_State * const L )
{
  const int i = lua_gettop ( L ) ;

  if ( 0 < i ) { lua_pop ( L, i ) ; }

  return i ;
}

/* message handler function used by lua_pcall() */
static int errmsgh ( lua_State * const L )
{
  const char * msg = lua_tostring ( L, 1 ) ;

  /* error object is no string or number ? */
  if ( NULL == msg ) {
    /* does it have a metamethod that produces a string ? */
    if ( luaL_callmeta ( L, 1, "__tostring" ) &&
      LUA_TSTRING == lua_type ( L, -1 ) )
    {
      /* this is the error message */
      return 1 ;
    } else {
      msg = lua_pushfstring ( L,
        "(error object is a %s value)",
        luaL_typename ( L, 1 ) ) ;
    }
  }

  /* append a standard traceback */
  luaL_traceback ( L, L, msg, 1 ) ;
  return 1 ;
}

/* create and set up a new Lua state (i. e. interpreter) */
static lua_State * new_vm ( void )
{
  lua_State * L = NULL ;

  while ( NULL == ( L = luaL_newstate () ) ) {
    do_sleep ( 5, 0 ) ;
  }

  if ( L ) {
    /* open all standard Lua libs */
    luaL_openlibs ( L ) ;

    /* register the above module */
    regMod ( L, "ux" ) ;

    /* useful global info variables */
    lua_pushboolean ( L, 1 ) ;
    lua_setglobal ( L, "_PESI_STANDALONE" ) ;

#if defined (OS)
    (void) lua_pushliteral ( L, OS ) ;
#else
    (void) lua_pushliteral ( L, "Unix" ) ;
#endif
    lua_setglobal ( L, "OSNAME" ) ;

    /* set version information */
    (void) lua_pushliteral ( L,
      "lux version " LUX_VERSION " for " LUA_VERSION " ("
#if defined (OS)
      OS ")"
#else
      "Unix)"
#endif
      " compiled " __DATE__ " " __TIME__
      ) ;
    lua_setglobal ( L, "_RUNLUA" ) ;

#ifdef SUPERVISORD
    (void) lua_pushliteral ( L,
      "pesi supervise version " LUX_VERSION " for " LUA_VERSION " ("
# if defined (OS)
      OS ")"
# else
      "Unix)"
# endif
      " compiled " __DATE__ " " __TIME__
      ) ;
    lua_setglobal ( L, "_SUPERVISE" ) ;
#endif
  }

  return L ;
}

static int report ( lua_State * const L, const char * const pname, const int r,
  const char * script )
{
  if ( LUA_OK != r ) {
    /* some error(s) ocurred while compiling or running Lua code */
    const char * msg = NULL ;

    if ( script && * script ) {
      (void) fprintf ( stderr,
        "%s:\tError in script \"%s\":\n"
        , pname, script ) ;
    } else {
      (void) fprintf ( stderr, "%s:\tERROR:\n", pname ) ;
    }

    switch ( r ) {
      case LUA_ERRSYNTAX :
        (void) fprintf ( stderr, "%s:\tsyntax error\n", pname ) ;
        break ;
      case LUA_ERRMEM :
        (void) fprintf ( stderr, "%s:\tout of memory\n", pname ) ;
        break ;
      /*
      case LUA_ERRGCMM :
        (void) fprintf ( stderr,
          "%s:\terror while running a __gc metamethod\n", pname ) ;
        break ;
      */
      case LUA_ERRFILE :
        (void) fprintf ( stderr, "%s:\tfile access error:\n", pname ) ;

        if ( script && * script ) {
          (void) fprintf ( stderr,
            "%s:\tcannot open and/or read script file \"%s\"\n"
            , pname, script ) ;
        } else {
          (void) fprintf ( stderr,
            "%s:\tcannot open and/or read script file\n", pname ) ;
        }
        break ;
      default :
        (void) fprintf ( stderr, "%s:\tunknown error\n", pname ) ;
        break ;
    }

    msg = lua_tostring ( L, -1 ) ;

    if ( msg && * msg ) {
      (void) fprintf ( stderr,
        "%s:\terror message:\n\t%s\n\n", pname, msg ) ;
    }

    lua_pop ( L, 1 ) ;
    (void) fflush ( stderr ) ;
  }

  return r ;
}

static int push_args ( lua_State * const L, const int argc, char ** argv )
{
  int i, n = lua_gettop ( L ) ;

  for ( i = 0 ; argc > i ; ++ i ) {
    if ( argv [ i ] && argv [ i ] [ 0 ] ) {
      (void) lua_pushstring ( L, argv [ i ] ) ;
    }
  }

  n = n - lua_gettop ( L ) ;
  return n ;
}

static void show_version ( const char * const pname )
{
  (void) fputs ( pname, stdout ) ;
  (void) puts (
    ":\tlux v" LUX_VERSION " for " LUA_VERSION " ("
#if defined (OS)
    OS ")"
#else
    "Unix)"
#endif
    " compiled " __DATE__ " " __TIME__
    ) ;
  (void) puts ( LUA_COPYRIGHT ) ;
  (void) fflush ( stdout ) ;
}

static void show_usage ( const char * const pname )
{
  (void) fprintf ( stderr,
    "\nusage:\t%s -blah\n\n"
    , pname ) ;
  (void) fflush ( stderr ) ;
}

static int check4script ( const char * const path, const char * const pname )
{
  int i = 0 ;
  errno = 0 ;

  if ( is_fnr ( path ) ) {
    return 0 ;
  }

  i = ( 0 < errno ) ? errno : 0 ;
  (void) fprintf ( stderr,
    "%s\tcannot read script \"%s\"\n"
    , pname, path ) ;
  (void) fflush ( stderr ) ;
  cannot ( pname, "open script for reading", errno ) ;

  return -1 ;
}

/* set process resource (upper) limits */
static int set_rlimits ( const uid_t u )
{
  struct rlimit rlim ;

  (void) getrlimit ( RLIMIT_CORE, & rlim ) ;
  rlim . rlim_cur = 0 ;

  if ( 0 == u ) { rlim . rlim_max = RLIM_INFINITY ; }

  /* set the SOFT (!!) default upper limit of coredump sizes to zero
   * (i. e. disable coredumps). can raised again later in child/sub
   * processes as needed.
   */
  return setrlimit ( RLIMIT_CORE, & rlim ) ;
}

static int writer ( lua_State * const L, const void * p, const size_t s, void * u )
{
  (void) L ;

  if ( p && u && ( 0 < s ) ) {
    return 1 > fwrite ( p, s, 1, (FILE *) u ) ;
  }

  return -1 ;
}

/* command line flags */
enum {
  FLAG_STMT		= 0x01,
  FLAG_INTER		= 0x02,
  FLAG_SCR		= 0x01,
  FLAG_PARSE		= 0x02,
  FLAG_SAVEBC		= 0x04,
  FLAG_STRIP		= 0x08,
} ;

/* load and compile a given Lua script to bytecode */
static int compile_script ( lua_State * const L,
  const char * const pname, const unsigned long int f,
  const char * const script, const char * output, const int narg )
{
  int i = luaL_loadfile ( L, script ) ;

  if ( LUA_OK != i ) {
    (void) fprintf ( stderr,
      "%s:\tcannot load and compile script \"%s\"\n"
      , pname, script ) ;
    return report ( L, pname, i, script ) ;
  } else if ( FLAG_PARSE & f ) {
    return i ;
  } else if ( LUA_OK == i ) {
    FILE * fp = NULL ;

    output = ( output && * output ) ? output : "out.lc" ;
    fp = fopen ( output, "wb" ) ;
    /* fp = fp ? fp : stdout ;	*/

    if ( NULL == fp ) {
      (void) fprintf ( stderr,
        "%s:\tcannot open output file \"%s\" for writing:\n\t%s\n"
        , pname, output, strerror ( errno ) ) ;
      return -1 ;
    }

    i = lua_dump ( L, writer, fp, ( FLAG_STRIP & f ) ? 1 : 0 ) ;

    if ( i ) {
      (void) fprintf ( stderr,
        "%s:\terror while writing bytecode to output file \"%s\"\n"
        , pname, output ) ;
    }

    if ( ferror ( fp ) ) {
    }

    if ( fclose ( fp ) ) {
      (void) fprintf ( stderr,
        "%s:\tcannot close script file \"%s\":\n\t%s\n"
        , pname, script, strerror ( errno ) ) ;
    }
  }

  (void) clear_stack ( L ) ;
  return i ;
}

/* load, compile and run a given chunk of Lua code */
static int run_chunk ( lua_State * const L,
  const char * const pname, const unsigned long int f,
  const char * const s, const int narg, int * rp )
{
  int i = 0 ;

  if ( ( FLAG_SCR & f ) && ( NULL == s ) ) {
    (void) fprintf ( stderr,
      "%s:\tno script given, will read Lua code from stdin\n"
      , pname ) ;
  }

  /* read and parse the Lua code from the given string or script (or stdin).
   * the result is stored as a Lua function on top of the stack
   * when successful.
   */
  i = ( FLAG_SCR & f ) ?
    luaL_loadfile ( L, s ) : luaL_loadstring ( L, s ) ;

  /* call the chunk if loading and compiling it succeeded */
  if ( LUA_OK == i ) {
    /* the chunk was successfully read, parsed, and its code
     * has been stored as a function on top of the Lua stack.
     */
    const int n = lua_gettop ( L ) - narg ;
    /* push our error message handler function onto the stack */
    lua_pushcfunction ( L, errmsgh ) ;
    lua_insert ( L, n ) ;
    i = lua_pcall ( L, narg, 1, n ) ;
    /* remove our message handler function from the stack */
    lua_remove ( L, n ) ;
  }

  if ( LUA_OK == i ) {
    /* success: parsing and executing the script succeeded.
     * check for an integer result
     */
    * rp = lua_isinteger ( L, -1 ) ? lua_tointeger ( L, -1 ) : 0 ;
  } else {
    if ( s && * s ) {
      (void) fprintf ( stderr,
        "%s:\tFailad to run script \"%s\":\n"
        , pname, s ) ;
    }

    (void) report ( L, pname, i, s ) ;
  }

  (void) clear_stack ( L ) ;
  return i ;
}

static void defsigs ( void )
{
  int i ;
  struct sigaction sa ;

  (void) memset ( & sa, 0, sizeof ( struct sigaction ) ) ;
  sa . sa_flags = SA_RESTART ;
  (void) sigemptyset ( & sa . sa_mask ) ;
  sa . sa_handler = SIG_DFL ;

  /* restore default swignal handling behaviour */
  for ( i = 1 ; NSIG > i ; ++ i ) {
    if ( SIGKILL != i && SIGSTOP != i ) {
      (void) sigaction ( i, & sa, NULL ) ;
    }
  }

  /* catch SIGCHLD */
  sa . sa_handler = pseudo_sighand ;
  (void) sigaction ( SIGCHLD, & sa, NULL ) ;
  /* unblock all signals */
  (void) sigprocmask ( SIG_SETMASK, & sa . sa_mask, NULL ) ;
}

static int imain ( const int argc, char ** argv )
{
  int i, j, k, n, r = 0 ;
  unsigned long int f = 0 ;
  const char * s = NULL ;
  const char * output = NULL ;
  const char * pname = NULL ;
  lua_State * L = NULL ;
  const uid_t myuid = getuid () ;
  extern int opterr, optind, optopt ;
  extern char * optarg ;

  /* set up some variables first */
  opterr = 1 ;
  pname = ( ( 0 < argc ) && argv && * argv && ** argv ) ? * argv : "runlua" ;
  progname = ( ( 0 < argc ) && argv && * argv && ** argv ) ? * argv : "runlua" ;

  /* drop possible orivileges we might have */
  (void) setegid ( getgid () ) ;
  (void) seteuid ( myuid ) ;

  /* secure file creation mask */
  (void) umask ( 00022 | ( 00077 & umask ( 00077 ) ) ) ;

  /* disable core dumps by setting the corresponding SOFT (!!)
   * limit to zero.
   * it can be raised again later (in child processes) as needed.
   */
  (void) set_rlimits ( myuid ) ;

  /* restore default dispostions for all signals */
  defsigs () ;

  /* create a new Lua VM (state) */
  errno = 0 ;
  L = new_vm () ;

  if ( NULL == L ) {
    if ( 0 < errno ) {
      perror ( "cannot create Lua VM" ) ;
    } else {
      (void) fputs ( pname, stderr ) ;
      (void) fputs ( ":\tcannot create Lua VM:\n\tNot enough memory\n"
        , stderr ) ;
    }

    (void) fflush ( NULL ) ;
    return 111 ;
  }

  luaL_checkversion ( L ) ;
  /* create some global helper variables in this VM */
  (void) lua_pushstring ( L, pname ) ;
  lua_setglobal ( L, "RUNLUA" ) ;

  /* create a new Lua table holding ALL command line args */
  lua_newtable ( L ) ;
  (void) lua_pushstring ( L, pname ) ;
  lua_rawseti ( L, -2, -1 ) ;

  for ( i = 0, j = 0 ; argc > i ; ++ i ) {
    if ( argv [ i ] && argv [ i ] [ 0 ] ) {
      (void) lua_pushstring ( L, argv [ i ] ) ;
      lua_rawseti ( L, -2, j ++ ) ;
    }
  }

  /* set the global name of this Lua table */
  lua_setglobal ( L, "ARGV" ) ;

  (void) clear_stack ( L ) ;
  /* parse the command line args to figure out what to do */
  while ( 0 < ( i = getopt ( argc, argv, ":cC:e:g:hHio:pqR:s:u:vVw:" ) ) ) {
    switch ( i ) {
      case 'c' :
        f |= FLAG_SAVEBC ;
        break ;
      case 'd' :
        f |= FLAG_STRIP ;
        break ;
      case 'i' :
        f |= FLAG_INTER ;
        break ;
      case 'p' :
        f |= FLAG_PARSE ;
        break ;
      case 'o' :
        if ( optarg && * optarg ) {
          output = optarg ;
        }
        break ;
      case 'C' :
        if ( optarg && * optarg && chdir ( optarg ) ) {
          perror ( "chdir failed" ) ;
          r = 111 ;
          goto fin ;
        }
        break ;
      case 'w' :
        if ( optarg && * optarg ) {
          char * str = strchr ( optarg, '.' ) ;
          j = k = 0 ;
          j = atoi ( optarg ) ;
          if ( str && * str ) { k = atoi ( str ) ; }
          j = ( 0 < j ) ? j : 0 ;
          k = ( 0 < k ) ? k : 0 ;
          if ( 0 < j || 0 < k ) { (void) delay ( j, k ) ; }
        }
        break ;
      case 'g' :
        if ( myuid ) { continue ; }
        else if ( optarg && * optarg ) {
          j = atoi ( optarg ) ;
          if ( 0 <= j && 0 != setegid ( j ) ) {
            perror ( "setegid failed" ) ;
            r = 111 ;
            goto fin ;
          }
        }
        break ;
      case 'u' :
        if ( myuid ) { continue ; }
        else if ( optarg && * optarg ) {
          j = atoi ( optarg ) ;
          if ( 0 <= j && 0 != seteuid ( j ) ) {
            perror ( "seteuid failed" ) ;
            r = 111 ;
            goto fin ;
          }
        }
        break ;
      case 'R' :
        if ( myuid ) { continue ; }
        else if ( optarg && * optarg ) {
          if ( chdir ( optarg ) || chroot ( optarg ) || chdir ( "/" ) ) {
            perror ( "chroot failed" ) ;
            r = 111 ;
            goto fin ;
          }
        }
        break ;
      case 'q' :
        break ;
      case 'e' :
        /* (Lua code) string to execute */
        if ( optarg && * optarg ) {
          f |= FLAG_STMT ;
          n = push_args ( L, argc, argv ) ;
          j = run_chunk ( L, pname, 0, optarg, ( 0 < n ) ? n : 0, & k ) ;
          if ( LUA_OK == j ) { r += k ; }
          else { r = j ? j : 1 ; goto fin ; }
        }
        break ;
      case 's' :
        /* script to execute */
        if ( optarg && * optarg ) {
          if ( check4script ( optarg, pname ) ) {
            r = 111 ;
            goto fin ;
          } else {
            s = optarg ;
          }
        }
        break ;
      case 'h' :
      case 'H' :
        show_version ( pname ) ;
        show_usage ( pname ) ;
        r = 0 ;
        goto fin ;
        break ;
      case 'v' :
      case 'V' :
        show_version ( pname ) ;
        r = 0 ;
        goto fin ;
        break ;
      case ':' :
        show_version ( pname ) ;
        (void) fprintf ( stderr,
          "\n%s:\tmissing argument for option \"-%c\"\n\n"
          , pname, optopt ) ;
        show_usage ( pname ) ;
        r = 100 ;
        goto fin ;
        break ;
      case '?' :
        show_version ( pname ) ;
        (void) fprintf ( stderr,
          "\n%s:\tunrecognized option \"-%c\"\n\n"
          , pname, optopt ) ;
        show_usage ( pname ) ;
        r = 100 ;
        goto fin ;
        break ;
      default :
        break ;
    }
  }

  n = k = 0 ;
  i = j = optind ;

  if ( 0 < i && argc > i ) {
    /* script to execute */
    if ( ( NULL == s ) && argv [ i ] && argv [ i ] [ 0 ] ) {
      s = argv [ i ] ;

      if ( check4script ( s, pname ) ) {
        r = 111 ;
        goto fin ;
      }
    }
  }

  if ( s && * s ) {
    (void) lua_pushstring ( L, s ) ;
    lua_setglobal ( L, "SCRIPT" ) ;
  }

  if ( ( FLAG_SAVEBC & f ) || ( FLAG_PARSE & f ) ) {
    if ( s && * s ) {
      n = push_args ( L, ( 0 < i && argc > i ) ? i : argc, argv ) ;
      r = compile_script ( L, pname, f, s, output, ( 0 < n ) ? n : 0 ) ;
    } else {
      r = 100 ;
      (void) fprintf ( stderr,
        "%s:\tno script to parse/compile\n", pname ) ;
      //goto fin ;
    }
  } else {
    n = push_args ( L, ( 0 < i && argc > i ) ? i : argc, argv ) ;
    i = run_chunk ( L, pname, FLAG_SCR & f, s, ( 0 < n ) ? n : 0, & j ) ;
    r = ( LUA_OK == i ) ? j : 1 ;
  }

  if ( ( FLAG_INTER & f ) && s && * s ) {
    if ( LUA_OK != i ) {
      (void) fprintf ( stderr,
        "%s:\tcould not run Lua script \"%s\"\n"
        , pname, s ) ;
    }

    (void) printf (
      "%s:\tentering interactive mode after running Lua script \"%s\"\n\n"
      , pname, s ) ;
    (void) fflush ( NULL ) ;
    (void) clear_stack ( L ) ;
    n = push_args ( L, ( 0 < i && argc > i ) ? i : argc, argv ) ;
    i = run_chunk ( L, pname, FLAG_SCR, NULL, ( 0 < n ) ? n : 0, & j ) ;
    r = ( LUA_OK == i ) ? j : 1 ;
  }

fin :
  (void) clear_stack ( L ) ;
  lua_close ( L ) ;
  (void) fflush ( NULL ) ;

  return r ;
}

int main ( const int argc, char ** argv )
{
  return imain ( argc, argv ) ;
}

