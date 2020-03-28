#!/tools/xgs/perl/5.8.5/bin/perl

use strict;
use warnings;
use Expect;
my $k=0;
my $j=0;
my $i=0;
my $EMULATOR_COMMAND=$ARGV[0];
my $EXECUTABLE=$ARGV[1];
#my $XCLBIN=$ARGV[2];
my $command;
my $status;
my $timeoutlevel;
my $delay;
my @timeout = (0,90,180,600,3600,7200,18000,43200,86400,172800,432000,691200); 
my $exit_val=0;

#$command="launch_emulator -no-reboot -runtime ocl -t hw_emu -sd-card-image sd_card.img -device-family Ultrascale";
$command=$EMULATOR_COMMAND;

my($exp)  = new Expect; #Set exp for Expect
my($exp1) = new Expect; #Set exp for Expect



if ($ARGV[2] eq '') { 
     $status="PASS";
}
else {
     $status=$ARGV[2];
}


if ($ARGV[3] eq '') { 
     $delay=100;
}
else {
     $timeoutlevel=$ARGV[3];    
     $delay= $timeout[int($timeoutlevel)];
}


$exp-> spawn("$command")
or die "ERROR: Cannot spawn $command: $!\n";

#| Detect if linux booted on Zynq:
my $booted = 0;
 while ($i<3) {
    if( $exp->expect(900, '-re', 'root@.*:.*#')) {
        $booted=1;
        printf "\nINFO: Linux kernel booted successfully\n";
        last;
    }
    
    if( $exp->expect(100, 'ZynqMP>')) {
        printf "\nERROR: Linux kernel boot failed\n";
        $exit_val=1;
        print "\nINFO: Exiting QEMU\n";
        sleep(10);
        $exp-> send("\x01");
        $exp-> send("x");
        $exp->soft_close();
        exit($exit_val);
    }
    
	sleep(10);
	
    $i=$i+1;

  }

if( $booted == 0) {
    printf "\nERROR: It appears that booting hanged. Exiting QEMU.\n";
    sleep(10);
    $exp-> send("\x01");
    $exp-> send("x");
    $exp->soft_close();
    $exit_val = 2;
    exit($exit_val);
}
  
#| Execute commands if system is up and running:
$exp-> send( "mount /dev/mmcblk0p1 /mnt\r");
$exp-> send( "cd /mnt\r");
$exp-> send( "$EXECUTABLE\r");

#| Check for finish of the host app:
while ($j<6)
{
    if( $exp-> expect(300, "PASS", "Pass" , "Compiled", "COMPILED", " Test completed successfully", "user memory:", "Test passed", "TEST PASSED", "Completed", "Embedded host run completed" )) {
        $k=1;
    }
    if( $exp-> expect(175, ':/mnt#')) {

        #if( $xp-> expect(15, $status)) {
        if($k==1) {
            #print "\nINFO: Embedded host run completed.\n";
            #$exp-> send( "echo 'INFO: Embedded host run completed.'");
            $exit_val=0;
            last;
          } 
            else {
               print "\nERROR: Host application did not complete - pass/end string not found. Exiting as a failure\n";
              #  exit(1);
                 $exit_val=1;
            last;
          }
    }
	$j=$j+1;
}

print "\nINFO: Exiting QEMU \n";
 sleep(10);
$exp-> send("\x01");
$exp-> send("x");
$exp->soft_close();
exit($exit_val);
