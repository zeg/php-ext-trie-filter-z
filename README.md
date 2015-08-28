php-ext-trie-filter
===================

php extension for spam word filter based on Double-Array Trie tree, it can detect if a spam word exists in a text message.

关键词过滤扩展，用于检查一段文本中是否出现敏感词，基于Double-Array Trie 树实现。

## 升级历史

### 2015-08-28

恢复了在搜索树中储存关键字附加值的功能 修正了非最长匹配取不到附加值的问题

简化trie_filter_search_all返回的结构

详见使用示例

### 2013-06-23
trie_filter_search_all，一次返回所有的命中词;修复内存泄露

## 依赖库

修改过的[libdatrie](https://github.com/zeg/libdatrie-z)

包含有 https://github.com/kmike/datrie/ 中的trie_state_get_terminal_data函数

## 安装步骤

下面的$LIB_PATH为依赖库安装目录，$INSTALL_PHP_PATH为PHP5安装目录。

### 安装libdatrie
    $ tar zxvf libdatrie-0.2.4.tar.gz
    $ cd libdatrie-0.2.4
    $ make clean
    $ ./configure --prefix=$LIB_PATH
    $ make
    $ make install

### 安装扩展   
    $ $INSTALL_PHP_PATH/bin/phpize
    $ ./configure --with-php-config=$INSTALL_PHP_PATH/bin/php-config --with-trie_filter=$LIB_PATH
    $ make
    $ make install

然后修改php.ini，增加一行：extension=trie_filter.so，然后重启PHP。

## 使用示例
	<?php
	$arrWord = array('word1', 'word2', 'word3');
	$resTrie = trie_filter_new(); //create an empty trie tree
	foreach ($arrWord as $k => $v) {
    	trie_filter_store($resTrie, $k, $v);//$k为对应此关键字的任意int32附加值 会在trie_filter_search/trie_filter_search_all结果中返回
	}
	trie_filter_save($resTrie, __DIR__ . '/blackword.tree');

	$resTrie = trie_filter_load(__DIR__ . '/blackword.tree');

	$strContent = 'hello word2 word1';
	$arrRet = trie_filter_search($resTrie, $strContent);
	print_r($arrRet); //Array(0 => 6, 1 => 5, 2 => 1)
	echo substr($strContent, $arrRet[0], $arrRet[1]); //word2
	
	
	$arrRet = trie_filter_search_all($resTrie, $strContent);
	print_r($arrRet); //Array(
	// 0 => 6, 第一个命中词的起始位置
	// 1 => 5, 第一个命中词的长度
	// 2 => 1 ,第一个命中词的附加值
	// 3 => 12, 第二个...
	// 4 => 5 , 第二个...
	// 5 => 0   第二个...
	// ...)
	

	$arrRet = trie_filter_search($resTrie, 'hello word');
	print_r($arrRet); //Array()

	trie_filter_free($resTrie);

# PHP版本

PHP 5.2 or later.

Windows is not support until now.

## License

Apache License 2.0

## 致谢

本项目是在[用于检测敏感词的 PHP 扩展](http://blog.anbutu.com/php/php-ext-trie-filter)的基础上改写的。

