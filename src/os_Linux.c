#if defined (OSLinux)

/*
 * wrapper/helper functions for Linux specific OS features
 */

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

static int Lsys_reboot ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_AUTOBOOT ) ) ;
}

static int Lsys_halt ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_HALT_SYSTEM ) ) ;
}

static int Lsys_poweroff ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_POWER_OFF ) ) ;
}

static int Lsys_hibernate ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_SW_SUSPEND ) ) ;
}

static int Lsys_kexec ( lua_State * const L )
{
  sync () ;
  return res0( L, "reboot", reboot ( RB_KEXEC ) ) ;
}

static int Lsys_sak_off ( lua_State * const L )
{
  return res0( L, "reboot", reboot ( RB_DISABLE_CAD ) ) ;
}

static int Lsys_sak_on ( lua_State * const L )
{
  return res0( L, "reboot", reboot ( RB_ENABLE_CAD ) ) ;
}

/*
 * (u)mount(2) related
 */

/* constants used by mount(2) */
static int Lget_mount_flags ( lua_State * const L )
{
  /* create a new table holding the constants (names as keys)
   * and their values
   */
  lua_newtable ( L ) ;

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

  /* return this table */
  return 1 ;
}

/* wrapper function for the mount(2) syscall */
static int Smount ( lua_State * const L )
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

/* mount(2) the procfs on /proc */
static int Lmount_proc ( lua_State * const L )
{
  (void) mkdir ( "/proc", 00755 ) ;

  return res0( L, "mount", mount ( "proc", "/proc", "proc",
    MS_NOSUID,
    "gid=0,hidepid=2" ) ) ;
}

/* remount the procfs */
static int Lremount_proc ( lua_State * const L )
{
  return res0( L, "mount", mount ( "proc", "/proc", "proc",
    MS_REMOUNT | MS_NOSUID,
    "gid=0,hidepid=2" ) ) ;
}

/* mount(2) the sysfs on /sys */
static int Lmount_sys ( lua_State * const L )
{
  (void) mkdir ( "/sys", 00755 ) ;

  return res0( L, "mount", mount ( "sys", "/sys", "sysfs",
    MS_NOSUID,
    NULL ) ) ;
}

/* mount(2) a tmpfs on /run */
static int Lmount_run ( lua_State * const L )
{
  (void) mkdir ( "/dev", 00755 ) ;

  return res0( L, "mount", mount ( "sys", "/sys", "sysfs",
    MS_NODEV | MS_NOSUID,
    NULL ) ) ;
}

/* mount(2)s the devtmpfs on /dev */
static int Lmount_dev ( lua_State * const L )
{
  (void) mkdir ( "/dev", 00755 ) ;

  return res0( L, "mount", mount ( "devtmpfs", "/dev", "devtmpfs",
    MS_NOSUID,
    NULL ) ) ;
}

/* mount(2) the devpts fs on /dev/pts */
static int Lmount_pts ( lua_State * const L )
{
  (void) mkdir ( "/dev", 00755 ) ;
  (void) mkdir ( "/dev/pts", 00755 ) ;

  return res0( L, "mount", mount ( "devpts", "/dev/pts", "devpts",
    MS_NOSUID,
    "gid=5,mode=620,ptmxmode=000" ) ) ;
}

/* mount(2) the mqueue fs on /dev/mqueue */
static int Lmount_mqueue ( lua_State * const L )
{
  (void) mkdir ( "/dev", 00755 ) ;
  (void) mkdir ( "/dev/mqueue", 00755 ) ;

  return res0( L, "mount", mount ( "mqueue", "/dev/mqueue", "mqueue",
    MS_NOSUID,
    NULL ) ) ;
}

/* mount(2) a tmpfs on /dev/shm */
static int Lmount_shm ( lua_State * const L )
{
  (void) mkdir ( "/dev", 00755 ) ;
  (void) mkdir ( "/dev/shm", 00755 ) ;

  return res0( L, "mount", mount ( "tmpfs", "/dev/shm", "tmpfs",
    MS_NOSUID,
    NULL ) ) ;
}

/* constants used by umount2(2) */
static int Lget_umount2_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  L_ADD_CONST( L, MNT_DETACH )
  L_ADD_CONST( L, MNT_EXPIRE )
  L_ADD_CONST( L, MNT_FORCE )
  L_ADD_CONST( L, UMOUNT_NOFOLLOW )

  return 1 ;
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
static int Lmtab_mount_point ( lua_State * const L )
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

