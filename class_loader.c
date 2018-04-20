/*
/+----------------------------------------------------------------------+
 | PHP CLI ClassLoader                                                  |
 +----------------------------------------------------------------------+
 | Copyright (c) 2016 Michał Brzuchalski                                |
 +----------------------------------------------------------------------+
 | This source file is subject to version 3.01 of the PHP license,      |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.php.net/license/3_01.txt.                                 |
 | If you did not receive a copy of the PHP license and are unable to   |
 | obtain it through the world-wide-web, please send a note to          |
 | license@php.net so we can mail you a copy immediately.               |
 +----------------------------------------------------------------------+
 | Author: Michał Brzuchalski <michal.brzuchalski@gmail.com>            |
 +----------------------------------------------------------------------+
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "main/php_ini.h"
#include "class_loader.h"
#include "Zend/zend_objects_API.h"

typedef struct _cl_obj cl_obj;
struct _cl_obj {
	HashTable    *prefixes;
	zend_object   std;
};

static inline cl_obj *cl_obj_from_obj(zend_object *obj) {
	return (cl_obj*)((char*)(obj) - XtOffsetOf(cl_obj, std));
}

#define Z_CLOBJECT_P(zv)  cl_obj_from_obj(Z_OBJ_P((zv)))

static zend_object *cl_object_new(zend_class_entry *class_type) /* {{{ */
{
	// cl_obj *intern = zend_object_alloc(sizeof(cl_obj), class_type);
    cl_obj *intern = ecalloc(1, sizeof(cl_obj) + zend_object_properties_size(class_type));
	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	return &intern->std;
} /* }}} */

static zend_object_handlers cl_object_handlers;


/* {{{ proto void Psr4Loader::__construct()
   Argument constructor */
ZEND_METHOD(Psr4Loader, __construct)
{
printf("\e[0;33mI am in Psr4Loader::__construct()... Yay!\e[0m\n");    
	zval *object = getThis();
	cl_obj *intern;

	intern = Z_CLOBJECT_P(object);
    intern->std.handlers = &cl_object_handlers;
}
/* }}} */

PHP_MINIT_FUNCTION(class_loader)
{
    zend_class_entry class_loader_ce;
    zend_class_entry psr4_loader_ce;
    
    zend_function_entry class_loader_methods[] = {
        PHP_ABSTRACT_ME(ClassLoader, loadClass,    arginfo_class_loader_loadClass)
        PHP_FE_END
    };
    INIT_CLASS_ENTRY(class_loader_ce, "ClassLoader", class_loader_methods);
    class_loader_ce_ptr = zend_register_internal_interface(&class_loader_ce);

    zend_function_entry psr4_loader_methods[] = {
        PHP_ME(Psr4Loader, __construct, arginfo_psr4_loader___construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
        PHP_FE_END
    };
    INIT_CLASS_ENTRY(psr4_loader_ce, "Psr4Loader", psr4_loader_methods);
	psr4_loader_ce.create_object = cl_object_new;
    psr4_loader_ce_ptr = zend_register_internal_class_ex(&psr4_loader_ce, NULL);
    memcpy(&cl_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers)); 
    cl_object_handlers.offset = XtOffsetOf(cl_obj, std);

    return SUCCESS;
}

PHP_RINIT_FUNCTION(class_loader)
{
#if defined(COMPILE_DL_CLASS_LOADER) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(class_loader)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(class_loader)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "class_loader support", "enabled");
    php_info_print_table_row(2, "class_loader version", PHP_CLASS_LOADER_VERSION);
    php_info_print_table_end();
}

static const zend_module_dep class_loader_deps[] = {
    ZEND_MOD_REQUIRED("pcre")
    ZEND_MOD_END
};

zend_module_entry class_loader_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    class_loader_deps,
    "class_loader",
    NULL,
    PHP_MINIT(class_loader),
    NULL,
    PHP_RINIT(class_loader),
    PHP_RSHUTDOWN(class_loader),
    PHP_MINFO(class_loader),
    PHP_CLASS_LOADER_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CLASS_LOADER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(class_loader)
#endif
