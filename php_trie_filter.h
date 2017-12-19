/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:  Lijun Wu    <wulijun01234@gmail.com>                           |
  |          zeg			<zeg.email@gmail.com>
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include <iconv.h>
#include <datrie/trie.h>

#ifndef PHP_TRIE_FILTER_H
#define PHP_TRIE_FILTER_H

extern zend_module_entry trie_filter_module_entry;
#define phpext_trie_filter_ptr &trie_filter_module_entry

#ifdef PHP_WIN32
#define PHP_TRIE_FILTER_API __declspec(dllexport)
#else
#define PHP_TRIE_FILTER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define ALPHA_CHARSET	"UCS-4LE"
#define PHP_TRIE_FILTER_RES_NAME "Trie tree filter"

PHP_MINIT_FUNCTION(trie_filter);
PHP_MSHUTDOWN_FUNCTION(trie_filter);
PHP_RINIT_FUNCTION(trie_filter);
PHP_RSHUTDOWN_FUNCTION(trie_filter);
PHP_MINFO_FUNCTION(trie_filter);

PHP_FUNCTION(trie_filter_load);
PHP_FUNCTION(trie_filter_search);
PHP_FUNCTION(trie_filter_search_all);
PHP_FUNCTION(trie_filter_new);
PHP_FUNCTION(trie_filter_store);
PHP_FUNCTION(trie_filter_save);
PHP_FUNCTION(trie_filter_free);
//PHP_FUNCTION(trie_filter_retrieve);

#ifdef ZTS
#define TRIE_FILTER_G(v) TSRMG(trie_filter_globals_id, zend_trie_filter_globals *, v)
#else
#define TRIE_FILTER_G(v) (trie_filter_globals.v)
#endif

#endif	/* PHP_TRIE_FILTER_H */

#define KEYWORD_MAX_LEN 1024
#define TRIE_FILTER_UP 1
#define TRIE_FILTER_SP 2
#define TRIE_FILTER_NUM 4
#define TRIE_FILTER_GB 8
#define TRIE_FILTER_PLOAD 1

#define TRIE_CACHE_SIZE 100

#define TRIE_SEARCH_ALPHA_PROC(p, opt) if(opt!=0){  \
while((opt & TRIE_FILTER_SP) && *p==32 && *(p+1)!=TRIE_CHAR_TERM)\
	p++; \
if((opt & TRIE_FILTER_UP) && *p>96 && *p<123) \
	(*p)-=32; \
if((opt & TRIE_FILTER_NUM) && *p>47 && *p<58) \
	(*p)=48; \
}


#define TRIE_SEARCH_MB_NEXT(p, opt) if(opt & TRIE_FILTER_GB){  \
	if((*p>0x81 && *p<0xfe) && (*(p+1) && *(p+1)>0x40 && *(p+1)<0xfe && *(p+1)!=0x7f)) \
		p+=2; \
	else if((*p>0x81 && *p<0xfe) && (*(p+1) && *(p+1)>0x30 && *(p+1)<0x39) && (*(p+2) && *(p+2)>0x81 && *(p+2)<0xfe) && (*(p+3) && *(p+3)>0x30 && *(p+3)<0x39)) \
		p+=4; \
	else \
		p++; \
	} \
else{ \
	p++; \
	}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
