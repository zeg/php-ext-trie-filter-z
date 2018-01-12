php-ext-trie-filter
===================

php extension for spam word filter based on Double-Array Trie tree, it can detect if a spam word exists in a text message.

关键词过滤扩展 用于检查一段文本中是否出现关键词 基于Double-Array Trie 树实现 在大量关键词的情况下能保持较高的效率 

搜索树可驻留内存 减少加载开销*

有GB编码模式 GB系编码文本不会出现误匹配**

搜索树中可储存关键字附加值 可用附加值作为关键字ID 便于做组合条件匹配


* 每个php-fpm进程会服务若干次请求后销毁 
  持久加载只加载一次便可保留在内存中 在整个php-fpm进程生命周期内重复使用 减少了重复加载搜索树文件的开销

** GB2312/GBK/GB18030编码因设计古旧 逐字节检查很容易出现相邻两字前后半连接刚好命中关键字的现象 造成误匹配(例如“面还”中间的两个字节是“婊”)
   目前常用的utf8编码设计更为科学 长字符中每个字节有不同的首位 从而杜绝了此种情况的发生(大概


## 升级历史

### 2017-12-19

增加GB编码匹配模式 会按照GB18030编码逐字匹配(默认为逐字节匹配) 可以防止误中

增加了持久加载搜索树的功能

修正一些bug(大概

### 2015-09-31

增加转换大写 数字换0 忽略空格功能

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
	
	foreach ($arrWord as $k => $v)
    	trie_filter_store($resTrie, $k, $v);
		//$k为对应此关键字的任意int32附加值 会在trie_filter_search/trie_filter_search_all结果中返回
	
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

	trie_filter_free($resTrie);

## 使用示例2
	<?php
	$arrWord = array('Key', 'w ord', '12345678');
	$resTrie = trie_filter_new();

	echo "\nsave key\n"; 
	foreach ($arrWord as $k => $v){
		echo "id:",$k,' -> ',$v,"\n";
		trie_filter_store($resTrie, $k, $v, TRIE_FILTER_UP|TRIE_FILTER_SP|TRIE_FILTER_NUM);
		}
	//去掉关键字中的空格 小写转为大写 数字转为0
	//实际相当于 array('WORD', 'KEY', '00000000');

	trie_filter_save($resTrie, __DIR__ . '/blackword.tree');

	$resTrie = trie_filter_load(__DIR__ . '/blackword.tree');

	$strContent = 'hello wo rd WORD kEy 0123456789';
	$arrRet = trie_filter_search_all($resTrie, $strContent, TRIE_FILTER_UP|TRIE_FILTER_SP|TRIE_FILTER_NUM);
	//小写转为大写 数字转为0 忽略文本中的空格
	//实际相当于 'HELLOWORDWORDKEY0000000000'

	print_all($strContent, $arrRet);

	function print_all($str, $res) {
		echo "\ntext:$str\n" , "\nmatch ", count($res)/3, "\n";
		for($i=0,$c=count($res);$i<$c;$i+=3)
			echo 'id:' , $res[$i+2] , ' -> ' , substr($str, $res[$i], $res[$i+1]) , "\n";
		}

	trie_filter_free($resTrie);


	/*输出为

	save key
	id:0 -> Key
	id:1 -> w ord
	id:2 -> 12345678

	text:hello wo rd WORD kEy 0123456789

	match 6
	id:1 -> wo rd
	id:1 -> WORD
	id:0 -> kEy
	id:2 -> 01234567
	id:2 -> 12345678
	id:2 -> 23456789
	*/

# PHP版本

PHP 5.2 or later.

Windows is not support until now.

## License

Apache License 2.0

## 致谢

本项目是在[用于检测敏感词的 PHP 扩展](http://blog.anbutu.com/php/php-ext-trie-filter)的基础上改写的。

