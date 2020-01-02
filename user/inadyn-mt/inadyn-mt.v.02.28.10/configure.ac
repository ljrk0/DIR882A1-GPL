#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])

AC_INIT([inadyn-mt], [02.24.38], [bhoover@wecs.com])

AC_CONFIG_AUX_DIR([Cfg])

AC_CANONICAL_TARGET([])

AM_INIT_AUTOMAKE([inadyn-mt],[02.24.38])

AC_CONFIG_SRCDIR([src/dyndns.c])

AC_CONFIG_HEADERS([src/config.h])

AC_LANG([C])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_CHECK_HEADERS([ao/ao.h arpa/inet.h fcntl.h locale.h memory.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h \ 
sys/socket.h sys/time.h syslog.h unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC

AC_ARG_ENABLE(sound,
	AC_HELP_STRING([--disable-sound], [disable audible alerts -- default audible]))
AC_ARG_ENABLE(threads,
	AC_HELP_STRING([--disable-threads], [disable multithreaded flow control -- default threads if not audible disabled]))
AC_ARG_ENABLE(threads,
	AC_HELP_STRING([--enable-threads], [enable multithreaded flow control -- default no threads if audible disabled]))
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug], [enable debug output]))
AC_ARG_ENABLE(build,
	AC_HELP_STRING([--enable-build=(linux,solaris,mac)], [platform; defaults to linux]))
AC_ARG_ENABLE(dynamic,
	AC_HELP_STRING([--disable-dynamic], [disable shared lib dynamic load -- default dynamic]))

AC_SUBST(SNDFILE_LIB)
AC_SUBST(TARGET_ARCH)
AC_SUBST(IS_WITH_SOUND)
AC_SUBST(IS_WITH_THREADS)
AC_SUBST(ARCH_SPECIFIC_LIBS)
AC_SUBST(ARCH_SPECIFIC_CFLAGS)
AC_SUBST(ARCH_SPECIFIC_LDFLAGS)

#meta files install/dirs -- don't mkdirs if already exist
AC_SUBST(IS_INADYN_MT_DIR)
AC_SUBST(IS_INADYN_MT_LANG_DIR)
AC_SUBST(IS_INADYN_MT_EXTRA_DIR)
AC_SUBST(INSTALL_PREFIX)

SNDFILE_LIB=""
ARCH_SPECIFIC_LIBS=""
ARCH_SPECIFIC_CFLAGS=""
ARCH_SPECIFIC_LDFLAGS=""
TARGET_ARCH="linux"
INSTALL_PREFIX=""

#check for debug output
if test x$enable_debug = xyes ; then

	AC_DEFINE([DEBUG],[1],[More debug log output])
fi

if test x$enable_dynamic = xno ; then

	AC_DEFINE([NO_DYN_LOAD],[1],[Disallow load some libraries dynamically])
fi

if test x$enable_build = xsolaris ; then

	ARCH_SPECIFIC_LIBS="${ARCH_SPECIFIC_LIBS} -lsocket -lnsl -lrt"
fi

if test x$enable_build = xmac ; then

	ARCH_SPECIFIC_CFLAGS="${ARCH_SPECIFIC_CFLAGS} -pedantic"
fi

if test $prefix = "NONE" ; then

	INSTALL_PREFIX=/usr/share
else

	if test $prefix = "/usr/local" ; then

		INSTALL_PREFIX=/usr/local/etc

	else

		INSTALL_PREFIX=$prefix
	fi
fi

AC_DEFINE_UNQUOTED([META_ROOT],["$INSTALL_PREFIX/inadyn-mt"],[install prefix])

AM_CONDITIONAL([IS_INADYN_MT_DIR],[test -d $INSTALL_PREFIX/inadyn-mt])
AM_CONDITIONAL([IS_INADYN_MT_LANG_DIR],[test -d $INSTALL_PREFIX/inadyn-mt/lang])
AM_CONDITIONAL([IS_INADYN_MT_EXTRA_DIR],[test 1 = 1])

AM_CONDITIONAL([IS_WITH_SOUND],[test 1 = 1])
AM_CONDITIONAL([IS_WITH_THREADS],[test 1 = 1])

ARCH_SPECIFIC_LIBS="${ARCH_SPECIFIC_LIBS} -lpthread"

#default to sound, threads
if test x$enable_sound = xno ; then

	#no sound; no program wide threads unless specifically enabled

	AM_CONDITIONAL([IS_WITH_SOUND],[test 1 = 2])

	if ! test x$enable_threads = xyes ; then

		AM_CONDITIONAL([IS_WITH_THREADS],[test 1 = 2])
	else

		AC_DEFINE([USE_THREADS],[1],[Multithreaded flow control])
	fi
else

	#sound; threads unless specifically disabled

	AM_CONDITIONAL([IS_INADYN_MT_EXTRA_DIR],[test -d $INSTALL_PREFIX/inadyn-mt/extra])

	AC_DEFINE([USE_SNDFILE],[1],[Audible alerts on network status trouble])

	ARCH_SPECIFIC_LIBS="${ARCH_SPECIFIC_LIBS} -lm"

	if test x$enable_dynamic = xno ; then

		SNDFILE_LIB="-lao"
	else

		AC_SEARCH_LIBS([dlopen],[dl])
	fi

	if test x$enable_threads = xno ; then

		AM_CONDITIONAL([IS_WITH_THREADS],[test 1 = 2])
	else	
		AC_DEFINE([USE_THREADS],[1],[Multithreaded flow control])
	fi
fi

AC_PROG_CC

AM_PROG_CC_C_O

AC_PROG_INSTALL

AC_CONFIG_FILES([ \

	man/Makefile \

	src/Makefile \

	Makefile \
	])

AC_OUTPUT
