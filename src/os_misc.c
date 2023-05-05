/*
 * public domain
 */

/* helper function that returns the the number represented
 * by a given character in a given representation base
 */
static int chrtoi ( const int b, const int c )
{
  if ( 0 < c && 1 < b && 37 > b ) {
    int i = -1 ;

    if ( isspace ( c ) ) {
      return -1 ;
    } else if ( ispunct ( c ) ) {
      return 0 ;
    } else if ( isdigit ( c ) ) {
      i = c - '0' ;
    } else if ( isalpha ( c ) && isascii ( c ) ) {
      i = 10 + tolower ( c ) - 'a' ;
    }

    if ( 0 <= i && b > i ) { return i ; }
  }

  return -1 ;
}

/* helper function that parses a given integer literal string */
static lua_Unsigned strntou ( const int b, const lua_Unsigned max,
  size_t n, const char * str, const char ** end )
{
  if ( ( 1 < b ) && ( 37 > b ) && str && * str ) {
    lua_Unsigned m = 0, r = 0 ;
    int i = 1, j = strlen ( str ) ;

    m = ~ (lua_Unsigned) 0 ;
    m = ( 0 < max && max < m ) ? max : m ;

    while ( str && * str && ( m > r ) ) {
      int c = chrtoi ( b, * str ) ;

      if ( 0 > c ) {
        return -1 ;
      } else if ( 0 == c ) {
        i *= b ;
      } else if ( 0 < c && b > c ) {
        c *= i ;

        if ( LUA_MAXINTEGER <= c + r ) { return r ; }

        r += c ;
        i *= b ;
      } else if ( b <= c ) {
        return -1 ;
      }

      -- n ; ++ str ;
    }

    if ( 1 < i ) { return r ; }

    if ( end ) { * end = str ; }
    return r ;
  }

  if ( end ) { * end = NULL ; }
  return 0 ;
}

static ssize_t fd_puts ( int fd, const char * const msg )
{
  fd = ( 0 > fd ) ? STDOUT_FILENO : fd ;

  if ( msg && * msg ) {
    return write ( fd, msg, strlen ( msg ) ) ;
  }

  return write ( fd, "\n", 1 ) ;
}

static ssize_t print2fd ( int fd, const char * const msg )
{
  ssize_t i = 0 ;
  fd = ( 0 > fd ) ? STDOUT_FILENO : fd ;

  if ( msg && * msg ) {
    return write ( fd, msg, strlen ( msg ) ) ;
  }

  return i + write ( fd, "\n", 1 ) ;
}

/*
 * integer bitwise ops
 */

/* bitwise (unary) negation */
static int x_bitneg ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;

    for ( i = 1 ; n >= i ; ++ i ) {
      lua_pushinteger ( L, ~ luaL_checkinteger ( L, i ) ) ;
      lua_replace ( L, i ) ;
    }

    return n ;
  }

  return luaL_error ( L, "integer arg required" ) ;
}

/* bitwise and for multiple integer args */
static int x_bitand ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i ;
    lua_Integer j = luaL_checkinteger ( L, 1 ) ;

    for ( i = 2 ; n >= i ; ++ i ) {
      j &= luaL_checkinteger ( L, i ) ;
    }

    lua_pushinteger ( L, j ) ;
    return 1 ;
  }

  return luaL_error ( L, "2 integer args required" ) ;
}

static int x_bitor ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i ;
    lua_Integer j = luaL_checkinteger ( L, 1 ) ;

    for ( i = 2 ; n >= i ; ++ i ) {
      j |= luaL_checkinteger ( L, i ) ;
    }

    lua_pushinteger ( L, j ) ;
    return 1 ;
  }

  return luaL_error ( L, "2 integer args required" ) ;
}

static int x_bitxor ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i ;
    lua_Integer j = luaL_checkinteger ( L, 1 ) ;

    for ( i = 2 ; n >= i ; ++ i ) {
      j ^= luaL_checkinteger ( L, i ) ;
    }

    lua_pushinteger ( L, j ) ;
    return 1 ;
  }

  return luaL_error ( L, "2 integer args required" ) ;
}

static int x_bitlshift ( lua_State * const L )
{
  lua_pushinteger ( L,
    luaL_checkinteger ( L, 1 ) << luaL_checkinteger ( L, 2 ) ) ;
  return 1 ;
}

static int x_bitrshift ( lua_State * const L )
{
  lua_pushinteger ( L,
    luaL_checkinteger ( L, 1 ) >> luaL_checkinteger ( L, 2 ) ) ;
  return 1 ;
}

