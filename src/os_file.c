/*
 * wrapper functions to file related syscalls that operate on file names
 */

#define DIR_METATABLE "Directory Metatable"
#define LOCK_METATABLE "Lock Metatable"

typedef struct dir_data_s {
  char closed ;
  DIR * dirp ;
} dir_data_t ;

/* TODO: add support for (f)utimes */

/* call sync () */
static int u_sync ( lua_State * const L )
{
  sync () ;
  return 0 ;
}

static int u_fsync ( lua_State * const L )
{
  return res_bool_zero ( L, fsync ( luaL_checkinteger ( L, 1 ) ) ) ;
}

static int u_fdatasync ( lua_State * const L )
{
  return res_bool_zero ( L, fdatasync ( luaL_checkinteger ( L, 1 ) ) ) ;
}

/* wrapper function to dirname */
static int u_dirname ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    (void) lua_pushstring ( L, dirname ( (char *) path ) ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 1, "path required" ) ;
}

/* wrapper function to basename */
static int u_basename ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    (void) lua_pushstring ( L, basename ( (char *) path ) ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 1, "path required" ) ;
}

/* basename function using strrchr */
static int l_base_name ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    char * str = strrchr ( path, '/' ) ;

    if ( str && ( '/' == * str ) ) { ++ str ; }
    if ( str && * str ) {
      (void) lua_pushstring ( L, str ) ;
    } else {
      (void) lua_pushstring ( L, path ) ;
    }

    return 1 ;
  }

  return luaL_argerror ( L, 1, "path required" ) ;
}

static int l_dir_name ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    char * str = strrchr ( path, '/' ) ;

    if ( str && ( '/' == * str ) ) { * str = '\0' ; }
    (void) lua_pushstring ( L, path ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 1, "path required" ) ;
}

/* does a given path end with a slash ? */
static int Lends_with_slash ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    lua_pushboolean ( L, ends_with_slash ( path ) ) ;
    return 1 ;
  }

  return luaL_error ( L, "path required" ) ;
}

/* wrapper function for realpath(3) */
static int Srealpath ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    char * rp = realpath ( path, NULL ) ;

    if ( rp ) {
      (void) lua_pushstring ( L, rp ) ;
      free ( rp ) ;
      return 1 ;
    } else {
      const int e = errno ;
      return luaL_error ( L, "realpath( %s ) failed: %s (errno %d)",
        path, strerror ( e ), e ) ;
    }
  }

  return luaL_error ( L, "path required" ) ;
}

/* wrapper to mkdir(2) */
static int u_mkdir ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  mode_t m = 007777 & (lua_Unsigned) luaL_optinteger ( L, 2, 00700 ) ;
  m |= 00700 ;

  if ( path && * path ) {
    return res_bool_zero ( L, mkdir ( path, m ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid dir path" ) ;
}

/* wrapper to mkfifo(3) */
static int u_mkfifo ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  mode_t m = 007777 & (lua_Unsigned) luaL_optinteger ( L, 2, 00600 ) ;
  m |= 00600 ;

  if ( path && * path ) {
    return res_bool_zero ( L, mkfifo ( path, m ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid filename" ) ;
}

static int l_mkpath ( lua_State * const L )
{
  size_t s = 0 ;
  const char * path = luaL_checklstring ( L, 1, & s ) ;
  const mode_t m = 00700 |
    ( 007777 & (lua_Unsigned) luaL_optinteger ( L, 2, 00700 ) ) ;

  if ( ( 0 < s ) && path && * path ) {
    return res_bool_zero ( L, mkpath ( m, (char *) path, s ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid pathname" ) ;
}

/* create special files with mknod(2) */
static int u_mknod ( lua_State * const L )
{
  dev_t dev = 0 ;
  const char * path = luaL_checkstring ( L, 1 ) ;
  const char * type = luaL_checkstring ( L, 2 ) ;
  mode_t m = (lua_Unsigned) luaL_checkinteger ( L, 3 ) ;
  m |= 000600 ;

  if ( ! ( path && * path ) ) {
    return luaL_argerror ( L, 1, "invalid filename" ) ;
  }

  if ( ! ( type && * type ) ) {
    type = "reg" ;
  }

  switch ( * type ) {
    case 'B' :
    case 'b' :
      m |= S_IFBLK ;
      dev = makedev (
        (lua_Unsigned) luaL_checkinteger ( L, 4 ),
        (lua_Unsigned) luaL_checkinteger ( L, 5 ) ) ;
      break ;
    case 'C' :
    case 'c' :
      m |= S_IFCHR ;
      dev = makedev (
        (lua_Unsigned) luaL_checkinteger ( L, 4 ),
        (lua_Unsigned) luaL_checkinteger ( L, 5 ) ) ;
      break ;
    case 'F' :
    case 'f' :
      m |= S_IFIFO ;
      break ;
    case 'S' :
    case 's' :
      m |= S_IFSOCK ;
      break ;
    case 'R' :
    case 'r' :
    default :
      m |= S_IFREG ;
      break ;
  }

  return res_bool_zero ( L, mknod ( path, m, dev ) ) ;
}

/* wrapper to rmdir(2) */
static int u_rmdir ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L, rmdir ( path ) ) ;
  }

  return luaL_argerror ( L, 1, "empty path" ) ;
}

/* wrapper to remove(3) */
static int u_remove ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L, remove ( path ) ) ;
  }

  return luaL_argerror ( L, 1, "empty path" ) ;
}

/* wrapper to unlink(2) */
static int u_unlink ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L, unlink ( path ) ) ;
  }

  return luaL_argerror ( L, 1, "empty path" ) ;
}

