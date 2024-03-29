/*
 * some posix bindings for Lua (as a Lua module)
 * wrappers to system calls for Lua 5.3+
 *
 * public domain
 */

/*
#include "common.h"
*/
#include "version.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* constant with an unique address to use as key in the Lua registry */
static const char Key = 'X' ;

/* include the Lua wrapper functions */
#include "os_aux.c"
#include "os_misc.c"
#include "os_proc.c"
#include "os_time.c"
#include "os_svipc.c"
#include "os_file.c"
#include "os_at.c"
#include "os_io.c"
#include "os_env.c"
#include "os_match.c"
#include "os_regex.c"
#include "os_pw.c"
#include "os_fs.c"
#include "os_socket.c"
/*
#include "os_net.c"
*/
#include "os_gen.c"

/* OS specific functions */
#if defined (OSbsd)
#endif

#if defined (OSLinux)
  /* Linux specific functions */
#  include "os_Linux.c"
#elif defined (OSfreebsd)
#elif defined (OSsolaris) || defined (OSsunos5)
#  include "os_streams.c"
#endif

#include "os_sig.c"

/* this procedure exports important posix constants to Lua */
static void add_const ( lua_State * const L )
{
  /* create a new (sub)table holding the constants (names as keys)
   * and their values
   */
  lua_newtable ( L ) ;

  /* add the constants now to that (sub)table */
#if defined (_POSIX_VERSION)
  L_ADD_CONST( L, _POSIX_VERSION )
#endif
#if defined (_POSIX_C_SOURCE)
  L_ADD_CONST( L, _POSIX_C_SOURCE )
#endif
#if defined (_XOPEN_SOURCE)
  L_ADD_CONST( L, _XOPEN_SOURCE )
#endif
  L_ADD_CONST( L, EOF )
  L_ADD_CONST( L, PATH_MAX )
  L_ADD_CONST( L, CLOCKS_PER_SEC )
  L_ADD_CONST( L, FD_SETSIZE )

  /* constants used by the *at() syscalls */
#if defined (AT_FDCWD)
  L_ADD_CONST( L, AT_FDCWD )
  L_ADD_CONST( L, AT_EACCESS )
  L_ADD_CONST( L, AT_EMPTY_PATH )
  L_ADD_CONST( L, AT_REMOVEDIR )
  L_ADD_CONST( L, AT_SYMLINK_FOLLOW )
  L_ADD_CONST( L, AT_SYMLINK_NOFOLLOW )
  L_ADD_CONST( L, AT_NO_AUTOMOUNT )
#endif

  /* constants used by wait(p)id */
  L_ADD_CONST( L, WNOHANG )
  L_ADD_CONST( L, WNOWAIT )
  L_ADD_CONST( L, WEXITED )
  L_ADD_CONST( L, WSTOPPED )
  L_ADD_CONST( L, WCONTINUED )
  L_ADD_CONST( L, WUNTRACED )
  L_ADD_CONST( L, P_ALL )
  L_ADD_CONST( L, P_PID )
  L_ADD_CONST( L, P_PGID )
  /*
  L_ADD_CONST( L, P_PIDFD )
  */

  /* constants used by clock_(g,s)ettime(2) et al */
#if defined (_POSIX_TIMERS) && (0 < _POSIX_TIMERS)
  L_ADD_CONST( L, CLOCK_REALTIME )
#  ifdef OSLinux
  L_ADD_CONST( L, CLOCK_REALTIME_COARSE )
#  endif
#  ifdef _POSIX_MONOTONIC_CLOCK
  L_ADD_CONST( L, CLOCK_MONOTONIC )
#    ifdef OSLinux
  L_ADD_CONST( L, CLOCK_MONOTONIC_COARSE )
  L_ADD_CONST( L, CLOCK_MONOTONIC_RAW )
  L_ADD_CONST( L, CLOCK_BOOTTIME )
#    endif
#  endif
#  ifdef _POSIX_CPUTIME
  L_ADD_CONST( L, CLOCK_PROCESS_CPUTIME_ID )
#  endif
#  ifdef _POSIX_THREAD_CPUTIME
  L_ADD_CONST( L, CLOCK_THREAD_CPUTIME_ID )
#  endif
#endif

  /* constants used by SysV ipc */
  L_ADD_CONST( L, IPC_CREAT )
  L_ADD_CONST( L, IPC_EXCL )
  L_ADD_CONST( L, IPC_NOWAIT )
  L_ADD_CONST( L, IPC_PRIVATE )
  L_ADD_CONST( L, IPC_SET )
  L_ADD_CONST( L, IPC_RMID )
  L_ADD_CONST( L, IPC_STAT )
  L_ADD_CONST( L, MSG_NOERROR )
#ifdef OSLinux
  L_ADD_CONST( L, MSG_INFO )
  L_ADD_CONST( L, MSG_STAT )
  L_ADD_CONST( L, SEM_INFO )
  L_ADD_CONST( L, SEM_STAT )
  /*
  L_ADD_CONST( L, MSGMNI )
  L_ADD_CONST( L, SHMMNI )
  L_ADD_CONST( L, MSGMNB )
  L_ADD_CONST( L, MSGMAX )
  */
#  ifdef _GNU_SOURCE
  L_ADD_CONST( L, IPC_INFO )
  L_ADD_CONST( L, MSG_COPY )
  L_ADD_CONST( L, MSG_EXCEPT )
#  endif
#endif

  /* constants used by fcntl */
  L_ADD_CONST( L, F_GETFD )
  L_ADD_CONST( L, F_SETFD )
  L_ADD_CONST( L, F_GETFL )
  L_ADD_CONST( L, F_SETFL )
  L_ADD_CONST( L, F_GETLK )
  L_ADD_CONST( L, F_SETLK )
  L_ADD_CONST( L, F_SETLKW )
  L_ADD_CONST( L, F_DUPFD )
  L_ADD_CONST( L, F_DUPFD_CLOEXEC )
  L_ADD_CONST( L, F_GETOWN )
  L_ADD_CONST( L, F_SETOWN )
#ifdef OSLinux
  L_ADD_CONST( L, F_RDLCK )
  L_ADD_CONST( L, F_WRLCK )
  L_ADD_CONST( L, F_UNLCK )
  /*
  L_ADD_CONST( L, F_ADD_SEALS )
  L_ADD_CONST( L, F_GET_SEALS )
  L_ADD_CONST( L, F_SEAL_SEAL )
  L_ADD_CONST( L, F_SEAL_SHRINK )
  L_ADD_CONST( L, F_SEAL_GROW )
  L_ADD_CONST( L, F_SEAL_WRITE )
  */
# ifdef _GNU_SOURCE
  L_ADD_CONST( L, F_GETOWN_EX )
  L_ADD_CONST( L, F_SETOWN_EX )
  L_ADD_CONST( L, F_GETPIPE_SZ )
  L_ADD_CONST( L, F_SETPIPE_SZ )
  L_ADD_CONST( L, F_GETLEASE )
  L_ADD_CONST( L, F_SETLEASE )
  L_ADD_CONST( L, F_GETSIG )
  L_ADD_CONST( L, F_SETSIG )
  L_ADD_CONST( L, F_OFD_GETLK )
  L_ADD_CONST( L, F_OFD_SETLK )
  L_ADD_CONST( L, F_OFD_SETLKW )
  L_ADD_CONST( L, F_NOTIFY )
  L_ADD_CONST( L, DN_ACCESS )
  L_ADD_CONST( L, DN_ATTRIB )
  L_ADD_CONST( L, DN_CREATE )
  L_ADD_CONST( L, DN_DELETE )
  L_ADD_CONST( L, DN_MODIFY )
  L_ADD_CONST( L, DN_RENAME )
  L_ADD_CONST( L, DN_MULTISHOT )
  L_ADD_CONST( L, SEEK_DATA )
  L_ADD_CONST( L, SEEK_HOLE )
# endif
  /* constants used by the Linux memfd_create() syscall */
  L_ADD_CONST( L, MFD_CLOEXEC )
  L_ADD_CONST( L, MFD_ALLOW_SEALING )
#endif

  /* constants used by lseek etc */
  L_ADD_CONST( L, SEEK_SET )
  L_ADD_CONST( L, SEEK_CUR )
  L_ADD_CONST( L, SEEK_END )

#ifdef OSLinux
  /* constants used by ioctl */
  L_ADD_CONST( L, TIOCSCTTY )

  /* constants for inotify (Linux only !) */
  L_ADD_CONST( L, IN_ACCESS )
  L_ADD_CONST( L, IN_ATTRIB )
  L_ADD_CONST( L, IN_CLOSE_WRITE )
  L_ADD_CONST( L, IN_CLOSE_NOWRITE )
  L_ADD_CONST( L, IN_CREATE )
  L_ADD_CONST( L, IN_DELETE )
  L_ADD_CONST( L, IN_DELETE_SELF )
  L_ADD_CONST( L, IN_MODIFY )
  L_ADD_CONST( L, IN_MOVE_SELF )
  L_ADD_CONST( L, IN_MOVED_FROM )
  L_ADD_CONST( L, IN_MOVED_TO )
  L_ADD_CONST( L, IN_OPEN )
  L_ADD_CONST( L, IN_ALL_EVENTS )
  L_ADD_CONST( L, IN_CLOSE )
  L_ADD_CONST( L, IN_MOVE )
  L_ADD_CONST( L, IN_DONT_FOLLOW )
  L_ADD_CONST( L, IN_EXCL_UNLINK )
  L_ADD_CONST( L, IN_MASK_ADD )
  L_ADD_CONST( L, IN_ONESHOT )
  L_ADD_CONST( L, IN_ONLYDIR )
  L_ADD_CONST( L, IN_IGNORED )
  L_ADD_CONST( L, IN_ISDIR )
  L_ADD_CONST( L, IN_Q_OVERFLOW )
  L_ADD_CONST( L, IN_UNMOUNT )

  /* constants for fanotify (Linux only!) */
  L_ADD_CONST( L, FAN_CLASS_PRE_CONTENT )
  L_ADD_CONST( L, FAN_CLASS_CONTENT )
  L_ADD_CONST( L, FAN_CLASS_NOTIF )
  L_ADD_CONST( L, FAN_CLOEXEC )
  L_ADD_CONST( L, FAN_NONBLOCK )
  L_ADD_CONST( L, FAN_UNLIMITED_QUEUE )
  L_ADD_CONST( L, FAN_UNLIMITED_MARKS )
#endif

  /* constants used by fnmatch(3) */
  add_fnmatch_flags ( L ) ;

  /* constants used by glob(3) */
  add_glob_flags ( L ) ;

  /* constants for wordexp(3) */
  add_wordexp_flags ( L ) ;

  /* constants for the POSIX regex API */
  L_ADD_CONST( L, REG_EXTENDED )
  L_ADD_CONST( L, REG_ICASE )
  L_ADD_CONST( L, REG_NOSUB )
  L_ADD_CONST( L, REG_NEWLINE )
  L_ADD_CONST( L, REG_NOTBOL )
  L_ADD_CONST( L, REG_NOTEOL )
  L_ADD_CONST( L, REG_NOMATCH )

  /* constants used by open(2) */
  L_ADD_CONST( L, O_RDONLY )
  L_ADD_CONST( L, O_WRONLY )
  L_ADD_CONST( L, O_RDWR )
  L_ADD_CONST( L, O_CLOEXEC )
  L_ADD_CONST( L, O_NONBLOCK )
  L_ADD_CONST( L, O_NDELAY )
  L_ADD_CONST( L, O_CREAT )
  L_ADD_CONST( L, O_TRUNC )
  L_ADD_CONST( L, O_APPEND )
  L_ADD_CONST( L, O_EXCL )
  L_ADD_CONST( L, O_NOCTTY )
  L_ADD_CONST( L, O_DIRECTORY )
  L_ADD_CONST( L, O_SYNC )
  L_ADD_CONST( L, O_ASYNC )
  L_ADD_CONST( L, O_DIRECT )
  L_ADD_CONST( L, O_DSYNC )
  L_ADD_CONST( L, O_NOATIME )
  L_ADD_CONST( L, O_NOFOLLOW )
  L_ADD_CONST( L, O_PATH )
  L_ADD_CONST( L, O_TMPFILE )

  /* constants for stat(2) */
  L_ADD_CONST( L, S_IFMT )
  L_ADD_CONST( L, S_IFREG )
  L_ADD_CONST( L, S_IFDIR )
  L_ADD_CONST( L, S_IFLNK )
  L_ADD_CONST( L, S_IFIFO )
  L_ADD_CONST( L, S_IFSOCK )
  L_ADD_CONST( L, S_IFBLK )
  L_ADD_CONST( L, S_IFCHR )
  L_ADD_CONST( L, S_ISUID )
  L_ADD_CONST( L, S_ISGID )
  L_ADD_CONST( L, S_ISVTX )
  L_ADD_CONST( L, S_IRWXU )
  L_ADD_CONST( L, S_IRWXG )
  L_ADD_CONST( L, S_IRWXO )
  L_ADD_CONST( L, S_IRUSR )
  L_ADD_CONST( L, S_IRGRP )
  L_ADD_CONST( L, S_IROTH )
  L_ADD_CONST( L, S_IWUSR )
  L_ADD_CONST( L, S_IWGRP )
  L_ADD_CONST( L, S_IWOTH )
  L_ADD_CONST( L, S_IXUSR )
  L_ADD_CONST( L, S_IXGRP )
  L_ADD_CONST( L, S_IXOTH )

  /* constants for futimens(2) */
  add_futimens_flags ( L ) ;

  /* constants for interval timers */
  L_ADD_CONST( L, ITIMER_REAL )
  L_ADD_CONST( L, ITIMER_VIRTUAL )
  L_ADD_CONST( L, ITIMER_PROF )

  /* signal number constants */
  L_ADD_CONST( L, NSIG )
  L_ADD_CONST( L, SIGHUP )
  L_ADD_CONST( L, SIGINT )
  L_ADD_CONST( L, SIGTERM )
  L_ADD_CONST( L, SIGALRM )
  L_ADD_CONST( L, SIGUSR1 )
  L_ADD_CONST( L, SIGUSR2 )
  L_ADD_CONST( L, SIGCONT )
  L_ADD_CONST( L, SIGSTOP )
  L_ADD_CONST( L, SIGKILL )
  L_ADD_CONST( L, SIGCHLD )
  L_ADD_CONST( L, SIGTRAP )
  L_ADD_CONST( L, SIGQUIT )
  L_ADD_CONST( L, SIGABRT )
  L_ADD_CONST( L, SIGPIPE )
  L_ADD_CONST( L, SIGTSTP )
  L_ADD_CONST( L, SIGTTIN )
  L_ADD_CONST( L, SIGTTOU )
  L_ADD_CONST( L, SIGSEGV )
  L_ADD_CONST( L, SIGFPE )
  L_ADD_CONST( L, SIGILL )
  L_ADD_CONST( L, SIGBUS )
  L_ADD_CONST( L, SIGPOLL )
  L_ADD_CONST( L, SIGIO )
  L_ADD_CONST( L, SIGSYS )
  L_ADD_CONST( L, SIGPROF )
  L_ADD_CONST( L, SIGURG )
  L_ADD_CONST( L, SIGVTALRM )
  L_ADD_CONST( L, SIGXCPU )
  L_ADD_CONST( L, SIGXFSZ )
  L_ADD_CONST( L, SIGIOT )
#ifdef SIGEMT
  L_ADD_CONST( L, SIGEMT )
#endif
#ifdef SIGWINCH
  L_ADD_CONST( L, SIGWINCH )
#endif
#ifdef SIGLOST
  L_ADD_CONST( L, SIGLOST )
#endif
#ifdef SIGCLD
  L_ADD_CONST( L, SIGCLD )
#endif
#ifdef SIGPWR
  L_ADD_CONST( L, SIGPWR )
#endif
#ifdef SIGINFO
  L_ADD_CONST( L, SIGINFO )
#endif
#ifdef SIGUNUSED
  L_ADD_CONST( L, SIGUNUSED )
#endif
  /* add posix real time signal constants where possible */
#ifdef SIGRTMIN
  L_ADD_CONST( L, SIGRTMIN )
#endif
#ifdef SIGRTMAX
  L_ADD_CONST( L, SIGRTMAX )
#endif

  /* socket(2) constants */
  add_socket_flags ( L ) ;

  /* constants used by syslog() */
  /* options */
  L_ADD_CONST( L, LOG_CONS )
  L_ADD_CONST( L, LOG_NDELAY )
  L_ADD_CONST( L, LOG_ODELAY )
  L_ADD_CONST( L, LOG_NOWAIT )
  L_ADD_CONST( L, LOG_PID )
  L_ADD_CONST( L, LOG_PERROR )
  /* facilities */
  L_ADD_CONST( L, LOG_AUTH )
  L_ADD_CONST( L, LOG_AUTHPRIV )
  L_ADD_CONST( L, LOG_CRON )
  L_ADD_CONST( L, LOG_DAEMON )
  L_ADD_CONST( L, LOG_FTP )
  L_ADD_CONST( L, LOG_KERN )
  L_ADD_CONST( L, LOG_LPR )
  L_ADD_CONST( L, LOG_MAIL )
  L_ADD_CONST( L, LOG_NEWS )
  L_ADD_CONST( L, LOG_SYSLOG )
  L_ADD_CONST( L, LOG_USER )
  L_ADD_CONST( L, LOG_UUCP )
  L_ADD_CONST( L, LOG_LOCAL0 )
  L_ADD_CONST( L, LOG_LOCAL1 )
  L_ADD_CONST( L, LOG_LOCAL2 )
  L_ADD_CONST( L, LOG_LOCAL3 )
  L_ADD_CONST( L, LOG_LOCAL4 )
  L_ADD_CONST( L, LOG_LOCAL5 )
  L_ADD_CONST( L, LOG_LOCAL6 )
  L_ADD_CONST( L, LOG_LOCAL7 )
  /* levels */
  L_ADD_CONST( L, LOG_EMERG )
  L_ADD_CONST( L, LOG_ALERT )
  L_ADD_CONST( L, LOG_CRIT )
  L_ADD_CONST( L, LOG_ERR )
  L_ADD_CONST( L, LOG_WARNING )
  L_ADD_CONST( L, LOG_NOTICE )
  L_ADD_CONST( L, LOG_INFO )
  L_ADD_CONST( L, LOG_DEBUG )

  /* constants for utmp */
  /*
  L_ADD_CONST( L, EMPTY )
  L_ADD_CONST( L, RUN_LVL )
  L_ADD_CONST( L, BOOT_TIME )
  L_ADD_CONST( L, NEW_TIME )
  L_ADD_CONST( L, OLD_TIME )
  L_ADD_CONST( L, INIT_PROCESS )
  L_ADD_CONST( L, LOGIN_PROCESS )
  L_ADD_CONST( L, USER_PROCESS )
  L_ADD_CONST( L, DEAD_PROCESS )
  */

  /* constants for quotactl */
#if defined (OSLinux)
  L_ADD_CONST( L, Q_QUOTAON )
  L_ADD_CONST( L, Q_QUOTAOFF )
  L_ADD_CONST( L, Q_GETQUOTA )
  L_ADD_CONST( L, Q_SETQUOTA )
  L_ADD_CONST( L, Q_GETINFO )
  L_ADD_CONST( L, Q_SETINFO )
//L_ADD_CONST( L, Q_GETNEXTQUOTA )
  L_ADD_CONST( L, Q_GETFMT )
//L_ADD_CONST( L, Q_GETSTATS )
  L_ADD_CONST( L, Q_SYNC )
  L_ADD_CONST( L, Q_XQUOTAON )
  L_ADD_CONST( L, Q_XQUOTAOFF )
  L_ADD_CONST( L, Q_XQUOTARM )
  L_ADD_CONST( L, Q_XGETQUOTA )
//L_ADD_CONST( L, Q_XGETNEXTQUOTA )
  L_ADD_CONST( L, Q_XSETQLIM )
  L_ADD_CONST( L, Q_XGETQSTAT )
#endif

  /* constants for Linux syscalls */
#if defined (OSLinux)
  /* constants used by mount(2) */
  add_mount_flags ( L ) ;

  /* constants used by umount2(2) */
  add_umount2_flags ( L ) ;

  /* constants used by swapon(2) */
  add_swapon_flags ( L ) ;

  /* constants used by reboot(2) */
  add_reboot_flags ( L ) ;

  /* constants for the clone/unshare(2) Linux syscalls */
  L_ADD_CONST( L, CLONE_FILES )
  L_ADD_CONST( L, CLONE_FS )
  L_ADD_CONST( L, CLONE_NEWIPC )
  L_ADD_CONST( L, CLONE_NEWNET )
  L_ADD_CONST( L, CLONE_NEWPID )
  L_ADD_CONST( L, CLONE_NEWUSER )
  L_ADD_CONST( L, CLONE_NEWUTS )
  /*
  L_ADD_CONST( L, CLONE_NEWCGROUP )
  L_ADD_CONST( L, CLONE_NS )
  */
  L_ADD_CONST( L, CLONE_SIGHAND )
  L_ADD_CONST( L, CLONE_SYSVSEM )
  L_ADD_CONST( L, CLONE_THREAD )
  L_ADD_CONST( L, CLONE_VM )

 /* Linux capabilities */
  L_ADD_CONST( L, CAP_AUDIT_CONTROL )
  L_ADD_CONST( L, CAP_AUDIT_READ )
  L_ADD_CONST( L, CAP_AUDIT_WRITE )
  L_ADD_CONST( L, CAP_BLOCK_SUSPEND )
  L_ADD_CONST( L, CAP_CHOWN )
  L_ADD_CONST( L, CAP_DAC_OVERRIDE )
  L_ADD_CONST( L, CAP_DAC_READ_SEARCH )
  L_ADD_CONST( L, CAP_FOWNER )
  L_ADD_CONST( L, CAP_FSETID )
  L_ADD_CONST( L, CAP_IPC_LOCK )
  L_ADD_CONST( L, CAP_IPC_OWNER )
  L_ADD_CONST( L, CAP_KILL )
  L_ADD_CONST( L, CAP_LEASE )
  L_ADD_CONST( L, CAP_LINUX_IMMUTABLE )
  L_ADD_CONST( L, CAP_MAC_ADMIN )
  L_ADD_CONST( L, CAP_MAC_OVERRIDE )
  L_ADD_CONST( L, CAP_MKNOD )
  L_ADD_CONST( L, CAP_NET_ADMIN )
  L_ADD_CONST( L, CAP_NET_BIND_SERVICE )
  L_ADD_CONST( L, CAP_NET_BROADCAST )
  L_ADD_CONST( L, CAP_NET_RAW )
  L_ADD_CONST( L, CAP_SETUID )
  L_ADD_CONST( L, CAP_SETGID )
  L_ADD_CONST( L, CAP_SETFCAP )
  L_ADD_CONST( L, CAP_SETPCAP )
  L_ADD_CONST( L, CAP_SYS_ADMIN )
  L_ADD_CONST( L, CAP_SYS_BOOT )
  L_ADD_CONST( L, CAP_SYS_CHROOT )
  L_ADD_CONST( L, CAP_SYS_MODULE )
  L_ADD_CONST( L, CAP_SYS_NICE )
  L_ADD_CONST( L, CAP_SYS_PACCT )
  L_ADD_CONST( L, CAP_SYS_PTRACE )
  L_ADD_CONST( L, CAP_SYS_RAWIO )
  L_ADD_CONST( L, CAP_SYS_RESOURCE )
  L_ADD_CONST( L, CAP_SYS_TIME )
  L_ADD_CONST( L, CAP_SYS_TTY_CONFIG )
  L_ADD_CONST( L, CAP_SYSLOG )
  L_ADD_CONST( L, CAP_WAKE_ALARM )
#endif

  /* add the subtable to the main module table.
   * the subtable is now on top of the Lua stack,
   * while the main module table should be at the stack
   * position directly below it (i. e. at index -2).
   */
  lua_setfield ( L, -2, "Constants" ) ;

  /* end of function add_const */
}

