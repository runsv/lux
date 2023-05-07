/*
 * Lua bindings for libeinfo,
 * OpenRC's enhanced information output library
 * used to print (colorful where possible) informational output
 * to terminals or to syslog.
 *
 * public domain
 */

#include "feat.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

/* Lua API provided by liblua */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* function prototypes exported by the einfo lib */
#include <einfo.h>

#define EVERSION	"0.00.04"

/* indents the einfo lines.
 * for each indent you should outdent when done.
 */
static int x_eindent ( lua_State * const L )
{
  eindent () ;
  return 0 ;
}

static int x_eindentv ( lua_State * const L )
{
  eindentv () ;
  return 0 ;
}

static int x_eoutdent ( lua_State * const L )
{
  eoutdent () ;
  return 0 ;
}

static int x_eoutdentv ( lua_State * const L )
{
  eoutdentv () ;
  return 0 ;
}

/* display informational messages.
 *
 * The einfo family of functions display messages in a consistent manner
 * across applications. Basically they prefix the message with
 * " * ". If the terminal can handle color then we color the * based on
 * the command used. Otherwise we are identical to the printf function.
 *
 * - einfo  - green
 * - ewarn  - yellow
 * - eerror - red
 *
 * The n suffix denotes that no new line should be printed.
 * The v suffix means only print if EINFO_VERBOSE is yes.
 * The x suffix means function will exit() returning failure.
 */
static int x_einfo ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfo ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_einfon ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfon ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_einfov ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfov ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_einfovn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfovn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ewarn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ewarnn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ewarnv ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnv ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ewarnvn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnvn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ewarnx ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    ewarnx ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_eerror ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) eerror ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_eerrorn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) eerrorn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_eerrorx ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    eerrorx ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

/* display informational messages that may take some time.
 * similar to einfo, but we add ... to the end of the message.
 */
static int x_ebegin ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ebegin ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int x_ebeginv ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ebeginv ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

/*
static int x_ebeginvn ( lua_State * const L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ebeginvn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}
*/

/* (e)end an ebegin.
 * if you ebegin, you should eend also.
 * eend places [ ok ] or [ !! ] at the end of the terminal line depending on
 * retval (0 or ok, anything else for !!).
 */
static int x_eend ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eend ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int x_eendv ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eendv ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

/*
static int x_eendvn ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eendvn ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}
*/

static int x_ewend ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewend ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int x_ewendv ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewendv ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

/*
static int x_ewendvn ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewendvn ( i, fmt ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}
*/

/* the following e*end*2 functions don't return their first
 * (Lua) integer argument to the caller.
 */
static int x_eend2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eend ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int x_eendv2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eendv ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

/*
static int x_eendvn2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eendvn ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}
*/

static int x_ewend2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewend ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int x_ewendv2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewendv ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

/*
static int x_ewendvn2 ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewendvn ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}
*/

/* prefix each einfo line with something */
static int x_eprefix ( lua_State * const L )
{
  const char * pfx = luaL_checkstring ( L, 1 ) ;

  if ( pfx && * pfx ) {
    eprefix ( pfx ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

/* ebracket allows you to specifiy the position, color and message */
static int x_ebracket ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  int c = luaL_checkinteger ( L, 2 ) ;
  const char * msg = luaL_checkstring ( L, 3 ) ;

  if ( msg && * msg ) {
    c = ( 0 < c && 6 >= c ) ? c : 1 ;
    ebracket ( i, c, msg ) ;
    return 0 ;
  }

  return luaL_error ( L, "2 integers and non empty string required" ) ;
}

/* write something to syslog */
static int x_elog ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    elog ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "integer and non empty string required" ) ;
}

/* struct array that holds the exported Lua C functions and their names */
static const luaL_Reg efunc [ ] = {
  /* begin of struct array */
  { "eindent",			x_eindent	},
  { "eindentv",			x_eindentv	},
  { "eoutdent",			x_eoutdent	},
  { "eoutdentv",		x_eoutdentv	},
  { "einfo",			x_einfo		},
  { "einfon",			x_einfon	},
  { "einfov",			x_einfov	},
  { "einfovn",			x_einfovn	},
  { "ewarn",			x_ewarn		},
  { "ewarnn",			x_ewarnn	},
  { "ewarnv",			x_ewarnv	},
  { "ewarnn",			x_ewarnn	},
  { "ewarnvn",			x_ewarnvn	},
  { "ewarnx",			x_ewarnx	},
  { "eerror",			x_eerror	},
  { "eerrorn",			x_eerrorn	},
  { "eerrorx",			x_eerrorx	},
  { "ebegin",			x_ebegin	},
  { "ebeginv",			x_ebeginv	},
  { "eend",			x_eend		},
  { "eendv",			x_eendv		},
  { "ewend",			x_ewend		},
  { "ewendv",			x_ewendv	},
  { "eend2",			x_eend2		},
  { "eendv2",			x_eendv2	},
  { "ewend2",			x_ewend2	},
  { "ewendv2",			x_ewendv2	},
  { "eprefix",			x_eprefix	},
  { "ebracket",			x_ebracket	},
  { "elog",			x_elog		},
  { "esyslog",			x_elog		},
/*
  { "ebeginvn",			x_ebeginvn	},
  { "eendvn",			x_eendvn	},
  { "eendvn2",			x_eendvn2	},
  { "ewendvn",			x_ewendvn	},
  { "ewendvn2",			x_ewendvn2	},
 */

  /* last sentinel entry to mark the end of this array */
  { NULL,                       NULL		}

  /* end of struct array */
} ;

static int openMod ( lua_State * L )
{
  luaL_newlib ( L, efunc ) ;
  (void) lua_pushliteral ( L, "_VERSION" ) ;
#ifdef EVERSION
  (void) lua_pushliteral ( L, EVERSION ) ;
#else
  (void) lua_pushliteral ( L, "0.00.04" ) ;
#endif
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

int luaopen_einfo ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_ei ( lua_State * L )
{
  return openMod ( L ) ;
}

