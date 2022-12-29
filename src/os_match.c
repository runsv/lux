/*
 * wrappers for pattern matching APIs and syscalls like
 * posix glob(), fnmatch(), and wordexp()
 */

/*
 * fnmatch() related functions
 */

/* constants for fnmatch(3) */
static void add_fnmatch_flags ( lua_State * const L )
{
  L_ADD_CONST( L, FNM_NOESCAPE )
  L_ADD_CONST( L, FNM_PATHNAME )
  L_ADD_CONST( L, FNM_PERIOD )
#ifdef __GLIBC__
  L_ADD_CONST( L, FNM_CASEFOLD )
  L_ADD_CONST( L, FNM_EXTMATCH )
#endif
}

/* wrapper function for fnmatch(3) */
static int u_fnmatch ( lua_State * const L )
{
  const char * pat = luaL_checkstring ( L, 1 ) ;
  const char * str = luaL_checkstring ( L, 2 ) ;

  if ( pat && str && * pat && * str ) {
    int r = 0 ;
    int f = luaL_optinteger ( L, 3, 0 ) ;

#ifdef __GLIBC__
    r = fnmatch ( pat, str, FNM_EXTMATCH | f ) ;
#else
    r = fnmatch ( pat, str, f ) ;
#endif

    lua_pushboolean ( L, r ? 0 : 1 ) ;
    return 1 ;
  }

  return luaL_error ( L, "pattern and string arguments required" ) ;
}

/*
 * glob() related functions
 */

/* constants for glob(3) */
static void add_glob_flags ( lua_State * const L )
{
  L_ADD_CONST( L, GLOB_ERR )
  L_ADD_CONST( L, GLOB_MARK )
  L_ADD_CONST( L, GLOB_NOESCAPE )
  L_ADD_CONST( L, GLOB_NOSORT )
  L_ADD_CONST( L, GLOB_DOOFFS )
  L_ADD_CONST( L, GLOB_NOCHECK )
  L_ADD_CONST( L, GLOB_APPEND )
#if defined(__GLIBC__)
  L_ADD_CONST( L, GLOB_PERIOD )
  L_ADD_CONST( L, GLOB_ALTDIRFUNC )
  L_ADD_CONST( L, GLOB_BRACE )
  L_ADD_CONST( L, GLOB_NOMAGIC )
  L_ADD_CONST( L, GLOB_TILDE )
  L_ADD_CONST( L, GLOB_TILDE_CHECK )
  L_ADD_CONST( L, GLOB_ONLYDIR )
#endif
  L_ADD_CONST( L, GLOB_ABORTED )
  L_ADD_CONST( L, GLOB_NOMATCH )
  L_ADD_CONST( L, GLOB_NOSPACE )
}

/* error reporting function */
static int globerr ( const char * epath, const int e )
{
  (void) fprintf ( stderr, "glob() failed: %s (errno %d)",
    epath, e ) ;
  return 0 ;
}

/* wrapper function for glob(3) */
static int u_glob ( lua_State * const L )
{
  const char * pat = luaL_checkstring ( L, 1 ) ;

  if ( pat && * pat ) {
    int r = 0 ;
    glob_t g ;
    int f = luaL_optinteger ( L, 2, 0 ) ;

#ifdef __GLIBC__
    r = glob ( pat, f | GLOB_PERIOD | GLOB_BRACE | GLOB_TILDE, globerr, & g ) ;
#else
    r = glob ( pat, f, globerr, & g ) ;
#endif

    if ( r ) {
      /* no match or error */
      lua_pushnil ( L ) ;
    } else {
      /* found matches for pattern "pat" */
      int i = 0 ;
      /* push a fresh table onto the stack to hold the results */
      lua_newtable ( L ) ;
      /* and fill it with the resulting matches (strings) */
      for ( i = 0 ; ( i < g . gl_pathc ) && g . gl_pathv [ i ] ; ++ i )
      {
        (void) lua_pushstring ( L, g . gl_pathv [ i ] ) ;
        lua_rawseti ( L, -2, 1 + i ) ;
      }
    }

    globfree ( & g ) ;

    if ( r ) {
      lua_pushinteger ( L, r ) ;

      if ( GLOB_NOSPACE == r ) {
        (void) lua_pushstring ( L, "out of memory" ) ;
        return 3 ;
      } else if ( GLOB_ABORTED == r ) {
        (void) lua_pushstring ( L, "read error" ) ;
        return 3 ;
      } else if ( GLOB_NOMATCH == r ) {
        (void) lua_pushstring ( L, "no matches found" ) ;
        return 3 ;
      }

      return 2 ;
    }

    return 1 ;
  }

  return luaL_argerror ( L, 1, "pattern string required" ) ;
}

/*
 * wordexp() related functions
 */

/* constants for wordexp(3) */
static void add_wordexp_flags ( lua_State * const L )
{
  L_ADD_CONST( L, WRDE_APPEND )
  L_ADD_CONST( L, WRDE_DOOFFS )
  L_ADD_CONST( L, WRDE_NOCMD )
  L_ADD_CONST( L, WRDE_REUSE )
  L_ADD_CONST( L, WRDE_SHOWERR )
  L_ADD_CONST( L, WRDE_UNDEF )
  L_ADD_CONST( L, WRDE_BADCHAR )
  L_ADD_CONST( L, WRDE_BADVAL )
  L_ADD_CONST( L, WRDE_CMDSUB )
  L_ADD_CONST( L, WRDE_NOSPACE )
  L_ADD_CONST( L, WRDE_SYNTAX )
}

/* wrapper function for wordexp(3) */
static int u_wordexp ( lua_State * const L )
{
  const char * pat = luaL_checkstring ( L, 1 ) ;

  if ( pat && * pat ) {
    wordexp_t we ;
    int f = luaL_optinteger ( L, 2, 0 ) ;
    const int r = wordexp ( pat, & we, f ) ;

    if ( r ) {
      /* no match or error */
      lua_pushnil ( L ) ;
    } else {
      /* found matches for pattern "pat" */
      int i = 0 ;
      /* push a fresh table onto the stack to hold the results */
      lua_newtable ( L ) ;
      /* and fill it with the resulting matches (strings) */
      for ( i = 0 ; ( i < we . we_wordc ) && we . we_wordv [ i ] ; ++ i )
      {
        (void) lua_pushstring ( L, we . we_wordv [ i ] ) ;
        lua_rawseti ( L, -2, 1 + i ) ;
      }
    }

    wordfree ( & we ) ;

    if ( r ) {
      lua_pushinteger ( L, r ) ;
      return 2 ;
    }

    return 1 ;
  }

  return luaL_argerror ( L, 1, "pattern string required" ) ;
}

