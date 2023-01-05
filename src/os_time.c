/*
 * time related wrapper functions
 * ADD: clock_gettime, timer_settimer etc
 */

/* wrapper function for the utime(2) syscall */
static int u_utime ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    const int n = lua_gettop ( L ) ;

    if ( 1 < n ) {
      struct utimbuf times ;
      times . modtime = luaL_checkinteger ( L, 2 ) ;

      if ( 2 < n ) {
        times . actime = luaL_checkinteger ( L, 3 ) ;
      } else {
        times . actime = time ( NULL ) ;
      }

      return res_bool_zero ( L, utime ( path, & times ) ) ;
    } else {
      return res_bool_zero ( L, utime ( path, NULL ) ) ;
    }
  }

  return luaL_argerror ( L, 1, "filename required" ) ;
}

static int do_utimes ( lua_State * const L, const char s )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    const int n = lua_gettop ( L ) ;

    if ( 4 < n ) {
      struct timeval times [ 2 ] ;
      times [ 0 ] . tv_sec = luaL_checkinteger ( L, 2 ) ;
      times [ 0 ] . tv_usec = luaL_checkinteger ( L, 3 ) ;
      times [ 1 ] . tv_sec = luaL_checkinteger ( L, 4 ) ;
      times [ 1 ] . tv_usec = luaL_checkinteger ( L, 5 ) ;
      return res_bool_zero ( L,
        s ? lutimes ( path, times ) : utimes ( path, times ) ) ;
    } else {
      return res_bool_zero ( L,
        s ? lutimes ( path, NULL ) : utimes ( path, NULL ) ) ;
    }
  }

  return luaL_argerror ( L, 1, "filename required" ) ;
}

/* wrapper function for the utimes(2) syscall */
static int u_utimes ( lua_State * const L )
{
  return do_utimes ( L, 0 ) ;
}

/* wrapper function for the lutimes(2) syscall */
static int u_lutimes ( lua_State * const L )
{
  return do_utimes ( L, 1 ) ;
}

/* wrapper function for the futimes(2) syscall */
static int u_futimes ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;
  const int fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= fd ) {
    if ( 4 < n ) {
      struct timeval times [ 2 ] ;
      times [ 0 ] . tv_sec = luaL_checkinteger ( L, 2 ) ;
      times [ 0 ] . tv_usec = luaL_checkinteger ( L, 3 ) ;
      times [ 1 ] . tv_sec = luaL_checkinteger ( L, 4 ) ;
      times [ 1 ] . tv_usec = luaL_checkinteger ( L, 5 ) ;
      return res_bool_zero ( L, futimes ( fd, times ) ) ;
    } else {
      return res_bool_zero ( L, futimes ( fd, NULL ) ) ;
    }
  }

  return luaL_argerror ( L, 1, "valid file descriptor required" ) ;
}

static void add_futimens_flags ( lua_State * const L )
{
  L_ADD_CONST( L, UTIME_NOW )
  L_ADD_CONST( L, UTIME_OMIT )
}

/* wrapper function for the futimens(2) syscall */
static int u_futimens ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;
  const int fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= fd ) {
    if ( 4 < n ) {
      struct timespec times [ 2 ] ;
      times [ 0 ] . tv_sec = luaL_checkinteger ( L, 2 ) ;
      times [ 0 ] . tv_nsec = luaL_checkinteger ( L, 3 ) ;
      times [ 1 ] . tv_sec = luaL_checkinteger ( L, 4 ) ;
      times [ 1 ] . tv_nsec = luaL_checkinteger ( L, 5 ) ;
      return res_bool_zero ( L, futimens ( fd, times ) ) ;
    } else {
      return res_bool_zero ( L, futimens ( fd, NULL ) ) ;
    }
  }

  return luaL_argerror ( L, 1, "valid file descriptor required" ) ;
}

/* wrapper function for the alarm(2) syscall */
static int u_alarm ( lua_State * const L )
{
  lua_pushinteger ( L, (lua_Unsigned) alarm (
    (lua_Unsigned) luaL_checkinteger ( L, 1 )
  ) ) ;

  return 1 ;
}

/* wrapper function for the ualarm(3) syscall */
static int u_ualarm ( lua_State * const L )
{
  lua_pushinteger ( L, (lua_Unsigned) ualarm (
    (lua_Unsigned) luaL_checkinteger ( L, 1 ),
    (lua_Unsigned) luaL_optinteger ( L, 2, 0 )
  ) ) ;

  return 1 ;
}

/* wrapper function for getitimer */
static int u_getitimer ( lua_State * const L )
{
  int i = 0, e = 0 ;
  struct itimerval it ;

  i = luaL_checkinteger ( L, 1 ) ;
  i = getitimer ( i, & it ) ;
  e = errno ;
  lua_pushinteger ( L, i ) ;
  lua_pushinteger ( L, e ) ;
  lua_pushinteger ( L, it . it_interval . tv_sec ) ;
  lua_pushinteger ( L, it . it_interval . tv_usec ) ;
  lua_pushinteger ( L, it . it_value . tv_sec ) ;
  lua_pushinteger ( L, it . it_value . tv_usec ) ;

  return 6 ;
}

