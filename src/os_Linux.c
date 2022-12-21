#if defined (OSLinux)

/*
 * wrapper/helper functions for Linux specific syscalls
 */

static int u_syncfs ( lua_State * const L )
{
  return res_bool_zero ( L, syncfs ( luaL_checkinteger ( L, 1 ) ) ) ;
}

static int Sgettid ( lua_State * const L )
{
  lua_pushinteger ( L, syscall ( SYS_gettid ) ) ;
  return 1 ;
}

/*
 * reboot(2) related
 */

#ifndef RB_AUTOBOOT
# define RB_AUTOBOOT		0x01234567
#endif

#ifndef RB_HALT_SYSTEM
# define RB_HALT_SYSTEM		0xcdef0123
#endif

#ifndef RB_POWER_OFF
# define RB_POWER_OFF		0x4321fedc
#endif

#ifndef RB_DISABLE_CAD
# define RB_DISABLE_CAD		0
#endif

#ifndef RB_ENABLE_CAD
# define RB_ENABLE_CAD		0x89abcdef
#endif

#ifndef RB_SW_SUSPEND
# define RB_SW_SUSPEND		0xd000fce1
#endif

#ifndef RB_KEXEC
# define RB_KEXEC		0x45584543
#endif

static void add_reboot_flags ( lua_State * const L )
{
  L_ADD_CONST( L, RB_AUTOBOOT )
  L_ADD_CONST( L, RB_HALT_SYSTEM )
  L_ADD_CONST( L, RB_POWER_OFF )
  L_ADD_CONST( L, RB_DISABLE_CAD )
  L_ADD_CONST( L, RB_ENABLE_CAD )
  L_ADD_CONST( L, RB_SW_SUSPEND )
  L_ADD_CONST( L, RB_KEXEC )
}

static int u_reboot ( lua_State * const L )
{
  return res_bool_zero ( L, reboot ( luaL_checkinteger ( L, 1 ) ) ) ;
}

static int l_sys_reboot ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_AUTOBOOT ) ) ;
}

static int l_sys_halt ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_HALT_SYSTEM ) ) ;
}

static int l_sys_poweroff ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_POWER_OFF ) ) ;
}

static int l_sys_hibernate ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_SW_SUSPEND ) ) ;
}

static int l_sys_kexec ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_KEXEC ) ) ;
}

static int l_sys_sak_off ( lua_State * const L )
{
  return res0( L, "reboot", reboot ( RB_DISABLE_CAD ) ) ;
}

static int l_sys_sak_on ( lua_State * const L )
{
  return res0( L, "reboot", reboot ( RB_ENABLE_CAD ) ) ;
}

/*
 * (u)mount(2) related
 */

/* constants used by mount(2) */
static void add_mount_flags ( lua_State * const L )
{
  L_ADD_CONST( L, MS_REMOUNT )
  L_ADD_CONST( L, MS_MOVE )
  L_ADD_CONST( L, MS_BIND )
  L_ADD_CONST( L, MS_SHARED )
  L_ADD_CONST( L, MS_PRIVATE )
  L_ADD_CONST( L, MS_SLAVE )
  L_ADD_CONST( L, MS_UNBINDABLE )
  L_ADD_CONST( L, MS_DIRSYNC )
  L_ADD_CONST( L, MS_LAZYTIME )
  L_ADD_CONST( L, MS_MANDLOCK )
  L_ADD_CONST( L, MS_NOATIME )
  L_ADD_CONST( L, MS_NODIRATIME )
  L_ADD_CONST( L, MS_NODEV )
  L_ADD_CONST( L, MS_NOEXEC )
  L_ADD_CONST( L, MS_NOSUID )
  L_ADD_CONST( L, MS_RDONLY )
  L_ADD_CONST( L, MS_REC )
  L_ADD_CONST( L, MS_RELATIME )
  L_ADD_CONST( L, MS_SILENT )
  L_ADD_CONST( L, MS_STRICTATIME )
  L_ADD_CONST( L, MS_SYNCHRONOUS )
}

/* wrapper function for the mount(2) syscall */
static int u_mount ( lua_State * const L )
{
  const char * const src = luaL_checkstring ( L, 1 ) ;
  const char * const mp = luaL_checkstring ( L, 2 ) ;
  const char * const type = luaL_checkstring ( L, 3 ) ;
  const lua_Unsigned f = (lua_Unsigned) luaL_optinteger ( L, 4, 0 ) ;
  const char * const data = luaL_optstring ( L, 5, NULL ) ;

  if ( src && mp && type && * src && * mp && * type ) {
    return res0 ( L, "mount", mount ( src, mp, type,
      ( 0 < f ) ? (unsigned long int) f : 0, data )
      ) ;
  }

  return luaL_error ( L, "invalid args" ) ;
}

