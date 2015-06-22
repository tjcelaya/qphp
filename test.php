<?php
require_once 'dbg.inc';
require_once 'k.inc';

class KTest extends PHPUnit_Framework_TestCase {

  protected $conn = null;

  public function setUp(){
    $this->conn = $k = new K("localhost", 1234); }
  public function tearDown(){ }

  private function q($stmt) { return $this->conn->k($stmt); }

  public function testAtoms() {
    $this->assertEquals(1, $this->q('1'));
    $this->assertEquals('a', $this->q('`a'));
    $this->assertEquals('a', $this->q('"a"'));
    $this->assertEquals('abc', $this->q('"abc"'));
  }
  public function testLists() {
    $this->assertEquals([1,2,3], $this->q('1 2 3'));
    $this->assertEquals([[1,2,3]], $this->q('enlist 1 2 3'));
    $this->assertEquals([1,2,3,'a','b'], $this->q('1 2 3,`a`b'));
    $this->assertEquals(['a','b','c'], $this->q('`a`b`c'));
    $this->assertEquals(['a'], $this->q('enlist `a'));
    $this->assertEquals([['a','b','c']], $this->q('enlist `a`b`c'));
  }
  public function testDicts() {
    $this->assertEquals(['a'=>1,'b'=>2], $this->q('`a`b!1 2'));
    $this->assertEquals(['a'=>[1],'b'=>[2]], $this->q('`a`b!enlist each 1 2'));
    $this->assertEquals(['a'=>1,'b'=>2,'c'=>'d'], $this->q('`a`b`c!1 2,`d'));
  }
  public function testSingleRowTable() {
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
      ], $this->q('([]a:enlist 1;b: enlist 2)'));
    $this->assertEquals(
      [
        ['a'=>'c','b'=>'d'],
      ], $this->q('([]a:enlist `c;b: enlist `d)'));
    $this->assertEquals(
      [
        ['a'=>'c'],
      ], $this->q('([]a:enlist `c)'));
    $this->assertEquals(
      [
        ['a'=>'d','b'=>[1,2,3],'c'=>['q'=>5,'w'=>6]],
      ], $this->q('([]a:enlist `d;b:enlist 1 2 3;c:(enlist `q`w!5 6))'));
    // $this->assertEquals(
    //   [
    //     (object) ['a'=>(object)['c'],'b'=>(object)['d']],
    //   ], $this->q('([]a:enlist "c";b: enlist "d")'));
  }
  public function testMultiRowTable() {
    $this->assertEquals(
      [
        ['a'=>'c'],
        ['a'=>'d'],
      ], $this->q('([]a:`c`d)'));
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
        ['a'=>3,'b'=>4],
      ], $this->q('([]a:1 3;b:2 4)'));
    $this->assertEquals(
      [
        ['a'=>'d','b'=>[1,2,3],'c'=>['q'=>5,'w'=>6]],
        ['a'=>'e','b'=>[7,8,9],'c'=>['e'=>10,'r'=>11]],
      ], $this->q('([]a:`d`e;b:(1 2 3;7 8 9);c:( (`q`w!5 6);(`e`r!10 11) ))'));
  }
  public function testSortedSingleRowTable() {
    // global $DP; $DP = true;
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
      ], $this->q('`s#([]a:enlist 1;b:enlist 2)'));
    $this->assertEquals(
      [
        ['a'=>'c'],
      ], $this->q('`s#([]a:enlist `c)'));
  }
  public function testSortedMultiRowTable() {
    // global $DP; $DP = true;
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
        ['a'=>3,'b'=>4],
      ], $this->q('`s#([]a:1 3;b:2 4)'));
    $this->assertEquals(
      [
        ['a'=>'c'],
        ['a'=>'d'],
      ], $this->q('`s#([]a:`c`d)'));
  }
  public function testKeyedTable() {
    global $DP; $DP = true;
    $this->assertEquals(
      [
        (object) ['a'=>1,'b'=>2]
      ], $this->q('([a:enlist 1]b:enlist 2)'));
    // $this->assertEquals(
    //   [
    //     (object) ['a'=>1,'b'=>2]
    //   ], $this->q('([a:1 3]b:2 4)'));
  }

}