#if defined (OSsolaris) || defined (OSsunos5)

/*
 * wrapper/helper functions for SunOS5/Solaris specific OS features
 */

/*
 * sigsend(set)(2) related
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

