/*
 * Lua bindings for libeinfo,
 * OpenRC's enhanced information output library
 * used to print (colorful where possible) informational output
 * to terminals or to syslog.
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

/* prototypes functions exported by the einfo lib */
#include <einfo.h>

#define EVERSION	"0.00.04"

/* indents the einfo lines.
 * for each indent you should outdent when done.
 */
static int Leindent ( lua_State * L )
{
  eindent () ;
  return 0 ;
}

static int Leindentv ( lua_State * L )
{
  eindentv () ;
  return 0 ;
}

static int Leoutdent ( lua_State * L )
{
  eoutdent () ;
  return 0 ;
}

static int Leoutdentv ( lua_State * L )
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
static int Leinfo ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfo ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leinfon ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfon ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leinfov ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfov ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leinfovn ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) einfovn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lewarn ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lewarnn ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lewarnv ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnv ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lewarnvn ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ewarnvn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lewarnx ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    ewarnx ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leerror ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) eerror ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leerrorn ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) eerrorn ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Leerrorx ( lua_State * L )
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
static int Lebegin ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ebegin ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

static int Lebeginv ( lua_State * L )
{
  const char * fmt = luaL_checkstring ( L, 1 ) ;

  if ( fmt && * fmt ) {
    (void) ebeginv ( fmt ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

/*
static int Lebeginvn ( lua_State * L )
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
static int Leend ( lua_State * L )
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

static int Leendv ( lua_State * L )
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
static int Leendvn ( lua_State * L )
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

static int Lewend ( lua_State * L )
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

static int Lewendv ( lua_State * L )
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
static int Lewendvn ( lua_State * L )
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
static int Leend2 ( lua_State * L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) eend ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int Leendv2 ( lua_State * L )
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
static int Leendvn2 ( lua_State * L )
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

static int Lewend2 ( lua_State * L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  const char * fmt = luaL_checkstring ( L, 2 ) ;

  if ( fmt && * fmt ) {
    (void) ewend ( i, fmt ) ;
    return 0 ;
  }

  return luaL_error ( L, "non empty string and integer required" ) ;
}

static int Lewendv2 ( lua_State * L )
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
static int Lewendvn2 ( lua_State * L )
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
static int Leprefix ( lua_State * L )
{
  const char * pfx = luaL_checkstring ( L, 1 ) ;

  if ( pfx && * pfx ) {
    eprefix ( pfx ) ;
    return 0 ;
  }

  return luaL_argerror ( L, 1, "non empty string required" ) ;
}

/* ebracket allows you to specifiy the position, color and message */
static int Lebracket ( lua_State * L )
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
static int Lelog ( lua_State * L )
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
  { "eindent",			Leindent	},
  { "eindentv",			Leindentv	},
  { "eoutdent",			Leoutdent	},
  { "eoutdentv",		Leoutdentv	},
  { "einfo",			Leinfo		},
  { "einfon",			Leinfon		},
  { "einfov",			Leinfov		},
  { "einfovn",			Leinfovn	},
  { "ewarn",			Lewarn		},
  { "ewarnn",			Lewarnn		},
  { "ewarnv",			Lewarnv		},
  { "ewarnn",			Lewarnn		},
  { "ewarnvn",			Lewarnvn	},
  { "ewarnx",			Lewarnx		},
  { "eerror",			Leerror		},
  { "eerrorn",			Leerrorn	},
  { "eerrorx",			Leerrorx	},
  { "ebegin",			Lebegin		},
  { "ebeginv",			Lebeginv	},
  { "eend",			Leend		},
  { "eendv",			Leendv		},
  { "ewend",			Lewend		},
  { "ewendv",			Lewendv		},
  { "eend2",			Leend2		},
  { "eendv2",			Leendv2		},
  { "ewend2",			Lewend2		},
  { "ewendv2",			Lewendv2	},
  { "eprefix",			Leprefix	},
  { "ebracket",			Lebracket	},
  { "elog",			Lelog		},
  { "esyslog",			Lelog		},
/*
  { "ebeginvn",			Lebeginvn	},
  { "eendvn",			Leendvn		},
  { "eendvn2",			Leendvn2	},
  { "ewendvn",			Lewendvn	},
  { "ewendvn2",			Lewendvn2	},
 */

  /* last sentinel entry to mark the end of this array */
  { NULL,                       NULL		}

  /* end of struct array */
} ;

static int openMod ( lua_State * L )
{
  luaL_newlib ( L, efunc ) ;
  (void) lua_pushliteral ( L, "_Version" ) ;
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

int luaopen_modeinfo ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_einfomod ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_luaeinfo ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_leinfo ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_meinfo ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_ei ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_ein ( lua_State * L )
{
  return openMod ( L ) ;
}

int luaopen_einf ( lua_State * L )
{
  return openMod ( L ) ;
}

