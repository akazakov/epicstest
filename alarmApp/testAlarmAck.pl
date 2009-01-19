eval 'exec perl -S $0 ${1+"$@"}'  # -*- Mode: perl -*-
    if $running_under_some_shell;

#testAlarm pvname outfile

use Cwd;

$hostarch = $ENV{"EPICS_HOST_ARCH"};

$len = @ARGV;
unless($len==2) {Cleanup(1,"Must have two arguments");}

$channel = $ARGV[0];
$outfile = $ARGV[1];
unlink("$outfile");
$bin = getcwd();
$bin = "$bin/../bin/$hostarch";

$acktvalues = "0 1";
@acktvalues = split(" ",$acktvalues);
$acksvalues = "1 2";
@acksvalues = split(" ",$acksvalues);
$values = "2 4 10 16 18";
$noalarmvalue = "10";
@values = split(" ",$values);

$caputacks = "${bin}/caputacks";
$caputackt = "${bin}/caputackt";
$cagetalarm ="${bin}/cagetalarm";

system("echo 'test of global alarm acknowledge' >> $outfile");
foreach $ackt (@acktvalues) {
    if(system("$caputackt $channel $ackt >> $outfile")) { die "failure\n";}
    foreach $value (@values) {
	foreach $acks (@acksvalues) {
	    if(system("caput -t $channel $value >> $outfile")) {
		die "failure\n";
	    }
	    if(system("$cagetalarm $channel  >> $outfile")) {
		die "failure\n";
	    }
	    if(system("echo caputacks $channel $acks >> $outfile")) {
		die "failure\n";
	    }
	    if(system("$caputacks $channel $acks >> $outfile")) {
		die "failure\n";
	    }
	    if(system("$cagetalarm $channel  >> $outfile")) {
		die "failure\n";
	    }
	    if(system("caput -t $channel 10.0 >> $outfile")) {
		die "failure\n";
	    }
	    if(system("$cagetalarm $channel  >> $outfile")) {
		die "failure\n";
	    }
	}
    }
}

sub Cleanup
{
    my ($rtncode, @message) = @_;

    foreach $line ( @message )
    {
	print "$line\n";
    }
    print "\nUsage:\n\n",
	"$0 pvname outfile\n";
    exit $rtncode;
}