/* wrapper function for the swapoff syscall */
static int Sswapoff ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 0 < n ) {
    int i ;

    for ( i = 1 ; n >= i ; ++ i ) {
      const char * const path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        if ( swapoff ( path ) ) {
          return rep_err ( L, "swapoff", errno ) ;
        }
      } else {
        return luaL_argerror ( L, i, "invalid swap path" ) ;
      }
    }

    return 0 ;
  }

  return luaL_error ( L, "swap path argument required" ) ;
}

/* constants used by swapon(2) */
static int Lget_swapon_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  L_ADD_CONST( L, SWAP_FLAG_DISCARD )
  L_ADD_CONST( L, SWAP_FLAG_PREFER )
  L_ADD_CONST( L, SWAP_FLAG_PRIO_MASK )
  L_ADD_CONST( L, SWAP_FLAG_PRIO_SHIFT )

  return 1 ;
}

/* wrapper function for the swapon(2) syscall */
static int Sswapon ( lua_State * const L )
{
  const int n = lua_gettop ( L ) ;

  if ( 1 < n ) {
    int i ;
    const int f = (int) luaL_checkinteger ( L, 1 ) ;

    for ( i = 2 ; n >= i ; ++ i ) {
      const char * const path = luaL_checkstring ( L, i ) ;

      if ( path && * path ) {
        if ( swapon ( path, f ) ) {
          return rep_err ( L, "swapon", errno ) ;
        }
      } else {
        return luaL_argerror ( L, i, "invalid swap path" ) ;
      }
    }

    return 0 ;
  }

  return luaL_error ( L, "flag bitmask and path argument required" ) ;
}

/* binding for the unshare(2) Linux syscall */
static int Sunshare ( lua_State * const L )
{
  int f = luaL_optinteger ( L, 1, 0 ) ;

  f = f ? f :
    CLONE_FILES | CLONE_FS | CLONE_NEWIPC | CLONE_NEWNET
    | CLONE_NEWNS | CLONE_NEWUTS ;

  return res_zero ( L, unshare ( f ) ) ;
}