/* wrapper to rename(2) */
static int u_rename ( lua_State * const L )
{
  const char * target = luaL_checkstring ( L, 1 ) ;
  const char * dest = luaL_checkstring ( L, 2 ) ;

  if ( target && dest && * target && * dest ) {
    return res_bool_zero ( L, rename ( target, dest ) ) ;
  }

  return luaL_error ( L, "target and destination args required" ) ;
}

/* wrapper to chmod(2) */
static int u_chmod ( lua_State * const L )
{
  const mode_t m = 007777 & (lua_Unsigned) luaL_checkinteger ( L, 1 ) ;
  const char * const path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    /*
	return luaL_error ( L, "chmod( %s, %d ) failed: %s (errno %d)",
          path, m, strerror ( i ), i ) ;
    */
    return res_bool_zero ( L, chmod ( path, m ) ) ;
  }

  return luaL_argerror ( L, 2, "invalid file name" ) ;
}

static int do_chown ( lua_State * const L, const char f )
{
  const uid_t u = (lua_Unsigned) luaL_checkinteger ( L, 1 ) ;
  const gid_t g = (lua_Unsigned) luaL_checkinteger ( L, 2 ) ;
  const char * const path = luaL_checkstring ( L, 3 ) ;

  if ( ( 0 <= u ) && ( 0 <= g ) && path && * path ) {
    /*
    if ( f ? chown ( path, u, g ) : lchown ( path, u, g ) ) {
      i = errno ;
      return luaL_error ( L, "%s( %s, %d, %d ) failed: %s (errno %d)",
        f ? "chown" : "lchown", path, u, g, strerror ( i ), i ) ;
    } else {
        return luaL_argerror ( L, i, "invalid path" ) ;
    }
    */
    return
      res_bool_zero ( L, f ? chown ( path, u, g ) : lchown ( path, u, g ) ) ;
  }

  return luaL_error ( L, "valid UID, GID, and file path required" ) ;
}

/* wrapper to chown(2) */
static int u_chown ( lua_State * const L )
{
  return do_chown ( L, 1 ) ;
}

/* wrapper to lchown(2) */
static int u_lchown ( lua_State * const L )
{
  return do_chown ( L, 0 ) ;
}

/* wrapper to fchown(2) */
/*
static int Lfchown ( lua_State * const L )
{
  return 0 ;
}
*/

/* wrapper to fchmod(2) */
static int Sfchmod ( lua_State * const L )
{
  const mode_t m = 007777 & (lua_Unsigned) luaL_checkinteger ( L, 1 ) ;
  const int fd = luaL_checkinteger ( L, 2 ) ;

  if ( 0 > fd ) {
    return luaL_argerror ( L, 1, "invalid fd" ) ;
  }

  return res_bool_zero ( L, fchmod ( fd, m ) ) ;
}

/* wrapper to link() that creates hard links */
static int u_link ( lua_State * const L )
{
  const char * target = luaL_checkstring ( L, 1 ) ;
  const char * dest = luaL_checkstring ( L, 2 ) ;

  if ( target && dest && * target && * dest ) {
    /*
    if ( link ( target, dest ) ) {
      const int e = errno ;
      return luaL_error ( L, "link( %s, %s ) failed: %s (errno %d)",
        target, dest, strerror ( e ), e ) ;
    }
    */
    return res_bool_zero ( L, link ( target, dest ) ) ;
  }

  return luaL_error ( L, "target and destination file names required" ) ;
}

/* wrapper to symlink() */
static int u_symlink ( lua_State * const L )
{
  const char * target = luaL_checkstring ( L, 1 ) ;
  const char * dest = luaL_checkstring ( L, 2 ) ;

  if ( target && dest && * target && * dest ) {
    return res_bool_zero ( L, symlink ( target, dest ) ) ;
  }

  return luaL_error ( L, "target and destination file names required" ) ;
}

/* wrapper function for the truncate() syscall */
static int Struncate ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i ;
    off_t s = luaL_checkinteger ( L, 1 ) ;
    const char * path = NULL ;

    if ( 0 > s ) {
      return luaL_argerror ( L, 1, "negative offset size" ) ;
    }

    for ( i = 1 ; n >= i ; ++ i ) {
      path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        if ( truncate ( path, s ) ) {
          i = errno ;
          return luaL_error ( L, "truncate( %s, %d ) failed: %s (errno %d)",
            path, s, strerror ( i ), i ) ;
        }
      } else {
        return luaL_argerror ( L, i, "invalid file name" ) ;
      }
    }
  }

  return luaL_error ( L, "offset size and file name required" ) ;
}