/* this array contains the needed Lua wrapper functions for posix syscalls */
static const luaL_Reg sys_func [ ] =
{
  /* begin of struct array for exported Lua C functions */

  /* functions imported from "os_proc.c" : */
  { "getuid",			u_getuid	},
  { "geteuid",			u_geteuid	},
  { "getgid",			u_getgid	},
  { "getegid",			u_getegid	},
  { "getpid",			u_getpid	},
  { "getppid",			u_getppid	},
  { "setuid",			u_setuid	},
  { "setgid",			u_setgid	},
  { "seteuid",			u_seteuid	},
  { "setegid",			u_setegid	},
  { "setreuid",			u_setreuid	},
  { "setregid",			u_setregid	},
  { "getresuid",		u_getresuid	},
  { "getresgid",		u_getresgid	},
  { "setresuid",		u_setresuid	},
  { "setresgid",		u_setresgid	},
#if defined (OSLinux)
  { "setfsuid",			u_setfsuid	},
  { "setfsgid",			u_setfsgid	},
#endif
  { "getpgid",			u_getpgid	},
  { "setpgid",			u_setpgid	},
  { "getpgrp",			u_getpgrp	},
  { "setpgrp",			u_setpgrp	},
  { "getsid",			u_getsid	},
  { "setsid",			u_setsid	},
  { "do_setsid",		x_dosetsid	},
  { "tcgetpgrp",		Stcgetpgrp	},
  { "tcsetpgrp",		Stcsetpgrp	},
  { "getpriority",		Sgetpriority	},
  { "setpriority",		Ssetpriority	},
  { "nice",			u_nice		},
  { "group_member",		Sgroup_member	},
  { "initgroups",		Sinitgroups	},
  { "getgroups",		Sgetgroups	},
  { "setgroups",		Ssetgroups	},
  { "getgrouplist",		Sgetgrouplist	},
  { "kill",			u_kill		},
  { "killpg",			u_killpg	},
  { "raise",			u_raise		},
#ifdef RLIMIT_AS
  { "getrlimit_addrspace",	Lgetrlimit_addrspace	},
  { "setrlimit_addrspace",	Lsetrlimit_addrspace	},
  { "softlimit_addrspace",	Lsoftlimit_addrspace	},
#endif
#ifdef RLIMIT_CORE
  { "getrlimit_core",		Lgetrlimit_core		},
  { "setrlimit_core",		Lsetrlimit_core		},
  { "softlimit_core",		Lsoftlimit_core		},
#endif
#ifdef RLIMIT_CPU
  { "getrlimit_cpu",		Lgetrlimit_cpu		},
  { "setrlimit_cpu",		Lsetrlimit_cpu		},
  { "softlimit_cpu",		Lsoftlimit_cpu		},
#endif
#ifdef RLIMIT_DATA
  { "getrlimit_data",		Lgetrlimit_data		},
  { "setrlimit_data",		Lsetrlimit_data		},
  { "softlimit_data",		Lsoftlimit_data		},
#endif
#ifdef RLIMIT_FSIZE
  { "getrlimit_fsize",		Lgetrlimit_fsize	},
  { "setrlimit_fsize",		Lsetrlimit_fsize	},
  { "softlimit_fsize",		Lsoftlimit_fsize	},
#endif
#ifdef RLIMIT_LOCKS
  { "getrlimit_locks",		Lgetrlimit_locks	},
  { "setrlimit_locks",		Lsetrlimit_locks	},
  { "softlimit_locks",		Lsoftlimit_locks	},
#endif
#ifdef RLIMIT_MEMLOCK
  { "getrlimit_memlock",	Lgetrlimit_memlock	},
  { "setrlimit_memlock",	Lsetrlimit_memlock	},
  { "softlimit_memlock",	Lsoftlimit_memlock	},
#endif
#ifdef RLIMIT_MSGQUEUE
  { "getrlimit_msgqueue",	Lgetrlimit_msgqueue	},
  { "setrlimit_msgqueue",	Lsetrlimit_msgqueue	},
  { "softlimit_msgqueue",	Lsoftlimit_msgqueue	},
#endif
#ifdef RLIMIT_NICE
  { "getrlimit_nice",		Lgetrlimit_nice		},
  { "setrlimit_nice",		Lsetrlimit_nice		},
  { "softlimit_nice",		Lsoftlimit_nice		},
#endif
#ifdef RLIMIT_NOFILE
  { "getrlimit_nofile",		Lgetrlimit_nofile	},
  { "setrlimit_nofile",		Lsetrlimit_nofile	},
  { "softlimit_nofile",		Lsoftlimit_nofile	},
#endif
#ifdef RLIMIT_NPROC
  { "getrlimit_nproc",		Lgetrlimit_nproc	},
  { "setrlimit_nproc",		Lsetrlimit_nproc	},
  { "softlimit_nproc",		Lsoftlimit_nproc	},
#endif
#ifdef RLIMIT_RSS
  { "getrlimit_rss",		Lgetrlimit_rss	},
  { "setrlimit_rss",		Lsetrlimit_rss	},
  { "softlimit_rss",		Lsoftlimit_rss	},
#endif
#ifdef RLIMIT_RTPRIO
  { "getrlimit_rt_prio",	Lgetrlimit_rt_prio	},
  { "setrlimit_rt_prio",	Lsetrlimit_rt_prio	},
  { "softlimit_rt_prio",	Lsoftlimit_rt_prio	},
#endif
#ifdef RLIMIT_RTTIME
  { "getrlimit_rt_time",	Lgetrlimit_rt_time	},
  { "setrlimit_rt_time",	Lsetrlimit_rt_time	},
  { "softlimit_rt_time",	Lsoftlimit_rt_time	},
#endif
#ifdef RLIMIT_SIGPENDING
  { "getrlimit_sigpending",	Lgetrlimit_sigpending	},
  { "setrlimit_sigpending",	Lsetrlimit_sigpending	},
  { "softlimit_sigpending",	Lsoftlimit_sigpending	},
#endif
#ifdef RLIMIT_STACK
  { "getrlimit_stack",		Lgetrlimit_stack	},
  { "setrlimit_stack",		Lsetrlimit_stack	},
  { "softlimit_stack",		Lsoftlimit_stack	},
#endif
  { "getrlimit",		Sgetrlimit	},
  { "setrlimit",		Ssetrlimit	},
  { "umask",			u_umask		},
  { "sec_umask",		Lsec_umask	},
  { "delay",			Ldelay		},
  { "do_sleep",			Ldo_sleep	},
  { "hard_sleep",		Lhard_sleep	},
  { "sleep",			Ssleep		},
  { "usleep",			Susleep		},
  { "nanosleep",		Snanosleep	},
  { "wait",			u_wait		},
  { "waitpid",			u_waitpid	},
  { "waitid",			u_waitid	},
  { "set_subreaper",		Lset_subreaper	},
  { "is_subreaper",		Lis_subreaper	},
  { "exit",			u_exit		},
  { "abort",			u_abort		},
  { "_exit",			u__exit		},
  { "pause",			u_pause		},
  { "fork",			u_fork		},
  { "chroot",			u_chroot	},
  { "chdir",			u_chdir		},
  { "getcwd",			u_getcwd	},
  { "fchdir",			u_fchdir	},
  { "daemon",			u_daemon	},
  { "execl",			l_execl		},
  { "execl0",			l_execl0	},
  { "execlp",			l_execlp	},
  { "execlp0",			l_execlp0	},
  { "execv",			l_execv		},
  { "execv0",			l_execv0	},
  { "execvp",			l_execvp	},
  { "execvp0",			l_execvp0	},
  { "execve",			l_execve	},
  { "execve0",			l_execve0	},
  { "xexecl",			x_execl		},
  { "xexecl0",			x_execl0	},
  { "xexeclp",			x_execlp	},
  { "xexeclp0",			x_execlp0	},
  { "defsig_execl",		Ldefsig_execl	},
  { "defsig_execlp",		Ldefsig_execlp	},
  { "vfork_exec_nowait",	Lvfork_exec_nowait	},
  { "vfork_exec_wait",		Lvfork_exec_wait	},
  { "vrun",			Lvfork_exec_wait	},
  { "vfork_execp_nowait",	Lvfork_execp_nowait	},
  { "vfork_execp_wait",		Lvfork_execp_wait	},
  { "vrunp",			Lvfork_execp_wait	},
  { "vfork_exec0_nowait",	Lvfork_exec0_nowait	},
  { "vfork_exec0_wait",		Lvfork_exec0_wait	},
  { "vrun0",			Lvfork_exec0_wait	},
  { "vfork_execp0_nowait",	Lvfork_execp0_nowait	},
  { "vfork_execp0_wait",	Lvfork_execp0_wait	},
  { "vrunp0",			Lvfork_execp0_wait	},
  { "daemonize",		Ldaemonize	},
#ifndef OSopenbsd
  { "sigqueue",			Ssigqueue	},
#endif
#if defined (__GLIBC__) && defined (_GNU_SOURCE)
#endif
#if defined (OSsolaris) || defined (OSsunos5)
  { "sigsendset",		Ssigsendset	},
  { "kill_all_sol",		Lkill_all_sol	},
#endif
  /* end of imported functions from "os_proc.c" */

  /* functions imported from "os_time.c" : */
  { "utime",			u_utime		},
  { "utimes",			u_utimes	},
  { "lutimes",			u_lutimes	},
  { "futimes",			u_futimes	},
  { "futimens",			u_futimens	},
  { "alarm",			u_alarm		},
  { "ualarm",			u_ualarm	},
  { "getitimer",		u_getitimer	},
  { "setitimer",		u_setitimer	},
  { "tzset",			Stzset		},
  { "tzget",			Ltzget		},
  { "time",			Stime		},
  /*
  { "stime",			Sstime		},
  { "ftime",			Sftime		},
  */
  { "gettimeofday",		Sgettimeofday	},
  { "settimeofday",		Ssettimeofday	},
  { "ctime",			Sctime		},
  { "clock",			Sclock		},
  { "times",			Stimes		},
  { "getrusage_proc",		Lgetrusage_proc		},
  { "getrusage_children",	Lgetrusage_children	},
  { "getrusage_child",		Lgetrusage_children	},
#ifdef RUSAGE_THREAD
  { "getrusage_thread",		Lgetrusage_thread	},
#endif
#if defined (_POSIX_TIMERS) && (0 < _POSIX_TIMERS)
  { "clock_getcpuclockid",	Sclock_getcpuclockid	},
  { "clock_getres",		Sclock_getres		},
  { "clock_gettime",		Sclock_gettime		},
  { "clock_settime",		Sclock_settime		},
#endif
#if defined (OSLinux)
#elif defined (OSfreebsd)
#elif defined (OSnetbsd)
#elif defined (OSopenbsd)
#endif
  /* end of imported functions from "os_time.c" */

  /* functions imported from "os_svipc.c" : */
  { "ftok",			Sftok },
  { "msgget",			Smsgget },
  { "msgctl",			Smsgctl },
  { "msgsnd",			Smsgsnd },
  { "msgrcv",			Smsgrcv },
  { "mqv_set",			Lmqv_set },
  { "mqv_stat",			Lmqv_stat },
  { "mqv_remove",		Lmqv_remove },
  { "mqv_rm",			Lmqv_rm },
  { "mqv_empty",		Lmqv_empty },
  { "mqv_close",		Lmqv_close },
  { "mqv_open",			Lmqv_open },
  /* end of imported functions from "os_svipc.c" */

  /* functions imported from "os_streams.c" : */
#if defined (OSsolaris) || defined (OSsunos5)
  { "isastream",		Lisastream },
  { "fattach",			Lfattach },
  { "fdetach",			Ldetach },
  { "fputmsg",			Lputmsg },
  { "fputpmsg",			Lputpmsg },
  { "fgetmsg",			Lgetmsg },
  { "fgetpmsg",			Lgetpmsg },
  { "serv_stream",		Lserv_stream },
  { "check_stream",		Lcheck_stream },
  { "req_stream_fd",		Lreq_stream_fd },
#endif
  /* end of imported functions from "os_streams.c" */

  /* functions imported from "os_sig.c" : */
  { "strsignal",		Sstrsignal },
  { "pause_forever",		Lpause_forever },
  { "reset_sigs",		Lreset_sigs },
  { "block_all_sigs",		Lblock_all_sigs },
  { "unblock_all_sigs",		Lunblock_all_sigs },
  { "block_sig",		Lblock_sig },
  { "unblock_sig",		Lunblock_sig },
  { "sig_wait_all",		Lsig_wait_all },
  { "sig_timed_wait_all",	Lsig_timed_wait_all },
  { "sig_reset",		Lsig_reset },
  { "got_sig",			Lgot_sig	},
  { "trap_sig",			Ltrap_sig	},
#if defined (OSLinux)
  /*
  { "signalfd",			Lsignalfd	},
  */
#endif
  /* end of imported functions from "os_sig.c" */

  /* functions imported from "os_env.c" : */
  { "clearenv",			u_clearenv	},
  { "unsetenv",			u_unsetenv	},
  { "getenv",			u_getenv	},
#if defined (__GLIBC__) && defined (_GNU_SOURCE)
  { "secure_getenv",		u_secure_getenv	},
#endif
  { "setenv",			u_setenv	},
  { "putenv",			u_putenv	},
  { "get_environ",		Lget_environ	},
  { "addenv",			Laddenv		},
  { "newenv",			Lnewenv		},
  /* end of imported functions from "os_env.c" */

  /* functions imported from "os_file.c" : */
  { "sync",			u_sync		},
  { "fsync",			u_fsync		},
  { "fdatasync",		u_fdatasync	},
  { "dirname",			u_dirname	},
  { "basename",			u_basename	},
  { "base_name",		l_base_name	},
  { "dir_name",		        l_dir_name	},
  { "ends_with_slash",		Lends_with_slash	},
  { "realpath",			Srealpath	},
  { "truncate",			Struncate	},
  { "remove",			u_remove	},
  { "unlink",			u_unlink	},
  { "rename",			u_rename	},
  { "chmod",			u_chmod		},
  { "chown",			u_chown		},
  { "lchown",			u_lchown	},
  { "mkdir",			u_mkdir		},
  { "mkpath",			l_mkpath	},
  { "rmdir",			u_rmdir		},
  { "link",			u_link		},
  { "symlink",			u_symlink	},
  { "mkfifo",			u_mkfifo	},
  { "mknod",			u_mknod		},
  { "create_file",		l_create_file	},
  { "touch",			l_touch		},
  { "create",			l_create	},
  { "stat",			u_stat		},
  { "lstat",			u_lstat		},
  { "fstat",			u_fstat		},
  { "S_ISBLK",			mode_is_blk	},
  { "S_ISCHR",			mode_is_chr	},
  { "S_ISDIR",			mode_is_dir	},
  { "S_ISFIFO",			mode_is_fifo	},
  { "S_ISLNK",			mode_is_lnk	},
  { "S_ISREG",			mode_is_reg	},
  { "S_ISSOCK",			mode_is_sock	},
  { "readlink",			Sreadlink	},
  { "fdcompare",		Lfdcompare	},
  { "is_tmpfs",			Lis_tmpfs	},
  { "test_path",		Ltest_path	},
  { "is_nt",			Lis_nt		},
  { "is_newer",			Lis_nt		},
  { "isNewer",			Lis_nt		},
  { "is_ot",			Lis_ot		},
  { "is_older",			Lis_ot		},
  { "isOlder",			Lis_ot		},
  { "is_eq",			Lis_eq		},
  { "is_ef",			Lis_eq		},
  { "is_eqf",			Lis_eq		},
  { "is_eqFile",		Lis_eq		},
  { "is_hard_link",		Lis_eq		},
  { "isHardLink",		Lis_eq		},
  { "is_same_file",		Lis_eq		},
  { "isSameFile",		Lis_eq		},
  { "copy_file",		Lcopy_file	},
  { "move_file",		Lmove_file	},
  { "read_file",		Lread_file	},
  { "ftruncate",		Sftruncate	},
  { "fchmod",			Sfchmod		},
  { "scandir",			Sscandir	},
  { "list_dir",			get_dirent	},
  { "list_dir",			list_dir	},
  { "dir",			dir_iter_factory	},
  { "rm",			Lrm		},
  { "rmr",			Lrmr		},
/*{ "fchown",			Sfchown		},	*/
#if defined (__GLIBC__) && defined (_GNU_SOURCE)
#endif
  /* end of imported functions from "os_file.c" */

  /* functions imported from "os_io.c" : */
  { "open",			u_open	},
  { "creat",			Screat	},
  { "dup",			Sdup	},
  { "xdup",			Lxdup		},
  { "dup_fd",			Ldup_fd		},
  { "dup2",			Sdup2	},
  { "xdup2",			Lxdup2		},
  { "dup_to",			Ldup_to		},
  { "dup3",			Sdup3	},
  { "close",			Sclose	},
  { "close_noint",		Lclose_fd	},
  { "close_fd",			Lclose_fd	},
  { "fd_close",			Lclose_fd	},
  { "close_all",		Lclose_all	},
  { "lseek",			Slseek	},
  { "read",			Sread	},
  { "buf_read",			Lbuf_read	},
  { "buf_read_close",		Lbuf_read_close		},
  { "write",			Swrite	},
  { "pipe",			u_pipe	},
  { "open_pipe",		Lpipe	},
  { "stream_pipe",		Lstream_pipe	},
  /*
  { "socketpair",		Ssocketpair	},
  */
  { "fd_has_data",		Lfd_has_data	},
  { "fds_have_data",		Lfds_have_data	},
  { "wait_for_fd_data",		Lwait_for_fd_data	},
  { "fd_has_input",		Lfd_has_input	},
  { "poll_input_fd",		Lpoll_input_fd	},
  { "redirio",			Lredirio	},
  { "isatty",			Sisatty		},
  { "ttyname",			Sttyname	},
  { "ctermid",			Sctermid	},
  { "ctty",			Sctty	},
  { "flushall",			Lflushallout	},
  { "flushallout",		Lflushallout	},
  { "puts",			Sputs	},
  { "fileno",			Lfileno	},
  { "fdopen",			Lfdopen	},
  { "chvt",			l_chvt	},
#if defined (OSLinux)
  { "memfd_create",		Smemfd_create	},
  { "pipe2",			u_pipe2	},
#endif
  /* end of imported functions from "os_io.c" */

#if defined (AT_FDCWD)
  /* functions imported from "os_at.c": */
  { "openat",			u_openat	},
  { "faccessat",		u_faccessat	},
  { "mkdirat",			u_mkdirat	},
  { "mkfifoat",			u_mkfifoat	},
  { "unlinkat",			u_unlinkat	},
  { "renameat",			u_renameat	},
  { "linkat",			u_linkat	},
  { "symlinkat",		u_symlinkat	},
  { "fchmodat",			u_fchmodat	},
  { "fchownat",			u_fchownat	},
  { "utimensat",		u_utimensat	},
  { "fstatat",			u_fstatat	},
  /* end of imported functions from "os_at.c" */
#endif

  /* functions imported from "os_pw.c": */
  { "getpwuid",			Sgetpwuid	},
  { "getpwnam",			Sgetpwnam	},
  { "getgrgid",			Sgetgrgid	},
  { "getgrnam",			Sgetgrnam	},
  { "getuser",			Lgetuser	},
  { "getpw",			getpwu		},
  { "getgroup",			Lgetgroup	},
  { "getgr",			getgre		},
  /* end of imported functions from "os_pw.c" */

  /* functions imported from "os_match.c" : */
  { "glob",			u_glob		},
  { "wordexp",			u_wordexp	},
  { "fnmatch",			u_fnmatch	},
  /* end of imported functions from "os_match.c" */

  /* functions imported from "os_regex.c": */
  { "simple_regmatch",		simple_regmatch	},
  { "sregmatch",		simple_regmatch	},
  { "grep",			l_grep		},
  { "ncgrep",			l_ncgrep	},
  /* end of imported functions from "os_regex.c" */

  /* functions imported from "os_fs.c": */
  { "statvfs",			Sstatvfs	},
  { "fstatvfs",			Sfstatvfs	},
  { "statfs",			Sstatfs		},
  { "fstatfs",			Sfstatfs	},
  { "mountpoint",		Lmountpoint	},
  { "is_mount_point",		Lmountpoint	},
  /* end of imported functions from "os_fs.c" */

  /* functions imported from "os_socket.c": */
  { "socket",			u_socket	},
  { "socketpair",		u_socketpair	},
  /* end of imported functions from "os_socket.c" */

  /* functions imported from "os_net.c": */
  /*
  { "if_up",			Lif_up		},
  { "if_down",			Lif_down	},
  { "route_add_netmask",	Lroute_add_netmask	},
  { "route_add_defgw",		Lroute_add_defgw	},
  */
  /* end of imported functions from "os_net.c" */

  /* functions imported from "os_gen.c" : */
  { "umount",			Sunmount	},
  { "unmount",			Sunmount	},
  /* end of imported functions from "os_gen.c" */

  /* functions imported from "os_Linux.c" : */
#if defined (OSLinux)
  /* Linux specific functions */
  { "reboot",			u_reboot	},
  { "sys_reboot",		l_sys_reboot	},
  { "sys_halt",			l_sys_halt	},
  { "sys_poweroff",		l_sys_poweroff	},
  { "sys_hibernate",		l_sys_hibernate	},
  { "sys_kexec",		l_sys_kexec	},
  { "sys_sak_off",		l_sys_sak_off	},
  { "sys_sak_on",		l_sys_sak_on	},
  { "syncfs",			u_syncfs	},
  { "mount",			u_mount		},
  { "swapon",			u_swapon	},
  { "swapoff",			u_swapoff	},
  { "gettid",			Sgettid		},
  { "unshare",			u_unshare	},
  { "setns",			u_setns		},
  { "capget",			Scapget		},
  { "capset",			Scapset		},
  { "sysinfo",			u_sysinfo	},
  /*
  { "set_ns",			l_setns		},
  { "set_name_space",		l_setns		},
  { "load_module",		l_load_module	},
  { "setup_iface_lo",		l_setup_iface_lo	},
  { "get_pseudofs",		l_get_pseudofs	},
  { "cgroup_level",		l_cgroup_level	},
  */
# if defined (__GLIBC__)
  { "mtab_mount_point",		l_mtab_mount_point	},
  { "is_mtab_mount_point",	l_mtab_mount_point	},
# endif
#endif
  /* end of imported functions from "os_Linux.c" */

  /* skalibs wrapper functions */
#ifdef SKALIBS_VERSION
#endif
  /* end of skalibs wrapper functions */

  /* functions imported from "os_misc.c" : */
  { "bitneg",			x_bitneg	},
  { "bitand",			x_bitand	},
  { "bitor",			x_bitor		},
  { "bitxor",			x_bitxor	},
  { "bitlshift",		x_bitlshift	},
  { "bitrshift",		x_bitrshift	},
  { "add",			x_add		},
  { "subarr",			x_subarr	},
  { "strtoi",			x_strtoi	},
  { "octintstr",		x_octintstr	},
  { "chomp",			x_chomp		},
  { "get_errno",		x_get_errno	},
  { "strerror",			u_strerror	},
  { "strerror_r",		u_strerror_r	},
  { "getprogname",		x_getprogname	},
  { "uname",			u_uname		},
  { "acct",			u_acct		},
  { "sethostname",		u_sethostname		},
  { "gethostname",		u_gethostname		},
  { "setdomainname",		u_setdomainname		},
  { "getdomainname",		u_getdomainname		},
  { "gethostid",		u_gethostid		},
  { "sethostid",		u_sethostid		},
  { "get_urandom_int",		x_get_urandom_int	},
#if defined (OSLinux)
  { "pivot_root",		u_pivot_root		},
  { "get_random_int",		x_get_random_int	},
#elif defined (OSsolaris) || defined (OSsunos5)
  { "uadmin",			u_uadmin	},
#endif
  /* end of imported functions from "os_misc.c" */

  /* local to this file */
  /* end of local functions */

  /* last sentinel entry to mark the end of this array */
  { NULL,			NULL }

  /* end of struct array sys_func [] */
} ;

