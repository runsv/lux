/*
 * Lua bindings for the *at() syscalls
 */

/*
 * openat() and the other system calls and library functions
 * that take a directory file descriptor argument (i.e.,
 * execveat(2),  faccessat(2), fanotify_mark(2),
 * fchmodat(2),  fchownat(2), fspick(2), fstatat(2),
 * futimesat(2), linkat(2), mkdirat(2), mknodat(2),
 * mount_setattr(2), move_mount(2), name_to_handle_at(2),
 * open_tree(2), openat2(2), readlinkat(2), renameat(2),
 * renameat2(2), statx(2), symlinkat(2), unlinkat(2),
 * utimensat(2),  mkfifoat(3),  and scandirat(3))
 * address two problems with the older interfaces that preceded them.
 */

#if defined (AT_FDCWD)

/* wrapper function for the openat(2) syscall */
static int u_openat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    int r = 0 ;
    int f = luaL_checkinteger ( L, 3 ) ;
    f = f ? f : O_RDONLY ;

    if ( ( O_CREAT & f ) || ( O_TMPFILE & f ) ) {
      mode_t m = luaL_optinteger ( L, 4, 00600 ) ;
      m = ( 007777 & m ) ? m : 00600 ;
      r = openat ( dirfd, path, f, m ) ;
    } else {
      r = openat ( dirfd, path, f ) ;
    }

    if ( 0 > r ) {
      return res_nil ( L ) ;
    }

    lua_pushinteger ( L, r ) ;
    return 1 ;
  }

  return luaL_argerror ( L, 2, "invalid filename" ) ;
}

/* wrapper function for the faccessat(2) syscall */
static int u_faccessat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    const int what = luaL_checkinteger ( L, 3 ) ;
    return res_bool_zero ( L, faccessat ( dirfd, path,
      what, luaL_optinteger ( L, 4, 0 ) ) ) ;
  }

  return luaL_argerror ( L, 2, "invalid filename" ) ;
}

/* wrapper function for the mkdirat(2) syscall */
static int u_mkdirat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    mode_t m = luaL_optinteger ( L, 3, 00755 ) ;
    m = ( 007777 & m ) ? m : 00755 ;
    return res_bool_zero ( L, mkdirat ( dirfd, path, m ) ) ;
  }

  return luaL_argerror ( L, 2, "invalid dir path" ) ;
}

/* wrapper function for the mkfifoat(2) syscall */
static int u_mkfifoat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    mode_t m = luaL_optinteger ( L, 3, 00600 ) ;
    m = ( 007777 & m ) ? m : 00600 ;
    return res_bool_zero ( L, mkfifoat ( dirfd, path, m ) ) ;
  }

  return luaL_argerror ( L, 2, "invalid filename" ) ;
}

/* wrapper function for the unlinkat(2) syscall */
static int u_unlinkat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L,
      unlinkat ( dirfd, path, luaL_optinteger ( L, 3, 0 ) ) ) ;
  }

  return luaL_argerror ( L, 2, "invalid filename" ) ;
}

/* wrapper function for the renameat(2) syscall */
static int u_renameat ( lua_State * const L )
{
  const int ofd = luaL_checkinteger ( L, 1 ) ;
  const char * opath = luaL_checkstring ( L, 2 ) ;
  const int nfd = luaL_checkinteger ( L, 3 ) ;
  const char * npath = luaL_checkstring ( L, 4 ) ;

  if ( opath && npath && * opath && * npath ) {
    return res_bool_zero ( L, renameat ( ofd, opath, nfd, npath ) ) ;
  }

  return luaL_error ( L, "old pathname and new pathname required" ) ;
}

/* wrapper function for the linkat(2) syscall */
static int u_linkat ( lua_State * const L )
{
  const int ofd = luaL_checkinteger ( L, 1 ) ;
  const char * opath = luaL_checkstring ( L, 2 ) ;
  const int nfd = luaL_checkinteger ( L, 3 ) ;
  const char * npath = luaL_checkstring ( L, 4 ) ;

  if ( opath && npath ) {
    return res_bool_zero ( L,
      linkat ( ofd, opath, nfd, npath, luaL_optinteger ( L, 5, 0 ) ) ) ;
  }

  return luaL_error ( L, "old pathname and new pathname required" ) ;
}

/* wrapper function for the symlinkat(2) syscall */
static int u_symlinkat ( lua_State * const L )
{
  const char * target = luaL_checkstring ( L, 1 ) ;
  const int nfd = luaL_checkinteger ( L, 2 ) ;
  const char * path = luaL_checkstring ( L, 3 ) ;

  if ( target && path ) {
    return res_bool_zero ( L, symlinkat ( target, nfd, path ) ) ;
  }

  return luaL_error ( L, "target and linkpath required" ) ;
}

/* wrapper function for the fchmodat(2) syscall */
static int u_fchmodat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    mode_t m = luaL_checkinteger ( L, 3 ) ;
    return res_bool_zero ( L, fchmodat ( dirfd, path, m,
      luaL_optinteger ( L, 4, 0 ) ) ) ;
  }

  return luaL_argerror ( L, 2, "filename required" ) ;
}

/* wrapper function for the fchownat(2) syscall */
static int u_fchownat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    const uid_t owner = luaL_checkinteger ( L, 3 ) ;
    const gid_t group = luaL_checkinteger ( L, 4 ) ;
    return res_bool_zero ( L, fchownat ( dirfd, path,
      owner, group, luaL_optinteger ( L, 5, 0 ) ) ) ;
  }

  return luaL_argerror ( L, 2, "filename required" ) ;
}

/* wrapper function for the utimensat(2) syscall */
static int u_utimensat ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * path = luaL_checkstring ( L, 2 ) ;

  if ( path && * path ) {
    int f = 0 ;

    if ( 5 < n ) {
      struct timespec times [ 2 ] ;
      times [ 0 ] . tv_sec = luaL_checkinteger ( L, 3 ) ;
      times [ 0 ] . tv_nsec = luaL_checkinteger ( L, 4 ) ;
      times [ 1 ] . tv_sec = luaL_checkinteger ( L, 5 ) ;
      times [ 1 ] . tv_nsec = luaL_checkinteger ( L, 6 ) ;

      if ( 6 < n ) {
        f = luaL_checkinteger ( L, 7 ) ;
      }

      return res_bool_zero ( L, utimensat ( dirfd, path, times, f ) ) ;
    } else if ( 2 < n ) {
      f = luaL_checkinteger ( L, 3 ) ;
      return res_bool_zero ( L, utimensat ( dirfd, path, NULL, f ) ) ;
    } else {
      return res_bool_zero ( L, utimensat ( dirfd, path, NULL, 0 ) ) ;
    }
  }

  return luaL_argerror ( L, 1, "filename required" ) ;
}

/* wrapper function for the fstatat(2) syscall */
static int u_fstatat ( lua_State * const L )
{
  const int dirfd = luaL_checkinteger ( L, 1 ) ;
  const char * const path = luaL_checkstring ( L, 2 ) ;
  int f = luaL_optinteger ( L, 3, 0 ) ;

  if ( path ) {
    struct stat st ;

    if ( fstatat ( dirfd, path, & st, f ) ) {
      return res_nil ( L ) ;
    }

    return get_stat_res ( L, & st ) ;
  }

  return luaL_error ( L, "pathname expected" ) ;
}

#endif