/* function that calculates sum and average of its args */
static int x_add ( lua_State * const L )
{
  int i ;
  lua_Number sum = 0.0 ;
  const int n = lua_gettop ( L ) ; /* number of arguments */

  for ( i = 1 ; i <= n ; ++ i ) {
    if ( 0 == lua_isnumber ( L, i ) ) {
      lua_pushliteral ( L, "incorrect argument" ) ;
      return lua_error ( L ) ;
    }

    sum += lua_tonumber ( L, i ) ;
  }

  lua_pushnumber ( L, sum ) ;
  lua_pushnumber ( L, sum / n ) ;
  return 2 ; /* number of results */
}

/* function that tries to get the specified subarray of a table
 * similar to string . sub(). it does the same that function
 * does for strings. so indexing starts at 1, not at 0.
 * the result is always a (possibly empty) table */
static int x_subarr ( lua_State * const L )
{
  int i = 1, j = 0, k = 1 ;
  size_t s = 0 ;

  /* was the required table passed as first arg ? */
  luaL_checktype ( L, 1, LUA_TTABLE ) ;
  /* get it's length without invoking metamethods */
  s = lua_rawlen ( L, 1 ) ;
  i = (int) luaL_checkinteger ( L, 2 ) ;
  j = (int) luaL_optinteger ( L, 3, s ) ;
  /* push a new empty result table onto the stack */
  lua_newtable ( L ) ;

  /* return that fresh empty table if the given table
   * is empty or no sequence */
  if ( 0 >= s ) {
    return 1 ;
  }
  /* translate given negative index args */
  if ( 0 > i ) {
    i += 1 + s ;
  }
  if ( 0 > j ) {
    j += 1 + s ;
  }
  /* ensure the first given index i (possibly after
   * translation) is at least 1 */
  if ( 1 > i ) {
    i = 1 ;
  }
  /* esnsure j is not greater than the table (raw) length s */
  if ( j > s ) {
    j = s ;
  }
  /* if the (corrected) i is greater than the (corrected) j
   * return the newly created empty table on top of the
   * stack as result */
  if ( i > j ) {
    return 1 ;
  }

  for ( k = i ; k <= j ; ++ k ) {
    /* the first argument (which is at stack index 1) was the given table */
    (void) lua_rawgeti ( L, 1, k ) ;
    /* now the element arg1 [ k ] is on the stack and we set index k
    ** of the new table above the old given one to it's value */
    lua_rawseti ( L, -2, k ) ;
  }

  return 1 ;
}

/* try to parse the given integer literal string as integer */
static int x_strtoi ( lua_State * const L )
{
  const char * const str = luaL_checkstring ( L, 1 ) ;

  if ( str && * str ) {
    int e = 0 ;
    long int i = 0 ;
    char * end = NULL ;
    int base = luaL_optinteger ( L, 2, 0 ) ;

    if ( ( 1 == base ) || ( 0 > base ) || ( 36 < base ) ) {
      return luaL_argerror ( L, 2, "invalid representation base" ) ;
    }

    errno = 0 ;
    i = strtol ( str, & end, base ) ;
    e = errno ;
    lua_pushinteger ( L, i ) ;

    if ( 0 < e ) {
      (void) lua_pushstring ( L, strerror ( e ) ) ;
      lua_pushinteger ( L, e ) ;
      return 3 ;
    }

    if ( str == end ) {
      (void) lua_pushfstring ( L,
        "String \"%s\" contains no base %d digits", str, base ) ;
      return 2 ;
    }

    if ( '\0' != * end ) {
      (void) lua_pushfstring ( L,
        "String \"%s\" contains further characters after number: \"%s\"",
       	str, end ) ;
      return 2 ;
    }

    return 1 ;
  }

  return luaL_argerror ( L, 1, "integer literal string required" ) ;
}

/* convert a string consisting of on octal integer literal
 * to an int. useful for handling unix modes since Lua lacks
 * octal integer literals.
 */
static int x_octintstr ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;
    unsigned int u ;
    const char * str = NULL ;

    for ( i = 1 ; n >= i ; ++ i ) {
      str = luaL_checkstring ( L, i ) ;

      if ( str && * str && ( 1 == sscanf ( str, "%o", & u ) ) ) {
        lua_pushinteger ( L, (lua_Unsigned) u ) ;
        lua_replace ( L, i ) ;
      } else {
        return luaL_argerror ( L, i, "invalid octal integer string" ) ;
      }
    }

    return n ;
  }

  return luaL_error ( L, "octal integer string required" ) ;
}

/* remove \n and anything after */
static int x_chomp ( lua_State * const L )
{
  const char * str = luaL_checkstring ( L, 1 ) ;

  if ( str && * str ) {
    char * s = (char *) str ;
    (void) chomp ( s ) ;

    if ( s && * s ) {
      (void) lua_pushstring ( L, s ) ;
      return 1 ;
    }
  }

  return 0 ;
}