/* bindings for the setns(2) Linux syscall */
static int Ssetns ( lua_State * const L )
{
  const int fd = luaL_checkinteger ( L, 1 ) ;
  const int t = luaL_optinteger ( L, 2, 0 ) ;

  if ( 0 <= fd ) {
    return res_zero ( L, setns ( fd, t ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid fd" ) ;
}

static int Lsetns ( lua_State * const L )
{
  const char * path = luaL_checkstring ( L, 1 ) ;
  const int t = luaL_optinteger ( L, 2, 0 ) ;

  if ( path && * path ) {
    int i ;
    const int fd = open ( path, O_RDONLY | O_CLOEXEC ) ;

    if ( 0 > fd ) {
      i = errno ;
      lua_pushinteger ( L, -3 ) ;
      lua_pushinteger ( L, i ) ;
      return 2 ;
    } else {
      const int r = setns ( fd, t ) ;
      i = r ? errno : 0 ;

      /* is it ok or even necessary to close the fd here ? */
      while ( close ( fd ) && ( EINTR == errno ) ) { ; }

      if ( r ) {
        lua_pushinteger ( L, -1 ) ;
        lua_pushinteger ( L, i ) ;
        return 2 ;
      }

      lua_pushinteger ( L, 0 ) ;
      return 1 ;
    }
  }

  return luaL_argerror ( L, 1, "invalid ns path" ) ;
}

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
static int Ssysinfo ( lua_State * const L )
{
  struct sysinfo si ;

  if ( sysinfo ( & si ) ) {
    const int e = errno ;
    lua_pushnil ( L ) ;
    lua_pushinteger ( L, e ) ;
    return 2 ;
  } else {
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

  return 0 ;
}

/* block until /dev/urandom contains enough entropy */
static int Lrandom_ready ( lua_State * const L )
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
static int Lload_module ( lua_State * const L )
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
 * networking related
 */

/* enable the loopback interface */
static int Lsetup_iface_lo ( lua_State * const L )
{
#if 0
  int i = socket ( PF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_IP ) ;

  if ( 0 > i ) {
    i = errno ;
    return luaL_error ( L, "socket() failed: %s (errno %d)",
      strerror ( i ), i ) ;
  } else {
    struct sockaddr_in ifaddr ;
    struct ifreq ipreq ;

    ifaddr . sin_family = AF_INET ;
    /* 127.0.0.1 */
    ifaddr . sin_addr = 16777343 ;
    ipreq . ifr_name = "lo" ;
    ipreq . ifr_addr = * ( (struct sockaddr *) & ifaddr ) ;
    (void) ioctl ( i, SIOCSIFADDR, & ipreq ) ;
    ipreq . ifr_flags = IFF_UP | IFF_LOOPBACK | IFF_RUNNING ;
    (void) ioctl ( i, SIOCSIFFLAGS, & ipreq ) ;
    (void) close_fd ( i ) ;
  }
#endif

  return 0 ;
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
static int Lget_pseudofs ( lua_State * const L )
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

static int Lcgroup_level ( lua_State * const L )
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

/*
 * netlink uevents
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#define _LINUX_TIME_H
#include <linux/cn_proc.h>
#include <errno.h>

/* socket for netlink connection  */
static int nl_sock ;

#define NL_MESSAGE_SIZE (sizeof(struct nlmsghdr) + sizeof(struct cn_msg) + \
                         sizeof(int))

void connect_to_netlink ( void )
{
  struct sockaddr_nl sa_nl; /* netlink interface info */
  char buff[NL_MESSAGE_SIZE];
  struct nlmsghdr *hdr; /* for telling netlink what we want */
  struct cn_msg *msg;   /* the actual connector message */

  /* connect to netlink socket */
  nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

  if (-1 == nl_sock) {
    rb_raise(rb_eStandardError, "%s", strerror(errno));
  }

  bzero(&sa_nl, sizeof(sa_nl));
  sa_nl.nl_family = AF_NETLINK;
  sa_nl.nl_groups = CN_IDX_PROC;
  //sa_nl.nl_pid = getpid();
  sa_nl.nl_pid = 0 ;

  if ( 0 > bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl))) {
    rb_raise(rb_eStandardError, "%s", strerror(errno));
  }

  /* Fill header */
  hdr = (struct nlmsghdr *)buff;
  hdr->nlmsg_len = NL_MESSAGE_SIZE;
  hdr->nlmsg_type = NLMSG_DONE;
  hdr->nlmsg_flags = 0;
  hdr->nlmsg_seq = 0;
  hdr->nlmsg_pid = getpid();

  /* Fill message */
  msg = (struct cn_msg *)NLMSG_DATA(hdr);
  msg->id.idx = CN_IDX_PROC;  /* Connecting to process information */
  msg->id.val = CN_VAL_PROC;
  msg->seq = 0;
  msg->ack = 0;
  msg->flags = 0;
  msg->len = sizeof(int);
  * (int *) msg->data = PROC_CN_MCAST_LISTEN;

  if (-1 == send ( nl_sock, hdr, hdr -> nlmsg_len, 0 ) ) {
    rb_raise ( rb_eStandardError, "%s", strerror ( errno ) ) ;
  }
}

/* end of Ruby Ext */

#ifndef SO_RCVBUFFORCE
#define SO_RCVBUFFORCE 33
#endif
enum { RCVBUF = 2 * 1024 * 1024 } ;

int uevent_main ( int argc, char ** argv )
{
  int fd ;
  struct sockaddr_nl sa ;

  INIT_G() ;
  argv ++ ;

  // Subscribe for UEVENT kernel messages
  sa.nl_family = AF_NETLINK;
  sa.nl_pad = 0;
  sa.nl_pid = getpid();
  sa.nl_groups = 1 << 0;
  fd = xsocket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  xbind ( fd, (struct sockaddr *) &sa, sizeof(sa) ) ;
  close_on_exec_on ( fd ) ;

	// Without a sufficiently big RCVBUF, a ton of simultaneous events
	// can trigger ENOBUFS on read, which is unrecoverable.
	// Reproducer:
	//	uevent mdev &
	// 	find /sys -name uevent -exec sh -c 'echo add >"{}"' ';'
	//
	// SO_RCVBUFFORCE (root only) can go above net.core.rmem_max sysctl
  setsockopt_SOL_SOCKET_int ( fd, SO_RCVBUF,      RCVBUF ) ;
  setsockopt_SOL_SOCKET_int ( fd, SO_RCVBUFFORCE, RCVBUF ) ;

  if ( 0 ) {
    int z ;
    socklen_t zl = sizeof ( z ) ;
    getsockopt ( fd, SOL_SOCKET, SO_RCVBUF, &z, &zl ) ;
    bb_error_msg ( "SO_RCVBUF:%d", z ) ;
  }

  while ( 1 ) {
    int idx ;
    ssize_t len ;
    char * netbuf ;
    char * s, * end ;

		// In many cases, a system sits for *days* waiting
		// for a new uevent notification to come in.
		// We use a fresh mmap so that buffer is not allocated
		// until kernel actually starts filling it.
		netbuf = mmap(NULL, BUFFER_SIZE,
					PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANON,
					/* ignored: */ -1, 0);
		if (netbuf == MAP_FAILED)
			bb_perror_msg_and_die("mmap");

		// Here we block, possibly for a very long time
		len = safe_read(fd, netbuf, BUFFER_SIZE - 1);
		if (len < 0)
			bb_perror_msg_and_die("read");
		end = netbuf + len;
		*end = '\0';

		// Each netlink message starts with "ACTION@/path"
		// (which we currently ignore),
		// followed by environment variables.
		if (!argv[0])
			putchar('\n');
		idx = 0;
		s = netbuf;
		while (s < end) {
			if ( ! argv [ 0 ] )
				puts ( s );
			if ( strchr ( s, '=' ) && idx < MAX_ENV )
				env [ idx ++ ] = s ;
			s += strlen(s) + 1;
		}

		env [ idx ] = NULL ;
		idx = 0 ;
		while ( env [ idx ] )
			putenv ( env [ idx ++ ] ) ;

		if ( argv [ 0 ] )
			spawn_and_wait ( argv ) ;
		idx = 0 ;
		while ( env [ idx ] )
			bb_unsetenv ( env [ idx ++ ] ) ;
		munmap ( netbuf, BUFFER_SIZE ) ;
  }

  return 0 ;
}

/*
 * Arachsys uevent(d)
 */

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <sys/socket.h>

void error(int status, int errnum, char *format, ...) {
  va_list args;

  fprintf(stderr, "%s: ", progname);
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  if (errnum != 0)
    fprintf(stderr, ": %s\n", strerror(errnum));
  else
    fputc('\n', stderr);
  if (status != 0)
    exit(status);
}

/* Pass on HUP, INT, TERM, USR1, USR2 signals, and exit on SIGTERM. */
int uevent_main ( int argc, char ** argv )
{
  char buffer[BUFFER + 1], *cursor, *separator;
  int sock;
  ssize_t length;
  struct sockaddr_nl addr;

  if (argc > 1)
    subprocess(argv + 1);

  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  addr.nl_groups = 1;

  if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_)) < 0)
    error(EXIT_FAILURE, errno, "socket");

  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    error(EXIT_FAILURE, errno, "bind");

  while ( 1 ) {
    if ((length = recv(sock, &buffer, sizeof(buffer) - 1, 0)) < 0) {
      if (errno != EAGAIN && errno != EINTR)
        error(EXIT_FAILURE, errno, "recv");
      continue;
    }

    /* Null-terminate the uevent and replace stray newlines with spaces. */
    buffer[length] = 0;
    for (cursor = buffer; cursor < buffer + length; cursor++)
      if (*cursor == '\n')
        *cursor = ' ';

    if (strlen(buffer) >= length - 1) {
      /* No properties; fake a simple environment based on the header. */
      if ((cursor = strchr(buffer, '@'))) {
        *cursor++ = 0;
        printf("ACTION %s\n", buffer);
        printf("DEVPATH %s\n", cursor);
      }
    } else {
      /* Ignore header as properties will include ACTION and DEVPATH. */
      cursor = buffer;
      while (cursor += strlen(cursor) + 1, cursor < buffer + length) {
        if ((separator = strchr(cursor, '=')))
          *separator = ' ';
        puts(cursor);
      }
    }
    putchar('\n');
    fflush(stdout);
  }

  return EXIT_FAILURE;
}