/* open function for Lua to open this very module/library */
static int openMod ( lua_State * const L )
{
  /*
  struct utsname uts ;
  extern char ** environ ;
  */

  /* create a metatable for directory iterators */
  (void) dir_create_meta ( L ) ;
  /* add posix wrapper functions to module table */
  luaL_newlib ( L, sys_func ) ;
  /* add posix constants to module */
  add_const ( L ) ;

  /* set version information */
  (void) lua_pushliteral ( L, "_VERSION" ) ;
  (void) lua_pushliteral ( L,
    "lux version " LUX_VERSION " for " LUA_VERSION " ("
#if defined (OS)
    OS ")"
#else
    "Unix)"
#endif
    " compiled " __DATE__ " " __TIME__
    ) ;
  lua_rawset ( L, -3 ) ;

  /* save the current environment into a subtable */
  /* better not, let the user call the coresponding function
   * when needed instead. this saves some memory.
  if ( environ && * environ && ** environ ) {
    int i, j ;
    char * s = NULL ;

    lua_newtable ( L ) ;

    for ( i = 0 ; environ [ i ] && environ [ i ] [ 0 ] ; ++ i ) {
      s = environ [ i ] ;
      for ( j = 0 ; '\0' != s [ j ] && '=' != s [ j ] ; ++ j ) ;
      if ( 0 < j && '=' == s [ j ] ) {
        (void) lua_pushlstring ( L, s, j ) ;
        if ( '\0' == s [ 1 + j ] ) {
          (void) lua_pushliteral ( L, "" ) ;
        } else {
          (void) lua_pushstring ( L, 1 + j + s ) ;
        }
        lua_rawset ( L, -3 ) ;
      }
    }

    lua_setfield ( L, -2, "ENV" ) ;
  }
  */

  /* set some useful vars as we are at it */
  (void) lua_pushliteral ( L, "OSNAME" ) ;
  (void) lua_pushliteral ( L,
#if defined (OS)
    OS
#else
    "Unix"
#endif
    ) ;
  lua_rawset ( L, -3 ) ;

  return 1 ;
}

void regMod ( lua_State * const L, const char * const name )
{
  (void) openMod ( L ) ;
  lua_setglobal ( L, ( name && * name ) ? name : "sys" ) ;
}

/* used as as Lua module (shared object), so luaopen_* functions
 * are needed, define them here.
 */
int luaopen_lux ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_luana ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_sys ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_posix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_ox ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_pox ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_psx ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_px ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_unix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_unx ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_ux ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_xinu ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_xo ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_xu ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_osunix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_os_unix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_os_posix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_sys_posix ( lua_State * const L )
{
  return openMod ( L ) ;
}

int luaopen_sys_unix ( lua_State * const L )
{
  return openMod ( L ) ;
}

