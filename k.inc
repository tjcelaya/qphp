<?php
require_once 'dbg.inc';
error_reporting(-1);
class KException extends Exception {};
class KScan {
  public $s, $i, $n, $req;
  public function __toString() {
    return $this->req.PHP_EOL.
      '`'.chunk_split(bin2hex($this->s),2,'`').PHP_EOL.
      str_repeat(' ',($this->i*3))
      .'^'.PHP_EOL.
      $this->i.' / '.$this->n;
  }
  public function __construct($s, $req='') {dbgp('__construct called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    $this->s = $s;
    $this->i = 0;
    $this->n = strlen($s)-0;
    $this->req = $req;
  }
  public function r1($log=false) {dbgp('r1 called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    return $this->rn('c', 1);
  }
  public function rn($s, $n) {dbgp('rn called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    $a = unpack($s, $this->sn($n));
    return reset($a);
  }
  public function s1() {dbgp('s1 called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    if ($this->i >= $this->n) {
      return chr(0);
    }

    $i = $this->i;
    $this->i++;
    // dbgp('SOME KIND OF s1? '.bin2hex($this->s{$i}),CYAN);
    return $this->s{$i};
  }
  public function sn($n) {dbgp('sn called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    if ($this->i >= $this->n) {
      // dbgp('SOME KIND OF sn? '.bin2hex(str_repeat(chr(0), $n)),CYAN);
      throw new KException('reading past end');
    }

    $i = $this->i;
  global $scanner; dbgp("$scanner");
    $this->i += $n;
  global $scanner; dbgp("$scanner");
    if ($this->i <= $this->n) {
      // dbgp('SOME KIND OF sn? '.bin2hex(substr($this->s, $i, $n)),CYAN);

      return substr($this->s, $i, $n);
    }

    // dbgp('SOME KIND OF sn? '.bin2hex(substr($this->s, $i, $this->n - $i)),CYAN);
    return substr($this->s, $i, $this->n - $i);
  }
}
class K {
  private static $be = -1;
  private $h;

  private static function init() {
    if (self::$be === -1) {
      self::$be = (pack("N", -69) === pack("l", -69));
    }
  }

  public function __construct($h = "localhost", $p = 1234, $up = "anonymous") {
    dbgp('__construct called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    self::init();

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
    return (new static)->k($s);
  }
  public function ks($s) {dbgp('ks called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    if (func_num_args() === 1) {
      $x = self::enc(0, $s);
    } else {
      $x = self::enc(0, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function k($s) {dbgp("k called: $s",UNCOLOR); 
  global $scanner; dbgp("$scanner");
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    // dbgp($x);
    fwrite($this->h, $x);
    return $this->kr($s);
  }

  public function ka($s) {dbgp('ka called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function kr($req='') {dbgp('kr called',UNCOLOR); 
  global $scanner; dbgp("$scanner");
    $a = unpack("l2", fread($this->h, 8));
    $scanner = new KScan(fread($this->h, $a[2] - 8), $req);
    return self::dec($scanner);
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

  private static function d1($t, $x, $depth=0) {
    $depth++;
    dbgp(applyDepth("START d1: ".qtype($t),$depth),PURP);
    $result = null;
    switch ($t) {
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
        $a = $x->rn("l", 4);
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
      case 10:
        $result = $x->s1();
        break;
      case 11:
        $s = "";
        while (ord($c = $x->s1())) {
          $s .= $c;
        }
        $result = $s;
        break;
      case 14:
        $result = 86400 * (10957 + $x->rn("l", 4));
        break;
      case 101:
        $result = null;
        break;
      case 128:
        throw new KException(self::d1(11, $x));
      default:
        throw new Exception("Can't decode this $t yet");
    }
    dbgp(applyDepth(qtype($t).' acqd: ',$depth),GREEN,false);
    dbgp(applyDepth(var_export($result,true),$depth),CYAN,true);
    return $result;
  }

  private static function dec($x,$care=false,$depth=0) {
    $depth++;
    $t = $x->r1();
    dbgp(applyDepth("START d1: ".qtype($t),$depth),PURP);
    dd($t);

    if ($t < 0) {
      return self::d1(-$t, $x, $depth);
    }

    if ($t === 0) {
      dbgp("WE GOT a ".qtype($t)." IN HERE");
      $len=$x->rn('j', 4);
      dd($len);
    }



    if ($t == 99 || $t == 98) {
      throw new KException('no 98|99 yet');
      // case 98:
      //   dbgp(applyDepth(' ------- start a '.strtoupper(qtype($t)).' -------',$depth),RED);
      //   dbgp(applyDepth("TABLE S1",$depth),YALLO);
      //   $x->s1();
      //   dbgp(applyDepth("START TABLE KEYS",$depth),YALLO);
      //   $k = self::dec($x,false,$depth);
      //   dbgp(applyDepth("END   TABLE KEYS",$depth),YALLO);
      //   // dbgp(applyDepth('$k:'.var_export($k,true),$depth),UNCOLOR);
      //   dbgp(applyDepth("START TABLE VALS",$depth),YALLO);
      //   $v = self::dec($x,false,$depth);
      //   dbgp(applyDepth("END   TABLE VALS",$depth),YALLO);
      //   // dbgp(applyDepth('$v:'.var_export($v,true),$depth),UNCOLOR);
      //   $c = array();
      //   self::flip($c, $k);
      //   self::flip($c, $v);
      //   $result = $c;
      //   dbgp(applyDepth(' ------- finished a table -------',$depth),RED);
      //   break;
      // case 99:
      //   dbgp(applyDepth(' ------- start a '.strtoupper(qtype($t)).' -------',$depth),RED);
      //   dbgp(applyDepth("START DICT KEYS",$depth),YALLO);
      //   dbgp("$x");
      //   $k = self::dec($x,true,$depth);
      //   dbgp("$x");
      //   dbgp(applyDepth("END   DICT KEYS",$depth),YALLO);
      //   dbgp(applyDepth("START DICT VALS",$depth),YALLO);
      //   dbgp("$x");
      //   $v = self::dec($x,true,$depth);
      //   dbgp("$x");
      //   dbgp(applyDepth("END   DICT VALS",$depth),YALLO);
      //   // dbgp(applyDepth('$v:'.var_export($v,true),$depth),RED);
      //   dbgp(applyDepth(' ------- finished a dict -------',$depth),RED);

      //   $result = array_combine($k, $v);
      //   break;

      //   return self::d1($t, $x, $depth);
    }

    return self::d1($t, $x, $depth);


    // dbgp(applyDepth("READING ATTRS?: ".bin2hex($someBytes),$depth),RED);
    // fwrite(STDERR, applyDepth("READING ATTRS?: ",$depth));
    $someBytes = $x->r1();
    dbgp(applyDepth("READ ATTRS?: ".$someBytes,$depth),RED);
    if (!in_array($someBytes, [0,1,2,3])) {
     // eval(\Psy\sh());
     throw new Exception("Protocol error");
    }

    $n = self::d1(6, $x, $depth);
    if ($t === 10) {
      dbgp("$x",RED);
      return $x->sn($n);
    }
    $a = array();
    for ($i = 0; $i < $n; ++$i) {
      $a[] = self::d1($t, $x, $depth);
    }
    // dbgp("$x");
    // dbgp(applyDepth('WHAT IS THIS: '.var_export($a,true),$depth),RED);

    return $a;
  }
}
