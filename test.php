<?php
require_once 'dbg.inc';
require_once 'k.inc';

class KTest extends PHPUnit_Framework_TestCase {

  private $conn = null;

  public function setUp(){
    $this->conn = $k = new K("localhost", 1234); }
  public function tearDown(){ }

  private function q($stmt) { return $this->conn->k($stmt); }

  public function testAtoms() {
    // $this->assertEquals(25, $this->q('25i'));
    // $this->assertEquals(9, $this->q('9j'));
    global $DP; $DP = true;
    $this->assertEquals('A', $this->q('`A'));
    // $this->assertEquals('z', $this->q('"z"'));
  }
  public function testTemporal() {
    $this->markTestSkipped();
    $this->assertEquals('a', $this->q('2015.6.26'));
  }
  public function testLists() {
    $this->markTestSkipped();
    $this->assertEquals(['Z'], $this->q('enlist `Z'));
    $this->assertEquals(['Z'], $this->q('enlist "Z"'));
    $this->assertEquals('abc', $this->q('"abc"'));
    // $this->assertEquals([1,2,3], $this->q('1 2 3'));
    // $this->assertEquals([[1,2,3]], $this->q('enlist 1 2 3'));
    // $this->assertEquals([1,2,3,'a','b'], $this->q('1 2 3,`a`b'));
    // $this->assertEquals(['a','b','c'], $this->q('`a`b`c'));
    // $this->assertEquals([['a','b','c']], $this->q('enlist `a`b`c'));
  }
  public function testDicts() {
    $this->markTestSkipped();
    // global $DP; $DP = true;
    $this->assertEquals(['a'=>1,'b'=>2], $this->q('`a`b!1 2'));
    $this->assertEquals(['a'=>[1],'b'=>[2]], $this->q('`a`b!enlist each 1 2i'));
    $this->assertEquals(['a'=>1,'b'=>2,'c'=>'d'], $this->q('`a`b`c!1 2,`d'));
  }
  public function testSingleRowTable() {
    $this->markTestSkipped();
    global $DP; $DP = true;
    // $this->assertEquals(
    //   [
    //     ['a'=>1,'b'=>2],
    //   ], $this->q('([]a:enlist 1;b: enlist 2)'));
    // $this->assertEquals(
    //   [
    //     ['a'=>'c','b'=>'d'],
    //   ], $this->q('([]a:enlist `c;b: enlist `d)'));
    // $this->assertEquals(
    //   [
    //     ['a'=>'c'],
    //   ], $this->q('([]a:enlist `c)'));
    // $this->assertEquals(
    //   [
    //     ['a'=>'d','b'=>[1,2,3],'c'=>['q'=>5,'w'=>6]],
    //   ], $this->q('([]a:enlist `d;b:enlist 1 2 3;c:(enlist `q`w!5 6))'));
    $this->assertEquals(
      [
        ['q'=>'w','e'=>'r'],
      ], $this->q('([]q:enlist "e";w: enlist "r")'));
  }
  public function testMultiRowTable() {
    $this->markTestSkipped();
    $this->markTestSkipped();
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
    $this->assertEquals(
      [
        ['a'=>'c','b'=>'f'],
        ['a'=>'d','b'=>'g'],
        ['a'=>'e','b'=>'h'],
      ], $this->q('flip `a`b!(enlist "cde"),enlist"fgh"'));
  }
  public function testSortedSingleRowTable() {
    $this->markTestSkipped();
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
    $this->markTestSkipped();
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
    $this->markTestSkipped();
    // global $DP; $DP = true;
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