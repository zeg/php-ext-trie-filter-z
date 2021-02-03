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
  | license@php.net so we can mail you a copy immediately.              |
  +----------------------------------------------------------------------+
  | Author:  Lijun Wu    <wulijun01234@gmail.com>                     |
  |          zeg			<zeg.email@gmail.com>
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_trie_filter.h"

/* True global resources - no need for thread safety here */
static int le_trie_filter;
static int le_trie_filter_p;

/* {{{ trie_filter_functions[]
 *
 * Every user visible function must have an entry in trie_filter_functions[].
 */
zend_function_entry trie_filter_functions[] = {
	PHP_FE(trie_filter_load, NULL)
	PHP_FE(trie_filter_search, NULL)
    PHP_FE(trie_filter_search_all, NULL)
    PHP_FE(trie_filter_new, NULL)
    PHP_FE(trie_filter_store, NULL)
    PHP_FE(trie_filter_save, NULL)
    PHP_FE(trie_filter_free, NULL)
//	PHP_FE(trie_filter_retrieve, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in trie_filter_functions[] */
};
/* }}} */

/* {{{ trie_filter_module_entry
 */
zend_module_entry trie_filter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"trie_filter",
	trie_filter_functions,
	PHP_MINIT(trie_filter),
	PHP_MSHUTDOWN(trie_filter),
	NULL,
	NULL,
	PHP_MINFO(trie_filter),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TRIE_FILTER
ZEND_GET_MODULE(trie_filter)
#endif

/* {{{ PHP_INI
 */
/*
PHP_INI_BEGIN()
    PHP_INI_ENTRY("trie_filter.dict_charset", "utf-8", PHP_INI_ALL, NULL)
PHP_INI_END()
*/
/* }}} */

static void php_trie_filter_dtor(zend_resource *rsrc TSRMLS_DC)
{
	Trie *trie = (Trie *)rsrc->ptr;
	trie_free(trie);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(trie_filter)
{
	le_trie_filter = zend_register_list_destructors_ex(
			php_trie_filter_dtor, 
			NULL, PHP_TRIE_FILTER_RES_NAME, module_number);
	le_trie_filter_p = zend_register_list_destructors_ex(
			NULL, 
			php_trie_filter_dtor, PHP_TRIE_FILTER_RES_NAME, module_number);
	REGISTER_LONG_CONSTANT("TRIE_FILTER_UP", TRIE_FILTER_UP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TRIE_FILTER_SP", TRIE_FILTER_SP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TRIE_FILTER_NUM", TRIE_FILTER_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TRIE_FILTER_GB", TRIE_FILTER_GB, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TRIE_FILTER_PLOAD", TRIE_FILTER_PLOAD, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */



/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(trie_filter)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(trie_filter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "trie_filter support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource trie_filter_load(string dict_file_path)
   Returns resource id, or NULL on error*/
PHP_FUNCTION(trie_filter_load)
{
	Trie *trie;
	char *path;
	zend_string *hash_key;
	int path_len,hash_key_len;
	long opt=0;
	zval *ex;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", 
				&path, &path_len, &opt) == FAILURE) {
		RETURN_NULL();
	}

	hash_key = strpprintf( 0, "trie_filter:%s:%ld", path, opt);
	if ((ex = zend_hash_find(&EG(persistent_list), hash_key )) != NULL){
		RETURN_RES(zend_register_resource(Z_RES_P(ex)->ptr, le_trie_filter_p));
		zend_string_release(hash_key);
		return;
		}


	trie = trie_new_from_file(path);
	if (!trie) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
				"Unable to load %s", path);
		RETURN_NULL();
		zend_string_release(hash_key);
		return;
		}
	
	if(opt & TRIE_FILTER_PLOAD){
		zval lec;
		zend_resource le;
		RETURN_RES(zend_register_resource(trie, le_trie_filter_p));
		le.type = le_trie_filter_p;
		le.ptr = trie;
		ZVAL_RES(&lec, &le);
		zend_hash_update_ptr(&EG(persistent_list), hash_key, &lec);
		}
	else
		RETURN_RES(zend_register_resource(trie, le_trie_filter));

	zend_string_release(hash_key);

}
/* }}} */


static int trie_search_one(Trie *trie, AlphaChar *text, int *offset, TrieData *length, TrieData *data, long opt)
{
TrieState *s;
AlphaChar *p;
const AlphaChar *base;

base = text;
if (! (s = trie_root(trie)))
	return -1;

while (*text) {
	TRIE_SEARCH_ALPHA_PROC(text, opt);
	p = text;
	if (! trie_state_is_walkable(s, *p)) {
		trie_state_rewind(s);
		TRIE_SEARCH_MB_NEXT(text,opt);
		continue;
		} 
	else {
		trie_state_walk(s, *p++);
		}

	while (trie_state_is_walkable(s, *p) && ! trie_state_is_terminal(s)){
		trie_state_walk(s, *p++);
		TRIE_SEARCH_ALPHA_PROC(p, opt);
		}

	if (trie_state_is_terminal(s)) {
		*offset = text - base;
		*length = p - text;
		*data = trie_state_get_terminal_data(s);
		trie_state_free(s);
		return 1;
		}

	trie_state_rewind(s);
	TRIE_SEARCH_MB_NEXT(text,opt);
	}
trie_state_free(s);

return 0;
}//fe