/* returns error msg belonging to current errno or given int arg */
static int x_get_errno ( lua_State * const L )
{
  const int e = errno ;

  lua_pushinteger ( L, e ) ;
  (void) lua_pushstring ( L, strerror ( e ) ) ;

  return 2 ;
}

/* sets the current errno to given int arg or 0 */
static int u_strerror ( lua_State * const L )
{
  const int e = luaL_checkinteger ( L, 1 ) ;

  if ( 0 < e ) {
    char * str = strerror ( e ) ;

    if ( str && * str ) {
      (void) lua_pushstring ( L, str ) ;
    } else {
      (void) lua_pushfstring ( L, "unknown error code %d", e ) ;
    }

    return 1 ;
  }

  return luaL_argerror ( L, 1, "positive error code required" ) ;
}

static int u_strerror_r ( lua_State * const L )
{
  const int e = luaL_checkinteger ( L, 1 ) ;

  if ( 0 < e ) {
    int i ;
    char buf [ 200 ] = { 0 } ;
#if defined (__GLIBC__) && defined (_GNU_SOURCE)
    char * const str = strerror_r ( e, buf, sizeof ( buf ) - 1 ) ;

    if ( str && * str ) {
      (void) lua_pushstring ( L, buf ) ;
      return 1 ;
    }

    i = errno ;
#else
    i = strerror_r ( e, buf, sizeof ( buf ) - 1 ) ;

    if ( 0 == i ) {
      (void) lua_pushstring ( L, buf ) ;
      return 1 ;
    }
#endif

    return luaL_error ( L, "strerror_r( %d ) failed: %s (errno %d)",
      e, strerror ( i ), i ) ;
  }

  return luaL_argerror ( L, 1, "positive error code required" ) ;
}

static int x_getprogname ( lua_State * const L )
{
#if defined (__GLIBC__) || defined (OSopenbsd) || defined (OSsolaris) || defined (OSnetbsd) || defined (OSfreebsd) || defined (OSdragonfly)
  const char * const pname =
# if defined (__GLIBC__) || defined (OSopenbsd)
    __progname
# else
    getprogname ()
# endif
    ;

  if ( pname && * pname ) {
    (void) lua_pushstring ( L, pname ) ;
    return 1 ;
  }

  return luaL_error ( L, "program name has not been set" ) ;
#else
  return luaL_error ( L, "platform not supported" ) ;
#endif
}

