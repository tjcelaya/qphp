<?php
define('YALLO', "\033[33m");
define('RED', "\033[31m");
define('GREEN', "\033[32m");
define('PURP', "\033[35m");
define('CYAN', "\033[36m");
define('UNCOLOR', "\033[0m");
$DP = false;
function dbgp($m, $color = YALLO,$nl = false) {
  global $DP;
  if (!$DP) return;  
  if ($m instanceof KScan === false &&
      is_array($m) || is_object($m) || is_bool($m))
    $m = var_export($m, true);
  echo PHP_EOL . $color . $m . UNCOLOR . ($nl?PHP_EOL:'');}
function qtype($t) {
  $maybeAtom = $t<0;
  return [
    0   => 'general list',
    1   => 'boolean',
    2   => 'guid',
    4   => 'byte',
    5   => 'short',
    6   => 'int',
    7   => 'long',
    8   => 'real',
    9   => 'float',
    10  => 'char',
    11  => 'symbol',
    12  => 'timestamp',
    13  => 'month',
    14  => 'date',
    15  => 'datetime',
    16  => 'timespan',
    17  => 'minute',
    18  => 'second',
    19  => 'time',
    98  => 'table',
    99  => 'dict',
    100 => 'lambda',
    101 => 'unary primitive',
    102 => 'binary primitive',
    103 => 'ternary (operator)',
    104 => 'projection',
    105 => 'composition',
    106 => 'f',
    107 => 'f/',
    108 => 'f\\',
    109 => 'f\':',
    110 => 'f/:',
    111 => 'f\:',
    112 => 'dynamic load',
  ][$t].' '.($maybeAtom?'(atom)':'(list)');
}
