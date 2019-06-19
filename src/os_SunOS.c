#if defined (OSsolaris) || defined (OSsunos5)

/*
 * wrapper/helper functions for SunOS5/Solaris specific OS features
 */

/*
 * sigsend(set)(2) related
 */

/* constants used by sigsend(set)(2) */
static int Lget_sigsend_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  /* idtype constants */
  L_ADD_CONST( L, P_ALL )
  L_ADD_CONST( L, P_CID )
  L_ADD_CONST( L, P_CTID )
  L_ADD_CONST( L, P_GID )
  L_ADD_CONST( L, P_MYID )
  L_ADD_CONST( L, P_PGID )
  L_ADD_CONST( L, P_PID )
  L_ADD_CONST( L, P_PROJID )
  L_ADD_CONST( L, P_SID )
  L_ADD_CONST( L, P_TASKID )
  L_ADD_CONST( L, P_UID )
  /* operation constants */
  L_ADD_CONST( L, POP_AND )
  L_ADD_CONST( L, POP_DIFF )
  L_ADD_CONST( L, POP_OR )
  L_ADD_CONST( L, POP_XOR )

  return 1 ;
}

/* binding for the sigsend(2) syscall */
static int Ssigsend ( lua_State * const L )
{
  const idtype_t t = (idtype_t) luaL_checkinteger ( L, 1 ) ;
  const id_t i = (id_t) luaL_checkinteger ( L, 2 ) ;
  const int s = (int) luaL_optinteger ( L, 3, SIGTERM ) ;

  if ( 0 <= s && NSIG > s ) {
    return res0 ( L, "sigsend", sigsend ( t, i, s ) ) ;
  }

  return luaL_argerror ( L, 3, "invalid signal number" ) ;
}

/* binding for the sigsendset(2) syscall */
static int Ssigsendset ( lua_State * const L )
{
  int s = SIGTERM ;
  procset_t ps ;

  ps . p_op = (idop_t) luaL_checkinteger ( L, 1 ) ;
  ps . p_lidtype = (idtype_t) luaL_checkinteger ( L, 2 ) ;
  ps . p_lid = (id_t) luaL_checkinteger ( L, 3 ) ;
  ps . p_ridtype = (idtype_t) luaL_checkinteger ( L, 4 ) ;
  ps . p_rid = (id_t) luaL_checkinteger ( L, 5 ) ;
  s = (int) luaL_optinteger ( L, 6, SIGTERM ) ;

  if ( 0 <= s && NSIG > s ) {
    return res0 ( L, "sigsendset", sigsendset ( & ps, s ) ) ;
  }

  return luaL_argerror ( L, 6, "invalid signal number" ) ;
}

/* use sigsendset on Solaris to avoid signaling our own process */
static int Lkill_all ( const int sig )
{
  const int s = (int) luaL_optinteger ( L, 1, SIGTERM ) ;

  if ( 0 <= s && NSIG > s ) {
    procset_t pset ;

    pset . p_op = POP_DIFF ;
    pset . p_lidtype = P_ALL ;
    pset . p_ridtype = P_PID ;
    pset . p_rid = P_MYID ;

    return res0 ( L, "sigsendset", sigsendset ( & pset, s ) ) ;
  }

  return luaL_argerror ( L, 1, "invalid signal number" ) ;
}

/*
 * swapctl(2) related
 */

/*
 * uadmin(2) related
 */

/* constants used by uadmin(2) */
static int Lget_mount_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  /* command constants */
  L_ADD_CONST( L, A_DUMP )
  L_ADD_CONST( L, A_FREEZE )
  L_ADD_CONST( L, A_REBOOT )
  L_ADD_CONST( L, A_REMOUNT )
  L_ADD_CONST( L, A_SHUTDOWN )
  /* function constants */
  L_ADD_CONST( L, AD_BOOT )
  L_ADD_CONST( L, AD_CHECK_SUSPEND_TO_DISK )
  L_ADD_CONST( L, AD_CHECK_SUSPEND_TO_RAM )
  L_ADD_CONST( L, AD_FASTREBOOT )
  L_ADD_CONST( L, AD_HALT )
  L_ADD_CONST( L, AD_IBOOT )
  L_ADD_CONST( L, AD_POWEROFF )
  L_ADD_CONST( L, AD_SUSPEND_TO_DISK )
  L_ADD_CONST( L, AD_SUSPEND_TO_RAM )

  return 1 ;
}

/* binding for the uadmin(2) syscall */
static int Suadmin ( lua_State * const L )
{
  return res0 ( L, uadmin ( (int) luaL_checkinteger ( L, 1 ),
    (int) luaL_checkinteger ( L, 2 ), NULL ) ) ;
}

static int Suadmin_shutdown ( lua_State * const L, const int c )
{
  sync () ;

  return res_zero ( L,
    uadmin ( A_SHUTDOWN, ( 0 < c ) ? c : AD_BOOT, NULL ) ) ;
}

static int Suadmin_reboot ( lua_State * const L )
{
  return Suadmin_shutdown ( L, AD_BOOT ) ;
}

static int Suadmin_halt ( lua_State * const L )
{
  return Suadmin_shutdown ( L, AD_HALT ) ;
}

static int Suadmin_poweroff ( lua_State * const L )
{
  return Suadmin_shutdown ( L, AD_POWEROFF ) ;
}

/*
 * (u)mount(2) related
 */

/* constants used by mount(2) */
static int Lget_mount_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  L_ADD_CONST( L, MS_DATA )
  L_ADD_CONST( L, MS_GLOBAL )
  L_ADD_CONST( L, MS_NOSUID )
  L_ADD_CONST( L, MS_OPTIONSTR )
  L_ADD_CONST( L, MS_OVERLAY )
  L_ADD_CONST( L, MS_RDONLY )
  L_ADD_CONST( L, MS_REMOUNT )

  return 1 ;
}

/* constants used by umount2(2) */
static int Lget_umount2_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  L_ADD_CONST( L, MS_FORCE )

  return 1 ;
}

#endif /* #ifdef OSsolaris */

