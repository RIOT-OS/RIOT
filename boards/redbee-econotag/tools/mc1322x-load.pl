#!/usr/bin/perl -w

use Device::SerialPort;
use Term::ReadKey;
use Getopt::Long;
use Time::HiRes qw(usleep);

use strict;

my $filename = '';
my $second = '';
my $term = '/dev/ttyUSB0';
my $baud = '115200';
my $verbose;
my $rts = 'rts';
my $command = '';
my $first_delay = 50;
my $second_delay = 100;
my $do_exit;
my $zerolen;

GetOptions ('file=s' => \$filename,
            'secondfile=s' => \$second,
            'zerolen' => \$zerolen,
            'terminal=s' => \$term,
            'verbose' => \$verbose,
            'u|baud=s' => \$baud,
            'rts=s' => \$rts,
            'command=s' => \$command,
            'a=s' => \$first_delay,
            'b=s' => \$second_delay,
            'exit' => \$do_exit,
    ) or die 'bad options';

$| = 1;

if($filename eq '') {
    print "Example usage: mc1322x-load.pl -f foo.bin -t /dev/ttyS0 -b 9600\n";
    print "          or : mc1322x-load.pl -f flasher.bin -s flashme.bin  0x1e000,0x11223344,0x55667788\n";
    print "          or : mc1322x-load.pl -f flasher.bin -z  0x1e000,0x11223344,0x55667788\n";
    print "       -f required: binary file to load\n";
    print "       -s optional: secondary binary file to send\n";
    print "       -z optional: send a zero length file as secondary\n";
    print "       -t, terminal default: /dev/ttyUSB0\n";
    print "       -u, --baud baud rate default: 115200\n";
    print "       -r [none|rts] flow control default: rts\n";
    print "       -c command to run for autoreset: \n";
    print "              e.g. -c 'bbmc -l redbee-econotag -i 0 reset'\n";
    print "       -e exit instead of dropping to terminal display\n";
    print "       -a first  intercharacter delay, passed to usleep\n";
    print "       -b second intercharacter delay, passed to usleep\n";
    print "\n";
    print "anything on the command line is sent\n";
    print "after all of the files.\n\n";
    exit;
}

if (!(-e $filename)) { die "file $filename not found\n"; }
if (($second ne '') && !(-e $second)) { die "secondary file $second not found\n"; }

my $ob = Device::SerialPort->new ($term) or die "Can't start $term\n";
    # next test will die at runtime unless $ob

$ob->baudrate($baud);
$ob->parity('none');
$ob->databits(8);
$ob->stopbits(1);
if($rts eq 'rts') {
    $ob->handshake('rts');
} else {
    $ob->handshake('none');
}
$ob->read_const_time(1000); # 1 second per unfulfilled "read" call
$ob->rts_active(1);

my $s = 0;
my $reset = 0;
my $size = 0;

while(1) {

    my $c; my $count; my $ret = ''; my $test='';

    if($s == 1) { print "secondary send...\n"; }

    $ob->write(pack('C','0'));

    if(($command ne '') &&
       ($reset eq 0)) {
        $reset++;
        system($command);
    }

    if($s == 1) {
        $test = 'ready';
    } else {
        $test = 'CONNECT';
    }

    until($ret =~ /$test$/) {
        ($count,$c) = $ob->read(1);
        if ($count == 0) {
            print '.';
            $ob->write(pack('C','0'));
            next;
        }
        $ret .= $c;
    }
    print $ret . "\n";

    if (-e $filename || (defined($zerolen) && ($s == 1))) {

        if(defined($zerolen) && ($s == 1)) {
            $size = 0;
        } else {
            $size = -s $filename;
        }

        print ("Size: $size bytes\n");
        $ob->write(pack('V',$size));

        if(($s == 0) ||
           ((!defined($zerolen)) && ($s == 1))) {
            open(FILE, $filename) or die($!);
            print "Sending $filename\n";

            my $i = 1;
            while(read(FILE, $c, 1)) {
                $i++;
                usleep($first_delay)  if ( $s == 0 ) && ($first_delay != 0);
                usleep($second_delay) if ( $s == 1 ) && ($second_delay != 0);
                $ob->write($c);
            }
        }
    }

    last if ($s==1);
    if((-e $second) || defined($zerolen)) {
        $s=1; $filename = $second;
    } else {
        last;
    }

}

print "done sending files.\n";

if(scalar(@ARGV)!=0) {
    print "sending " ;
    print @ARGV;
    print ",\n";

    $ob->write(@ARGV);
    $ob->write(',');
}

if(defined($do_exit)) {
    exit;
}

my $c; my $count;
while(1) {
    ($count, $c) = $ob->read(1);
    print $c if (defined($count) && ($count != 0));
}

$ob -> close or die "Close failed: $!\n";
ReadMode 0;
undef $ob;  # closes port AND frees memory in perl
exit;