/* wrapper function for the truncate syscall */
static int Sftruncate ( lua_State * const L )
{
  int fd = luaL_checkinteger ( L, 1 ) ;
  off_t s = luaL_checkinteger ( L, 2 ) ;

  if ( 0 > fd ) {
    return luaL_argerror ( L, 1, "invalid fd" ) ;
  } else if ( 0 > s ) {
    return luaL_argerror ( L, 2, "invalid size" ) ;
  } else {
    return res_zero ( L, ftruncate ( fd, s ) ) ;
  }

  return 0 ;
}

static int l_create_file ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    const mode_t mode = luaL_optinteger ( L, 2, 00600 ) ;
    return res_bool_zero ( L, create_file ( path, mode ) ) ;
  }

  return luaL_argerror ( L, 1, "filename required" ) ;
}

/* touch(1) like function */
static int l_touch ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    const char * path = luaL_checkstring ( L, 1 ) ;

    if ( path && * path ) {
      if ( 5 > n ) {
        return res_bool_zero ( L, touch ( path, NULL ) ) ;
      } else {
        struct timeval tv [ 2 ] ;
        tv [ 0 ] . tv_sec = luaL_checkinteger ( L, 2 ) ;
        tv [ 0 ] . tv_usec = luaL_checkinteger ( L, 3 ) ;
        tv [ 1 ] . tv_sec = luaL_checkinteger ( L, 4 ) ;
        tv [ 1 ] . tv_usec = luaL_checkinteger ( L, 5 ) ;
        return res_bool_zero ( L, touch ( path, tv ) ) ;
      }
    }
  }

  return luaL_error ( L, "filename and time arguments required" ) ;
}

/* make sure a given file exists and has the right permissions */
static int l_create ( lua_State * const L )
{
  const char * path = NULL ;
  const int n = lua_gettop ( L ) ;

  if ( 1 > n ) { return 0 ; }
  else if ( 1 <= n ) { path = luaL_checkstring ( L, 1 ) ; }

  if ( path && * path ) {
    int i = 0, uid = -3, gid = -3 ;
    mode_t mode = 0 ;

    if ( 2 <= n ) { mode = (lua_Unsigned) luaL_checkinteger ( L, 2 ) ; }
    if ( 3 <= n ) { uid = luaL_checkinteger ( L, 3 ) ; }
    if ( 4 <= n ) { uid = luaL_checkinteger ( L, 4 ) ; }

    mode = mode ? mode : 00644 ;
    i = create ( path, mode, uid, gid ) ;
    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return 0 ;
}

/* wrapper to readlink */
static int Sreadlink ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    struct stat st ;
    int i = lstat ( path, & st ) ;
    int e = errno ;

    if ( i ) {
      lua_pushnil ( L ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    } else if ( S_ISLNK( st . st_mode ) ) {
      luaL_Buffer b ;
      char * p = luaL_buffinitsize ( L, & b, 1 + st . st_size ) ;
      const ssize_t s = readlink ( path, p, st . st_size ) ;
      e = errno ;

      if ( 0 > s ) {
        lua_pushnil ( L ) ;
        lua_pushinteger ( L, e ) ;
        return 2 ;
      } else { p [ s ] = '\0' ; }

      p [ st . st_size ] = '\0' ;
      //if ( s < st . st_size ) { }
      luaL_pushresultsize ( & b, 1 + st . st_size ) ;
      return 1 ;
    }

    lua_pushnil ( L ) ;
    return 1 ;
  }

  return 0 ;
}

/*
 * wrapper functions to directory related syscalls
 */

