#if defined (OSsolaris) || defined (OSsunos5)

/*
 * wrapper/helper functions for SunOS5/Solaris specific OS features
 */

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

