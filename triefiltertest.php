<?php
ini_set('display_errors', 1);
error_reporting(E_ALL^E_NOTICE);



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
//小写转为大写 数字转为0 忽略关键字中的空格
//实际相当于 'HELLO WO RD WORD KEY 0123456789'

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