test:
	 phpunit test.php; while inotifywait -e close_write *; do perl -E 'say "\n" x `tput lines`'; phpunit test.php ;done
