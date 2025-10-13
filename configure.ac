AC_INIT([bss], [1.0.0], [bug-report-address])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_PROGS(MAKE, [make gmake])
if test -z "$MAKE" ]; then
   AC_MSG_ERROR([No make program found])
fi

AC_CHECK_PROGS(MAKE, [make gmake])
if test -z "$MAKE"; then
   AC_MSG_ERROR([No make program found])
fi

AC_CONFIG_FILES([
   Makefile
])

AC_OUTPUT