static int trie_search_all(Trie *trie, AlphaChar *text, zval *data, long opt)
{
TrieState *s;
AlphaChar *p;
const AlphaChar *base;
//zval *word = NULL;

base = text;
if (! (s = trie_root(trie)))
	return -1;

while (*text) {
	TRIE_SEARCH_ALPHA_PROC(text, opt);

	if(! trie_state_is_walkable(s, *text)) {
		trie_state_rewind(s);
		TRIE_SEARCH_MB_NEXT(text,opt);
		continue;
		}

	p = text;

	while(*p && trie_state_is_walkable(s, *p) && ! trie_state_is_leaf(s)) {
	trie_state_walk(s, *p++);
	if (trie_state_is_terminal(s)) { 
			//MAKE_STD_ZVAL(word);
			//array_init_size(word, 3);
			add_next_index_long(data, (long)(text - base));
			add_next_index_long(data, (long)(p - text));
			add_next_index_long(data, (long)trie_state_get_terminal_data(s));
			//add_next_index_zval(data, word);
			}
		TRIE_SEARCH_ALPHA_PROC(p, opt);
		}
	trie_state_rewind(s);
	TRIE_SEARCH_MB_NEXT(text,opt);
	}
trie_state_free(s);

return 0;
}//fe

/*
PHP_FUNCTION(trie_filter_retrieve)
{
	Trie *trie;
	zval *trie_resource;
	unsigned char *text;
	int text_len, i, ret;
	TrieData data;

	AlphaChar *alpha_text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
				&trie_resource, &text, &text_len) == FAILURE) {
		RETURN_FALSE;
	}

    array_init(return_value);
    if (text_len < 1 || strlen(text) != text_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "input is empty");
		return;
	}

	ZEND_FETCH_RESOURCE(trie, Trie *, &trie_resource, -1, 
			PHP_TRIE_FILTER_RES_NAME, le_trie_filter);

	alpha_text = emalloc(sizeof(AlphaChar) * text_len + 1);

	for (i = 0; i < text_len; i++) {
		alpha_text[i] = (AlphaChar) text[i];
	}

	alpha_text[text_len] = TRIE_CHAR_TERM;

	ret = trie_retrieve(trie, alpha_text, &data);
    efree(alpha_text);
	if (ret) {
		add_next_index_long(return_value, data);
	} else {
        RETURN_FALSE;
    }
}
*/

/* {{{ proto array trie_filter_search(int trie_tree_identifier, string centent)
   Returns info about first keyword, or false on error*/
PHP_FUNCTION(trie_filter_search)
{
	Trie *trie;
	zval *trie_resource;
	unsigned char *text;

	int text_len, offset = -1, i, ret;
   TrieData length = 0, data;
	long opt=0;
	
	AlphaChar *alpha_text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", 
				&trie_resource, &text, &text_len, &opt) == FAILURE) {
		RETURN_FALSE;
	}

    array_init(return_value);
    if (text_len < 1 || strlen(text) != text_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "input is empty");
		RETURN_FALSE;
	}

	if( (trie = (Trie *)zend_fetch_resource2(Z_RES_P(trie_resource), PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p)) == NULL){
		RETURN_FALSE;
		}

	alpha_text = emalloc(sizeof(AlphaChar) * text_len + 1);

	for (i = 0; i < text_len; i++) {
		alpha_text[i] = (AlphaChar) text[i];
	}

	alpha_text[text_len] = TRIE_CHAR_TERM;

	ret = trie_search_one(trie, alpha_text, &offset, &length, &data, opt);
    efree(alpha_text);
	if (ret == 0) {
        return;
    } else if (ret == 1) {
		add_next_index_long(return_value, (long)offset);
		add_next_index_long(return_value, (long)length);
		add_next_index_long(return_value, (long)data);
	} else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto array trie_filter_search_all(int trie_tree_identifier, string centent)
   Returns info about all keywords, or false on error*/
