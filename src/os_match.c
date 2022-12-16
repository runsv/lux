/*
 * wrappers for pattern matching APIs and syscalls like
 * posix glob, fnmatch and regex
 */

/*
 * glob related functions
 */

/* wrapper function to the glob syscall */
static int Sglob ( lua_State * L )
{
  int r = 0 ;
  glob_t g ;
  const char * pat = luaL_checkstring( L, 1 ) ;

  if ( NULL == pat ) {
    lua_pushnil( L ) ;
    return 1 ;
  }

#ifdef __GLIBC__
  r = glob( pat, GLOB_PERIOD | GLOB_BRACE | GLOB_TILDE, NULL, & g ) ;
#else
  r = glob( pat, 0, NULL, & g ) ;
#endif

  if ( r ) {
    /* no match or error */
    lua_pushnil( L ) ;
  } else {
    /* found matches for pattern "pat" */
    int i ;
    /* push a fresh table onto the stack to hold the results */
    lua_newtable( L ) ;
    /* and fill it with the resulting matches (strings) */
    for ( i = 0 ; ( i < g . gl_pathc ) && g . gl_pathv [ i ] ; ++ i )
    {
      (void) lua_pushstring( L, g . gl_pathv [ i ] ) ;
      lua_rawseti( L, -2, 1 + i ) ;
    }
  }

  globfree( & g ) ;
  if ( r ) {
    lua_pushinteger( L, r ) ;
    return 2 ;
  }

  return 1 ;
}

/*
 * fnmatch related functions
 */

/* wrapper function for the fnmatch syscall */
static int Sfnmatch ( lua_State * L )
{
  int r = 0 ;
  const char * pat = luaL_checkstring( L, 1 ) ;
  const char * str = luaL_checkstring( L, 2 ) ;

  if ( ! ( pat && str ) ) {
    lua_pushboolean( L, 0 ) ;
    return 1 ;
  }

#ifdef __GLIBC__
  r = fnmatch( pat, str, FNM_EXTMATCH ) ;
#else
  r = fnmatch( pat, str, 0 ) ;
#endif

  if ( r ) {
    /* no match or another error occurred */
    lua_pushboolean( L, 0 ) ;
  } else {
    /* matched, ok */
    lua_pushboolean( L, 1 ) ;
  }

  return 1 ;
}

/*
 * wordexp related functions
 */

/* wrapper function for wordexp */
static int Swordexp ( lua_State * L )
{
  int r = 0 ;
  wordexp_t we ;
  const char * pat = luaL_checkstring( L, 1 ) ;

  if ( NULL == pat ) {
    lua_pushnil( L ) ;
    return 1 ;
  }

  r = wordexp( pat, & we, 0 ) ;

  if ( r ) {
    /* no match or error */
    lua_pushnil ( L ) ;
  } else {
    /* found matches for pattern "pat" */
    int i ;
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

