<?php
define('YALLO', "\033[33m");
define('RED', "\033[31m");
define('GREEN', "\033[32m");
define('PURP', "\033[35m");
define('CYAN', "\033[36m");
define('UNCOLOR', "\033[0m");
$DP = false;
function b2h() { return call_user_func_array('bin2hex',func_get_args());}
function dd() { echo"\n";call_user_func_array('var_dump',func_get_args()); die;}
function dbgp($m, $color = YALLO,$nl = false) {
  global $DP;
  if (!$DP) return;  
  if ($m instanceof KScan === false && !is_string($m))
    $m = var_export($m, true);
  echo PHP_EOL . $color . $m . UNCOLOR . ($nl?PHP_EOL:'');}
function qtype($t) {
  $maybeAtom = $t<0;
  return [
    0   => 'general list',
    1   => 'boolean',     // 0x01
    2   => 'guid',        // 0x02
    4   => 'byte',        // 0x04
    5   => 'short',       // 0x05
    6   => 'int',         // 0x06
    7   => 'long',        // 0x07
    8   => 'real',        // 0x08
    9   => 'float',       // 0x09
    10  => 'char',        // 0x0a
    11  => 'symbol',      // 0x0b
    12  => 'timestamp',   // 0x0c
    13  => 'month',       // 0x0d
    14  => 'date',        // 0x0e
    15  => 'datetime',    // 0x0f
    16  => 'timespan',    // 0x10
    17  => 'minute',      // 0x11
    18  => 'second',      // 0x12
    19  => 'time',        // 0x13

    98  => 'table',       // 0x62

    99  => 'dict',        // 0x63
    127 => 'sorted dict', // 0x7f

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
  ][$maybeAtom?-$t:$t].' ('.($maybeAtom?'atom':'list').')';
}
function applyDepth($str, $depth) {
    return implode("\n",array_map(function($s)use($depth){return str_repeat('    ', $depth).$s;},explode("\n", $str)));
}
