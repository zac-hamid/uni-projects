#!/usr/bin/perl -w

# Written by Zachary Hamid, Oct 2015
# Bitter, definitely not a Twitter clone, seriously don't even bother checking there's absolutely no similarities.
# http://cgi.cse.unsw.edu.au/~cs2041/assignments/bitter/

use CGI qw/:all/;
use CGI::Carp qw/fatalsToBrowser warningsToBrowser/;
use POSIX 'strftime';
use CGI::Cookie;
use HTML::Entities;

my $n;
my $heading_quote = "\"Tell us all your problems\"";
my $home_index;

sub main() {
    my %cookies = fetch CGI::Cookie; 
    $dataset_size = "medium";
    $users_dir = "dataset-$dataset_size/users";
    $bleats_dir = "dataset-$dataset_size/bleats";

    if (!(-e "dataset-$dataset_size/name-search.cache")){
	generate_name_cache();
    }

    my @users = sort(glob("$users_dir/*"));
    foreach my $users (@users) {
	$users =~ s/dataset-$dataset_size\/users\///;	
	$bleatCache{$users} = "::";
	$bleatMentions{$users} = "";
    }
    if (defined param('search') && param('search_field') ne ''){
	if (param('search_type') eq 'user'){
		filter_search(param('search_field'));
	} elsif (param('search_type') eq 'bleats'){
		filter_search_bleats(param('search_field'));
	}
    }

    if (defined param('logout')){
	$cookies{'logged_in'}->value(0);
	$cookies{'user'}->value("");
	$cookies{'pass'}->value("");
    }
    if (defined $cookies{'user'}){
    	$username = param('username') || $cookies{'user'}->value() || '';
    } else { $username = ''; }

    if (defined $cookies{'user'}){
    	$password = param('password') || $cookies{'pass'}->value() || '';
    } else { $password = ''; }

    $login_string = "Please enter your username and password.";
    #GET THIS TO RUN ONLY WHEN INITIALLY LOGGING IN
    if (defined $username && defined $password && !$logged_in && $username ne "" && $password ne ""){
	if(validate_pass($username, $password)){
		$cookies{'logged_in'}->value(1);
	} else {
		$cookies{'logged_in'}->value(0); 
		$login_string = "<font color=\"red\">Username or password incorrect</font>";
	}
    } 

    if (defined param('home') || defined param('login')){
	if (defined $username && defined $password){
		my $index = index_of_user($username);
		$cookies{'n'}->value("$index");
		$cookies{'home_index'}->value("$index");
	}
    }

    if (defined $cookies{'n'}){
    	$n = param('n') || $cookies{'n'}->value() || 0;
    } else { $n = 0; }
    foreach (keys %cookies){
	if ($_ eq 'home_index'){
		$home_index = $cookies{'home_index'}->value();
	}
    }
    if (defined $cookies{'logged_in'}){
    	$logged_in = $cookies{'logged_in'}->value() || 0;
    } else { $logged_in = 0; }

    if (defined param('prev')){ $n--; Delete('prev'); }
    elsif (defined param('next')){ $n++; Delete('next');}

    $USER = cookie(-name    =>'user',
                   -value   =>"$username",
                   -expires =>"+1h");
    $PASS = cookie(-name    =>'pass',
                   -value   =>"$password",
                   -expires =>"+1h");
    $N = cookie(-name  =>'n',
                -value =>"$n");
    $HOME_INDEX = cookie(-name  =>'home_index',
                         -value =>"$home_index");
    $LOGGED_IN = cookie(-name  =>'logged_in',
                        -value =>"$logged_in");

   if (defined param ('post_bleat')){ 
	post_bleat(); 
	Delete('post_bleat', 'bleat_create'); 
    	print redirect(
		-location => "bitter.cgi",
		-cookie => [$USER, $PASS, $N, $HOME_INDEX, $LOGGED_IN]
	);
    }

    # print start of HTML ASAP to assist debugging if there is an error in the script
    print page_header();
    # OR LISTEN/UNLISTEN
    if (defined param('post_bleat')){
    foreach my $users (@users){ generate_listeners($users); }

    foreach my $users (@users){
	my @bleats = ();
	open (my $fh, '<', "$users_dir/$users/bleats.txt") or die "cannot open < $users_dir/$users/bleats.txt: $!";
	while($line = <$fh>){
		chomp $line;
		push(@bleats, $line);
	}
	close $fh;
	foreach my $bleat (@bleats){
		filter_bleats("$bleats_dir/$bleat");
	}
    }

    if (-e "dataset-$dataset_size/bleats.cache"){
	unlink("dataset-$dataset_size/bleats.cache");
    } 
    foreach my $users (@users){
	generate_bleats_cache($users);
    }
    }

    # Now tell CGI::Carp to embed any warning in HTML
    warningsToBrowser(1);

    # define some global variables
    $debug = 0;

    if ($debug) { foreach (keys %cookies){ print $cookies{$_}; }}

    if (!defined $logged_in || !defined $cookies{'logged_in'} || !$cookies{'logged_in'}->value()){
	print login_page();
    } elsif(defined param('login') || defined param('home')) {
	if (defined $username && defined $password && $username ne "" && $password ne ""){
		user_page($n);
	} else { print login_page(); }
    } else { user_page($n); }    

    print page_trailer();
} 

