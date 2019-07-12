/*
 * Wrapper and helper functions for (u)mount(2), swapo(n,ff)(2),
 * and file system related syscalls.
 */

/* helper functions that pass the result of successful stat(v)fs() calls
 * back to Lua
 */
static int res_statvfs ( lua_State * const L, struct statvfs * const stp )
{
  lua_newtable ( L ) ;

  (void) lua_pushliteral ( L, "fsid" ) ;
  lua_pushinteger ( L, stp -> f_fsid ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "flag" ) ;
  lua_pushinteger ( L, stp -> f_flag ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "namemax" ) ;
  lua_pushinteger ( L, stp -> f_namemax ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "bsize" ) ;
  lua_pushinteger ( L, stp -> f_bsize ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "frsize" ) ;
  lua_pushinteger ( L, stp -> f_frsize ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "blocks" ) ;
  lua_pushinteger ( L, stp -> f_blocks ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "bfree" ) ;
  lua_pushinteger ( L, stp -> f_bfree ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "bavail" ) ;
  lua_pushinteger ( L, stp -> f_bavail ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "files" ) ;
  lua_pushinteger ( L, stp -> f_files ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "ffree" ) ;
  lua_pushinteger ( L, stp -> f_ffree ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "favail" ) ;
  lua_pushinteger ( L, stp -> f_favail ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

static int res_statfs ( lua_State * const L, struct statfs * const stp )
{
  lua_newtable ( L ) ;

  return 1 ;
}

/* bindings for the (f)stat(v)fs() syscalls */
static int Sstatvfs ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    struct statvfs st ;

    if ( statvfs ( path, & st ) ) {
      const int e = errno ;
      lua_pushnil ( L ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    } else {
      return res_statvfs ( L, & st ) ;
    }
  }

  return luaL_argerror ( L, 1, "invalid path" ) ;
}

static int Sfstatvfs ( lua_State * const L )
{
  const int fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= fd ) {
    struct statvfs st ;

    if ( fstatvfs ( fd, & st ) ) {
      const int e = errno ;
      lua_pushnil ( L ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    } else {
      return res_statvfs ( L, & st ) ;
    }
  }

  return luaL_argerror ( L, 1, "invalid fd" ) ;
}

static int Sstatfs ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    struct statfs st ;

    if ( statfs ( path, & st ) ) {
      const int e = errno ;
      lua_pushnil ( L ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    } else {
      return res_statfs ( L, & st ) ;
    }
  }

  return luaL_argerror ( L, 1, "invalid path" ) ;
}

static int Sfstatfs ( lua_State * const L )
{
  const int fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= fd ) {
    struct statfs st ;

    if ( fstatfs ( fd, & st ) ) {
      const int e = errno ;
      lua_pushnil ( L ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    } else {
      return res_statfs ( L, & st ) ;
    }
  }

  return luaL_argerror ( L, 1, "invalid fd" ) ;
}

static int stat_parent_dira ( const char * const path, const size_t s,
  struct stat * const stp )
{
  size_t i = 0 ;
#if defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && ! defined (__STDC_NO_VLA__)
  char buf [ 4 + s ] ;
#else
  char * buf = (char *) alloca ( 4 + s ) ;
#endif

  for ( i = 0 ; ( s > i + 4 ) && '\0' != path [ i ] ; ++ i ) {
    buf [ i ] = path [ i ] ;
  }

  buf [ i ++ ] = '/' ;
  buf [ i ++ ] = '.' ;
  buf [ i ++ ] = '.' ;
  buf [ i ] = '\0' ;

  return lstat ( buf, stp ) ;
}

#define MP_PATH_LEN		1000

static int stat_parent_dir ( const char * const path, struct stat * const stp )
{
  size_t i = 0 ;
  char buf [ 4 + MP_PATH_LEN ] = { 0 } ;

  for ( i = 0 ; ( MP_PATH_LEN > i + 4 ) && '\0' != path [ i ] ; ++ i ) {
    buf [ i ] = path [ i ] ;
  }

  buf [ i ++ ] = '/' ;
  buf [ i ++ ] = '.' ;
  buf [ i ++ ] = '.' ;
  buf [ i ] = '\0' ;

  return lstat ( buf, stp ) ;
}

/* see if a given directory path is a mount point */
static int is_mount_point ( const char * const path )
{
  struct stat st ;

  if ( lstat ( path, & st ) || ! S_ISDIR( st . st_mode ) ) {
    return 0 ;
  } else if (
#if defined (OSLinux)
    0 < st . st_ino && 4 > st . st_ino
#else
    /* this is traditional, and what FreeBSD/PC-BSD does.
     * on-disc volumes on Linux mostly do this, too.
     */
    2 == st . st_ino
#endif
    )
  {
    return 1 ;
  } else {
    struct stat st2 ;
    const size_t s = strlen ( path ) ;

    if ( MP_PATH_LEN < s && 0 != stat_parent_dira ( path, s, & st2 ) ) {
      return 0 ;
    } else if ( 0 < s && MP_PATH_LEN >= s &&
      0 != stat_parent_dir ( path, & st2 ) )
    {
      return 0 ;
    } 

    return S_ISDIR( st2 . st_mode ) && (
      st2 . st_dev != st . st_dev ||
      /* this is true for the root dir "/" */
      st2 . st_ino == st . st_ino ) ;
  }

  return 0 ;
}

#undef MP_PATH_LEN

/* check whether a given directory path is a mountpoint */
static int Lmountpoint ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    lua_pushboolean ( L, is_mount_point ( path ) ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 1, "mount point path required" ) ;
}