/*
 * Netlink functions for IFUP/IFDN/GW events
 */

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>		/* IFNAMSIZ */
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>

static int nlmsg_validate ( struct nlmsghdr * nh, size_t len )
{
	if (!NLMSG_OK(nh, len))
		return 1;

	if (nh->nlmsg_type == NLMSG_DONE) {
		_d("Done with netlink messages.");
		return 1;
	}

	if (nh->nlmsg_type == NLMSG_ERROR) {
		_d("Netlink reports error.");
		return 1;
	}

	return 0;
}

static void nl_route ( struct nlmsghdr * nlmsg )
{
	struct rtmsg *r;
	struct rtattr *a;
	int la;
	int gw = 0, dst = 0, mask = 0, idx = 0;

	if ( nlmsg -> nlmsg_len < NLMSG_LENGTH( sizeof ( struct rtmsg ) ) ) {
		_e("Packet too small or truncated!");
		return;
	}

	r  = NLMSG_DATA(nlmsg);
	a  = RTM_RTA(r);
	la = RTM_PAYLOAD(nlmsg);
	while (RTA_OK(a, la)) {
		void *data = RTA_DATA(a);
		switch (a->rta_type) {
		case RTA_GATEWAY:
			gw = *((int *)data);
			//_d("GW: 0x%04x", gw);
			break;

		case RTA_DST:
			dst = *((int *)data);
			mask = r->rtm_dst_len;
			//_d("MASK: 0x%04x", mask);
			break;

		case RTA_OIF:
			idx = *((int *)data);
			//_d("IDX: 0x%04x", idx);
			break;
		}

		a = RTA_NEXT(a, la);
	}

	if ((!dst && !mask) && (gw || idx)) {
		if (nlmsg->nlmsg_type == RTM_DELROUTE)
			cond_clear("net/route/default");
		else
			cond_set("net/route/default");
	}
}