/* returns a table containing all entries of a given dir using scandir */
static int Sscandir ( lua_State * const L )
{
  int i = 0, r = -3 ;
  const char * path = NULL ;
  struct dirent ** names = NULL ;

  path = luaL_checkstring ( L, 1 ) ;
  if ( 0 == ( path && * path ) ) { return 0 ; }
  r = scandir ( path, & names, NULL, alphasort ) ;

  if ( 0 > r ) {
    i = errno ;
    if ( names ) { free ( names ) ; }
    lua_pushnil ( L ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  } else if ( names ) {
    /* create a fresh table to hold the results
     * (i. e. the directory entries)
     */
    lua_newtable ( L ) ;

    for ( i = 0 ; i < r ; ++ i ) {
      (void) lua_pushstring ( L, names [ i ] -> d_name ) ;
      lua_rawseti( L, -2, 1 + i ) ;
    }

    for ( i = r - 1 ; i >= 0 ; -- i ) {
      free ( names [ i ] -> d_name ) ;
      free ( names [ i ] ) ;
    }

    free ( names ) ;
    /* return the new table */
    return 1 ;
  }

  return 0 ;
}

/* returns a table containing all entries of a given dir */
static int get_dirent ( lua_State * const L )
{
  int i = 0 ;
  DIR * dir = NULL ;
  struct dirent * entry = NULL ;
  const char * path = NULL ;

  path = luaL_checkstring ( L, 1 ) ;
  if ( 0 == ( path && * path ) ) { return 0 ; }
  dir = opendir ( path ) ;

  if ( NULL == dir ) {
    i = errno ;
    lua_pushnil ( L ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  }

  /* create a new table to hold the results */
  lua_newtable ( L ) ;

  for ( i = 1 ; NULL != ( entry = readdir ( dir ) ) ; ++ i ) {
    (void) lua_pushstring ( L, entry -> d_name ) ;
    lua_rawseti ( L, -2, i ) ;
  }

  i = closedir ( dir ) ;
  if ( i ) {
    i = errno ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  }

  /* table is already on top of the stack,
   * so return it to caller
   */
  return 1 ;
}

#if 0
/* TODO: dir iterator from the PIL book */
static int iter_dir ( lua_State * L )
{
  return 0 ;
}

static int diriter ( lua_State * const L )
{
  return 0 ;
}

/* no need for the rest atm */
/* wrapper to opendir */
static int Sopendir ( lua_State * const L )
{
  const char * path = NULL ;
  path = luaL_checkstring( L, 1 ) ;

  if ( NULL != path ) {
    d = opendir( path ) ;
    if ( NULL != d ) {
      lua_pushlightuserdata( L, (void *) d ) ;
      return 1 ;
    } else {
      const int e = errno ;
      lua_pushnil( L ) ;
      lua_pushinteger( L, e ) ;
      return 2 ;
    }
  }

  lua_pushnil( L ) ;
  return 1 ;
}

/* wrapper to fopendir */
static int Sfopendir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to closedir */
static int Sclosedir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to readdir */
static int Sreaddir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to rewinddir */
static int Srewinddir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to dirfd */
static int Sdirfd ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to seekdir */
static int Sseekdir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to scandir */
static int Sscandir ( lua_State * const L )
{
  return 0 ;
}

/* wrapper to telldir */
static int Stelldir ( lua_State * const L )
{
  return 0 ;
}
#endif

/* wrapper to the access(2) syscall */
static int Lchkacc ( lua_State * const L, const char eff, const int f )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;
    const char * path = NULL ;

    for ( i = 1 ; n >= i ; ++ i ) {
      path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        if (
#if (defined (__GLIBC__) && defined (_GNU_SOURCE)) || defined (OSfreebsd)
          eff ? eaccess ( path, f ) : access ( path, f )
#else
          access ( path, f )
#endif
        )
        {
          lua_pushboolean ( L, 0 ) ;
          return 1 ;
        }
      } else {
        return luaL_argerror ( L, i, "non empty file name required" ) ;
      }
    }

    lua_pushboolean ( L, 1 ) ;
    return 1 ;
  }

  return luaL_error ( L, "file names required" ) ;
}

/* wrapper to the (l)stat(2) syscall */
static int Lftype ( lua_State * const L, const unsigned long int f, const mode_t m )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;
    const char * path = NULL ;

    for ( i = 1 ; n >= i ; ++ i ) {
      path = luaL_checkstring ( L, i ) ;

      if ( path && * path && ftype ( f, m, path ) ) {
        ; /* ok */
      } else {
        lua_pushboolean ( L, 0 ) ;
        return 1 ;
      }
    }

    lua_pushboolean ( L, 1 ) ;
    return 1 ;
  }

  return luaL_error ( L, "file names required" ) ;
}

/*
 * file permission tests using access(2)
 */

/* checks if a given file exists */
static int Lacc_e ( lua_State * const L )
{
  return Lchkacc ( L, 0, F_OK ) ;
}

static int Lacc_r ( lua_State * const L )
{
  return Lchkacc ( L, 0, R_OK ) ;
}

static int Lacc_w ( lua_State * const L )
{
  return Lchkacc ( L, 0, W_OK ) ;
}

static int Lacc_x ( lua_State * const L )
{
  return Lchkacc ( L, 0, X_OK ) ;
}

static int Lacc_rw ( lua_State * const L )
{
  return Lchkacc ( L, 0, R_OK | W_OK ) ;
}

static int Lacc_rx ( lua_State * const L )
{
  return Lchkacc ( L, 0, R_OK | X_OK ) ;
}

static int Lacc_wx ( lua_State * const L )
{
  return Lchkacc ( L, 0, W_OK | X_OK ) ;
}

static int Lacc_rwx ( lua_State * const L )
{
  return Lchkacc ( L, 0, R_OK | W_OK | X_OK ) ;
}

/*
 * file permission tests using e(iud)access(3)
 */

#if (defined (__GLIBC__) && defined (_GNU_SOURCE)) || defined (OSfreebsd)
static int Leacc_r ( lua_State * const L )
{
  return Lchkacc ( L, 1, R_OK ) ;
}

static int Leacc_w ( lua_State * const L )
{
  return Lchkacc ( L, 1, W_OK ) ;
}

