# configure.ac for the gpligc/ogie project

AC_INIT([gpligc], [1.11])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor gettimeofday pow sqrt strcasecmp strchr strtol])

# check host
AC_CANONICAL_HOST

# some checks dont work on my mingw32 compile platform
AS_IF(
  [test "x$host_os" != xmingw32],
  AC_CHECK_LIB([glut], [glutInit], [], [AC_MSG_ERROR([glut library not found]) ])
  AC_CHECK_LIB([GLU], [gluErrorString], [], [AC_MSG_ERROR([GLU library not found]) ])
  AC_CHECK_LIB([jpeg], [jpeg_std_error], [], [AC_MSG_ERROR([jpeg library not found])] )
  ,
  echo "Platform is $host_os  setting LIBS=-lglut -lglu32 -lopengl32 -ljpeg"
  LIBS="-lglut -lglu32 -lopengl32 -ljpeg"
)
# -static-libgcc -static-libstdc++  works, but produces larger archive files


# on pc12 this is needed as of ? now?
AC_CHECK_LIB([GL], [glBegin])
#[glXCreatePbuffer])
AC_CHECK_LIB([X11], [XOpenDisplay])

AC_CHECK_FUNC([glXCreatePbuffer], [AC_DEFINE([HAVE_GLXPBUFFER], [1],
                               [Define if glXCreatePbuffer exists.])])

# use of osmesa is default, but optional
AC_ARG_ENABLE([osmesa],
            [AS_HELP_STRING([--enable-osmesa],  [support for osmesa @<:@default=check@:>@])],
            [],  [enable_osmesa=check])
LIBOSMESA=
  AS_IF(
    [test "x$enable_osmesa" != xno],
    [AC_CHECK_LIB([OSMesa], [OSMesaCreateContextExt], [], [if test "x$enable_osmesa" != xcheck; then
      AC_MSG_FAILURE([--enable-osmesa was given, but test for osmesa failed])
      fi])
    ]
  )


# use of gpsd is default, but optional
AC_ARG_WITH([gpsd],
            [AS_HELP_STRING([--with-gpsd],  [support for gpsd @<:@default=check@:>@])],
            [],  [with_gpsd=check])
LIBGPS=
  AS_IF(
    [test "x$with_gpsd" != xno],
    [AC_CHECK_LIB([gps], [read], [], [if test "x$with_gpsd" != xcheck; then
      AC_MSG_FAILURE([--with-gpsd was given, but test for gpsd failed])
      fi])
    ]
  )

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 perl/Makefile
 images/Makefile
])
AC_OUTPUT
