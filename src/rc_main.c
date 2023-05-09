/*
 * public domain
 */

/*
#include "common.h"
*/
#include "version.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "os_aux.c"
#include "rc_aux.c"
#include "rc_utils.c"

/* OS specific functions */
#if defined (OSbsd)
#endif

#if defined (OSLinux)
  /* Linux specific functions */
#elif defined (OSfreebsd)
#elif defined (OSsolaris) || defined (OSsunos5)
#endif

/* constant with an unique address to use as key in the Lua registry */
static const char Key = 'K' ;

/* this procedure exports important posix constants to Lua */
static void add_const ( lua_State * const L )
{
  /* create a new (sub)table holding the constants (names as keys)
   * and their values
   */
  lua_newtable ( L ) ;

  /* add the constants now to that (sub)table */
  /* signal number constants */
  L_ADD_CONST( L, SIGUSR2 )
  L_ADD_CONST( L, SIGCONT )
  L_ADD_CONST( L, SIGSTOP )
  L_ADD_CONST( L, SIGKILL )
  L_ADD_CONST( L, SIGCHLD )
  L_ADD_CONST( L, SIGTRAP )
  L_ADD_CONST( L, SIGQUIT )
  L_ADD_CONST( L, SIGABRT )
  L_ADD_CONST( L, SIGPIPE )
  L_ADD_CONST( L, SIGTSTP )
  L_ADD_CONST( L, SIGTTIN )
  L_ADD_CONST( L, SIGTTOU )
  L_ADD_CONST( L, SIGSEGV )
  L_ADD_CONST( L, SIGFPE )
  L_ADD_CONST( L, SIGILL )

  /* add the subtable to the main module table.
   * the subtable is now on top of the Lua stack,
   * while the main module table should be at the stack
   * position directly below it (i. e. at index -2).
   */
  lua_setfield ( L, -2, "Constants" ) ;

  /* end of function add_const */
}

/* this array contains the needed Lua wrapper functions for posix syscalls */
static const luaL_Reg sys_func [ ] =
{
  /* begin of struct array for exported Lua C functions */

  /* functions imported from "rc_net.c": */
  /*
  { "if_up",			Lif_up		},
  { "if_down",			Lif_down	},
  { "route_add_netmask",	Lroute_add_netmask	},
  { "route_add_defgw",		Lroute_add_defgw	},
  */
  /* end of imported functions from "rc_net.c" */

  /* functions imported from "rc_utils.c" : */
  { "init_urandom",		x_init_urandom	},
  /* end of imported functions from "rc_utils.c" */

  /* last sentinel entry to mark the end of this array */
  { NULL,			NULL }

  /* end of struct array sys_func [] */
} ;

/* open function for Lua to open this very module/library */
static int openMod ( lua_State * const L )
{
  /*
  struct utsname uts ;
  extern char ** environ ;
  */

  /* create a metatable for directory iterators */
  /*
  (void) dir_create_meta ( L ) ;
   */
  /* add posix wrapper functions to module table */
  luaL_newlib ( L, sys_func ) ;
  /* add posix constants to module */
  add_const ( L ) ;

  /* set version information */
  (void) lua_pushliteral ( L, "_VERSION" ) ;
  (void) lua_pushliteral ( L,
    "lux version " LUX_VERSION " for " LUA_VERSION " ("
#if defined (OS)
    OS ")"
#else
    "Unix)"
#endif
    " compiled " __DATE__ " " __TIME__
    ) ;
  lua_rawset ( L, -3 ) ;

  /* save the current environment into a subtable */
  /* better not, let the user call the coresponding function
   * when needed instead. this saves some memory.
  if ( environ && * environ && ** environ ) {
    int i, j ;
    char * s = NULL ;

    lua_newtable ( L ) ;

    for ( i = 0 ; environ [ i ] && environ [ i ] [ 0 ] ; ++ i ) {
      s = environ [ i ] ;
      for ( j = 0 ; '\0' != s [ j ] && '=' != s [ j ] ; ++ j ) ;
      if ( 0 < j && '=' == s [ j ] ) {
        (void) lua_pushlstring ( L, s, j ) ;
        if ( '\0' == s [ 1 + j ] ) {
          (void) lua_pushliteral ( L, "" ) ;
        } else {
          (void) lua_pushstring ( L, 1 + j + s ) ;
        }
        lua_rawset ( L, -3 ) ;
      }
    }

    lua_setfield ( L, -2, "ENV" ) ;
  }
  */

  /* set some useful vars as we are at it */
  (void) lua_pushliteral ( L, "OSNAME" ) ;
  (void) lua_pushliteral ( L,
#if defined (OS)
    OS
#else
    "Unix"
#endif
    ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

void regMod ( lua_State * const L, const char * const name )
{
  (void) openMod ( L ) ;
  lua_setglobal ( L, ( name && * name ) ? name : "sys" ) ;
}

/* used as as Lua module (shared object), so luaopen_* functions
 * are needed, define them here.
 */
int luaopen_rc ( lua_State * const L )
{
  return openMod ( L ) ;
}