static int Leacc_x ( lua_State * const L )
{
  return Lchkacc ( L, 1, X_OK ) ;
}

static int Leacc_rw ( lua_State * const L )
{
  return Lchkacc ( L, 1, R_OK | W_OK ) ;
}

static int Leacc_rx ( lua_State * const L )
{
  return Lchkacc ( L, 1, R_OK | X_OK ) ;
}

static int Leacc_wx ( lua_State * const L )
{
  return Lchkacc ( L, 1, W_OK | X_OK ) ;
}

static int Leacc_rwx ( lua_State * const L )
{
  return Lchkacc ( L, 1, R_OK | W_OK | X_OK ) ;
}
#endif

/*
 * file tests with stat(2)
 */

/* checks if a given regular file exists */
static int Lis_f ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFREG ) ;
}

/* checks if a given directory exists */
static int Lis_d ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFDIR ) ;
}

static int Lis_p ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFIFO ) ;
}

static int Lis_s ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFSOCK ) ;
}

static int Lis_b ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFBLK ) ;
}

static int Lis_c ( lua_State * const L )
{
  return Lftype ( L, 0, S_IFCHR ) ;
}

static int Lis_l ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW, S_IFLNK ) ;
}

/* checks if a given file exists and is empty */
static int Lis_fz ( lua_State * const L )
{
  return Lftype ( L, FTEST_SZZ, S_IFREG ) ;
}

/* checks if a given file exists and is not empty */
static int Lis_fn ( lua_State * const L )
{
  return Lftype ( L, FTEST_SZNZ, S_IFREG ) ;
}

/*
 * file tests with lstat(2)
 */

/* checks if 2 given fds refer to the same (open) file */
static int Lfdcompare ( lua_State * const L )
{
  int i = 0 ;
  int fd1 = luaL_checkinteger ( L, 1 ) ;
  int fd2 = luaL_checkinteger ( L, 2 ) ;

  if ( 0 <= fd1 && 0 <= fd2 ) {
    i = fdcompare ( fd1, fd2 ) ;
  }

  lua_pushboolean ( L, i ) ;
  return 1 ;
}

/* checks if a given dir is mountpoint of a tmpfs */
static int Lis_tmpfs ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  const char * mtab = luaL_optstring ( L, 2, NULL ) ;

  if ( path && * path ) {
    lua_pushboolean ( L, is_tmpfs ( path, mtab ) ) ;
  } else {
    lua_pushboolean ( L, 0 ) ;
  }

  return 1 ;
}

/* checks if a given file path exists */
static int Ltest_path ( lua_State * const L )
{
  int r = 0 ;
  const char * path = luaL_checkstring ( L, 1 ) ;
  mode_t ftype = (lua_Unsigned) luaL_checkinteger ( L, 2 ) ;
  int acc = luaL_optinteger ( L, 3, F_OK ) ;
  mode_t perm = (lua_Unsigned) luaL_optinteger ( L, 4, 0 ) ;

  if ( 0 == ( path && * path ) ) {
    return 0 ;
  } else if ( 0 == access ( path, F_OK | acc ) ) {
    struct stat st ;

    if ( S_IFMT & S_IFLNK & ftype ) {
      r = lstat ( path, & st ) ;
    } else {
      r = stat ( path, & st ) ;
    }

    if ( r ) {
      const int e = errno ;
      /*
      switch ( errno ) {
        case ELOOP :
        case EACCES :
        case ENOTDIR :
        case ENOENT :
        default :
          lua_pushboolean ( L, 0 ) ;
          lua_pushinteger ( L, errno ) ;
          return 2 ;
          break ;
      }
      */
      lua_pushboolean ( L, 0 ) ;
      lua_pushinteger ( L, e ) ;
      return 2 ;
    }

    r = 0 ;

    if ( ( 0 < st . st_nlink ) && ( S_IFMT & st . st_mode & ftype ) )
    {
      if ( perm ) {
        r = perm & st . st_mode ;
      } else {
        r = 1 ;
      }
    }
  }

  lua_pushboolean ( L, r ) ;
  return 1 ;
}

/*
 * wrappers for the stat(2) family of syscalls
 */

