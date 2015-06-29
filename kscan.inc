<?php
require_once 'dbg.inc';
error_reporting(-1);
class KException extends Exception {};
class KScan {
	public $s, $i, $n, $req;
	public function __toString() {
		return $this->req . PHP_EOL .
		'`' . chunk_split(substr(bin2hex($this->s),$this->i*2),2,'`') . PHP_EOL
		// .'`' . chunk_split(bin2hex($this->s), 2, '`') . PHP_EOL
		// .str_repeat(' ', ($this->i * 3))
		. '^' . PHP_EOL .
		$this->i . ' / ' . $this->n . PHP_EOL;
	}
	public function __construct($s, $req = '') {
		dbgp('__construct called', UNCOLOR);
		global $scanner;
		dbgp("$scanner");
		$this->s = $s;
		$this->i = 0;
		$this->n = strlen($s) - 0;
		$this->req = $req;
	}
	public function rn($s, $n) {
		dbgp('rn called s:' . $s . " n:$n", UNCOLOR);
		// global $scanner;
		// dbgp("$scanner");
		$a = unpack($s, $this->sn($n));
		// dbgp("$scanner");
		return reset($a);
	}
	public function sn($n,$psy=false) {
		dbgp("sn called n: $n", UNCOLOR);
		global $scanner;
		// dbgp("$scanner",CYAN);
		dbgp('early return sn? ' .
			var_export($this->i >= $this->n,true)
			// .' $this->i >= $this->n '.var_export($this->i >= $this->n,true)
			// .' sOZ? '.var_export($stopOnZero,true)
			// .' cur? '.bin2hex($this->s{$this->i})
			// .' $this->s{$this->i} === "\0" ?'.var_export($this->s{$this->i} === "\0",true)
			, CYAN);

		if ($this->n<$this->i+$n)throw new Exception('Attempting to read past end');

		dbgp("inc by $n\npre:\n$scanner");
		$i = $this->i;
		$this->i += $n;
		// if($psy)eval(\Psy\sh());
		dbgp("post:\n$scanner");

		dbgp('SOME KIND OF sn? '.bin2hex(substr($this->s, $i, $n)),CYAN);
		return substr($this->s, $i, $n);
	}
}

