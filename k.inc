<?php
require_once 'dbg.inc';

class KException extends Exception {};
class KScan {
  private $s, $i, $n, $req;
  public function __toString() {
    return $this->req.PHP_EOL.
      bin2hex($this->s).PHP_EOL.
      str_repeat(' ',($this->i*2))
      .'^'.PHP_EOL.
      $this->i.' / '.$this->n;
  }
  public function __construct($s, $req='') {
    $this->s = $s;
    $this->i = 0;
    $this->n = strlen($s)-0;
    $this->req = $req;
  }
  public function r8($log=false) {
    if ($log) eval(\Psy\sh());

    if ($this->i >= $this->n) {
      dbgp("STOP I THINK",RED);
      return -1;
    }
    $i = $this->i;
    $this->i++;
    $x = ord($this->s{$i});
    if ($x >= 128) {
      $x -= 256;
    }

    return $x;
  }
  public function rn($s, $n) {
    $a = unpack($s, $this->sn($n));
    return $a[1];
  }
  public function s1() {
    if ($this->i >= $this->n) {
      return chr(0);
    }

    $i = $this->i;
    $this->i++;
    return $this->s{$i};
  }
  public function sn($n) {
    if ($this->i >= $this->n) {
      return str_repeat(chr(0), $n);
    }

    $i = $this->i;
    $this->i += $n;
    if ($this->i <= $this->n) {
      return substr($this->s, $i, $n);
    }

    return substr($this->s, $i, $this->n - $i);
  }
}
class K {
  private static $be = -1;
  private $h;

  public static function init() {
    if (self::$be === -1) {
      self::$be = (pack("N", -69) === pack("l", -69));
    }
  }

  public function __construct($h = "localhost", $p = 1234, $up = "anonymous") {
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

  public function ks($s) {
    if (func_num_args() === 1) {
      $x = self::enc(0, $s);
    } else {
      $x = self::enc(0, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function k($s) {
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    // dbgp($x);
    fwrite($this->h, $x);
    return $this->kr($s);
  }

  public function ka($s) {
    if (func_num_args() === 1) {
      $x = self::enc(1, $s);
    } else {
      $x = self::enc(1, func_get_args());
    }
    fwrite($this->h, $x);
  }

  public function kr($req='') {
    $a = unpack("l2", fread($this->h, 8));
    return self::dec(new KScan(fread($this->h, $a[2] - 8), $req));
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

  private static function om(&$c) {
    $o = array();
    foreach ($c as $i => &$x) {
      $o[$i] = (object) $x;
    }
    return $o;
  }

  private static function d1($t, $x) {
    dbgp("START d1: ".qtype($t),RED);
    dbgp("$x");
    $result = null;
    switch ($t) {
      case 0:
        $result = self::dec($x);
        break;
      case 1:
        $result = 1 == $x->r8();
        break;
      case 4:
        $result = $x->r8();
        break;
      case 5:
        $result = $x->rn("s", 2);
        break;
      case 6:
        $result = $x->rn("l", 4);
        break;
      case 7:
        $a = unpack("l2", $x->sn(8));
        if ($a[2] == -1 && $a[1] < 0) {
          $result = $a[1];
          break;
        }
        if ($a[2]) {
          throw new Exception("Number too long(64)");
        }
        $result = $a[1];
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
      case 98:
        $x->s1();
        $k = self::dec($x);
        $v = self::dec($x);
        $c = array();
        self::flip($c, $k);
        self::flip($c, $v);
        $maybeTable=self::om($c);
        $result = $maybeTable;
        break;
      case 99:
        $k = self::dec($x,true);
        dbgp('$k:'.var_export($k,true),PURP);
        $v = self::dec($x,true);
        dbgp('$v:'.var_export($v,true),PURP);

        $result = array_combine($k, $v);
        break;
      case 101:
        $result = null;
        break;
      case 128:
        throw new KException(self::d1(11, $x));
      default:
        throw new Exception("Can't decode this $t yet");
    }
    dbgp(qtype($t).' acqd: ',GREEN,false);
    dbgp($result,CYAN,true);
    return $result;
  }

  private static function dec($x,$care=false) {
    // if($care) eval(\Psy\sh());

    // if($care) dbgp((string)$x);
    $t = $x->r8();
    // if($care) dbgp($t,RED);
    // if($care) dbgp((string)$x);

    if ($t < 0) {
      return self::d1(-$t, $x);
    }

    if ($t == 99 || $t == 98) {

      if($care) {
        // eval(\Psy\sh());
      }
      return self::d1($t, $x);
    }

    $x->r8();
    // if ($someBytes != 0) {
    //  eval(\Psy\sh());
    //  throw new Exception("Protocol error");
    // }

    $n = self::d1(6, $x);
    if ($t === 10) {
      return $x->sn($n);
    }
    $a = array();
    for ($i = 0; $i < $n; ++$i) {
      $a[] = self::d1($t, $x);
    }
    // if($care) eval(\Psy\sh());

    return $a;
  }
}