#if defined (OSLinux)
/* changes the root filesystem */
static int u_pivot_root ( lua_State * const L )
{
#  if defined (SYS_pivot_root)
  const char * new = luaL_checkstring ( L, 1 ) ;
  const char * old = luaL_checkstring ( L, 2 ) ;

  if ( new && old && * new && * old ) {
    long int i = syscall ( SYS_pivot_root, new, old ) ;

    if ( i ) {
      i = errno ;
      lua_pushinteger ( L, -1 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      lua_pushinteger ( L, 0 ) ;
      return 1 ;
    }
  }
#  endif

  return 0 ;
}
#endif

/* reads some bytes from /dev/urandom to create a random
 * integer and returns it
 */
static int x_get_urandom_int ( lua_State * const L )
{
  int e = 0 ;
  ssize_t s = 0 ;
  lua_Integer u = 0 ;
  const int fd = open ( "/dev/urandom", O_RDONLY | O_NONBLOCK | O_CLOEXEC ) ;

  if ( 0 > fd ) {
    return res_nil ( L ) ;
  }

  s = read ( fd, & u, sizeof ( u ) ) ;
  e = errno ;
  (void) close_fd ( fd ) ;

  if ( 0 > s ) {
    errno = e ;
    return res_nil ( L ) ;
  } else if ( 0 == s ) {
    lua_pushinteger ( L, 0 ) ;
  } else if ( 0 < s ) {
    lua_pushinteger ( L, u ) ;
  }

  /*
  if ( sizeof ( u ) > s )
  { lua_pushinteger ( L, 0 ) ; }
  else { lua_pushinteger ( L, u ) ; }
  */

  return 1 ;
}

#if defined (OSLinux)
/* get a random int from /dev/urandom */
static int x_get_random_int ( lua_State * const L )
{
  lua_Integer u = 0 ;
  const ssize_t s = getrandom ( & u, sizeof ( u ), GRND_NONBLOCK ) ;

  if ( 0 > s ) {
    return res_nil ( L ) ;
  } else if ( 0 == s ) {
    lua_pushinteger ( L, 0 ) ;
  } else if ( 0 < s ) {
    lua_pushinteger ( L, u ) ;
  }

  return 1 ;
}
#endif

/* wrapper to the uname syscall */
static int u_uname ( lua_State * const L )
{
  struct utsname utsn ;

  if ( uname ( & utsn ) ) {
    return res_nil ( L ) ;
  } else {
    /* lua_createtable ( L, 0, 5 ) ; */
    lua_newtable ( L ) ;

    (void) lua_pushliteral ( L, "os" ) ;
    (void) lua_pushstring ( L, utsn . sysname ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "sysname" ) ;
    (void) lua_pushstring ( L, utsn . sysname ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "host" ) ;
    (void) lua_pushstring ( L, utsn . nodename ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "nodename" ) ;
    (void) lua_pushstring ( L, utsn . nodename ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "arch" ) ;
    (void) lua_pushstring ( L, utsn . machine ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "machine" ) ;
    (void) lua_pushstring ( L, utsn . machine ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "os_version" ) ;
    (void) lua_pushstring ( L, utsn . version ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "version" ) ;
    (void) lua_pushstring ( L, utsn . version ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "os_release" ) ;
    (void) lua_pushstring ( L, utsn . release ) ;
    lua_rawset ( L, -3 ) ;

    (void) lua_pushliteral ( L, "release" ) ;
    (void) lua_pushstring ( L, utsn . release ) ;
    lua_rawset ( L, -3 ) ;
  }

  return 1 ;
}

/* wrapper to acct (process accounting) */
static int u_acct ( lua_State * const L )
{
  const char * fname = luaL_optstring ( L, 1, NULL ) ;

  return res_bool_zero ( L, acct ( fname ) ) ;
}

/* wrapper to quotactl */
/*
static int u_quotactl ( lua_State * const L )
{
#if defined (OSLinux)
#elif defined (OSsolaris)
#else
#endif
  return 0 ;
}
*/

/* wrapper to sethostname(2) to set/change the systen's host name */
static int u_sethostname ( lua_State * const L )
{
  const char * name = luaL_checkstring ( L, 1 ) ;

  if ( name && * name ) {
    size_t s = strlen ( name ) ;
    s = ( HOST_NAME_MAX < s ) ? HOST_NAME_MAX : s ;

    if ( 0 < s ) {
      if ( sethostname ( name, s ) ) {
        const int e = errno ;
        return luaL_error ( L, "sethostname() failed: %s (errno %d)",
          strerror ( e ), e ) ;
      }

      return 0 ;
    }
  }

  return luaL_error ( L, "non empty host name required" ) ;
}

/* wrapper to gethostname(2) */
static int u_gethostname ( lua_State * const L )
{
  char buf [ 2 + HOST_NAME_MAX ] = { 0 } ;

  if ( gethostname ( buf, 1 + HOST_NAME_MAX ) ) {
    const int e = errno ;
    return luaL_error ( L, "gethostname() failed: %s (errno %d)",
      strerror ( e ), e ) ;
  } else if ( buf [ 0 ] ) {
    (void) lua_pushstring ( L, buf ) ;
    return 1 ;
  }

  return 0 ;
}

/* wrapper to setdomainname(2) */
static int u_setdomainname ( lua_State * const L )
{
#ifdef OSLinux
  const char * name = luaL_checkstring ( L, 1 ) ;

  if ( name && * name ) {
    if ( setdomainname ( name, strlen ( name ) ) ) {
      const int e = errno ;
      return luaL_error ( L, "setdomainname() failed: %s (errno %d)",
        strerror ( e ), e ) ;
    }

    return 0 ;
  }

  return luaL_error ( L, "non empty domain name required" ) ;
#else
  return luaL_error ( L, "platform not supported" ) ;
#endif
}

/* wrapper to getdomainname(2) */
static int u_getdomainname ( lua_State * const L )
{
#ifdef OSLinux
  char buf [ 66 ] = { 0 } ;

  if ( getdomainname ( buf, 65 ) ) {
    const int e = errno ;
    return luaL_error ( L, "getdomainname() failed: %s (errno %d)",
      strerror ( e ), e ) ;
  } else if ( buf [ 0 ] ) {
    (void) lua_pushstring ( L, buf ) ;
    return 1 ;
  }

  return 0 ;
#else
  return luaL_error ( L, "platform not supported" ) ;
#endif
}

static int u_gethostid ( lua_State * const L )
{
  lua_pushinteger ( L, gethostid () ) ;
  return 1 ;
}

static int u_sethostid ( lua_State * const L )
{
  long int i = luaL_checkinteger ( L, 1 ) ;

  if ( sethostid ( i ) ) {
    const int e = errno ;
    return luaL_error ( L, "sethostid() failed: %s (errno %d)",
      strerror ( e ), e ) ;
  }

  return 0 ;
}

