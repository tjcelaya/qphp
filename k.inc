<?php
class KException extends Exception {};
class KScan {
  private $s,$i,$n;
  public function __construct($s) {
    $this->s=$s;
    $this->i=0;
    $this->n=strlen($s);
  }
  public function rn($s, $n) {$a=unpack($s, $this->sn($n));return reset($a);}
  public function sn($n) {
    if ($this->n<$this->i+$n)throw new Exception('Attempting to read past end');
    $i=$this->i;
    $this->i += $n;
    return substr($this->s, $i, $n);
  }
}
class K {
  const ROW_ASSOC='assoc';
  const ROW_OBJ='object';
  private static $be = -1; // big endian?
  private static $DICT = 'DICT';
  private static $TABLE = 'TABLE';
  private static $bigint = true; // 'q' format flag supported?
  private static $zeroTime = 946684800; // 2000.01.01

  private $h;
  private $collStrs=false;
  private $dt_fmt=false;
  private $row_fmt=self::ROW_ASSOC;

  private static function init() {
    if(self::$be === -1) {
      self::$be = (pack("N", -69) === pack("L", -69));
      try { pack('q',1); } catch (\Exception $e) {self::$bigint=false;}
    }
  }
  public function __construct($h="localhost",$p=1234,$up="anonymous",$opts=[]) {
    self::init();
    if (isset($opts['dt_fmt'])) {
      $this->dt_fmt=$opts['dt_fmt'];}
    if (isset($opts['collStrs'])) {
      $this->collStrs=$opts['collStrs'];}
    if (isset($opts['row_fmt'])&&in_array($opts['row_fmt'],[self::ROW_ASSOC,self::ROW_OBJ])) {
      $this->row_fmt=$opts['row_fmt'];}

    $this->h = fsockopen($h,$p);
    fwrite($this->h, $up.pack("cc", 1,0));
    if(ord(fgetc($this->h)) != 1) throw new Exception("Can't connect to k: $h:$p");
  }
  function __destruct() {
    fclose($this->h);
  }
  public function ks($s) {
    if(func_num_args()===1){$x=self::enc(0,$s);}else{$x=self::enc(0,func_get_args());}
    fwrite($this->h,$x);
  }
  public function k($s) {
    if(func_num_args()===1){$x=self::enc(1,$s);}else{$x=self::enc(1,func_get_args());}
    fwrite($this->h,$x);
    return $this->kr();
  }
  public function ka($s) {
    if(func_num_args()===1){$x=self::enc(1,$s);}else{$x=self::enc(1,func_get_args());}
    fwrite($this->h,$x);
  }
  public function kr() {
    $a=unpack("l2", fread($this->h,8));
    $dataReturned = self::dec(new KScan(fread($this->h,$a[2]-8)));
    if($this->collStrs) self::implodeCharLists($dataReturned);
    return $dataReturned;
  }
  public static function implodeCharLists(&$data) {
    foreach ($data as &$potentiallyCharList) {
      if(is_array($potentiallyCharList)) {
        if (isset($potentiallyCharList[0])&&is_string($potentiallyCharList[0])) {
          try {$potentiallyCharList=implode('',$potentiallyCharList);}catch(Exception $e){echo "woops\n";}
        } else {
          self::implodeCharLists($potentiallyCharList);
        }
      }
    }
  }
  public static function S($x) {
    $s = pack("c", -11) . $x . pack("c", 0);
    return (object)array("s" => $s);
  }
  public static function G($x=null) {
    if($x === null) {
      if(function_exists('openssl_random_pseudo_bytes')) $x = openssl_random_pseudo_bytes(16);
      else{$x="";for($i=0;$i<16;++$i)$x.=chr(mt_rand(0,255));}
    } else { $x = hex2bin(str_replace("-","",$x)); }
    return (object)array("s" => pack("c", -2) . $x);
  }

  private static function aa($x) {
    $n=min(100,count($x));for($i=0;$i<$n;++$i)if(!isset($x[$i]))return true;
    return false;
  }

