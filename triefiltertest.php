<?php
ini_set('display_errors', 1);
error_reporting(E_ALL^E_NOTICE);


if(!file_exists(__DIR__ . '/blackword.tree')){
	$arrWord = array('Key', 'w ord', '12345678');
	$resTrie = trie_filter_new();

	echo "\nsave key\n"; 
	foreach ($arrWord as $k => $v){
		echo "id:",$k,' -> ',$v,"\n";
		trie_filter_store($resTrie, 
			$k, //$k作为附加值储存
			$v, 
			TRIE_FILTER_UP //小写转为大写
			|TRIE_FILTER_SP //去掉关键字中的空格
			|TRIE_FILTER_NUM //数字转为0
			//实际相当于 array('WORD', 'KEY', '00000000');
			);
		}


	trie_filter_save($resTrie, __DIR__ . '/blackword.tree');
	}

$resTrie = trie_filter_load(__DIR__ . '/blackword.tree',
	TRIE_FILTER_PLOAD//持久加载 如果内存中有已加载的匹配树则直接使用 否则从文件加载
	//可在第一次运行后删除blackword.tree文件再次运行以测试持久加载是否生效
	);

$strContent = 'hello wo rd WORD kEy 0123456789';
$arrRet = trie_filter_search_all($resTrie, $strContent, 
		TRIE_FILTER_UP //小写转为大写
		|TRIE_FILTER_SP //忽略文本中的空格
		|TRIE_FILTER_NUM //数字转为0
		//实际相当于 'HELLOWORDWORDKEY0000000000'
		|TRIE_FILTER_GB //以GB编码进行匹配 如果数据是UTF8编码无需此项
		);


print_r($arrRet); 
//Array(
// 0 => ??, 第一个命中词的起始位置
// 1 => ??, 第一个命中词的长度
// 2 => ?? ,第一个命中词的附加值
// 3 => ??, 第二个...
// 4 => ?? , 第二个...
// 5 => ??   第二个...
// ...)

function print_all($str, $res) {
	echo "\ntext:$str\n" , "\nmatch ", count($res)/3, "\n";
	for($i=0,$c=count($res);$i<$c;$i+=3)
		echo 'id:' , $res[$i+2] , ' -> ' , substr($str, $res[$i], $res[$i+1]) , "\n";
	}

trie_filter_free($resTrie);//释放资源不影响持久加载


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