/* wrapper function for setitimer */
static int u_setitimer ( lua_State * const L )
{
  int i = 0, e = 0, r = 0 ;
  struct itimerval it, it2 ;
  const int n = lua_gettop ( L ) ;

  if ( 2 > n ) { return 0 ; }

  i = luaL_checkinteger ( L, 1 ) ;
  r = getitimer ( i, & it ) ;
  e = errno ;
  r = luaL_checkinteger ( L, 2 ) ;
  it . it_value . tv_sec = r ;
  r = setitimer ( i, & it, & it2 ) ;
  e = errno ;
  lua_pushinteger ( L, r ) ;
  lua_pushinteger ( L, e ) ;
  lua_pushinteger ( L, it . it_interval . tv_sec ) ;
  lua_pushinteger ( L, it . it_interval . tv_usec ) ;
  lua_pushinteger ( L, it . it_value . tv_sec ) ;
  lua_pushinteger ( L, it . it_value . tv_usec ) ;

  return 6 ;
}

/* wrapper function for timer_create(2) */
/*
static int u_timer_create ( lua_State * const L )
{
  return 0 ;
}
*/

extern int daylight ;
extern long int timezone ;
extern char * tzname [ 2 ] ;

static int Stzset ( lua_State * L )
{
  tzset () ;
  return 0 ;
}

static int Ltzget ( lua_State * L )
{
  lua_newtable ( L ) ;
  tzset () ;

  (void) lua_pushliteral ( L, "daylight" ) ;
  lua_pushboolean ( L, daylight ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "timezone" ) ;
  lua_pushinteger ( L, timezone ) ;
  lua_rawset ( L, -3 ) ;

  if ( tzname [ 0 ] && tzname [ 0 ] [ 0 ] ) {
    (void) lua_pushliteral ( L, "tzname" ) ;
    (void) lua_pushstring ( L, tzname [ 0 ] ) ;
    lua_rawset ( L, -3 ) ;
  }

  if ( tzname [ 1 ] && tzname [ 1 ] [ 0 ] ) {
    (void) lua_pushliteral ( L, "daylight_tzname" ) ;
    (void) lua_pushstring ( L, tzname [ 1 ] ) ;
    lua_rawset ( L, -3 ) ;
  }

  return 1 ;
}

static int Stime ( lua_State * L )
{
  lua_pushinteger ( L, time ( NULL ) ) ;
  return 1 ;
}

/* stime() is deprecated
static int Sstime ( lua_State * L )
{
  time_t t = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= t ) {
    return res_zero ( L, stime ( & t ) ) ;
  }

  return luaL_argerror ( L, 1, "non negative integer required" ) ;
}
*/

static int Sctime ( lua_State * L )
{
  const time_t t = time ( NULL ) ;
  const char * ts = ctime ( & t ) ;

  if ( ts && * ts ) {
    lua_pushstring ( L, ts ) ;
    return 1 ;
  }

  return 0 ;
}

/*
static int Sftime ( lua_State * L )
{
  struct timeb tb ;

  (void) ftime ( & tb ) ;
  lua_pushinteger ( L, tb . time ) ;
  lua_pushinteger ( L, tb . millitm ) ;
  lua_pushinteger ( L, tb . timezone ) ;
  lua_pushinteger ( L, tb . dstflag ) ;
  return 4 ;
}
*/

static int Sclock ( lua_State * L )
{
  lua_pushinteger ( L, clock () ) ;
  return 1 ;
}

