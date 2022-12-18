/*
 * (POSIX) regex related functions
 */

/* upper limit for the number of saved regex subexpression matches */
#define NSUB	64

/* See if a given string contains/matches the given regex pattern.
 * Does not return matching substrings. It only returns true
 * (string contains a matching substring) or false.
 */
static int simple_regmatch ( lua_State * L )
{
  const char * pat = luaL_checkstring ( L, 1 ) ;
  const char * str = luaL_checkstring ( L, 2 ) ;

  if ( pat && str && * pat && * str ) {
    regex_t preg ;
    int r = regcomp ( & preg, pat, REG_NOSUB | REG_EXTENDED ) ;

    if ( r ) {
      /* pattern failed to compile */
      char errbuf [ 256 ] = { 0 } ;
      (void) regerror ( r, & preg, errbuf, sizeof ( errbuf ) - 1 ) ;
      regfree ( & preg ) ;
      lua_pushboolean ( L, 0 ) ;
      (void) lua_pushstring ( L, errbuf ) ;
      lua_pushinteger ( L, r ) ;
      return 3 ;
    }

    r = regexec ( & preg, str, 0, NULL, 0 ) ;
    regfree ( & preg ) ;
    lua_pushboolean ( L, r ? 0 : 1 ) ;
    return 1 ;
  }

  return luaL_error ( L, "regex pattern and string arguments required" ) ;
}

/* regex wrapper function that returns (substring) matches */
/* TODO: complete it */
static int regmatch ( lua_State * L )
{
  int r = 0 ;
  regex_t preg ;
  const char * re = luaL_checkstring ( L, 1 ) ;
  const char * str = luaL_checkstring ( L, 2 ) ;

  if ( ! ( re && str ) ) {
    lua_pushboolean ( L, 0 ) ;
    return 1 ;
  }

  r = regcomp ( & preg, re, REG_EXTENDED ) ;

  if ( r ) {
    /* pattern failed to compile */
    char errbuf [ 128 ] = { 0 } ;
    r = regerror ( r, & preg, errbuf, 127 ) ;
    regfree ( & preg ) ;
    lua_pushboolean ( L, 0 ) ;
    (void) lua_pushstring ( L, errbuf ) ;
    return 2 ;
  } else {
    regmatch_t pmatch ;
    r = regexec ( & preg, str, 1, & pmatch, 0 ) ;
    regfree ( & preg ) ;

    if ( r ) {
      /* no match or error */
      lua_pushboolean ( L, 0 ) ;
      return 1 ;
    } else if ( 0 <= pmatch . rm_so ) {
      /* pattern matched string */
      luaL_Buffer b ;
      lua_pushboolean ( L, 1 ) ;
      luaL_buffinit ( L, & b ) ;
      luaL_addlstring ( & b, str + pmatch . rm_so,
        pmatch . rm_eo - pmatch . rm_so ) ;
      luaL_addchar ( & b, '\0' ) ;
      luaL_pushresult ( & b ) ;
      return 2 ;
#if 0
      int i, j = 0 ;
      lua_pushboolean ( L, 1 ) ;
      for ( i = 0 ; i < 16 ; ++ i ) {
        if ( 0 <= pmatch [ i ] . rm_so ) {
          ++ j ;
          (void) lua_pushstring ( L, str ) ;
        }
      } /* end for */
      lua_pushinteger ( L, j ) ;
#endif
    } else {
      /* pattern matched string */
      lua_pushboolean ( L, 1 ) ;
      return 1 ;
    }
  }

  return 0 ;
}

/* matches a given POSIX regex pattern against a given string */
static int rex_match ( lua_State * L, const char * const pat,
  const char * const str, int f, int f2 )
{
  if ( pat && str && * pat && * str ) {
    int i = 0 ;
    regex_t re ;

    /* always use extended POSIX regex matching */
    f |= REG_EXTENDED ;
    i = regcomp ( & re, pat, f ) ;

    if ( i ) {
      /* the regex pattern failed to compile */
      char buf [ 256 ] = { 0 } ;

      (void) regerror ( i, & re, buf, sizeof ( buf ) - 1 ) ;
      /* is that ok for a non compiling pattern ? */
      regfree ( & re ) ;
      (void) lua_pushnil ( L ) ;
      (void) lua_pushstring ( L, buf ) ;
      (void) lua_pushinteger ( L, i ) ;
      return 3 ;
    } else {
      const char * s = str ;
      regoff_t off, len ;
      regmatch_t pmatch [ 1 + NSUB ] ;
      i = 0 ;

      while ( 0 == regexec ( & re, s, ARRAY_SIZE( pmatch ), pmatch, f2 ) )
      {
        int j = 0 ;

        if ( 0 == i ) {
          lua_newtable ( L ) ;
        }

        off = pmatch [ 0 ] . rm_so + ( s - str ) ;
        len = pmatch [ 0 ] . rm_eo - pmatch [ 0 ] . rm_so ;
	/*
  printf("#%d:\n", i);
  printf("offset = %jd; length = %jd\n", (intmax_t) off, (intmax_t) len);
  printf("substring = \"%.*s\"\n", len, s + pmatch[0].rm_so);
        */
        (void) lua_pushlstring ( L, s + pmatch [ 0 ] . rm_so, len ) ;
        lua_rawseti ( L, -2, 1 + i ) ;

        for ( j = 1 ; NSUB >= j ; ++ j ) {
          if ( 0 > pmatch [ j ] . rm_so ) { break ; }
          len = pmatch [ j ] . rm_eo - pmatch [ j ] . rm_so ;
          (void) lua_pushlstring ( L, s + pmatch [ j ] . rm_so, len ) ;
          lua_rawseti ( L, -2, 1 + i + j ) ;
        }

        s += pmatch [ 0 ] . rm_eo ;
        ++ i ;
      }

      if ( 0 < i ) {
      } else {
        lua_pushnil ( L ) ;
      }

      return 1 ;
    }
  }

  return luaL_error ( L, "regex pattern and string required" ) ;
}

/* case insensitive regex match */
static int l_ncgrep ( lua_State * L )
{
  int i = lua_gettop ( L ) ;

  if ( 1 < i ) {
    const char * pat = luaL_checkstring ( L, 1 ) ;
    const char * str = luaL_checkstring ( L, 2 ) ;

    if ( pat && str && * pat && * str ) {
      i = 0 ;
      i = luaL_optinteger ( L, 3, REG_ICASE ) ;
      i |= REG_EXTENDED | REG_ICASE ;
      return rex_match ( L, pat, str, i, 0 ) ;
    } else {
      return luaL_error ( L, "invalid string args" ) ;
    }
  }

  return luaL_error ( L, "not enough args" ) ;
}

/* matches a regex pattern against a given string */
static int l_grep ( lua_State * L )
{
  int i = lua_gettop ( L ) ;

  if ( 1 < i ) {
    const char * pat = luaL_checkstring ( L, 1 ) ;
    const char * str = luaL_checkstring ( L, 2 ) ;

    if ( pat && str && * pat && * str ) {
      i = 0 ;
      i = luaL_optinteger ( L, 3, 0 ) ;
      i |= REG_EXTENDED ;
      return rex_match ( L, pat, str, i, 0 ) ;
    } else {
      return luaL_error ( L, "invalid string args" ) ;
    }
  }

  return luaL_error ( L, "missing args" ) ;
}

