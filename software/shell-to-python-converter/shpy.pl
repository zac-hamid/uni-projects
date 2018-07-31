#!/usr/bin/perl

# written by andrewt@cse.unsw.edu.au August 2015
# as a starting point for COMP2041/9041 assignment 
# http://cgi.cse.unsw.edu.au/~cs2041/assignment/shpy

@toPrint = ();
@imported = ();
$indent = 0;

while ($line = <>) {
	chomp $line;
	$print = "\t" x $indent;
	$done = 0;
	if ($line =~ /^#!/ && $. == 1) {
		$version = "#!/usr/bin/python2.7 -u";
		print "$version\n";
	} elsif ($line =~ m/^\s*#/){ #if line is a comment
		$line =~ s/^\s*//;
		$print .= "$line\n";
		push(@toPrint, $print);
	} elsif ($line =~ m/^$/){ #if empty line, carry over to python
		push(@toPrint, "\n");
	} elsif ($line =~ m/^\s*for\s+/){ #for loop
		$line =~ s/\s+/ /g; #remove multiple spaces
		$line =~ s/^\s*for\s//;
		($var, $line) = split('\s', $line, 2); #retrieve the variable to increment
		$line =~ s/^\s*[^\s]+\s//; #remove the variable
		@words = split('\s', $line);
		foreach $word (@words){
			if ($word =~ m/\b[0-9]+\b/){ #if word is just a number don't surround with quotes
				$word = "$word";
			#if word doesn't contain file-search metacharacters, surround with quotes
			} elsif ($word =~ m/^\$/){
				$word =~ s/\$//;
			} elsif ($word =~ m/^[^\*\?\s]+$/){ 
				$word = "'$word'";
			} else {
				foreach $import (@imported){ #check if glob has been imported
					if ($import eq "glob"){ $done = 1; }
				}
				if ($done != 1){ #if not, import it
					unshift(@toPrint, "import glob\n");
					push(@imported, "glob");
				}
				$word = "sorted(glob.glob(\"$word\"))";
			}
		}
		$print .= "for $var in ";
		$print .= join(', ', @words);
		$print .= ':';
		$indent++; #everything between this for and the next done will be indented
		push(@toPrint, "$print\n");
    } elsif ($line =~ m/^\s*(if|elif)\s*/){ #if command is if/elif
    	#control statement to match indentation correctly
    	if ($line =~ m/^\s*elif\s+/){
    		$indent--;
    		$print =~ s/^[\t]//;
    	}
    	$line =~ s/\ +/ /g;
    	$line =~ s/^\s*(if|elif)//; #trim command
    	$print .= "$1 ";
    	if ($line =~ m/\s*([^\s]+)\s*/){
    		if ($1 eq 'test' || $1 eq '['){
    			$line =~ s/\s*([^\s]+)\s*//;
    			if ($1 eq '['){
    				$line =~ s/\]\s*//;
    			}
    			@words = split('\s', $line);
    			foreach $i (0..$#words){
    				if ($words[$i] =~ m/^\$([0-9])$/){ #if word is a system var
    					foreach $import (@imported){
							if ($import eq "sys"){ $done = 1; }
						}
						if ($done != 1){ #import sys if not already imported
							unshift(@toPrint, "import sys\n");
							push(@imported, "sys");
						}
						$words[$i] = "sys.argv[$1]";
    				} elsif ($words[$i] =~ m/\b[0-9]+\b/){ #if word is just a number don't surround with quotes
						$words[$i] = "$words[$i]";
    				}  elsif ($words[$i] =~ m/\$/){ #if the word is a variable
						$words[$i] =~ s/\$//;
						if (exists($words[$i+1])){
							if($words[$i+1] =~ m/-[a-z]+/){
								$words[$i] = "int($words[$i])";
							}
						} elsif (exists($words[$i-1])){
							if($words[$i-1] =~ m/-[a-z]+/){
								$words[$i] = "int($words[$i])";
							}
						}
					} elsif ($words[$i] eq '=' || $words[$i] eq '-eq'){
    					$words[$i] = "=="; #python equivalent of equals
    				} elsif ($words[$i] eq '-ne'){
    					$words[$i] = "!=";
    				} elsif ($words[$i] eq '-lt'){
    					$words[$i] = "<";
    				} elsif ($words[$i] eq '-le'){
    					$words[$i] = "<=";
    				} elsif ($words[$i] eq '-gt'){
    					$words[$i] = ">";
    				} elsif ($words[$i] eq '-ge'){
    					$words[$i] = ">=";
    				} elsif ($words[$i] =~ m/\'[^\']*\'/){
    				} else {
    					$words[$i] = "'$words[$i]'";
    				}
    			}
    			$print .= join(' ', @words);
    			$print .= ":\n";
    			$indent++;
				push(@toPrint, "$print");
    		}
    	}
    } elsif ($line =~ m/^\s*(do|then)\s*$/){ #these have no meaning in python and can be removed
    } elsif ($line =~ m/^\s*else\s*$/){
    	$print =~ s/^[\t]//;
    	$print .= "else:\n";
		push(@toPrint, "$print");
    } elsif ($line =~ m/^\s*(done|fi)\s*$/){
		$indent--;
	#if the line is not a for loop/if statement but matches this, must contain a command
	} elsif ($line =~ m/^\s*[A-Za-z]+\s*/) {
		$done = 0;
		if ($line =~ m/^\s*echo/ || $line =~ m/=/){ $done = 1; }
		#control statement to determine if something was already imported.
		foreach $import (@imported){
			if ($line =~ m/^\s*cd/){
				if ($import eq "os"){ $done = 1; }
			} elsif ($line =~ m/^\s*(exit|read)/){
				if ($import eq "sys"){ $done = 1; }
			} else {
				if ($import eq "subprocess"){ $done = 1; }
			}
		}
		if ($done != 1){ #if not already imported, import it
			if ($line =~ m/^\s*cd\s*/){
				unshift(@toPrint, "import os\n");
				push(@imported, "os");
			} elsif ($line =~ m/^\s*(exit|read)\s*/){
				unshift(@toPrint, "import sys\n");
				push(@imported, "sys");
			} else {
				unshift(@toPrint, "import subprocess\n");
				push(@imported, "subprocess");
			}
		}
		if ($line =~ m/^\s*echo\s+/){ #if command is echo
			$done = 0;
			$newline = 0;
			$start = 0;
			$line =~ s/^\s*echo\s//; #trim echo
			#check for -n operator
			if($line =~ m/^\s*-n\s+/){
				$newline = 1;
				$line =~ s/^\s*-n\s+//;
			}
			@words = split(/\s/, $line);
			$print .= "print ";
			foreach $i (0 .. $#words){
				if ($start != 1){ #if not contained within a single quote
					if ($words[$i] =~ m/^\$([0-9])$/){ #if word is a system var
						foreach $import (@imported){
							if ($import eq "sys"){ $done = 1; }
						}
						if ($done != 1){ #import sys if not already imported
							unshift(@toPrint, "import sys\n");
							push(@imported, "sys");
						}
						$words[$i] = "sys.argv[$1]";
					} elsif ($words[$i] =~ m/^\'/){
						$start = 1;
					} elsif ($words[$i] =~ m/\$/){ #if the word is a variable
						$words[$i] =~ s/\$//;
					} elsif ($words[$i] =~ m/^[0-9]+$/){
					} else { #otherwise treat it as a string
						$words[$i] = "'$words[$i]'";
					}
					if ($i != 0){
						$print .= ", $words[$i]";
					} else {
						$print .= "$words[$i]";
					}
				} else {
					if ($i != 0){
						$print .= " $words[$i]";
					} else { $print .= "$words[$i]"; }
				}
				if ($words[$i] =~ m/\'$/){ #if an end single quote is found
						$start = 0;
				}
			}
			if ($newline == 1){
				$print .= ","; #, at end of string in python removes trailing newline
			}
			$print .= "\n";
		#only match directory straight after cd, ignore everything after this
		} elsif ($line =~ m/^\s*cd\s*([^\s]+)\s*/){
			$print .= "os.chdir(";
			$var = $1;
			
			if ($var =~ m/\'[^\']*\'$/ || $var =~ m/\$/){
				if ($var =~ m/\$/){
					$var =~ s/\$//;
				}
				$print .= "$var";
			} else {
				$print .= "'$var'";
			}
			$print .= ")\n";
		} elsif ($line =~ m/^\s*exit\s*([^\s]+)\s*/){ #if command is exit
			$print .= "sys.exit(";
			$print .= "$1";
			$print .= ")\n";
		} elsif ($line =~ m/^\s*read\s+([^\s]+)\s*/){ #if command is read
			$print .= "$1 = sys.stdin.readline().rstrip()\n";
		} elsif ($line =~ m/=/){ #if there is an assignment
			($var, $value) = split(/=/, $line);
			if ($value =~ m/\'[^\']*\'/){
				$print .= "$var = $value";
			} elsif ($value =~ m/\b[0-9]+\b/){
				$print .= "$var = $value";
			} else { $print .= "$var = '$value'"; }
			$print .= "\n";
		} else {
			$print .= "subprocess.call([";
			$line =~ s/^\s*//;
			@words = split(/\s/, $line);
			foreach $word (@words){
				if ($word =~ /\$/){ #if the word is a variable
					$word =~ s/\$//;
				} else { #otherwise treat it as a string
					$word = "'$word'";
				}	
			}
			$word = join(', ', @words);
			$print .= "$word])\n";
		}
		push(@toPrint, "$print");
	} else {
        # Lines we can't translate are turned into comments
        print "#$line\n";
    }
}

foreach $line (@toPrint){
	print $line;
}