static int Stimes ( lua_State * L )
{
  struct tms tm ;
  const clock_t c = times ( & tm ) ;

  if ( 0 > c ) {
    const int i = errno ;
    lua_pushinteger ( L, -1 ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  }

  lua_pushinteger ( L, c ) ;
  lua_pushinteger ( L, tm . tms_utime ) ;
  lua_pushinteger ( L, tm . tms_stime ) ;
  lua_pushinteger ( L, tm . tms_cutime ) ;
  lua_pushinteger ( L, tm . tms_cstime ) ;
  return 5 ;
}

static int res_usage ( lua_State * L, const int what )
{
  struct rusage us ;

  if ( getrusage ( what, & us ) ) {
    const int i = errno ;
    lua_pushnil ( L ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  }

  /* create a new table to hold the result */
  lua_newtable ( L ) ;

  (void) lua_pushliteral ( L, "utime_sec" ) ;
  lua_pushinteger ( L, us . ru_utime . tv_sec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "utime_usec" ) ;
  lua_pushinteger ( L, us . ru_utime . tv_usec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "stime_sec" ) ;
  lua_pushinteger ( L, us . ru_stime . tv_sec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "stime_usec" ) ;
  lua_pushinteger ( L, us . ru_stime . tv_usec ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "maxrss" ) ;
  lua_pushinteger ( L, us . ru_maxrss ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "minflt" ) ;
  lua_pushinteger ( L, us . ru_minflt ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "maxflt" ) ;
  lua_pushinteger ( L, us . ru_majflt ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "inblock" ) ;
  lua_pushinteger ( L, us . ru_inblock ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "oublock" ) ;
  lua_pushinteger ( L, us . ru_oublock ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "nvcsw" ) ;
  lua_pushinteger ( L, us . ru_nvcsw ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "nivcsw" ) ;
  lua_pushinteger ( L, us . ru_nivcsw ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

static int Lgetrusage_proc ( lua_State * L )
{
  return res_usage ( L, RUSAGE_SELF ) ;
}

static int Lgetrusage_children ( lua_State * L )
{
  return res_usage ( L, RUSAGE_CHILDREN ) ;
}

#ifdef RUSAGE_THREAD
static int Lgetrusage_thread ( lua_State * L )
{
  return res_usage ( L, RUSAGE_THREAD ) ;
}
#endif

static int Sgettimeofday ( lua_State * L )
{
  struct timeval tv ;

  if ( gettimeofday ( & tv, NULL ) ) {
    const int i = errno ;
    lua_pushinteger ( L, -1 ) ;
    lua_pushinteger ( L, i ) ;
  } else {
    lua_pushinteger ( L, tv . tv_sec ) ;
    lua_pushinteger ( L, tv . tv_usec ) ;
  }

  return 2 ;
}

static int Ssettimeofday ( lua_State * L )
{
  struct timeval tv ;

  tv . tv_sec = (lua_Unsigned) luaL_checkinteger ( L, 1 ) ;
  tv . tv_usec = (lua_Unsigned) luaL_optinteger ( L, 2, 0 ) ;
  return res_zero ( L, settimeofday ( & tv, NULL ) ) ;
}

#if defined (_POSIX_TIMERS) && (0 < _POSIX_TIMERS)
static int Sclock_getcpuclockid ( lua_State * L )
{
  int i = 0 ;
  clockid_t ci ;

  i = luaL_optinteger ( L, 1, 0 ) ;
  i = ( 0 < i ) ? i : 0 ;
  i = clock_getcpuclockid ( i, & ci ) ;
  lua_pushinteger ( L, i ) ;
  lua_pushinteger ( L, ci ) ;
  return 2 ;
}

static int Sclock_getres ( lua_State * L )
{
  int i, e = 0 ;
  struct timespec ts ;

  i = luaL_optinteger ( L, 1, CLOCK_REALTIME ) ;
  i = clock_getres ( i, & ts ) ;
  e = errno ;
  lua_pushinteger ( L, i ) ;

  if ( i ) {
    lua_pushinteger ( L, e ) ;
    return 2 ;
  } else {
    lua_pushinteger ( L, ts . tv_sec ) ;
    lua_pushinteger ( L, ts . tv_nsec ) ;
    return 3 ;
  }

  return 1 ;
}

static int Sclock_gettime ( lua_State * L )
{
  int i, e = 0 ;
  struct timespec ts ;

  i = luaL_optinteger ( L, 1, CLOCK_REALTIME ) ;
  i = clock_gettime ( i, & ts ) ;
  e = errno ;
  lua_pushinteger ( L, i ) ;

  if ( i ) {
    lua_pushinteger ( L, e ) ;
    return 2 ;
  } else {
    lua_pushinteger ( L, ts . tv_sec ) ;
    lua_pushinteger ( L, ts . tv_nsec ) ;
    return 3 ;
  }

  return 1 ;
}

static int Sclock_settime ( lua_State * L )
{
  int i, e = 0 ;
  struct timespec ts ;

  ts . tv_sec = (lua_Unsigned) luaL_checkinteger ( L, 1 ) ;
  ts . tv_nsec = (lua_Unsigned) luaL_checkinteger ( L, 2 ) ;
  i = luaL_optinteger ( L, 3, CLOCK_REALTIME ) ;
  i = clock_settime ( i, & ts ) ;
  e = errno ;
  lua_pushinteger ( L, i ) ;

  if ( i ) {
    lua_pushinteger ( L, e ) ;
    return 2 ;
  }

  return 1 ;
}
#endif

/*
 * adjust the system clock

static int hwclock_runs_in_utc ( void )
{
  FILE * fp = fopen ( "/etc/adjtime", "r" ) ;

  if ( fp ) {
    int i = 0, r = 0 ;
    char buf [ 128 ] = { 0 } ;

    while ( NULL != fgets ( buf , sizeof ( buf ) - 1, fp ) ) {
      ++ i ;

      if ( 3 >= i ) {
        char * str = strtok ( buf, " \t\n" ) ;

        if ( str && * str ) {
          r = strcmp ( "LOCAL", str ) ? 1 : 0 ;
        }

        break ;
      }
    }

    (void) fclose ( fp ) ;
    return r ;
  }

  return 1 ;
}
 */

