/*
 * wrapper functions for socket related syscalls
 *
 * public domain code
 */

static void add_socket_flags ( lua_State * const L )
{
  /* communication domains */
  L_ADD_CONST( L, AF_ALG )
  L_ADD_CONST( L, AF_APPLETALK )
  L_ADD_CONST( L, AF_ATMPVC )
  L_ADD_CONST( L, AF_AX25 )
  L_ADD_CONST( L, AF_UNIX )
  L_ADD_CONST( L, AF_LOCAL )
  L_ADD_CONST( L, AF_INET )
  L_ADD_CONST( L, AF_INET6 )
  L_ADD_CONST( L, AF_IPX )
  L_ADD_CONST( L, AF_KEY )
  L_ADD_CONST( L, AF_NETLINK )
  L_ADD_CONST( L, AF_PACKET )
  L_ADD_CONST( L, AF_X25 )

  /* socket types */
  L_ADD_CONST( L, SOCK_DGRAM )
  L_ADD_CONST( L, SOCK_PACKET )
  L_ADD_CONST( L, SOCK_RAW )
  L_ADD_CONST( L, SOCK_RDM )
  L_ADD_CONST( L, SOCK_SEQPACKET )
  L_ADD_CONST( L, SOCK_STREAM )
  L_ADD_CONST( L, SOCK_CLOEXEC )
  L_ADD_CONST( L, SOCK_NONBLOCK )
  L_ADD_CONST( L, SO_KEEPALIVE )
}

static int u_socketpair ( lua_State * const L )
{
  int sv [ 2 ] ;

  if ( socketpair (
    luaL_checkinteger ( L, 1 ),
    luaL_checkinteger ( L, 2 ),
    luaL_optinteger ( L, 3, 0 ),
    sv ) )
  {
    return res_nil ( L ) ;
  }

  lua_newtable ( L ) ;
  lua_pushinteger ( L, sv [ 0 ] ) ;
  lua_rawseti ( L, -2, 1 ) ;
  lua_pushinteger ( L, sv [ 1 ] ) ;
  lua_rawseti ( L, -2, 2 ) ;

  return 1 ;
}

static int u_socket ( lua_State * const L )
{
  const int fd = socket (
    luaL_checkinteger ( L, 1 ),
    luaL_checkinteger ( L, 2 ),
    luaL_optinteger ( L, 3, 0 ) ) ;

  if ( 0 > fd ) {
    return res_nil ( L ) ;
  }

  lua_pushinteger ( L, fd ) ;
  return 1 ;
}

static int u_bind ( lua_State * const L )
{
  return 1 ;
}

static int u_listen ( lua_State * const L )
{
  int i = luaL_checkinteger ( L, 1 ) ;
  int e = luaL_optinteger ( L, 2, 5 ) ;

  e = ( 0 < e ) ? e : 5 ;
  i = listen ( i, e ) ;
  e = errno ;
  lua_pushinteger ( L, i ) ;

  if ( i ) {
    lua_pushinteger ( L, e ) ;
    return 2 ;
  }

  return 1 ;
}

static int u_accept ( lua_State * const L )
{
  return 1 ;
}

#if 0
/* sets up the (Linux: abstract) unix domain socket the server
 * listens on for client requests
 */
static int serv_sock ( const char * path )
{
  if ( path && * path ) {
    int i = 0, fd = -1 ;
    struct sockaddr_un serv_ad ;
#if defined (OSLinux)
#else
#endif

    /* zero out the address struct before use */
    (void) memset ( & serv_ad, 0, sizeof ( serv_ad ) ) ;
    serv_ad . sun_family = AF_UNIX ;
#if defined (OSLinux)
    /* Linux: a leading NUL makes the unix domain socket abstract */
    serv_ad . sun_path [ 0 ] = '\0' ;
    (void) strncpy ( 1 + serv_ad . sun_path, path,
      sizeof ( serv_ad . sun_path ) - 2 ) ;
    fd = socket ( AF_UNIX,
      SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0 ) ;
#else
    (void) umask ( 00077 ) ;
    (void) strncpy ( serv_ad . sun_path, path,
      sizeof ( serv_ad . sun_path ) - 1 ) ;
    fd = socket ( AF_UNIX, SOCK_STREAM, 0 ) ;
    (void) unlink ( serv_ad . sun_path ) ;
#endif
    if ( 0 > fd ) { return -3 ; }
    /* set some fd flags */
    (void) fcntl ( fd, F_SETFD, FD_CLOEXEC ) ;
    (void) fcntl ( fd, F_SETFL, O_NONBLOCK ) ;
    /* bind socket to address before enabling credential passing
     * to avoid autobinding to an abstract unix socket on Linux
     */
    if ( bind ( fd, (struct sockaddr *) & serv_ad,
      sizeof ( struct sockaddr_un ) ) )
    { return -5 ; }
    /* set up credential passing for the socket where possible
     * (really necessary for the server socket ?)
     */
#if defined (OSLinux)
    i = 3 ;
    (void) setsockopt ( fd, SOL_SOCKET, SO_PASSCRED, & i, sizeof ( i ) ) ;
#elif defined (OSsolaris)
#elif defined (OSdragonfly)
#elif defined (OSfreebsd)
#elif defined (OSnetbsd)
#elif defined (OSopenbsd)
#else
    (void) chmod ( serv_ad . sun_path, 00700 ) ;
    i = getuid () ;
    if ( 1 > i ) { (void) umask ( 00022 ) ; }
#endif
    if ( listen ( fd, 5 ) ) { return -9 ; }
    else { return fd ; }
  }

  return -1 ;
}