sub login_page {
    return <<eof
    <div class="login_wrapper">
    <form method="POST">
    $login_string
    <div style="margin-bottom:10px;">
    <label for="text" style="margin-right:5px;">Username:</label>
    <input type="textfield" name="username">
    </div>
    <div style="margin-bottom:10px;">
    <label for="text" style="margin-right:5px;">Password:</label>
    <input type="password" name="password">
    </div>
    <input type="submit" name="login" value="Login">
    </form>
    </div>
eof
}

#
# Show unformatted details for user "n".
# Increment parameter n and store it as a hidden variable
#
sub user_page {
    $string = "";
    @bleats = ();
    @bleatsToShow = ();
    my $count = shift;
    my @users = sort(glob("$users_dir/*"));
    my $user_to_show  = $users[$n % @users];
    open(my $fh, "<", "$user_to_show/bleats.txt") or die "cannot open < $user_to_show/bleats.txt: $!";
    while($line = <$fh>){
	if (!defined $posted{$line}){
		push(@bleats, "$line");
		$posted{$line} = 1;
	}
    }
    close($fh);
    my $userTest = $user_to_show;
    $userTest =~ s/$users_dir\///;
    if ($userTest eq $username){
	    open(my $fh2, "<", "dataset-$dataset_size/bleats.cache") or die "cannot open < dataset-$dataset_size/bleats.cache: $!";
	    while($line = <$fh2>){
		my $currUser = $user_to_show;
		$currUser =~ s/$users_dir\///;
		$line =~ /([A-Za-z0-9]+):/;
		if ($1 eq $currUser){
			(my $tempuser, my $tempbleats, my $templistens) = split(/:/, $line);
			(my @listens) = split(/\s/, $templistens);
			(my @tempbleat) = split(/\s/, $tempbleats);
			foreach my $temp (@tempbleat){
					push(@bleats, $temp);
			}
			foreach my $listen (@listens){
				open(my $fh3, "<", "$users_dir/$listen/bleats.txt") or die "cannot open < $users_dir/$listen/bleats.txt: $!";
				while ($line_bleat = <$fh3>){
					foreach my $posted (@posted){
						push(@bleats, "$line_bleat");
					}
				}
				close $fh3;
			}
			last;
		}
    	}
    	close $fh2;
    }
    foreach my $bleat (@bleats) { $bleat =~ s/\0//g; }
    @bleats = sort { $a <=> $b } @bleats;

    foreach my $bleat (@bleats){
	chomp $bleat;
	my $file = "$bleats_dir/$bleat";
	my $toPush = "<div class=\"bleat_whole\">";
	$toPush .= join '', filter_bleats("$file");
	$toPush .= "</div>";
	push(@bleatsToShow, $toPush);
    }
    @bleatsToShow = reverse (@bleatsToShow);
 
    $bleatsDisplay = join '<p>', @bleatsToShow;
    my $profile_picture = "$user_to_show/profile.jpg";
    if (!(-e $profile_picture)){ $profile_picture = ""; }
    
    my $details_filename = "$user_to_show/details.txt";
    $details = join '', filter_profile($details_filename);
    ($name, $user, @details) = split(/\n/, $details, 3);
    $details = join('', @details);
    if ($profile_picture ne ''){
    	$string = "<img src=\"$profile_picture\" alt=\"$user\" class=\"profile_img\">";
    } else {
	$string = "<div class=\"no_profile_img\"></div>"
    }

print <<eof;
<div class="bitter_main_wrapper">
	<form method="POST" action="">
		<input type="submit" name="home" value="Home" class="bitter_button">
		<input type="submit" name="prev" value="< Prev user" class="bitter_button">
		<input type="submit" name="next" value="Next user >" class="bitter_button">
		<input type="submit" name="logout" value="Logout" class="bitter_button">
	</form>
	<p>
<div class="search_bar">
<form method="POST" action="">
	<input type="radio" name="search_type" value="user" checked>User
	<input type="radio" name="search_type" value="bleats">Bleats
	<input type="textfield" name="search_field" class="search_field">
	<input type="submit" name="search" value="Search" class="search_button">
	
</form>
</div>
eof
if (defined param('search') && param('search_field') ne ''){
	print search_page();
} else {
print <<eof;
<div class="bitter_user_details">
$string
<div class="profile_details">
<font size="3" class="fullname">$name</font> ( <font class="username">$user</font> )
$details
</div>
</div>
<div class="bleat_contain">
eof
	if ($home_index == $n){
	print <<EOF;
		<form method="POST" action="">
    		<input type="textfield" maxlength="142" onblur="this.placeholder = 'What\'s on your mind? (Max 142 characters)'" onfocus="this.placeholder = ''" name="bleat_create" placeholder="What\'s on your mind? (Max 142 characters)" class="bleat_create">
    		<input type="submit" name="post_bleat" value="Post">
		</form>
EOF
}
print <<END;
$bleatsDisplay
</div>
</div>
END
}
}

sub search_page {
    return <<eof
	<div class="search_main_wrapper">
<div style="border-radius:1em;border:thin solid #000000; background-color:#F5FCFF;margin-bottom:0.5em;padding:0.5em;">
<font size="6" style="font-weight:bold;font-style:italic;font-family:verdana;">Matches</font>
</div>
$searchResults
	</div>
eof
}


#
# HTML placed at the top of every page
#
sub page_header {

my $header_cookies = header(-cookie=>[$USER, $PASS, $N, $HOME_INDEX, $LOGGED_IN]);
return <<eof
$header_cookies
<!DOCTYPE html>
<html lang="en">
<head>
<title>Bitter</title>
<link href="bitter.css" rel="stylesheet">
</head>
<body bgcolor="#DFEEF5">
<div class="top_bar_heading">
<div class="bitter_heading">
Bitter
</div>
<div class="heading_quote">
$heading_quote
</div>
</div>
eof
}


#
# HTML placed at the bottom of every page
# It includes all supplied parameter values as a HTML comment
# if global variable $debug is set
#
sub page_trailer {
    my $html = "";
    $html .= join("", map("<!-- $_=".param($_)." -->\n", param())) if $debug;
    $html .= end_html;
    return $html;
}

sub filter_profile{
	@output = ();
	@fields = ("full_name", "username", "home_suburb", "home_latitude", "home_longitude", "listens");
	$file = shift;
	open(my $fh, "<", "$file") or die "cannot open < $file: $!";
	while($line = <$fh>){
		if ($line =~ m/^password:\ /){
			next;
		} elsif ($line =~ m/^email:\ /){
			next;
		} elsif ($line =~ m/^listens:/){
			@listens = split(/\s/, $line);
			@listens = sort{
				if ($a eq 'listens:') { return -1; }
				elsif ($b eq 'listens:') { return 1; }
			}@listens;
		}
		foreach my $field (@fields){
			if ($line =~ m/^$field:/){
				if ($field eq 'full_name'){
					$line =~ s/^$field: //;
				} elsif ($field eq 'username'){
					$line =~ s/^$field: //;
				}
				$info{$field} = $line;
			}
		}
		
		foreach my $key (keys %info){
			$info{$key} =~ s/^([a-z])/\u$1/;
			$info{$key} =~ s/^([A-Za-z0-9]+)_/$1 /;

		}
	}
	foreach my $field (@fields){
		if (defined $info{$field}){
			if ($field eq 'listens'){
				$line = "listens:";
				foreach my $listen (1..$#listens){
					my $num = index_of_user($listens[$listen]);
					$line .= "\n<a href=\"bitter.cgi?n=$num\">\@$listens[$listen]</a>";	
				}
				$info{$field} = $line;
			} elsif ($field eq 'username'){
				$info{$field} = "\@$info{$field}";
			}
			push(@output, $info{$field});
		} else {
			$field =~ s/^([a-z])/\u$1/;
			$field =~ s/^([A-Za-z0-9]+)_/$1 /;
			$field =~ s/^([A-Za-z0-9]+)\s([A-Za-z0-9]+)/$1 \u$2/;
			push(@output, "$field: N/A\n");
		}
	}
	return @output;
}

sub generate_listeners {
	my @listens = ();
	my $user = shift;
	open(my $fh, "<", "$users_dir/$user/details.txt") or die "cannot open < $users_dir/$user/details.txt: $!";
	while($line = <$fh>){
		if ($line =~ m/^listens:/){
			@listens = split(/\s/, $line);
			@listens = sort{
				if ($a eq 'listens:') { return -1; }
				elsif ($b eq 'listens:') { return 1; }
			}@listens;
		}
	}
	foreach my $listen (1..$#listens){
		$listens[$listen] =~ s/dataset-$dataset_size\/users\///;
		$user =~ s/$users_dir\///;
		(my $tempuser, my $tempbleats, my $templistens) = split(/:/, $bleatCache{$listens[$listen]});
		$templistens .= "$user ";
		$bleatCache{$listens[$listen]} = "$listens[$listen]::$templistens";
	}
	close $fh;
}

sub filter_bleats {
	my @output = ();
	my @fields = ("username", "time", "bleat", "longitude", "latitude");
	my $file = shift;
	open(my $fh, "<", "$file") or die "cannot open < $file: $!";
	while($line = <$fh>){
		foreach my $field (@fields){
			if ($line =~ m/^$field:/){
				if ($line =~ m/^username:/){
					$line =~ s/^$field: //;
				} else {
					$line =~ s/^$field: //;
				}
				$bleat{$field} = $line;
			}
		}
	}
	close $fh;
	$index = index_of_user("$bleat{'username'}");
	$bleat{"username"} = "\@".$bleat{"username"};
	$bleat{"username"} =~ s/(.*)/<font class="username"><a href="bitter.cgi?n=$index">$1<\/a><\/font>/g;
	my @mentions = ($bleat{"bleat"} =~ /(@[A-Za-z0-9]+)/g);
	my @at = ($bleat{"bleat"} =~ /@([A-Za-z0-9]+)/g);
	foreach my $at (@at){
		my $index = index_of_user($at);
		$bleat{"bleat"} =~ s/\@$at/<font class="bleat_at"><a href="bitter.cgi?n=$index">\@$at<\/a><\/font>/g;
	}
	$bleat{"bleat"} =~ s/#([A-Za-z0-9]+)/<a href=\"bitter.cgi?search_type=bleats&search=search&search_field=%23$1\"><font class="bleat_hash">#$1<\/font><\/a>/g;
	$output{"time"} = epoch_to_date($bleat{"time"});

	foreach my $mention (@mentions){
		$mention =~ s/^@//;
		my $bleatFile = "$file ";
		$bleatFile =~ s/dataset-$dataset_size\/bleats\///;
		$bleatMentions{$mention} .= $bleatFile;
	}
	foreach my $field (@fields){
		if (defined $bleat{$field}){
			if ($field eq 'time'){
				$output{$field} = "$output{$field}<div class=\"bleats\">";
			} elsif ($field eq 'bleat'){
				$output{$field} = "$bleat{$field}</div>";
			} else { $output{$field} = $bleat{$field}; }
			push(@output, $output{$field});
		} else {
			push(@output, "N/A\n");
		}
	}
	return @output;
}

sub epoch_to_date {
	my $epoch = shift;
	my $date = strftime '[%H:%M:%S %d/%m/%Y]', localtime $epoch;
}

sub generate_bleats_cache {
	my $user = shift;
	$user =~ s/$users_dir\///;
	my @listens = ();
	(my $tempuser, my $tempbleats, my $templistens) = split(/:/, $bleatCache{$user});
	$tempuser = $user;
	$tempbleats = $bleatMentions{$user};
	$tempuser =~ s/\s*$//;
	$tempbleats =~ s/\s*$//;
	$templistens =~ s/\s*$//;
	$bleatCache{$user} = "$tempuser:$tempbleats:$templistens\n";

	if (-e "dataset-$dataset_size/bleats.cache"){
		open (my $fh, ">>dataset-$dataset_size/bleats.cache") or die "cannot open >> dataset-$dataset_size/bleats.cache: $!";
		print {$fh} $bleatCache{$user};
		close $fh;
	} else {
		open (my $fh, '>', "dataset-$dataset_size/bleats.cache") or die "cannot open > dataset-$dataset_size/bleats.cache: $!";
		print {$fh} $bleatCache{$user};
		close $fh;
	}
}

sub validate_pass {
	my $user = shift;
	my $pass = shift;
	my $user_valid = 0;
	my $pass_valid = 0;
	if (defined $user && defined $pass){
		my @users = sort(glob("$users_dir/*"));
		foreach $item (@users){
			$item =~ s/^dataset-$dataset_size\/users\///;
			if ($user eq "$item"){
				$user_valid = 1;
			}
		}
		if ($user_valid){
			open (my $fh, "<", "$users_dir/$user/details.txt") or die "cannot open < $users_dir/$user/details.txt: $!";
			while($line = <$fh>){
				if ($line =~ m/^password:/){
					$line =~ s/^password: //;
					chomp $line;
					if ($pass eq "$line"){
						$pass_valid = 1;
					}
				}
			}
		}
	}
	if ($pass_valid && $user_valid){ return 1; }
	else { return 0; }
}

sub post_bleat {
	my $text = param('bleat_create');
	if ($text eq '') { return; }
	my $user = $username;
	my $bleatNum;
	my $max = 0;
	my $longitude = "N/A";
	my $latitude = "N/A";
	my $time = get_curr_epoch_time();
	$text = HTML::Entities::encode($text);
	@bleats = sort(glob("$bleats_dir/*"));
	foreach $bleat (@bleats){
		$bleat =~ s/dataset-$dataset_size\/bleats\///;
		if ($bleat > $max){
			$max = $bleat;
		}
	}
	$bleatNum = $max + 1;
	if (-e "$bleats_dir/$bleatNum"){}
	else {
		open (my $fh, ">>$users_dir/$user/bleats.txt") or die "cannot open >> $users_dir/$user/bleats.txt: $!";
		print $fh "$bleatNum\n";
		close $fh;
		open (my $fh2, '>', "$bleats_dir/$bleatNum") or die "cannot open > $bleats_dir/$bleatNum: $!";
		print {$fh2} "username: $user\n";
		print {$fh2} "time: $time\n";
		print {$fh2} "bleat: $text\n";
		print {$fh2} "longitude: $longitude\n";
		print {$fh2} "latitude: $latitude\n";
		close $fh2;
	}
}

sub get_curr_epoch_time {
	return time;
}

sub index_of_user {
	$string = shift;
	my @users = sort(glob("$users_dir/*"));
	chomp $string;
	foreach my $index (0..$#users){
		$users[$index] =~ s/^dataset-$dataset_size\/users\///;
		if($string eq $users[$index]){
			return ($index + $#users + 1);
		}
	}
}

sub generate_name_cache {
	my $path = "dataset-$dataset_size";
	my $filename = "name-search.cache";
	my @data = ();
	my $name;
	my $User;
	my $count;
	@users = sort(glob("$users_dir/*"));
	foreach my $user (@users){
		open (my $fh, '<', "$user/details.txt") or die "cannot open < $user/details.txt: $!";
		while($line = <$fh>){
			chomp $line;
			if ($line =~ m/^username:/){
				$User = $line;
				$User =~ s/^username:\s*//;
				$count = index_of_user($User);
			} elsif ($line =~ m/^full_name:/){
				$name = $line;
				$name =~ s/^full_name:\s*//;
			}
		}
		if ($User ne "" && $name ne ""){
			push(@data, "$count:$User:$name");
		}
		$User = "";
		$name = "";
		close $fh;
	}
	open (my $fh2, '>', "$path/$filename") or die "cannot open > $path/$filename: $!";
	foreach my $data (@data){
		print {$fh2} "$data\n";
	}
	close $fh2;
}

sub filter_search {
	my $search = shift;
	my @searchOutput = ();
	$name_cache = "dataset-$dataset_size/name-search.cache";
	open(my $fh, '<', "$name_cache") or die "cannot open > $name_cache: $!";
	while($line = <$fh>){
		(my $num, my $user, my $name) = split(/:/, $line);
		if ($line =~ m/$search/i){
			push (@searchOutput, "<div class=\"search_results\" style=\"cursor: pointer;\" onclick=\"window.location='bitter.cgi?n=$num';\"><font class=\"fullname\">$name</font> <font class=\"username\">(\@$user)</font></div>");
		}
	}
	close $fh;
	if (!@searchOutput){ $searchResults = "<font color=\"red\" size=\"5\" style=\"font-style:italic\">No matches found.</font>"; }
	else {
		$searchResults = join("<br>", @searchOutput);
	}
}

sub filter_search_bleats {
	my $search = shift;
	my @searchOutput = ();
	my @bleats = sort(glob("$bleats_dir/*"));
	my @foundBleats = ();
	my @fields = ("username", "time", "bleat", "longitude", "latitude");
	foreach my $bleat(@bleats){
		$bleat =~ s/dataset-$dataset_size\/bleats\///;
		open(my $fh, "<", "dataset-$dataset_size/bleats/$bleat") or die "cannot open < dataset-$dataset_size/bleats/$bleat: $!";
		while($line = <$fh>){
			if ($line =~ m/^bleat:/){
				$line =~ s/^bleat:\s*//;
				if ($line =~ m/$search/i){
					push(@foundBleats, $bleat);
				}
			} else { next; }
		}
		close $fh;
	}
	if (!@foundBleats) { $searchResults = "<font color=\"red\" size=\"5\" style=\"font-style:italic\">No matches found.</font>"; }
	else {
		foreach my $found (@foundBleats){
			open(my $fh, "<", "$bleats_dir/$found") or die "cannot open < $bleats_dir/$found: $!";
			while ($line = <$fh>){
				foreach my $field (@fields){
					if ($line =~ m/^$field:/){
						if ($line =~ m/^username:/){
							$line =~ s/^$field: //;
						} else {
							$line =~ s/^$field: //;
						}
						$searched{$field} = $line;
					}
				}
			}
			close $fh;
			my $index = index_of_user("$searched{'username'}");
			$searched{"username"} = "\@$searched{'username'}";
			$searched{"username"} =~ s/(.*)/<font class="username"><a href="bitter.cgi?n=$index">$1<\/a><\/font>/g;
			
			my @at = ($searched{"bleat"} =~ /@([A-Za-z0-9]+)/g);
			foreach my $at (@at){
				my $index = index_of_user($at);
				$searched{"bleat"} =~ s/\@$at/<font class="bleat_at"><a href="bitter.cgi?n=$index">\@$at<\/a><\/font>/g;
			}
			$searched{"bleat"} =~ s/#([A-Za-z0-9]+)/<a href=\"bitter.cgi?search_type=bleats&search=search&search_field=%23$1\"><font class="bleat_hash">#$1<\/font><\/a>/g;
			$searchOut{"time"} = epoch_to_date($searched{"time"});

			foreach my $field (@fields){
				if (defined $searched{$field}){
					if ($field eq 'username'){
						$searchOut{$field} = "<div class=\"bleat_whole\">$searched{$field}";
					} elsif ($field eq 'time'){
						$searchOut{$field} = "$searchOut{$field}<div class=\"bleats\">";
					} elsif ($field eq 'bleat'){
						$searchOut{$field} = "$searched{$field}</div>";
					} elsif ($field eq 'latitude'){
						$searchOut{$field} = "$searched{$field}</div><p>";
					} else { $searchOut{$field} = $searched{$field}; }
					push(@searchOutput, $searchOut{$field});
				} elsif (!defined $searched{'latitude'} && $field eq 'latitude'){
					push(@searchOutput, "N/A</div><p>");
				} else {
					push(@searchOutput, "N/A\n");
				}
			}
		}
		$searchResults = join('', @searchOutput);
	}
}

main();
