<?php
define('YALLO', "\033[33m");
define('RED', "\033[31m");
define('GREEN', "\033[32m");
define('BLUE', "\033[35m");
define('CYAN', "\033[36m");
define('UNCOLOR', "\033[0m");
function dbgp($m, $color = YALLO) {
    if ($m instanceof KScan === false &&
            is_array($m) || is_object($m))
        $m = var_export($m, true);
  echo PHP_EOL . $color . $m . UNCOLOR;}

require_once 'k.inc';

class KTest extends PHPUnit_Framework_TestCase {

  protected $conn = null;

  public function setUp(){ $this->conn = $k = new K("localhost", 1234); }
  public function tearDown(){ }

  private function q($stmt) { return $this->conn->k($stmt); }

  public function testAtoms() {
    $this->assertTrue(1   == $this->q('1'));
    $this->assertTrue('a' == $this->q('`a'));
  }
  public function testLists() {
    $this->assertTrue([1,2,3]       == $this->q('1 2 3'));
    $this->assertTrue(['a','b','c'] == $this->q('`a`b`c'));
  }
  public function testDicts() {
    $this->assertTrue(['a'=>1,'b'=>2]       == $this->q('`a`b!1 2'));
  }
}