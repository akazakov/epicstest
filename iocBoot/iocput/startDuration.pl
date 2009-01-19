eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell; # startDuration.pl

use Cwd;
use Getopt::Std;

getopt "s";

if ($opt_s) {
    $time_to_sleep = $opt_s;
} else {		# Look for O.<arch> in current path
    $time_to_sleep = 20;
}

unless (@ARGV == 1) {
    print "Usage: startDuration.pl [-s sleepseconds] cmdfile\n";
    exit 2;
}

$origPATH = $ENV{"PATH"};
$epicsHostArch = $ENV{"EPICS_HOST_ARCH"};
$ENV{"PATH"} = "${origPATH}:../../bin/${epicsHostArch}";

$cmdfile = $ARGV[0];
open(IN, $cmdfile) or die "Can't open $file: $!\n";
while (<IN>) {
    chomp;
    push(@cmdlist,$_);
}
close IN;
while(1) {
    $len = @cmdlist;
    for($i = 0; $i < $len; $i++) {
        if(!defined($pid=fork())) {
             die "cannot fork: $!";
        } elsif ($pid) {
           $pidchild[$i] = $pid;
        } else {
            exec("$cmdlist[$i]");
            die "cant exec $cmdlist[$i]\n";
        }
    }
    sleep($time_to_sleep);
    for($i = 0; $i <$len; $i++) {
        $num = kill(9,$pidchild[$i]);
        if($num!=1) {
            die "kill failed\n";
        }
    }
    while(wait()!=-1) {};
    sleep(10);
}
