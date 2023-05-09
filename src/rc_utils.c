/*
 * public domain
 */

/*
 * s6-hiercp
 * skalib rm_r ==> rmr
 */

#if 0
static void print_timestamp ( void )
{
  float num1, num2 ;
  char uptime_str [ 30 ] = { 0 } ;
  FILE * uptimefile = fopen ( "/proc/uptime", "r" ) ;

  if ( uptimefile ) {
    (void) fgets ( uptime_str, 20, uptimefile ) ;
    (void) fclose ( uptimefile ) ;

    sscanf ( uptime_str, "%f %f", & num1, & num2 ) ;
    sprintf ( uptime_str, "[ %.6f ]", num1 ) ;

    (void) write ( STDIN_FILENO, uptime_str, strlen ( uptime_str ) ) ;
  }
}

/*
 * Esc[2K                     - Erases the entire current line.
 * define delline()             fputs ( "\033[2K", stdout )
 */
static void do_print ( const int action, const char * fmt, ... )
{
  size_t len = 0 ;
  va_list ap ;
  char buf [ 80 ] = { 0 } ;
  const char success [] = " \e[1m[ OK ]\e[0m\n" ;
  const char failure [] = " \e[7m[FAIL]\e[0m\n" ;
  const char warning [] = " \e[7m[WARN]\e[0m\n" ;
  const char pending [] = " \e[1m[ \\/ ]\e[0m\n" ;
  const char dots [] = " ....................................................................." ;

  if ( fmt ) {
		va_start(ap, fmt);
		len = vsnprintf(buf, sizeof(buf), fmt, ap);
		va_end(ap);

		//delline();
		print_timestamp();

		write(STDERR_FILENO, "\r", 1);
		write(STDERR_FILENO, buf, len);
		write(STDERR_FILENO, dots, 60 - len); /* pad with dots. */
  }

	switch ( action ) {
	case -1 :
		break ;

	case 0 :
		write ( STDIN_FILENO, success, sizeof ( success ) ) ;
		break ;

	case 1 :
		write ( STDIN_FILENO, failure, sizeof ( failure ) ) ;
		break ;

	case 2 :
		write ( STDIN_FILENO, warning, sizeof ( warning ) ) ;
		break ;

	default :
		write ( STDIN_FILENO, pending, sizeof ( pending ) ) ;
		break ;
	}
}

static void print_desc ( char * action, char * desc )
{
  do_print ( -1, "%s%s", action ? action : "", desc ? desc : "" ) ;
}

static int print_result ( int fail )
{
  do_print ( ! ! fail, NULL ) ;
}
#endif

/* ensure utmp database files exist and have the right permissions */
/* i = open ( UTMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 00644 ) ;	*/

/* setup random seed */
static int x_init_urandom ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    int i ;
    mode_t m = umask ( 00077 ) ;
    FILE * fp = fopen ( path, "w" ) ;

    if ( fp ) {
      int j = 128 ;
      uint32_t prng = 0 ;

      srandom ( time ( NULL ) % 3600 ) ;

      while ( 0 < j -- ) {
        prng = random () ;

        for ( i = 0 ; sizeof ( prng ) > i ; ++ i ) {
          (void) fputc ( ( prng >> ( i * CHAR_BIT ) ) & UCHAR_MAX, fp ) ;
        }
      }

      (void) fclose ( fp ) ;
    } else {
      i = errno ;
      (void) umask ( m ) ;
      return luaL_error ( L, "could not open \"%s\" for writing: %s",
        path, strerror ( i ) ) ;
    }

    /* restore old umask */
    (void) umask ( m ) ;
  }

  return 0 ;
}

#if 0
/* check if a given pidfile exists and contains a valid pid */
static int x_check_pidfile ( lua_State * const L )
{
  const char * pidf = luaL_checkstring ( L, 1 ) ;

  if ( pidf && * pidf && is_fnr ( pidf ) ) {
    FILE * fp = fopen ( pidf, "r" ) ;

    if ( fp ) {
      int e = 0, i = -3, pid = -3 ;

      i = fscanf ( fp, "%d", & pid ) ;
      e = errno ;
      (void) fclose ( fp ) ;

      if ( 1 == i && 1 < pid && 0 == kill ( pid, 0 ) ) {
        /* the daemon process is up and running,
         * return it's pid to caller.
         */
        lua_pushinteger ( L, pid ) ;
        return 1 ;
      } else if ( 1 != i && EINTR == e ) {
        /* scanning the pid file failed because we were
         * interupted by a signal.
         */
      } else {
        /* scanning the pid file failed, so remove it */
        (void) remove ( pidf ) ;
      }
    }
  }

  lua_pushinteger ( L, -3 ) ;
  return 1 ;
}
#endif

