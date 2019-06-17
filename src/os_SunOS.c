#if defined (OSsolaris) || defined (OSsunos5)

/*
 * wrapper/helper functions for SunOS5/Solaris specific OS features
 */

/* constants used by mount(2) */
static int Lget_mount_flags ( lua_State * const L )
{
  lua_newtable ( L ) ;

  /* constants used by mount */
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

  /* constants used by umount */
  L_ADD_CONST( L, MS_FORCE )

  return 1 ;
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

#endif /* #ifdef OSsolaris */

