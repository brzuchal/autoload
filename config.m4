PHP_ARG_ENABLE(CLASS_LOADER, whether to enable ClassLoader support,
[ --enable-class-loader   Enable ClassLoader support])

    AC_MSG_CHECKING([for PCRE headers location])
    for i in  /usr/include /usr/include/pcre /usr/local/include; do
      test -f $i/pcre.h && PCRE_INCDIR=$i
    done

    if test -z "$PCRE_INCDIR"; then
      AC_MSG_ERROR([Could not find pcre.h in $PHP_PCRE_REGEX])
    fi
    AC_MSG_RESULT([$PCRE_INCDIR])

if test "$PHP_CLASS_LOADER" = "yes"; then
  AC_DEFINE(HAVE_CLASS_LOADER, 1, [class_loader])
  PHP_NEW_EXTENSION(class_loader, class_loader.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