  private static function e1($x) {
    if(is_null($x)){return pack("cc",101,0);}
    if($x === true){return pack("cc",-1,1);}
    if($x === false){return pack("cc",-1,0);}
    if(is_string($x)){return pack("ccl",10,0,strlen($x)).$x;}
    if(is_integer($x)){return pack("cl",-6,$x);}
    if(is_float($x)){return pack("cd",-9,$x);}
    if(is_object($x)){return $x->s;}
    if(!is_array($x)){throw new Exception("Can't encode this: " . var_export($x));}
    $a="";
    if(self::aa($x)){//associative array->dict
      $n=count($x);$a=pack("cccl",99,0,0,$n);$v=array();
      foreach($x as $k => $_){$a.=pack("c",-11).$k.pack("c",0);$v[]=$_;}
      $x=$v;//fall through
    }
    $a.=pack("ccl",0,0,count($x));foreach($x as $b){$a.=self::e1($b);}
    return $a;
  }
  private static function enc($b,$x) {
    $x=self::e1($x);
    return pack("ccccl",!self::$be,$b,0,0,8+strlen($x)).$x;
  }
  private static function flip(&$c, &$x) {
    if(!is_array($x))return;
    foreach($x as$k=>&$a)if(is_array($a))foreach($a as$i=>&$v)$c[$i][$k]=$v;
    foreach($x as$k=>&$v)if(!is_array($v))foreach($c as $i=>&$_)$c[$i][$k]=$v;
  }
  private function d1($x, $t) {
    switch ($t) {
    case 1:return 1 == $x->r1();
    case 4:return $x->r1();
    case 5:return $x->rn("s", 2);
    case 6:return $x->rn("l", 4);
    case 7:
      if (self::$bigint) {
        return $x->rn("q", 8);
      } else {
        $a=unpack("l2",$x->sn(8));
        if($a[2]==-1&&$a[1]<0)
          return $a[1];
        if($a[2])throw new Exception("Number too long(64)");
      }
    case 8:return $x->rn("f", 4);
    case 9:return $x->rn("d", 8);
    case 10:return $x->sn(1);
    case 11:$s="";while($c=trim($x->sn(1))){$s.=$c;}return$s;
    case 12:
      $secs=null;
      if (self::$bigint) {
        $secs = $x->rn("q", 8);
      } else {
        $secs=unpack("l2",$x->sn(8));
        if($secs[2]==-1&&$secs[1]<0)$secs=$secs[1];
        if($secs[2])throw new Exception("Number too long(64)");
      }
      return gmdate('Y-m-d H:i:s', self::$zeroTime+$secs/1000000000); // there has to be a better way
    case 14:$date=86400*(10957+$x->rn("l", 4));return $this->dt_fmt?gmstrftime('%Y-%m-%d',$date):$date;
    case 101:return null;
    default:throw new Exception("Can't decode this $t yet");
    }
  }
  private function dec($x, $cxt = false) {
    // read the type
    $t = $x->rn('c',1);
    if($t===-128) throw new KException($this->d1($x,11));

    // handle atoms with d1
    if ($t < 0) {
      return $this->d1($x, -$t);
    }
    if ($t == 99 || $t == 127) {
      $keys = $this->dec($x,self::$DICT);
      $vals = $this->dec($x,self::$DICT);
      if(is_array(reset($keys)) && is_array(reset($vals))) {
        // in a keyed table
        $kT=[];
        for ($rows=count($keys); 0<$rows; $rows--) {
          $r=array_merge(array_shift($keys),array_shift($vals));
          if($this->row_fmt==self::ROW_OBJ) $r=(object)$r;
          $kT[]=$r;
        }
        return $kT;
      }
      return array_combine($keys, $vals);
    } elseif ($t == 98) {
      $attrs = $x->rn('c',1);
      $dictType = $x->rn('c',1);
      // get a list of syms for cols
      $cols = $this->dec($x,self::$TABLE);
      $rows = $this->dec($x,self::$TABLE);
      $tbl = self::transpose(array_combine($cols, $rows));
      // $cxt may need to become a stack at some point...
      if($this->row_fmt==self::ROW_OBJ&&$cxt!=self::$DICT){foreach($tbl as &$r){$r=(object)$r;}}
      return $tbl;
    }

    return $this->decn($x,$t);
  }

  private function decn($x,$t=0) {
    $attrs = $x->rn('c',1);
    $len = $x->rn('l', 4);
    $l=[];
    $decFn = $t==0?'dec':'d1';
    while ($len--) {$l[] = $this->$decFn($x, $t?:false);}
    return $l;
  }

  private static function transpose($arr) {
    $transposed = array();
    foreach($arr as $k => $iArr) {
      foreach($iArr as $iK => $iV) {
        $transposed[$iK][$k] = $iV;}}
    return $transposed;
  }
}