/* constants used by umount2(2) */
static void add_umount2_flags ( lua_State * const L )
{
  L_ADD_CONST( L, MNT_DETACH )
  L_ADD_CONST( L, MNT_EXPIRE )
  L_ADD_CONST( L, MNT_FORCE )
  L_ADD_CONST( L, UMOUNT_NOFOLLOW )
}

/* wrapper function for the umount2(2) syscall */
static int Sumount2 ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i, f = (int) luaL_checkinteger ( L, 1 ) ;
    f = ( 0 < f ) ? f : 0 ;

    for ( i = 2 ; n >= i ; ++ i ) {
      const char * const mp = luaL_checkstring ( L, i ) ;

      if ( mp && * mp ) {
        if ( umount2 ( mp, f ) ) {
          return rep_err ( L, "umount2", errno ) ;
        }
      } else {
        return luaL_argerror ( L, i, "not a mount point path" ) ;
      }
    }

    return 0 ;
  }

  return luaL_error ( L, "integer flag bitmask and mount point paths required" ) ;
}

/* see if a given directory is a mountpoint using the mtab file
 * requires the procfs being mounted on /proc
 */
#if defined (__GLIBC__)
static int l_mtab_mount_point ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;
  const char * const mtab = luaL_optstring ( L, 2, "/proc/self/mounts" ) ;

  if ( path && mtab && * path && * mtab ) {
    FILE * const fp = setmntent ( mtab, "r" ) ;

    if ( fp ) {
      char r = 0 ;
      struct mntent * mep = NULL ;

      while ( NULL != ( mep = getmntent ( fp ) ) ) {
        if ( path [ 0 ] == mep -> mnt_dir [ 0 ]
          && 0 == strcmp ( mep -> mnt_dir, path ) )
        {
          r = 1 ;
          break ;
        }
      }

      (void) endmntent ( fp ) ;
      lua_pushboolean ( L, r ? 1 : 0 ) ;
      return 1 ;
    }

    return rep_err ( L, "setmntent", errno ) ;
  }

  return luaL_error ( L, "mount point and mtab paths required" ) ;
}
#endif

/*
 * swapo(n,ff)(2) related
 */

/* wrapper function for the swapoff(2) syscall */
static int u_swapoff ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L, swapoff ( path ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid swap path" ) ;
}

/* constants used by swapon(2) */
static void add_swapon_flags ( lua_State * const L )
{
  L_ADD_CONST( L, SWAP_FLAG_DISCARD )
  L_ADD_CONST( L, SWAP_FLAG_PREFER )
  L_ADD_CONST( L, SWAP_FLAG_PRIO_MASK )
  L_ADD_CONST( L, SWAP_FLAG_PRIO_SHIFT )
}

