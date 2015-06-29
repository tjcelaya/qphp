<?php
require_once 'dbg.inc';
require_once 'k.inc';

class KTest extends PHPUnit_Framework_TestCase {

  private $conn = null;

  public function setUp(){
    $this->conn = $k = new K("localhost", 1234, 'anon', ['datetime'=>true]); }
  public function tearDown(){ }

  private function q($stmt) { return $this->conn->k($stmt); }
  private function simpleTests($arr) {foreach ($arr as $req => $res) {
    $this->assertEquals($this->q($req),$res);}}

  public function testAtoms($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->simpleTests([
      '25i'  => 25,
      '9j'   => 9,
      '`ABC' => 'ABC',
      '"z"'  => 'z',
    ]);
  }
  public function testTemporal($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->assertEquals($this->q('2015.06.26'),1435276800);
    // $this->assertEquals(
    //   new DateTime('2015-06-26'),
    //   (new K('localhost',1234,'',['dt'=>true]))->q('2015.06.26'));
  }
  public function testLists($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->simpleTests([
      'enlist `Z'     => ['Z'],
      'enlist "Z"'    => ['Z'],
      '"XYZ"'         => ['X','Y','Z'],
      '1 2 3i'        => [1,2,3],
      '1 2 3j'        => [1,2,3],
      'enlist 1 2 3'  => [[1,2,3]],
      '1 2 3,`Z`X'    => [1,2,3,'Z','X'],
      '`a`b`c'        => ['a','b','c'],
      'enlist `a`b`c' => [['a','b','c']],
      '(1 2 3;4 5 6)' => [[1,2,3],[4,5,6]],
    ]);
  }
  public function testDictsWithAtoms($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->simpleTests([
      '`X`Y!1 2'            => ['X'=>1,'Y'=>2],
      '`X`Y`Z!1 2j,`def'    => ['X'=>1,'Y'=>2,'Z'=>'def'],
      '`X`Y!1,enlist "qwe"' => ['X'=>1,'Y'=>['q','w','e']],
      '`X`Y`Z!"qwe"'        => ['X'=>'q','Y'=>'w','Z'=>'e'],
      '`Xx`Yy`Z!"qwe"'      => ['Xx'=>'q','Yy'=>'w','Z'=>'e'],
    ]);
  }
  public function testDictsWithLists($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->simpleTests([
      '`X`Y!(1 2;3 4)'               => ['X'=>[1,2],'Y'=>[3,4]],
      '`X`Y!enlist each 1 2i'        => ['X'=>[1],'Y'=>[2]],
      '`X`Y`Z!1 2,`d'                => ['X'=>1,'Y'=>2,'Z'=>'d'],
      '`X`Y!(enlist 12),enlist"er"'  => ['X'=>12,'Y'=>['e','r']],
      '`X`Y!12,enlist"er"'           => ['X'=>12,'Y'=>['e','r']],
      '`X`Y!(enlist"qw"),enlist"er"' => ['X'=>['q','w'],'Y'=>['e','r']],
    ]);

    // this needs to be fixed? should be the other way around
    $this->assertEquals(
      $this->q('`X`Y!(enlist"q"),enlist"er"'),
      ['X'=>'q','Y'=>['e','r']]
    );

  }
  public function testSingleRowTable($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->assertEquals(
      [
        ['X'=>1,'Y'=>2],
      ], $this->q('([]X:enlist 1;Y: enlist 2)'));
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
    $this->assertEquals(
      [
        ['q'=>'e','w'=>'r'],
      ], $this->q('([]q:enlist "e";w: enlist "r")'));
  }
  public function testMultiRowTable($skip=false) {
    if($skip)$this->markTestSkipped();
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
  public function testSortedSingleRowTable($skip=false) {
    if($skip)$this->markTestSkipped();
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
      ], $this->q('`s#([]a:enlist 1;b:enlist 2)'));
    $this->assertEquals(
      [
        ['a'=>'c'],
      ], $this->q('`s#([]a:enlist `c)'));
  }
  public function testSortedMultiRowTable($skip=false) {
    if($skip)$this->markTestSkipped();
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
  public function testKeyedTable($skip=false) {
    if($skip)$this->markTestSkipped();
      global $stop;$stop=true;

    $this->assertEquals(
      [
        ['a'=>1,'b'=>2]
      ], $this->q('([a:enlist 1]b:enlist 2)'));
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
        ['a'=>3,'b'=>4],
      ], $this->q('([a:1 3]b:2 4)'));
  }
  public function testThingsWithWeirdStructure() {
    $this->assertEquals(
      [
        [
          'a'=>['x'=>1,'y'=>[2,3,4]],
          'b'=>['q'=>3,'w'=>4],
        ]
      ],
      $this->q('flip `a`b!(enlist `x`y!1,enlist 2 3 4;enlist`q`w!3 4)'));
  }
}