static int get_stat_res ( lua_State * const L, struct stat * sp )
{
  /* table to hold the fields of the returned stat structure */
  lua_newtable ( L ) ;

  (void) lua_pushliteral ( L, "ino" ) ;
  lua_pushinteger ( L, sp -> st_ino ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "uid" ) ;
  lua_pushinteger ( L, sp -> st_uid ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "gid" ) ;
  lua_pushinteger ( L, sp -> st_gid ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "mode" ) ;
  lua_pushinteger ( L, sp -> st_mode ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "size" ) ;
  lua_pushinteger ( L, sp -> st_size ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "nlink" ) ;
  lua_pushinteger ( L, sp -> st_nlink ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "atime" ) ;
  lua_pushinteger ( L, sp -> st_atime ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "ctime" ) ;
  lua_pushinteger ( L, sp -> st_ctime ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "mtime" ) ;
  lua_pushinteger ( L, sp -> st_mtime ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "atimensec" ) ;
  lua_pushinteger ( L, sp -> st_atim . tv_nsec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "ctimensec" ) ;
  lua_pushinteger ( L, sp -> st_ctim . tv_nsec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "mtimensec" ) ;
  lua_pushinteger ( L, sp -> st_mtim . tv_nsec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "blksize" ) ;
  lua_pushinteger ( L, sp -> st_blksize ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "blocks" ) ;
  lua_pushinteger ( L, sp -> st_blocks ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "dev" ) ;
  lua_pushinteger ( L, sp -> st_dev ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "rdev" ) ;
  lua_pushinteger ( L, sp -> st_rdev ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

/* wrapper for the stat() syscall */
static int u_stat ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    struct stat st ;

    if ( stat ( path, & st ) ) {
      return res_nil ( L ) ;
    }

    return get_stat_res ( L, & st ) ;
  }

  return luaL_argerror ( L, 1, "pathname expected" ) ;
}

/* wrapper for the lstat() syscall */
static int u_lstat ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    struct stat st ;

    if ( lstat ( path, & st ) ) {
      return res_nil ( L ) ;
    }

    return get_stat_res ( L, & st ) ;
  }

  return luaL_argerror ( L, 1, "pathname expected" ) ;
}

/* wrapper for the fstat() syscall */
static int u_fstat ( lua_State * const L )
{
  const int fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= fd ) {
    struct stat st ;

    if ( fstat ( fd, & st ) ) {
      return res_nil ( L ) ;
    }

    return get_stat_res ( L, & st ) ;
  }

  return luaL_argerror ( L, 1, "invalid fd" ) ;
}

static int mode_is_blk ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISBLK( mode ) ) ;
  return 1 ;
}

static int mode_is_chr ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISCHR( mode ) ) ;
  return 1 ;
}

static int mode_is_dir ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISDIR( mode ) ) ;
  return 1 ;
}

static int mode_is_fifo ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISFIFO( mode ) ) ;
  return 1 ;
}

static int mode_is_lnk ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISLNK( mode ) ) ;
  return 1 ;
}

static int mode_is_reg ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISREG( mode ) ) ;
  return 1 ;
}

static int mode_is_sock ( lua_State * const L )
{
  const mode_t mode = luaL_checkinteger ( L, 1 ) ;
  lua_pushboolean ( L, S_ISSOCK( mode ) ) ;
  return 1 ;
}

/* copies the contents of one file to another */
static int Lcopy_file ( lua_State * const L )
{
  const char * src = luaL_checkstring ( L, 1 ) ;
  const char * dst = luaL_checkstring ( L, 2 ) ;

  if ( src && dst && * src && * dst ) {
    int i = 0 ;

    errno = 0 ;
    i = copy_file ( src, dst ) ;

    if ( i && errno ) {
      i = errno ;
      lua_pushinteger ( L, -1 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      lua_pushinteger ( L, i ) ;
      return 1 ;
    }
  }

  return 0 ;
}

/* tries to rename a given file */
static int Lmove_file ( lua_State * const L )
{
  const char * src = luaL_checkstring ( L, 1 ) ;
  const char * dst = luaL_checkstring ( L, 2 ) ;

  if ( src && dst && * src && * dst ) {
    int i = 0 ;

    errno = 0 ;
    i = move_file ( src, dst ) ;

    if ( i && errno ) {
      i = errno ;
      lua_pushinteger ( L, -1 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      lua_pushinteger ( L, i ) ;
      return 1 ;
    }
  }

  return 0 ;
}

/* read a given file at once into a Lua string */
static int Lread_file ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    int i = open ( path, O_RDONLY | O_NONBLOCK | O_CLOEXEC | O_NOCTTY ) ;

    if ( 0 > i ) {
      /* the open call failed */
      i = errno ;
      lua_pushinteger ( L, -3 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      /* get the file size */
      off_t o = lseek ( i, 0, SEEK_END ) ;
      int e = errno ;

      /* (re)set offset to the beginning of the file */
      if ( 0 < o && 0 == lseek ( i, 0, SEEK_SET ) ) {
        luaL_Buffer b ;
        char * p = luaL_buffinitsize ( L, & b, o ) ;
        const ssize_t s = read ( i, p, o ) ;
        e = errno ;

        if ( 0 > s ) {
          lua_pushinteger ( L, -9 ) ;
          lua_pushinteger ( L, e ) ;
        } else {
          lua_pushinteger ( L, s ) ;
        }

        luaL_pushresultsize ( & b, o ) ;
      } else {
        lua_pushinteger ( L, -5 ) ;
        lua_pushinteger ( L, e ) ;
      }

      close_fd ( i ) ;
      return 2 ;
    }
  }

  return 0 ;
}

/*
 * directory related functions
 */

/* returns the contents of a given directory in a new table */
static int list_dir ( lua_State * const L )
{
  int i = 0 ;
  DIR * dp ;
  struct dirent * d ;
  const char * dir = luaL_checkstring ( L, 1 ) ;

  if ( 0 == ( dir && * dir ) ) { return 0 ; }
  dp = opendir ( dir ) ;
  if ( NULL == dp ) {
    i = errno ;
    lua_pushinteger ( L, -3 ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  }

  lua_newtable( L ) ;
  i = 0 ;

  while ( NULL != ( d = readdir ( dp ) ) ) {
    (void) lua_pushstring ( L, d -> d_name ) ;
    lua_rawseti ( L, -2, ++ i ) ;
  }

  (void) closedir ( dp ) ;
  lua_pushinteger ( L, i ) ;
  lua_rotate ( L, -2, 2 ) ;
  return 2 ;
}

/*
 * directory itaretor functions
 */

/* directory iterator */
static int dir_iter ( lua_State * const L )
{
  dir_data_t * dp = (dir_data_t *) luaL_checkudata ( L, 1, DIR_METATABLE ) ;

  if ( NULL != dp ) {
    struct dirent * dentp = NULL ;

    luaL_argcheck ( L, 0 == dp -> closed, 1, "closed directory" ) ;
    dentp = readdir ( dp -> dirp ) ;

    if ( NULL != dentp ) {
      (void) lua_pushstring ( L, dentp -> d_name ) ;
      return 1 ;
    } else {
      /* no more dir entries, the dir handle can be closed */
      (void) closedir ( dp -> dirp ) ;
      dp -> closed = 3 ;
    }
  }

  return 0 ;
}

/* closes directory iterators */
static int dir_close ( lua_State * const L )
{
  dir_data_t * dp = (dir_data_t *) lua_touserdata ( L, 1 ) ;

  if ( NULL != dp ) {
    if ( 0 == dp -> closed && NULL != dp -> dirp )
    { (void) closedir ( dp -> dirp ) ; }
    dp -> closed = 3 ;
  }

  return 0 ;
}

/* factory of directory iterators */
static int dir_iter_factory ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    dir_data_t * dp ;

    lua_pushcfunction ( L, dir_iter ) ;
    dp = (dir_data_t *) lua_newuserdata ( L, sizeof ( dir_data_t ) ) ;
    luaL_getmetatable ( L, DIR_METATABLE ) ;
    lua_setmetatable ( L, -2 ) ;
    dp -> closed = 0 ;
    dp -> dirp = opendir ( path ) ;

    if ( NULL == dp -> dirp ) {
      luaL_error ( L, "cannot open %s: %s", path, strerror ( errno ) ) ;
    }

    return 2 ;
  }

  return 0 ;
}

