<?php
/**
 * Helper autocomplete for php trie-filter extension
 * @author wulijun01234 <wulijun01234@gmail.com>
 * @auther zeg <zeg.email@gmial.com>
 * @link https://github.com/zeg/php-ext-trie-filter-z
 */

/**
 * Load a trie tree from a saved trie tree file
 * 
 * @param string $strDictFile Path to saved trie tree file
 * @param int $flag TRIE_FILTER_PLOAD load once, keep in memory
 * @return resource The trie tree handler or NULL on error.
 */
function trie_filter_load($strDictFile, $flag=0) {
	
}

/**
 * Find if a spam word exists in the content
 * 
 * @param resource $resTree
 * @param string $strContent
 * @param int $flag TRIE_FILTER_UP: all a-z char replace with A-Z
 *							TRIE_FILTER_SP: ignore space char
 *							TRIE_FILTER_NUM: char 0-9 replace with 0
 *							TRIE_FILTER_GB: use this when you data in GB2312/GBK/GB18030 encode
 * @return array spam word info, like Array(0 => start position, 1 => spam word len, 2 => spam word anydata stored in trie_filter_store), if no one found, return empty array, false on error.
 * @example
 * <pre>
 * $arrSpamWord = trie_filter_search($resTree, $strContent);
 * if (! empty($arrSpamWord)) {
 *     echo substr($strContent, $arrSpamWord[0], $arrSpamWord[1]);
 * }
 * </pre>
 */
function trie_filter_search($resTree, $strContent, $flag=0) {
	
}

/**
 * Find all spam word exists in the content
 * 
 * @param resource $resTree
 * @param string $strContent
 *	@param int $flag TRIE_FILTER_UP: all a-z char replace with A-Z
 *							TRIE_FILTER_SP: ignore space char
 *							TRIE_FILTER_NUM: char 0-9 replace with 0
 * @return array spam word info, like 
	Array(
		0 => start position, 1 => spam word len , 2 => spam word anydata stored in trie_filter_store ...), if no one found, return empty array, false on error.
 * @example
 * <pre>
 * $arrSpamWord = trie_filter_search_all($resTree, $strContent);
 * if (! empty($arrSpamWord)) {
		$c = count($arrSpamWord);
 *     foreach ($i=0;$i<$c;$i+=3) {
 *         echo $arrSpamWord[$i+2].':'.substr($strContent, $arrSpamWord[$i], $arrSpamWord[$i+1])."\t\n";
 *     }
 * }
 * </pre>
 */
function trie_filter_search_all($resTree, $strContent, $flag=0) {
	
}

/**
 * Create an empty trie tree
 *
 * @return resource The trie tree handler or NULL on error.
 */
function trie_filter_new() {
	
}

/**
 * Add a word to the trie tree
 * 
 * @param resource $resTree
 * @param int $anyData
 * @param string $strWord
 *	@param int $flag TRIE_FILTER_UP: all a-z char replace with A-Z
 *							TRIE_FILTER_SP: remove space char
 *							TRIE_FILTER_NUM: char 0-9 replace with 0
 * @return bool true on success or false on error.
 */
function trie_filter_store($resTree, $anyData, $strWord, $flag=0) {

}

/* {{{ proto array (int trie_tree_identifier, string dict_path)
 Returns true, or false on error*/

/**
 * Save trie tree to a file
 *
 * @param resource $resTree
 * @param string $strDictFile
 * @return bool true on success or false on error.
 */
function trie_filter_save($resTree, $strDictFile) {

}

/**
 * Free trie tree
 * 
 * Trie tree will be destructed automaticly when script finished, however, you can free it yourself. 
 *
 * @param resource $resTree
 * @return bool true on success or false on error.
 */
function trie_filter_free($resTree) {

}