/* wrapper function for the swapon(2) syscall */
static int u_swapon ( lua_State * const L )
{
  const char * const path = luaL_checkstring ( L, 1 ) ;
  const int f = (int) luaL_optinteger ( L, 2, 0 ) ;

  if ( path && * path ) {
    return res_bool_zero ( L, swapon ( path, f ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid swap path" ) ;
}

/* binding for the unshare(2) Linux syscall */
static int u_unshare ( lua_State * const L )
{
  int f = luaL_optinteger ( L, 1, 0 ) ;

  f = f ? f :
    CLONE_FILES | CLONE_FS | CLONE_NEWIPC | CLONE_NEWNET
    | CLONE_NEWNS | CLONE_NEWUTS ;

  return res_bool_zero ( L, unshare ( f ) ) ;
}

/* constants used by setns(2) */
/* bindings for the setns(2) Linux syscall */
static int u_setns ( lua_State * const L )
{
  const int fd = luaL_checkinteger ( L, 1 ) ;
  const int t = luaL_optinteger ( L, 2, 0 ) ;

  if ( 0 <= fd ) {
    return res_bool_zero ( L, setns ( fd, t ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid fd" ) ;
}

#if 0
static int l_setns ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  const int t = luaL_optinteger ( L, 2, 0 ) ;

  if ( path && * path ) {
    const int fd = open ( path, O_RDONLY | O_CLOEXEC ) ;

    if ( 0 > fd ) {
      return res_false ( L ) ;
    } else {
      const int r = setns ( fd, t ) ;
      const int e = errno ;

      /* is it ok or even necessary to close the fd here? */
      while ( close ( fd ) && ( EINTR == errno ) ) { ; }

      if ( r ) {
        return rep_err ( L, "setns", e ) ;
      }

      lua_pushboolean ( L, 1 ) ;
      return 1 ;
    }
  }

  return luaL_argerror ( L, 1, "invalid ns path" ) ;
}
#endif

/*
 * caps related
 */

static int Scapget ( lua_State * const L )
{
#if defined (_LINUX_CAPABILITY_VERSION_3)
  int i = 0 ;
  struct __user_cap_header_struct hdr ;
  struct __user_cap_data_struct data ;

  i = luaL_optinteger ( L, 1, 0 ) ;
  hdr . pid = i ;
  hdr . version = _LINUX_CAPABILITY_VERSION_3 ;
  i = capget ( & hdr, & data ) ;

  if ( i && ( EINVAL == errno ) ) {
    i = capget ( & hdr, & data ) ;
  }

  if ( i ) {
    i = errno ;
    lua_pushinteger ( L, -1 ) ;
    lua_pushinteger ( L, i ) ;
    return 2 ;
  } else {
    lua_pushinteger ( L, 0 ) ;
    lua_pushinteger ( L, data . effective ) ;
    lua_pushinteger ( L, data . permitted ) ;
    lua_pushinteger ( L, data . inheritable ) ;
    return 4 ;
  }
#endif

  return 0 ;
}

static int Scapset ( lua_State * const L )
{
#if defined (_LINUX_CAPABILITY_VERSION_3)
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i = 0 ;
    struct __user_cap_header_struct hdr ;
    struct __user_cap_data_struct data ;

    if ( 3 < n ) {
      i = luaL_checkinteger ( L, 4 ) ;
    }

    hdr . pid = i ;
    hdr . version = _LINUX_CAPABILITY_VERSION_3 ;
    i = capget ( & hdr, & data ) ;

    if ( i && ( EINVAL == errno ) ) {
      i = capget ( & hdr, & data ) ;
    }

    if ( i ) {
      i = errno ;
      lua_pushinteger ( L, -1 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      if ( 0 < n ) {
        i = luaL_checkinteger ( L, 1 ) ;
        data . effective |= i ;
      }

      if ( 1 < n ) {
        i = luaL_checkinteger ( L, 2 ) ;
        data . permitted |= i ;
      }

      if ( 2 < n ) {
        i = luaL_checkinteger ( L, 3 ) ;
        data . inheritable |= i ;
      }

      return res_zero ( L, capset ( & hdr, & data ) ) ;
    }
  } else {
    return luaL_error ( L, "no args" ) ;
  }
#endif

  return 0 ;
}

/*
 * xattr - extended attributes
 * (lf)(g,s)etxattr. (lf)removexattr, (lf)listxattr
 */

/* make the result(s) of the sysinfo(2) function available to Lua code */
static int u_sysinfo ( lua_State * const L )
{
  struct sysinfo si ;

  if ( sysinfo ( & si ) ) {
    return res_nil ( L ) ;
  }

  lua_newtable ( L ) ;

  (void) lua_pushliteral ( L, "mem_unit" ) ;
  lua_pushinteger ( L, si . mem_unit ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "procs" ) ;
  lua_pushinteger ( L, si . procs ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "uptime" ) ;
  lua_pushinteger ( L, si . uptime ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "load" ) ;
  lua_pushinteger ( L, si . loads [ 0 ] ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "load2" ) ;
  lua_pushinteger ( L, si . loads [ 1 ] ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "load3" ) ;
  lua_pushinteger ( L, si . loads [ 2 ] ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "total_ram" ) ;
  lua_pushinteger ( L, si . totalram ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "free_ram" ) ;
  lua_pushinteger ( L, si . freeram ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "shared_ram" ) ;
  lua_pushinteger ( L, si . sharedram ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "buffer_ram" ) ;
  lua_pushinteger ( L, si . bufferram ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "total_swap" ) ;
  lua_pushinteger ( L, si . totalswap ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "free_swap" ) ;
  lua_pushinteger ( L, si . freeswap ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "total_high" ) ;
  lua_pushinteger ( L, si . totalhigh ) ;
  lua_rawset ( L, -3 ) ;

  (void) lua_pushliteral ( L, "free_high" ) ;
  lua_pushinteger ( L, si . freehigh ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

/* block until /dev/urandom contains enough entropy */
static int l_random_ready ( lua_State * const L )
{
  char c = 0 ;

  while ( 0 > syscall ( SYS_getrandom, & c, sizeof ( c ), 0 )
    && EINTR == errno )
  { ; }

  return 0 ;
}

/*
 * kernel module related functions
 */

/* load a given kernnel module with finit_module(2) */
static int l_load_module ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;

  if ( path && * path ) {
    long int i ;
    const char * params = luaL_optstring ( L, 2, "" ) ;
    const int fd = open ( path, O_RDONLY | O_CLOEXEC ) ;

    if ( 0 > fd ) {
      i = errno ;
      return luaL_error ( L,
        "cannot open \"%s\" for reading: %s (errno %d)",
        path, strerror ( i ), i ) ;
    }

    i = syscall ( SYS_finit_module, fd, "", 0 ) ;
    if ( i ) { i = errno ? errno : i ; }
    (void) close_fd ( fd ) ;

    if ( 0 < i ) {
      return luaL_error ( L, "finit_module ( %s ) failed: %s (errno %d)",
        path, strerror ( i ), i ) ;
    } else if ( 0 > i ) {
      return luaL_error ( L, "finit_module ( %s ) failed", path ) ;
    }

    return 0 ;
  }

  return luaL_error ( L, "invalid module path" ) ;
}

/*
 * inotify related functions
 */

/*
 * netlink related functions
 * netlink events of interest: UEVENT, ROUTE, GENERIC, CONNECTOR
 */

static ssize_t fd_recvmsg ( const int fd, struct msghdr * hdrp )
{
  if ( 0 <= fd ) {
    ssize_t i ;

    do { i = recvmsg ( fd, hdrp, MSG_DONTWAIT ) ; }
    while ( 0 > i && EINTR == errno ) ;

    return i ;
  }

  return -3 ;
}

/*
static void handle_netlink ( const int fd )
{
  ssize_t r ;
  struct sockaddr_nl sa ;
  struct iovec v [ 2 ] ;
  struct msghdr mh ;

  mh . msg_flags = 0 ;
  mh . msg_control = 0 ;
  mh . msg_controllen = 0 ;
  mh . msg_iov = v ;
  mh . msg_iovlen = 2 ;
  mh . msg_name = & sa ;
  mh . msg_namelen = sizeof ( struct sockaddr_nl ) ;

  r = fd_recvmsg ( fd, & mh ) ;

  if ( r < 0 ) {
    if ( errno == EPIPE ) {
      if (verbosity >= 2) strerr_warnw1x("received EOF on netlink") ;
      cont = 0 ;
      fd_close ( fd ) ;
      return ;
    } else strerr_diefu1sys ( 111, "receive netlink message" ) ;
  }

  if ( 0 == r ) return ;

  if ( mh . msg_flags & MSG_TRUNC )
    strerr_diefu2x(111, "buffer too small for ", "netlink message") ;

  if ( 0 < nl . nl_pid ) {
    if ( verbosity >= 3 ) {
      char fmt [ PID_FMT ] ;
      fmt [ pid_fmt ( fmt, nl.nl_pid ) ] = 0 ;
      strerr_warnw3x ("netlink message", " from userspace process ", fmt) ;
    }

    return ;
  }

  buffer_wseek ( buffer_1, r ) ;
  buffer_putnoflush ( buffer_1, "", 1 ) ;
}
*/

/* create a socket that listens for kernel uevents */
static int uevent_socket ( unsigned int s )
{
  const int fd = socket ( AF_NETLINK
    , SOCK_DGRAM | SOCK_CLOEXEC
    , NETLINK_KOBJECT_UEVENT ) ;

  if ( 0 <= fd ) {
    int i = 0 ;
    struct sockaddr_nl sa ;

    s = ( 4095 < s ) ? s : 65536 ;
    (void) memset ( & sa, 0, sizeof ( sa ) ) ;
    sa . nl_family = AF_NETLINK ;
    sa . nl_pad = 0 ;
    sa . nl_groups = 1 ;
    sa . nl_pid = 0 ;
    i = bind ( fd, (struct sockaddr *) & sa, sizeof ( sa ) ) ;
    i += setsockopt ( fd, SOL_SOCKET, SO_RCVBUF, & s, sizeof ( s ) ) ;
    i += setsockopt ( fd, SOL_SOCKET, SO_RCVBUFFORCE, & s, sizeof ( s ) ) ;

    if ( i ) {
      (void) close_fd ( fd ) ;
      return ( 0 > i ) ? i : -5 ;
    }
  }

  return fd ;
}

static int Luevent_socket ( lua_State * L )
{
  unsigned int s = 0 ;

  if ( geteuid () || getuid () ) {
    return luaL_error ( L, "must be super user" ) ;
  }

  s = luaL_optinteger ( L, 1, 0 ) ;
  s = ( 4095 < s ) ? s : 65536 ;
  return res_lt ( L, 0, uevent_socket ( s ) ) ;
}

/* read kernel uevents */
static int Lread_uevent ( lua_State * L )
{
  const int sd = luaL_checkinteger ( L, 1 ) ;

  /*
  if ( geteuid () || getuid () ) {
    return luaL_error ( L, "must be super user" ) ;
  }
  */

  if ( 0 > sd ) {
    return luaL_argerror ( L, 1, "invalid fd" ) ;
  } else {
    ssize_t r = 0 ;
    char buf [ 1 + ( 4 * 1024 ) ] = { 0 } ;

    do {
      r = read ( sd, buf, sizeof ( buf ) - 1 ) ;
    } while ( 0 > r && EINTR == errno ) ;
  }

  return 0 ;
}

#define BUF_SIZE	( 16 * 1024 )

static int Lread_mmap_uevent ( lua_State * L )
{
  const int sd = luaL_checkinteger ( L, 1 ) ;

  if ( 0 > sd ) {
    return luaL_argerror ( L, 1, "invalid fd" ) ;
  } else {
    int i ;
    ssize_t r = 0 ;
    char * ptr = NULL ;

   /* in many cases, a system sits for *days* waiting
    * for a new uevent notification to come in.
    * we use a fresh mmap so that the buffer is not allocated
    * until the kernel actually starts filling it.
    */
    ptr = mmap ( NULL, BUF_SIZE,
      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 ) ;

    if ( MAP_FAILED == ptr ) {
      i = errno ;
      lua_pushinteger ( L, -1 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    }

    do {
      /* here we block, possibly for a very long time */
      r = read ( sd, ptr, BUF_SIZE - 1 ) ;
    } while ( 0 > r && EINTR == errno ) ;

    if ( 0 > r ) {
      i = errno ;
    } else if ( 0 < r ) {
    } else if ( 0 == r ) {
    }

    (void) munmap ( ptr, BUF_SIZE ) ;
    lua_pushinteger ( L, r ) ;
    return 1 ;
  }

  return 0 ;
}

#if 0

/* returns all pseudo fs currently known to the (Linux) kernel */
static int l_get_pseudofs ( lua_State * const L )
{
#if defined (OSLinux)
  FILE * fp = NULL ;
  const char * path = luaL_optstring ( L, 1, NULL ) ;

  path = ( path && * path ) ? path : "/proc/filesystems" ;
  fp = fopen ( path, "r" ) ; 

  if ( fp ) {
    char * s = NULL ;
    char * s2 = NULL ;
    char buf [ 64 ] = { 0 } ;

    /* create a new Lua table for the results */
    lua_newtable ( L ) ;

    while ( NULL != fgets ( buf, sizeof ( buf ) - 1, fp ) ) {
      s = strtok_r ( buf, " \t\n", & s2 ) ;

      if ( s && * s && 0 == strncmp ( "nodev", s, 5 ) ) {
        s = strtok_r ( NULL, " \t\n", & s2 ) ;

        if ( s && * s ) {
          (void) lua_pushstring ( L, s ) ;
          lua_pushboolean ( L, 1 ) ;
          lua_rawset ( L, -3 ) ;
        }
      }
    }

    (void) fclose ( fp ) ;
    /* returns the result table at the top of the stack */
    return 1 ;
  }

  return 0 ;
#else
  return luaL_error ( L, "platform not supported" ) ;
#endif
}

static int l_cgroup_level ( lua_State * const L )
{
#if defined (OSLinux)
  const char * path = luaL_optstring ( L, 1, "/proc/filesystems" ) ;

  if ( path && * path ) {
    int i = 0 ;
    FILE * fp = fopen ( path, "r" ) ;

    if ( fp ) {
      char * s = NULL ;
      char * s2 = NULL ;
      char buf [ 101 ] = { 0 } ;

      while ( fgets ( buf, sizeof ( buf ) - 1, fp ) ) {
        s = strtok_r ( buf, " \t\n", & s2 ) ;
        if ( s && 'n' == * s ) {
          s = strtok_r ( NULL, " \t\n", & s2 ) ;
          if ( s && 'c' == * s ) {
            if ( 0 == strcmp ( s, "cgroup2" ) ) { i = 2 ; }
            else if ( 2 > i && 0 == strcmp ( s, "cgroup" ) ) { i = 1 ; }
          }
        }
      }

      (void) fclose ( fp ) ;
    }

    lua_pushinteger ( L, i ) ;
    return 1 ;
  }

  return 0 ;
#else
  return luaL_error ( L, "platform not supported" ) ;
#endif
}

#endif /* #if 0 */

#endif /* #ifdef OSLinux */