/* creates directory metatable */
static int dir_create_meta ( lua_State * const L )
{
  luaL_newmetatable ( L, DIR_METATABLE ) ;

  /* method table */
  lua_newtable ( L ) ;
  lua_pushcfunction ( L, dir_iter ) ;
  lua_setfield ( L, -2, "next" ) ;
  lua_pushcfunction ( L, dir_close ) ;
  lua_setfield ( L, -2, "close" ) ;

  /* metamethods */
  lua_setfield ( L, -2, "__index" ) ;
  lua_pushcfunction ( L, dir_close ) ;
  lua_setfield ( L, -2, "__gc" ) ;

  return 1 ;
}

/* TODO: wrapper for nftw(3) */

/* remove given files/dirs (recursively) */
static int Lrm ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i, j, r = 0 ;
    const char * path = NULL ;

    for ( i = 1 ; n >= i ; ++ i ) {
      path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        j = rm ( path ) ;
        r = j ? j : r ;
      } else {
        return luaL_argerror ( L, i, "empty file name" ) ;
        break ;
      } 
    }

    return res_zero ( L, r ) ;
  }

  return 0 ;
}

#if 0
static int rm_dir ( const char * pathname, const char top )
{
  char retval = true;
  DIR * dp ;
  struct dirent * d ;
  struct stat s ;
  char file [ PATH_MAX ] ;

	if ((dp = opendir(pathname)) == NULL)
		return false;

	errno = 0;
	while (((d = readdir(dp)) != NULL) && errno == 0) {
		if (strcmp(d->d_name, ".") != 0 &&
		    strcmp(d->d_name, "..") != 0)
		{
			snprintf(file, sizeof(file),
			    "%s/%s", pathname, d->d_name);
			if (stat(file, &s) != 0) {
				retval = false;
				break;
			}
			if (S_ISDIR(s.st_mode)) {
				if (!rm_dir(file, true))
				{
					retval = false;
					break;
				}
			} else {
				if (unlink(file)) {
					retval = false;
					break;
				}
			}
		}
	}

	closedir ( dp ) ;

	if ( ! retval )
		return false ;

	if ( top && rmdir ( pathname ) != 0 )
		return false ;

	return true ;
}
#endif