/* accept incoming client connections on the server's unix domain socket */
static int Lsock_acc ( lua_State * L )
{
  const int sock_fd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 <= sock_fd ) {
    int i = 0, cfd = -3 ;
    struct sockaddr_un peer_ad ;
    socklen_t s = sizeof ( struct sockaddr_un ) ;
#if defined (__GLIBC__) && defined (_GNU_SOURCE)
    struct ucred uc ;
#endif
#if defined (OSLinux)
#elif defined (OSfreebsd)
#elif defined (OSdragonfly)
#elif defined (OSnetbsd)
#elif defined (OSopenbsd)
#elif defined (OSsolaris)
#else
#endif

#if defined (__GLIBC__) && defined (_GNU_SOURCE)
    cfd = accept4 ( serv_fd, (struct sockaddr *) & peer_ad, & s,
      SOCK_NONBLOCK | SOCK_CLOEXEC ) ;
#else
    cfd = accept ( serv_fd, (struct sockaddr *) & peer_ad, & s ) ;
#endif
    i = errno ;
    lua_pushinteger ( L, cfd ) ;
    if ( 0 > cfd ) {
      lua_pushinteger ( L, i ) ;
      return 2 ;
    }

    /* retrieve peer credentials where possible */
#if defined (OSLinux)
#  if defined (__GLIBC__) && defined (_GNU_SOURCE)
    s = sizeof ( struct ucred ) ;
    if ( getsockopt ( cfd, SOL_SOCKET, SO_PEERCRED, & uc, & s ) ) {
      /* the getsockopt call failed. no further questions, Sir ! */
      return 1 ;
    } else {
      lua_pushinteger ( L, uc . uid ) ;
      lua_pushinteger ( L, uc . gid ) ;
      lua_pushinteger ( L, uc . pid ) ;
      return 4 ;
    }
#  endif
#elif defined (OSfreebsd)
#elif defined (OSdragonfly)
#elif defined (OSnetbsd)
#elif defined (OSopenbsd)
#elif defined (OSsolaris)
#else
    s = sizeof ( struct sockaddr_un ) ;
    peer_ad . sun_path [ s - sizeof ( peer_ad . sun_family ) ] = '\0' ;
    /* the client socket will be unlinked after closing the connection */
    /* is this really necessary/a good idea ? */
    if ( peer_ad . sun_path [ 0 ] ) {
      (void) unlink ( peer_ad . sun_path ) ;
    }
#endif

    return 1 ;
  }

  return 0 ;
}

/* this function sets up the supervisor main loop */
static int Lsetup_sup_Linux ( lua_State * L )
{
  int r = 0, s = 0, i = umask ( 00022 ) ;
  struct rlimit rlim ;
  const uid_t myuid = getuid () ;
  const gid_t mygid = getgid () ;
  const pid_t mypid = getpid () ;
  const char * sockpath = luaL_checkstring ( L, 1 ) ;

  /* run onetime setup tasks now */
  /* disable core dumps by setting the SOFT (!!) limit to 0 */
  rlim . rlim_cur = 0 ;
  rlim . rlim_max = RLIM_INFINITY ;
  /* initialise global variables */
  sigs_ok = 0 ;
  sockfd = mqvid = mqfd = sigfd = -3 ;

  /* set the file creation mask */
  if ( myuid ) {
    /* not super user, set the most secure and confining umask */
    (void) umask ( 00077 ) ;
  }

  /* this SOFT limit can be raised again later */
  (void) setrlimit ( RLIMIT_CORE, & rlim ) ;

  /* drop possible privileges: reset euid/egid to real uid/gid */
  (void) setgid ( mygid ) ;
  (void) setuid ( myuid ) ;
  /* create a new session */
  i = setsid () ;
  (void) setpgid ( 0, 0 ) ;
#ifdef OSbsd
  if ( 1 > myuid && 0 < i ) { (void) setlogin ( "root" ) ; }
#endif

  /* setup signal handling behaviour */
  setup_sigs () ;
  sigs_ok = 3 ;
  /* change working directory to / */
  (void) chdir ( "/" ) ;

  /* setup the (Linux: abstract) unix domain socket we use for
   * client requests
   */
  if ( sock && * sock ) {
    if ( 0 <= sockfd ) { close_fd ( sockfd ) ; sockfd = -3 ; }
#ifndef OSLinux
    (void) unlink ( sock ) ;
#endif
    sockfd = serv_init ( sock ) ;
  }

  lua_pushinteger ( L, sockfd ) ;
  return 3 ;
}

/* Linux: abstract unix domain socket address:
 * if sysinit: "\0/pesi/sys"
 * else : "\0/pesi/userUID/pidPID"
 */
static int Lcheck_sock ( lua_State * L )
{
  if ( 0 <= sockfd ) {
    struct pollfd pfd ;
    int i, w = 2 ;

    pfd . fd = -1 ;
    pfd . events = POLLIN ;
    w = luaL_optinteger ( L, 2, 2 ) ;
    w = ( 0 < w ) ? w : 2 ;

    /* see if we got client requests on the socket fd */
    i = poll ( & pfd, 1, w * 1000 ) ;
    if ( 0 < i ) {
      if ( POLLIN & pfd . revents ) {
      } else if ( POLLHUP & pfd . revents ) {
      }
    }
  }

  return 0 ;
}

#endif /* #if 0 */

