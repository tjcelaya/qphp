<?php

require_once("k.inc");

$k=new K("localhost", 1234);
foreach ([
    'T',
    // 'kT',
    // '() xkey kT',
  ] as $expr) {
    echo PHP_EOL.$expr.PHP_EOL."\033[33m=============\033[0m".PHP_EOL;
    var_dump($T=$k->k($expr));
}

echo "\033[31m========================================\033[0m".PHP_EOL;