/* helper function that removes files recursively */
static int rmd ( const char top, const char * path )
{
  int i, r = 0, fd = -1 ;
  DIR * dp ;
  struct dirent * de ;
  struct stat st ;

  fd = open ( path, O_CLOEXEC | O_PATH | O_DIRECTORY | O_NOFOLLOW ) ;
  if ( 0 > fd ) { return 1 ; }

  dp = fdopendir ( fd ) ;
  if ( NULL == dp ) {
    close_fd ( fd ) ;
    return 1 ;
  }

  while ( NULL != ( de = readdir ( dp ) ) )
  {
    if ( '.' == de -> d_name [ 0 ] ) {
       if ( '\0' == de -> d_name [ 1 ] ) { continue ; }
       else if ( '.' == de -> d_name [ 1 ] && '\0' == de -> d_name [ 2 ] )
       { continue ; }
    }

    if ( DT_DIR == de -> d_type ) {
      i = unlinkat ( fd, de -> d_name, AT_REMOVEDIR ) ;

      if ( i && ( ENOTEMPTY == errno || EEXIST == errno ) ) {
        r += rmd ( 0, de -> d_name ) ;
      } else if ( i ) { ++ r ; }

      continue ;
    } else if ( DT_UNKNOWN != de -> d_type ) {
      r -= unlinkat ( fd, de -> d_name, 0 ) ;
      continue ;
    }

    if ( 0 == fstatat ( fd, de -> d_name, & st, AT_SYMLINK_NOFOLLOW ) ) {
      if ( S_ISDIR ( st . st_mode ) ) {
        i = unlinkat ( fd, de -> d_name, AT_REMOVEDIR ) ;

        if ( i && ( ENOTEMPTY == errno || EEXIST == errno ) ) {
          r += rmd ( 0, de -> d_name ) ;
        } else if ( i ) { ++ r ; }
      } else {
        r -= unlinkat ( fd, de -> d_name, 0 ) ;
      }
    }
  }

  r -= closedir ( dp ) ;
  close_fd ( fd ) ;
  if ( top && rmdir ( path ) ) { return 1 ; }

  return r ;
}

/* (recursively) remove given files/directories */
static int Lrmr ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i, j, r = 0 ;
    struct stat st ;
    const char * path = NULL ;

    for ( i = 1 ; n >= i ; ++ i ) {
      path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        j = lstat ( path, & st ) ;

        if ( 0 == j && 0 < st . st_nlink ) {
          if ( S_ISDIR( st . st_mode ) ) {
            j = rmdir ( path ) ;

            if ( j && ( ENOTEMPTY == errno || EEXIST == errno ) )
            { r += rmd ( 3, path ) ; }
            else if ( j ) { ++ r ; }
          } else { r -= unlink ( path ) ; }
        }
      }
    }

    lua_pushinteger ( L, r ) ;
    return 1 ;
  }

  return 0 ;
}

/* add wrapper for makedev ? */

/*
 * file permission/mode tests
 */

static int Lis_su ( lua_State * const L )
{
  return Lftype ( L, 0, S_ISUID ) ;
}

static int Lis_sg ( lua_State * const L )
{
  return Lftype ( L, 0, S_ISGID ) ;
}

static int Lis_st ( lua_State * const L )
{
  return Lftype ( L, 0, S_ISVTX ) ;
}

static int Lis_or ( lua_State * const L )
{
  return Lftype ( L, FTEST_UID, 0 ) ;
}

static int Lis_gr ( lua_State * const L )
{
  return Lftype ( L, FTEST_GID, 0 ) ;
}

static int Lis_oe ( lua_State * const L )
{
  return Lftype ( L, FTEST_EFF | FTEST_UID, 0 ) ;
}

static int Lis_ge ( lua_State * const L )
{
  return Lftype ( L, FTEST_EFF | FTEST_GID, 0 ) ;
}

/*
 * no follow symlinks versions of the above
 */

static int Lis_su_nf ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW, S_ISUID ) ;
}

static int Lis_sg_nf ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW, S_ISGID ) ;
}

static int Lis_st_nf ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW, S_ISVTX ) ;
}

static int Lis_or_nf ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW | FTEST_UID, 0 ) ;
}

static int Lis_gr_nf ( lua_State * const L )
{
  return Lftype ( L, FTEST_NOFOLLOW | FTEST_GID, 0 ) ;
}

/*
 * compare 2 given files
 */

static int Lfcomp ( lua_State * const L, const unsigned int f )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  const char * path2 = luaL_checkstring ( L, 2 ) ;

  if ( path && path2 && * path && * path2 ) {
    lua_pushboolean ( L, fcompare ( f, path, path2 ) ) ;
    return 1 ;
  }

  return luaL_error ( L, "2 file names required" ) ;
}

/*
 * via stat(2) (i. e. follow symlinks)
 */

static int Lis_nt ( lua_State * const L )
{
  return Lfcomp ( L, FCOMP_MTIME_NEWER ) ;
}

static int Lis_ot ( lua_State * const L )
{
  return Lfcomp ( L, FCOMP_MTIME_OLDER ) ;
}

static int Lis_eq ( lua_State * const L )
{
  return Lfcomp ( L, FCOMP_INO ) ;
}

/*
 * via lstat(2) (i. e. don't follow symlinks)
 */

