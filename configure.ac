dnl Standard configure.ac
AC_PREREQ(2.53)
AC_INIT([mod_receta], [0.1.0], [manutalcual@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-bzip2])
AM_CONFIG_HEADER([config.h:config.in])
AC_CONFIG_SRCDIR([src/mod_receta.c])

if test "`cd $srcdir; /bin/pwd`" = "`/bin/pwd`"; then
  AC_MSG_ERROR([you must configure in a separate build directory])
fi

dnl This is here so we can set $subdirs directly based on configure fragments.
AC_CONFIG_SUBDIRS()

AC_CANONICAL_HOST

dnl Checks for programs.
dnl AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

CPPFLAGS=""
CXXFLAGS=""
CFLAGS=""
LDFLAGS=""
AM_CPPFLAGS=""
AM_CFLAGS=""
AM_CXXFLAGS=""
AM_LDFLAGS=""

# Define a macro that is used to parse a --with-apache parameter
# The macro is named "APACHE_DIR"
AC_DEFUN([APACHE_DIR],[

        AC_ARG_WITH(
                apache,
		[  --with-apache[=DIR]     Apache server directory],
		,
		[with_apache="no"]
	)

	AC_MSG_CHECKING(for Apache directory)

	if test "$with_apache" = "no"; then
		AC_MSG_ERROR( You need to specify the apache directory using --with-apache)
	else
		# make sure that a well known include file exists
		if test -e $with_apache/include/httpd.h; then
			apache_dir=$with_apache
			AC_MSG_RESULT([yes])
		else
			AC_MSG_ERROR( $with_apache not found.  Check the value you specified with --with-apache)
		fi
	fi

])

# Now call the APACHE_DIR macro that was just specified
APACHE_DIR

# Define a macro that is used to parse a --with-apache parameter
# The macro is named "APACHE_DIR"
AC_DEFUN([ENABLE_DEBUG],[

        AC_ARG_ENABLE(
                debug,
		[--enable-debug Enable debug],
		[cv_enable_debug=$enableval],
		[cv_enable_debug=$enableval]
	)

	AC_MSG_CHECKING(wether to enable debug output)

	if test "$enable_debug" = "yes"; then
        	AC_MSG_RESULT([yes])
	else
        	AC_MSG_RESULT([no])
	fi

        AM_CONDITIONAL(DEBUG, [test "$cv_enable_debug" = "yes"])

])

ENABLE_DEBUG

user=`whoami`
AC_DEFINE_UNQUOTED([FILES_PATH], ["/home/$user/public_html"], [Data files path])

dnl Checks for libraries.

dnl Checks for header files.
dnl AC_HEADER_STDC
dnl AC_CHECK_HEADERS([sys/types.h sys/socket.h stdlib.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
dnl AC_ARG_WITH([categoria],
dnl
dnl 	AC_HELP_STRING([--with-categoria=CAT],
dnl 	[Definir categoria de log]),
dnl 	[categoria=$withval], [LOG_CATEGORIA=DEBUG])

dnl if test "$categoria" != ""; then
dnl 	CXXFLAGS="$CXXFLAGS -DLOG_CATEGORIA=\\"$categoria\\""
dnl fi


dnl AM_CONDITIONAL(FEATURE, [test x"$the_feature" != x])
dnl AC_DEFUN(AC_MSG_SUMMARY, [echo $1])
dnl AC_DEFUN(WITH,dnl [
dnl if test ${2_FALSE} ; then
dnl 	echo $1: enabled
dnl else
dnl 	echo $1: disabled
dnl fi
dnl ])
dnl
dnl AC_MSG_SUMMARY(------------------)
dnl AC_MSG_SUMMARY(  FEATURES:)
dnl AC_MSG_SUMMARY(------------------)
dnl WITH(feature, FEATURE)
dnl AC_MSG_SUMMARY(------------------)
dnl
dnl Checks for library functions.

dnl AC_SUBST(CPPFLAGS)
dnl AC_SUBST(LDFLAGS)

AC_SUBST(apache_dir)

AC_CONFIG_FILES([Makefile
        src/Makefile])
AC_OUTPUT