PHP_FUNCTION(trie_filter_search_all)
{
	Trie *trie;
	zval *trie_resource;
	unsigned char *text;
	int text_len, i, ret;
	long opt=0;

	AlphaChar *alpha_text;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", 
				&trie_resource, &text, &text_len, &opt) == FAILURE) {
		RETURN_FALSE;
		}

	array_init(return_value);
	if (text_len < 1 || strlen(text) != text_len) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "input is empty");
		return;
		}

	if( (trie = (Trie *)zend_fetch_resource2(Z_RES_P(trie_resource), PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p)) == NULL){
		RETURN_FALSE;
		}

	alpha_text = emalloc(sizeof(AlphaChar) * text_len + 1);

	for (i = 0; i < text_len; i++) {
		alpha_text[i] = (AlphaChar) text[i];
	}
	
	alpha_text[text_len] = TRIE_CHAR_TERM;

	ret = trie_search_all(trie, alpha_text, return_value, opt);
	efree(alpha_text);
	if (ret == 0) {
		return;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto resource trie_filter_new()
   Returns resource id, or NULL on error*/
PHP_FUNCTION(trie_filter_new)
{
    Trie *trie;
    AlphaMap *alpha_map;
    int ret;

    alpha_map = alpha_map_new();
    if (! alpha_map) {
        RETURN_NULL();
    }

    if (alpha_map_add_range(alpha_map, 0x00, 0xff) != 0) {
        /* treat all strings as byte stream */
        alpha_map_free(alpha_map);
        RETURN_NULL();
    }

    trie = trie_new(alpha_map);
    alpha_map_free(alpha_map);
    if (! trie) {      
        RETURN_NULL();
    }
	RETURN_RES(zend_register_resource(trie, le_trie_filter));
}
/* }}} */


/* {{{ proto bool trie_filter_store(int trie_tree_identifier, int anyData, string keyword)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_store)
{
    Trie *trie;
	zval *trie_resource;
	unsigned char *keyword, *p;
	int keyword_len, i;
	long ldata,opt=0;
	TrieData data;
    AlphaChar alpha_key[KEYWORD_MAX_LEN+1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls|l", 
				&trie_resource, &ldata, &keyword, &keyword_len, &opt) == FAILURE) {
		RETURN_FALSE;
	}
    if (keyword_len > KEYWORD_MAX_LEN || keyword_len < 1) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "keyword should has [1, %d] bytes", KEYWORD_MAX_LEN);
        RETURN_FALSE;
    }
	
	//	ZEND_FETCH_RESOURCE2(trie, Trie *, &trie_resource, -1, 
	//		PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p);

	if( (trie = (Trie *)zend_fetch_resource2(Z_RES_P(trie_resource), PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p)) == NULL){
		RETURN_FALSE;
		}


    p = keyword;
    i = 0;
	
	data = (int32)ldata;

	while (*p && *p != '\n' && *p != '\r') {
		if((opt & TRIE_FILTER_SP) && *p == ' '){
			p++;
			continue;
			}
		alpha_key[i] = (AlphaChar)*p;
		if((opt & TRIE_FILTER_UP) && alpha_key[i]>96 && alpha_key[i]<123){
			alpha_key[i]-=32;
			}
		if((opt & TRIE_FILTER_NUM) && alpha_key[i]>47 && alpha_key[i]<58){
			alpha_key[i]=48;
			}
		i++;
		p++;
	}
	 
    alpha_key[i] = TRIE_CHAR_TERM;
    if (! trie_store(trie, alpha_key, data)) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool trie_filter_save(int trie_tree_identifier, string dict_path)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_save)
{
    Trie *trie;
    zval *trie_resource;
    unsigned char *filename;
    int filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", 
				&trie_resource, &filename, &filename_len) == FAILURE) {
		RETURN_FALSE;
		}
	if (filename_len < 1 || strlen(filename) != filename_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "save path required");
		RETURN_FALSE;
		}
	if( (trie = (Trie *)zend_fetch_resource2(Z_RES_P(trie_resource), PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p)) == NULL ){
		RETURN_FALSE;
		}
	if (trie_save(trie, filename)) {
		RETURN_FALSE;
		}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool trie_filter_free(int trie_tree_identifier)
   Returns true, or false on error*/
PHP_FUNCTION(trie_filter_free)
{
    Trie *trie;
    zval *trie_resource;
    //int resource_id;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &trie_resource) == FAILURE) {
        RETURN_FALSE;
    	}
	
	if( (trie = (Trie *)zend_fetch_resource2(Z_RES_P(trie_resource), PHP_TRIE_FILTER_RES_NAME, le_trie_filter, le_trie_filter_p) ) == NULL){
		RETURN_FALSE;
		}

	zend_hash_index_del(&EG(regular_list), Z_RESVAL_P(trie_resource));
	RETURN_TRUE;
    //resource_id = Z_RESVAL_P(trie_resource);
    //if (zend_list_delete(resource_id) == SUCCESS) {
    //    RETURN_TRUE;
    //}
    //RETURN_FALSE;   
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
