
define([svnversion], esyscmd([sh -c "svnversion|tr -d '\n'"]))dnl
AC_INIT([DTree],[1.5.svnversion],[adellj@gmail.com],[DTree])
AC_PREREQ(2.62)

AM_INIT_AUTOMAKE([DTree],[1.5])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
CPPFLAGS="-O3 -fPIC"

AC_CONFIG_FILES([Makefile
src/Makefile
src/inc/Makefile
src/libDTree/Makefile
src/DTree/Makefile])

AC_OUTPUT