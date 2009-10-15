AC_INIT([ckcore],[0.1])
AM_INIT_AUTOMAKE([-Wall -Werror])
LT_INIT

# These compilation flags must be set before AC_PROG_CC/CXX,
# or we'll get "-g -O2" by default.
CFLAGS="${CFLAGS=} -g3 -O2 -Wall"
CXXFLAGS="${CXXFLAGS=} -g3 -O2 -Wall"

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

AC_CONFIG_MACRO_DIR([m4])

# Version information (current:revision:age).
CKCORE_VERSION=1:0:0
AC_SUBST(CKCORE_VERSION)

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_DEFINE(_UNIX)
AC_DEFINE(_FILE_OFFSET_BIT,64,[Enable support for large files.])

AC_CONFIG_FILES([Makefile src/Makefile src/unix/Makefile])
AC_OUTPUT

