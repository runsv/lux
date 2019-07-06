/*
 * generic syscalls common to ALL posix platforms
 */

/* check whether a given directory path is a mountpoint */
static int Lmountpoint ( lua_State * const L )
{
  const char * const mp = luaL_checkstring ( L, 1 ) ;

  if ( mp && * mp ) {
    lua_pushboolean ( L, is_mount_point ( mp ) ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 1, "mount point path required" ) ;
}

/* wrapper function for the u(n)mount(2) syscall */
static int Sunmount ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;

    for ( i = 1 ; n >= i ; ++ i ) {
      const char * const mp = luaL_checkstring ( L, i ) ;

      if ( mp && * mp ) {
#if defined (OSbsd)
        if ( unmount ( mp ) ) { return rep_err ( L, "unmount", errno ) ; }
#else
        if ( umount ( mp ) ) { return rep_err ( L, "umount", errno ) ; }
#endif
      } else {
        return luaL_argerror ( L, i, "not a mount point path" ) ;
      }
    }

    return 0 ;
  }

  return luaL_error ( L, "mount point paths required" ) ;
}

