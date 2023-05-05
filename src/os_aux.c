/*
 * helper functions for all other Lua wrapper functions
 *
 * public domain
 */

static int x_os_not_sup ( lua_State * const L )
{
  return luaL_error ( L,
#ifdef OS
    OS ": platform not supported"
#else
    "platform not supported"
#endif
    ) ;
}

static int push_error ( lua_State * L, const int err, const char * fun,
  const char * what )
{
  int c = 0 ;
  const int n = lua_gettop ( L ) ;

  while ( ( c = * what ++ ) ) {
    switch ( c ) {
      case '~' :
        lua_pushnil ( L ) ;
        break ;
      case '#' :
        lua_pushinteger ( L, err ) ;
        break ;
      case '$' :
        (void) lua_pushfstring ( L,
          "%s() failed: %s (errno %d)",
          fun, strerror ( err ), err ) ;
        break ;
      case '0':
        lua_pushboolean ( L, 0 ) ;
        break ;
    }
  }

  return lua_gettop ( L ) - n ;
}

/* helper function that reports already occurred posix errors */
static int rep_err ( lua_State * const L, const char * const func, int e )
{
  e = ( 0 < e ) ? e : 0 ;
  lua_pushboolean ( L, 0 ) ;

  if ( func && * func ) {
    (void) lua_pushfstring ( L, "%s() failed: %s (errno %d)", func, strerror ( e ), e ) ;
  } else {
    (void) lua_pushfstring ( L, "syscall failed: %s (errno %d)", strerror ( e ), e ) ;
  }

  lua_pushinteger ( L, e ) ;
  return 3 ;
}

/* helper function that reports errors stored in errno
 * if a given condition is met
 */
static int chk_res ( lua_State * const L, const char * const func, const int c )
{
  if ( c ) {
    int e = errno ;
    e = ( 0 < e ) ? e : 0 ;
    lua_pushboolean ( L, 0 ) ;

    if ( func && * func ) {
      (void) lua_pushfstring ( L, "%s() failed: %s (errno %d)", func, strerror ( e ), e ) ;
    } else {
      (void) lua_pushfstring ( L, "syscall failed: %s (errno %d)", strerror ( e ), e ) ;
    }

    lua_pushinteger ( L, e ) ;
    return 3 ;
  }

  lua_pushboolean ( L, 1 ) ;
  return 1 ;
}

#define res0( L, m, r ) chk_res ( L, m, 0 == ( r ) )
#define resne0( L, m, r ) chk_res ( L, m, 0 != ( r ) )
#define resgt0( L, m, r ) chk_res ( L, m, 0 < ( r ) )
#define reslt0( L, m, r ) chk_res ( L, m, 0 > ( r ) )
#define resge0( L, m, r ) chk_res ( L, m, 0 <= ( r ) )
#define resle0( L, m, r ) chk_res ( L, m, 0 >= ( r ) )
#define reseq( L, m, v, r ) chk_res ( L, m, v == ( r ) )
#define resneq( L, m, v, r ) chk_res ( L, m, v != ( r ) )
#define resgt( L, m, v, r ) chk_res ( L, m, v < ( r ) )
#define reslt( L, m, v, r ) chk_res ( L, m, v > ( r ) )
#define resge( L, m, v, r ) chk_res ( L, m, v <= ( r ) )
#define resle( L, m, v, r ) chk_res ( L, m, v >= ( r ) )

static int res_zero ( lua_State * const L, const int res )
{
  if ( res ) {
    const int e = errno ;
    lua_pushinteger ( L, res ) ;
    (void) lua_pushstring ( L, strerror ( e ) ) ;
    lua_pushinteger ( L, e ) ;
    return 3 ;
  }

  lua_pushinteger ( L, 0 ) ;
  return 1 ;
}

static int res_lt ( lua_State * const L, const int min, const int res )
{
  if ( res < min ) {
    const int e = errno ;
    lua_pushinteger ( L, res ) ;
    (void) lua_pushstring ( L, strerror ( e ) ) ;
    lua_pushinteger ( L, e ) ;
    return 3 ;
  }

  lua_pushinteger ( L, res ) ;
  return 1 ;
}

static int res_bool_zero ( lua_State * const L, const int res )
{
  if ( res ) {
    const int e = errno ;
    lua_pushboolean ( L, 0 ) ;

    if ( 0 < e ) {
      (void) lua_pushstring ( L, strerror ( e ) ) ;
      lua_pushinteger ( L, e ) ;
      return 3 ;
    }

    return 1 ;
  }

  lua_pushboolean ( L, 1 ) ;
  return 1 ;
}

static int res_nil ( lua_State * const L )
{
  const int e = errno ;

  lua_pushnil ( L ) ;

  if ( 0 < e ) {
    (void) lua_pushstring ( L, strerror ( e ) ) ;
    lua_pushinteger ( L, e ) ;
    return 3 ;
  }

  return 1 ;
}

static int res_false ( lua_State * const L )
{
  const int e = errno ;

  lua_pushboolean ( L, 0 ) ;

  if ( 0 < e ) {
    (void) lua_pushstring ( L, strerror ( e ) ) ;
    lua_pushinteger ( L, e ) ;
    return 3 ;
  }

  return 1 ;
}