static void nl_link(struct nlmsghdr *nlmsg)
{
	int la;
	char ifname[IFNAMSIZ + 1];
	struct rtattr *a;
	struct ifinfomsg *i;

	if (nlmsg->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifinfomsg))) {
		_e("Packet too small or truncated!");
		return;
	}

	i  = NLMSG_DATA(nlmsg);
	a  = (struct rtattr *)((char *)i + NLMSG_ALIGN(sizeof(struct ifinfomsg)));
	la = NLMSG_PAYLOAD(nlmsg, sizeof(struct ifinfomsg));

	while (RTA_OK(a, la)) {
		if (a->rta_type == IFLA_IFNAME) {
			char msg[MAX_ARG_LEN];

			strlcpy(ifname, RTA_DATA(a), sizeof(ifname));
			switch (nlmsg->nlmsg_type) {
			case RTM_NEWLINK:
				/*
				 * New interface has appearad, or interface flags has changed.
				 * Check ifi_flags here to see if the interface is UP/DOWN
				 */
				if (i->ifi_change & IFF_UP) {
					snprintf(msg, sizeof(msg), "net/%s/up", ifname);

					if (i->ifi_flags & IFF_UP)
						cond_set(msg);
					else
						cond_clear(msg);

					if (string_compare("lo", ifname)) {
						snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
						cond_set(msg);
					}
				} else {
					snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
					cond_set(msg);
				}
				break;

			case RTM_DELLINK:
				/* NOTE: Interface has dissapeared, not link down ... */
				snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
				cond_clear(msg);
				break;

			case RTM_NEWADDR:
				_d("%s: New Address", ifname);
				break;

			case RTM_DELADDR:
				_d("%s: Deconfig Address", ifname);
				break;

			default:
				_d("%s: Msg 0x%x", ifname, nlmsg->nlmsg_type);
				break;
			}
		}

		a = RTA_NEXT(a, la);
	}
}

static void nl_callback(void *UNUSED(arg), int sd, int UNUSED(events))
{
	ssize_t len;
	static char buf[4096];
	struct nlmsghdr *nh;

	memset(buf, 0, sizeof(buf));
	len = recv(sd, buf, sizeof(buf), 0);
	if (len < 0) {
		if (errno != EINTR)	/* Signal */
			_pe("recv()");
		return;
	}

	for (nh = (struct nlmsghdr *)buf; !nlmsg_validate(nh, len); nh = NLMSG_NEXT(nh, len)) {
		//_d("Well formed netlink message received. type %d ...", nh->nlmsg_type);
		if (nh->nlmsg_type == RTM_NEWROUTE || nh->nlmsg_type == RTM_DELROUTE)
			nl_route(nh);
		else
			nl_link(nh);
	}
}

PLUGIN_INIT( plugin_init )
{
	int sd;
	struct sockaddr_nl sa;

	sd = socket(AF_NETLINK, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, NETLINK_ROUTE);
	if (sd < 0) {
		_pe("socket()");
		return;
	}

	memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_IPV4_ROUTE | RTMGRP_LINK; // | RTMGRP_NOTIFY | RTMGRP_IPV4_IFADDR;
	sa.nl_pid    = getpid();

	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		_pe("bind()");
		close(sd);
		return;
	}

  plugin.io.fd = sd ;
  plugin_register ( & plugin ) ;
}

#endif /* #if 0 */

#endif /* #ifdef OSLinux */

