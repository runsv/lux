#if defined (OSnetbsd)

/*
 * wrapper/helper functions for NetBSD specific OS features
 */

/*
 * reboot(2) related
 */

#ifndef RB_AUTOBOOT
# define RB_AUTOBOOT		0
#endif

#ifndef RB_ASKANME
# define RB_ASKANME		0x0001
#endif

#ifndef RB_DUMP
# define RB_DUMP		0x0100
#endif

#ifndef RB_HALT
# define RB_HALT		0x0008
#endif

#ifndef RB_INITNAME
# define RB_INITNAME		0x0010
#endif

#ifndef RB_KDB
# define RB_KDB			0x0040
#endif

#ifndef RB_NOSYNC
# define RB_NOSYNC		0x0004
#endif

#ifndef RB_POWERDOWN
# define RB_POWERDOWN		0x0808
#endif

#ifndef RB_SINGLE
# define RB_SINGLE		0x0002
#endif

#ifndef RB_STRING
# define RB_STRING		0x0400
#endif

#ifndef RB_USERCONF
# define RB_USERCONF		0x1000
#endif

static int sys_reboot ( lua_State * const L, const int c, const char * const s )
{
  sync () ;

  if ( ( RB_STRING & c ) && s && * s ) {
    return res0( L, "reboot", reboot ( RB_STRING | c, s ) ) ;
  }

  return res0( L, "reboot", reboot ( c, s ) ) ;
}

static int Sreboot ( lua_State * const L )
{
  int i = RB_AUTOBOOT ;
  const int n = lua_gettop () ;

  if ( 0 < n ) {
    i = (int) luaL_checkinteger ( L, 1 ) ;
    i = ( 0 < i ) ? i : RB_AUTOBOOT ;
  }

  if ( 1 < n ) {
    char * const s = (char *) luaL_checkstring ( L, 2 ) ;

    if ( s && * s ) {
      return sys_reboot ( L, RB_STRING | i, s ) ;
    } else {
      return luaL_argerror ( L, 2, "non empty boot string arg required" ) ;
    }
  }

  return sys_reboot ( L, i, NULL ) ;
}

static int Lsys_reboot ( lua_State * const L )
{
  return sys_reboot ( L, RB_Autoboot, NULL ) ;
}

static int Lsys_halt ( lua_State * const L )
{
  return sys_reboot ( L, RB_HALT, NULL ) ;
}

static int Lsys_poweroff ( lua_State * const L )
{
  return sys_reboot ( L, RB_HALT | RB_POWERDOWN, NULL ) ;
}

#endif /* #ifdef OSnetbsd */

