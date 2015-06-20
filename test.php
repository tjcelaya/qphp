<?php
require_once 'k.inc';

class KTest extends PHPUnit_Framework_TestCase {

  private $conn = null;

  public function setUp(){
    $this->conn = $k = new K("localhost", 1234); }
  public function tearDown(){ }

  private function q($stmt) { return $this->conn->k($stmt); }
  private function simpleTests($arr) {
    foreach ($arr as $req => $res) {
      $this->assertEquals($this->q($req),$res);}}

  public function testAtoms() {
    $this->simpleTests([
      '25i'  => 25,
      '9j'   => 9,
      '`ABC' => 'ABC',
      '"z"'  => 'z',
    ]);
  }
  public function testTemporal() {
    $this->assertEquals(1435276800,$this->q('2015.06.26'));
    $this->assertEquals('2015-06-26',(new K('localhost',1234,null,['dt_fmt'=>true]))->k('2015.06.26'));
  }
  public function testLists() {
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
  public function testDictsWithAtoms() {
    $this->simpleTests([
      '`X`Y!1 2'            => ['X'=>1,'Y'=>2],
      '`X`Y`Z!1 2j,`def'    => ['X'=>1,'Y'=>2,'Z'=>'def'],
      '`X`Y!1,enlist "qwe"' => ['X'=>1,'Y'=>['q','w','e']],
      '`X`Y`Z!"qwe"'        => ['X'=>'q','Y'=>'w','Z'=>'e'],
      '`Xx`Yy`Z!"qwe"'      => ['Xx'=>'q','Yy'=>'w','Z'=>'e'],
    ]);
  }
  public function testDictsWithLists() {
    $this->simpleTests([
      '`X`Y!(1 2;3 4)'               => ['X'=>[1,2],'Y'=>[3,4]],
      '`X`Y!enlist each 1 2i'        => ['X'=>[1],'Y'=>[2]],
      '`X`Y`Z!1 2,`d'                => ['X'=>1,'Y'=>2,'Z'=>'d'],
      '`X`Y!(enlist 12),enlist"er"'  => ['X'=>12,'Y'=>['e','r']],
      '`X`Y!12,enlist"er"'           => ['X'=>12,'Y'=>['e','r']],
      '`X`Y!(enlist"qw"),enlist"er"' => ['X'=>['q','w'],'Y'=>['e','r']],
    ]);

    $this->assertEquals(
      $this->q('`X`Y!(enlist"q"),enlist"er"'),
      ['X'=>'q','Y'=>['e','r']]
    );
  }
  public function testSingleRowTable() {
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
    $this->assertEquals(
      [
        ['a'=>'c','b'=>'f'],
        ['a'=>'d','b'=>'g'],
        ['a'=>'e','b'=>'h'],
      ], $this->q('flip `a`b!(enlist "cde"),enlist"fgh"'));
  }
  public function testSortedSingleRowTable() {
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
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2]
      ], $this->q('([a:enlist 1]b:enlist 2)'));
    $this->assertEquals(
      [
        ['a'=>1,'b'=>2],
        ['a'=>3,'b'=>4],
      ], $this->q('([a:1 3]b:2 4)'));

    $qFetchAsObjects = (new K('localhost',1234,null,['row_fmt'=>K::ROW_OBJ]));

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
    $this->assertEquals(
      // test passes, but is it correct?
      // x| (+`m`n!(8 9;3 4);12)
      // y| `m`n!(1;2 3)
      [
        "x"=>[[["m"=>8,"n"=>3],["m"=>9,"n"=>4]],12],
        "y"=>["m"=>1,"n"=>[2,3]]
      ],
      $this->q('`x`y!(((flip `m`n!(8 9;3 4));12);`m`n!(enlist 1),(enlist 2 3))'));
    $this->assertEquals(
      //  | m n
      // -| ---
      // x| 8 9
      ["x"=>["m"=>8,"n"=>9]],
      $this->q('(enlist `x)!(enlist `m`n!8 9)'));

    // how should we handle duplicate keys?
    $this->assertEquals(
      ["x"=>[["m"=>88,"n"=>99]]],
      $this->q('`x`x!(enlist `m`n!8 9;enlist `m`n!88 99)'));
    $this->assertEquals(
      ["x"=>["m"=>88,"n"=>99]],
      $this->q('`x`x!(`m`n!8 9;`m`n!88 99)'));

    $this->assertEquals(
      [
            ["a"=>"A","b"=>"B","c"=>"C","d"=>"D","e"=>"E"],
            ["a"=>"B","b"=>"C","c"=>"D","d"=>"E","e"=>"A"],
            ["a"=>"C","b"=>"D","c"=>"E","d"=>"A","e"=>"B"],
            ["a"=>"D","b"=>"E","c"=>"A","d"=>"B","e"=>"C"],
            ["a"=>"E","b"=>"A","c"=>"B","d"=>"C","e"=>"D"]
      ],
      $this->q('flip `a`b`c`d`e!(til 5) rotate\: `char$(`int$"A")+til 5'));
    $this->assertEquals(
      [
        "a" => [],
        "b" => [
           ["A","B","C","D","E"]
        ],
        "c" => [
           ["A","B","C","D","E"],
           ["B","C","D","E","A"]
        ],
        "d" => [
           ["A","B","C","D","E"],
           ["B","C","D","E","A"],
           ["C","D","E","A","B"]
        ],
        "e" => [
           ["A","B","C","D","E"],
           ["B","C","D","E","A"],
           ["C","D","E","A","B"],
           ["D","E","A","B","C"]
        ]
      ],
      $this->q('`a`b`c`d`e!(til 5) #\: (til 5) rotate\: `char$(`int$"A")+til 5'));

    // apparently we can handle this it just looked weird
    $this->assertEquals(
      ["x"=>["m"=>8,"n"=>9],"y"=>["m"=>1,"Z"=>[2,3]]],
      $this->q('`x`y!(`m`n!8 9;`m`Z!(enlist 1),(enlist 2 3))'));
          
    // (enlist `x`x)!((enlist `m`n!8 9))
    //    | m n
    // ---| ---
    // x x| 8 9
    // currently returns [ ['x','x','m'=>8,'n'=>9 ] ]


  }
  public function testFetchingAsObjects() {
    $qFetchAsObjects = (new K('localhost',1234,null,['row_fmt'=>K::ROW_OBJ]));
    $this->assertEquals(
      [(object) ['q'=>'e','w'=>'r']],
      $qFetchAsObjects->k('([]q:enlist "e";w: enlist "r")'));
    $this->assertEquals(
      [
        (object)['a'=>'d','b'=>[1,2,3],'c'=>['q'=>5,'w'=>6]],
        (object)['a'=>'word','b'=>99,'c'=>['q'=>55,'w'=>66]],
      ],
      $qFetchAsObjects->k('([]a:`d`word;b:(enlist 1 2 3),enlist 99;c:(enlist `q`w!5 6),enlist `q`w!55 66)'));
    // same as above but with a as a key
    $this->assertEquals(
      [
        (object)['a'=>'d','b'=>[1,2,3],'c'=>['q'=>5,'w'=>6]],
        (object)['a'=>'word','b'=>99,'c'=>['q'=>55,'w'=>66]],
      ],
      $qFetchAsObjects->k('([a:`d`word]b:(enlist 1 2 3),enlist 99;c:(enlist `q`w!5 6),enlist `q`w!55 66)'));

  }

  public function testErrors() {
    $this->setExpectedException('KException');
    $this->q('`m`n!1,2 3');
  }
}
