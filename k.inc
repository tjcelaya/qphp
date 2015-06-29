<?php
require_once 'dbg.inc';
require_once 'kscan.inc';

class K {
  const DICT = 9;
  const TABLE = 8;
  private static $be = -1;
  private static $bigint = true;

  private $h;
  private $collapseCharLists=false;
  private $dt=false;

  private static function init() {
    if (self::$be === -1) {
      self::$be = (pack("N", -69) === pack("l", -69));
      try { pack('q',1); } catch (\Exception $e) {$bigint=false;}
    }
  }

  public function __construct($h = "localhost", $p = 1234, $up = "anonymous", $opts=[]) {
    dbgp('__construct called', UNCOLOR);
    self::init();
    if (isset($opts['dt']) && $opts['dt']) {
      $this->dt=true;}

    $this->h = fsockopen($h, $p);
    fwrite($this->h, $up . pack("cc", 1, 0));
    if (ord(fgetc($this->h)) != 1) {
      throw new Exception("Can't connect to k: $h:$p");
    }
  }
  function __destruct() {
    fclose($this->h);
  }
  public static function q($s) {
    return (new static )->k($s);
  }
  public function ks($s) {                                                      dbgp('ks called', UNCOLOR);
    if (func_num_args() === 1) {
      $x = self::enc(0, $s);
    } else {
      $x = self::enc(0, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function k($s) {                                                       dbgp("k called: $s", UNCOLOR);
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    //                                                                          dbgp($x);
    fwrite($this->h, $x);
    return $this->kr($s);
  }

  public function ka($s) {                                                      dbgp('ka called', UNCOLOR);
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function kr($req = '') {                                               dbgp('kr called', UNCOLOR);
    $a = unpack("l2", fread($this->h, 8));
    $scanner = new KScan(fread($this->h, $a[2] - 8), $req);
    return $this->dec($scanner);
  }

  public static function S($x) {
    $s = pack("c", -11) . $x . pack("c", 0);
    return (object) array("s" => $s);
  }

  public static function G($x = null) {
    if ($x === null) {
      if (function_exists('openssl_random_pseudo_bytes')) {
        $x = openssl_random_pseudo_bytes(16);
      } else {
        $x = "";
        for ($i = 0; $i < 16; ++$i) {
          $x .= chr(mt_rand(0, 255));
        }
      }
    } else {
      $x = hex2bin(str_replace("-", "", $x));
    }
    return (object) array("s" => pack("c", -2) . $x);
  }

  private static function aa($x) {
    $n = min(100, count($x));
    for ($i = 0; $i < $n; ++$i) {
      if (!isset($x[$i])) {
        return true;
      }
    }
    return false;
  }

  private static function e1($x) {
    if (is_null($x)) {
      return pack("cc", 101, 0);
    }
    if ($x === true) {
      return pack("cc", -1, 1);
    }
    if ($x === false) {
      return pack("cc", -1, 0);
    }
    if (is_string($x)) {
      return pack("ccl", 10, 0, strlen($x)) . $x;
    }
    if (is_integer($x)) {
      return pack("cl", -6, $x);
    }
    if (is_float($x)) {
      return pack("cd", -9, $x);
    }
    if (is_object($x)) {
      return $x->s;
    }
    if (!is_array($x)) {
      throw new Exception("Can't encode this: " . var_export($x));
    }
    $a = "";
    if (self::aa($x)) {
      //associative array->dict
      $n = count($x);
      $a = pack("cccl", 99, 0, 0, $n);
      $v = array();
      foreach ($x as $k => $_) {
        $a .= pack("c", -11) . $k . pack("c", 0);
        $v[] = $_;
      }
      $x = $v; //fall through
    }
    $a .= pack("ccl", 0, 0, count($x));
    foreach ($x as $b) {
      $a .= self::e1($b);
    }
    return $a;
  }

  private static function enc($b, $x) {
    $x = self::e1($x);
    return pack("ccccl", !self::$be, $b, 0, 0, 8 + strlen($x)) . $x;
  }

  private static function flip(&$c, &$x) {
    if (!is_array($x)) {
      return;
    }

    foreach ($x as $k => &$a) {
      if (is_array($a)) {
        foreach ($a as $i => &$v) {
          $c[$i][$k] = $v;
        }
      }
    }

    foreach ($x as $k => &$v) {
      if (!is_array($v)) {
        foreach ($c as $i => &$_) {
          $c[$i][$k] = $v;
        }
      }
    }

  }

  // private static function om(&$c) {
  //   $o = array();
  //   foreach ($c as $i => &$x) {
  //     $o[$i] = (object) $x;
  //   }
  //   return $o;
  // }

  private function d1($x, $t, $depth = 0) {
      echo ($this->dt?'DT':'');
    $depth++;                                                                   dbgp(applyDepth("START d1: " . qtype(-$t), $depth), PURP);
    $result = null;
    switch ($t) {
      case 0:
        throw new KException('glist');
        $result = $this->dec($x);
      case 1:
        $result = 1 == $x->r1();
        break;
      case 4:
        $result = $x->r1();
        break;
      case 5:
        $result = $x->rn("s", 2);
        break;
      case 6:
        $result = $x->rn("l", 4);
        break;
      case 7:
        $a = $x->rn("q", 8);
        // if ($a[2] == -1 && $a[1] < 0) {
        //   $result = $a[1];
        //   break;
        // }
        $result = $a;
        break;
      case 8:
        $result = $x->rn("f", 4);
        break;
      case 9:
        $result = $x->rn("d", 8);
        break;
      case 10:                                                                  dbgp('read a char?');
        $result = $x->sn(1);
        break;
      case 11:
        $s = "";                                                                dbgp("get a sym ", RED);
        // while (!in_array($c = trim($x->sn(1,true)), ["\0", false])) {          dbgp("read a sym $c", RED);
        while ($c = trim($x->sn(1,true))) {                                     dbgp("read a sym $c", RED);
          $s .= $c;                                                             dbgp('$s so far ' . bin2hex($s), RED);
        }
        $result = $s;
        break;
      case 14:
        $result = 86400 * (10957 + $x->rn("l", 4));
        if ($this->dt) $result = new DateTime('@'.$result);
        break;
      case 101:
        $result = null;
        break;
      case 128:
        throw new KException($this->d1($x,11));
      default:
        throw new Exception("Can't decode this $t yet");
    }                                                                           dbgp(applyDepth(qtype(-$t) . ' acqd: ', $depth), GREEN, false);dbgp(applyDepth(var_export($result, true), $depth), CYAN, true);
    return $result;
  }

  private function dec($x, $cxt = false, $depth = 0) {
    $depth++;                                                                   dbgp(applyDepth("START dec: ", $depth), PURP);

    // read the type
    $t = $x->rn('c',1);
    if($t===-128) throw new KException($this->d1($x,11));
                                                                                dbgp(applyDepth("t: ".qtype($t), $depth), PURP);
    // handle atoms with d1
    if ($t < 0) {                                                               dbgp(applyDepth("getting a (single?) " . qtype($t), $depth), RED);
      return $this->d1($x, -$t, $depth);
    }
    if ($t == 99 || $t == 127) {
      $keys = $this->dec($x,$cxt?:self::DICT,$depth);
      $vals = $this->dec($x,$cxt?:self::DICT,$depth);
      if(is_array(reset($keys)) && is_array(reset($vals))){
        // in a keyed table
        $kT=[];
        for ($rows=count($keys); 0<$rows; $rows--) { 
          $kT[]=array_merge(array_shift($keys),array_shift($vals));
        }
        return $kT;
      }
      return array_combine($keys, $vals);
    } elseif ($t == 98) {
      $attrs = $x->rn('c',1);                                                   dbgp(applyDepth('throwing away table attrs',$depth), UNCOLOR);
      $dictType = $x->rn('c',1);                                                dbgp(applyDepth('dicts are tables, we get it',$depth), UNCOLOR);
      if ($dictType!==99) throw new Error('what');
      // get a list of syms for cols
                                                        dbgp(applyDepth('start cols',$depth),UNCOLOR);
      $cols = $this->dec($x,$cxt?:self::DICT,$depth);   dbgp(applyDepth('FINISH cols, start rows',$depth),UNCOLOR);
      $rows = $this->dec($x,$cxt?:self::DICT,$depth);   dbgp(applyDepth('FINISH rows',$depth),UNCOLOR);
      $colTable = array_combine($cols, $rows);
      return self::transpose($colTable);
      // return $cxt?$colTable:self::transpose($colTable);
    }

    return $this->decn($x,$t,$depth);
  }

  private function decn($x,$t=0,$depth=0) {
    $depth++;                                                                  dbgp(applyDepth("decn for a " . qtype($t), $depth), RED);
    $attrs = $x->rn('c',1);                                                    dbgp(applyDepth('throwing away list attrs',$depth), UNCOLOR);
    $len = $x->rn('l', 4);                                                     dbgp(applyDepth("getting $len things", $depth), RED);

    $l=[];
    $decFn = $t==0?'dec':'d1';

    while ($len--) {$l[] = $this->$decFn($x, $t?:false, $depth);}
    return $l;

    if ($t == 10 && $this->collapseCharLists) {                           dbgp(applyDepth('MAKE MA STRINGS IMPLODE',$depth),UNCOLOR);
      // return implode('', $l);
    }
    return $l;
  }

  private static function transpose($arr) {
    $out = array();
    foreach ($arr as $key => $subarr) {
      foreach ($subarr as $subkey => $subvalue) {
        $out[$subkey][$key] = $subvalue;
      }
    }
    return $out;
  }

}