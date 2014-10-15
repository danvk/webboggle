#!/usr/bin/perl
use Data::Dumper;
use IPC::Open2;

my $dataFile = "bogglestat.txt";
my $bestFile = "bestboggle.txt";
open DATA, ">>$dataFile" or die "Can't open $dataFile for append";
open BEST, ">>$bestFile" or die "Can't open $bestFile for append";
select((select(DATA), $|=1)[0]);
select((select(BEST), $|=1)[0]);

# Set up the bidirectional pipe
#my $exec = "./stripboggle";
#my $exec = "./allinone";
my $exec = "./boggle.exe web2a";
local (*boggleRead, *boggleWrite);
$bogglePid = open2(\*boggleRead, \*boggleWrite, $exec);
#chomp ($header = <boggleRead>);
#die "Invalid subprogram" if ($header ne "Stripped Boggle");
select((select(boggleWrite), $|=1)[0]);

# All the dice, 0-15
my @let = ([qw|x l d e r i|],
		   [qw|t w o o t a|],
		   [qw|f p s a k f|],
		   [qw|z h r l n n|],
		   [qw|h n e e g w|],
		   [qw|t t r e l y|],
		   [qw|e i u n e s|],
		   [qw|a h s p c o|],
		   [qw|s i o s t e|],
		   [qw|d v y r l e|],
		   [qw|u m c o i t|],
		   [qw|r w t e h v|],
		   [qw|o a b b j o|],
		   [qw|n g e e a a|],
		   [qw|t s t i y d|],
		   [qw|n u i h m q|]);

# --------------------
# memoization is particularly beneficial for the getScore function,
# since the operation is costly, and is often called with the same set
# of parameters.
my %statsMem = ();
my $numLookups = 0;
my $startTime = time;

sub getScore($) {
	my $ref = shift;
	$numLookups++;
	
	my ($dice, $rolls) = @$ref;
	my $cmd = "";
	for my $i(0..15) {
		$cmd.=$let[$dice->[$i]][$rolls->[$i]];
	}
	
	if ($statsMem{$cmd}) {
		return $statsMem{$cmd}
	}

	print boggleWrite "$cmd\n";
	my $ret;
	chomp ($sc = <boggleRead>);
	($sc) = $sc =~ /(\d+)/;
	$sc += 0;
	
	$statsMem{$cmd} = $sc;
	
	$sc
}

# ------------------------
# Returns a random permutation of the
# integers 0..15
sub randomPerm {
	my @lst = 0..15;
	my @ret = ();
	my $len=16;
	my $idx;
	while ($len) {
		$idx = int($len * rand);
		push @ret, splice(@lst,$idx,1);
		$len--;
	}
	return @ret;
}

sub randomBoard {
	my @dice  = randomPerm;
	my @rolls = map { int(6*rand) } (0..15);
	return [\@dice, \@rolls];
}

# ------------------------

sub printBoard {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	my $idx = 0;
	for (my $row=0; $row<4; $row++) {
		for (my $col=0; $col<4; $col++) {
			print " " if ($col>0);
			print $let[$dice->[$idx]][$rolls->[$idx]];
			$idx++;
		}
		print "\n";
	}
}

sub printShortBoard {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	for my $idx(0..15) {
		print $let[$dice->[$idx]][$rolls->[$idx]];
	}
}

# ------------------------

# "Mutations" to a board
# Each of these will be applied with some probability

# Pick a random die, and change its value
sub changeRoll($) {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	my $die = int(16*rand);
	my $newRoll = int(6*rand);
	$rolls->[$die] = $newRoll;
}

# Swap two dice
sub swapDice($) {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	my $die1 = int(16*rand);
	my $die2 = int(16*rand);
	($dice->[$die1],  $dice->[$die2])  = ($dice->[$die2],  $dice->[$die1]);
	($rolls->[$die1], $rolls->[$die2]) = ($rolls->[$die2], $rolls->[$die1]);
}

$reRollFreq = 0.2;
$swapFreq = 0.2;
my @meths = (\&changeRoll, \&swapDice);
sub mutate($) {
	my $bd = shift;
	my $r = 1.0;
	while (rand() < $r) {
		$meths[2*rand]->($bd);
		$r *= 0.36;
	}
}

sub getGoodBoard($) {
	my $n = shift;
	my $bestB = [];
	my $bestSc = 0;
	for (1..$n) {
		my $b = randomBoard();
		my $s = getScore($b);
		($bestB, $bestSc) = ($b, $s) if ($s > $bestSc);
	}
	$bestB
}

sub dupBoard($) {
	my $ref = shift;
	my ($dice, $rolls) = @$ref;
	my @dice = @$dice;
	my @rolls = @$rolls;
	
	[\@dice, \@rolls]
}

sub performance() {
	my $now = time;
	return "inf bds/sec" if ($now == $startTime);
	my $bdsPS = int(1000*$numLookups / ($now-$startTime))/1000;
	my $sPBd  = int(1000000/$bdsPS)/1000000;
	"$bdsPS bds/sec, $sPBd sec/bd"
}

$bestEver = [];
$bestScore = 0;
$rounds = 0;
while (++$rounds) {
	#print "Finding a good initial board...\n";
	#$b = getGoodBoard(250);
	$b = randomBoard();
	$bestSc = getScore($b);
	#print "Found:\n";
	#printBoard($b);
	#print "Score: $bestSc\n";
	printShortBoard($b); print " ($bestSc) -> ";
	print DATA "1\t$bestSc\n";

	$lastMut = 0;
	$gen = 2;
	
	while ($gen < $lastMut + 3000) {
		$c = dupBoard($b);
		mutate($c);
		$s = getScore($c);
		if ($s > $bestSc) {
			$b = $c;
			$bestSc = $s;
			#if ($gen > 1000) {
				#print "After $gen generations, found $s\n";
				#printBoard($b);
				#print "Score: $s\n";
			#}
			if ($bestSc >= 2500) {
				print "Found:\n";
				printBoard($b);
				print "Score: $s\n";

				$old = select(BEST);
				printBoard($b);
				print "Score: $s\n\n";
				select($old);
			}
			$lastMut = $gen;
			print DATA "$gen\t$s\n";
		}
		$gen++;
		#print +("$gen, performance: ", performance(), "\n") unless ($gen % 1000);
	}
	
	printShortBoard($b); print " ($bestSc)\n";
	
	if ($bestSc > $bestScore) {
		$bestScore = $bestSc;
		$bestEver = $b;
	}
	#print "Done\n\n";
	#print "Best ever:\n";
	if ($rounds % 25 == 0) {
		printBoard($bestEver);
		print "Score: $bestScore\n\n";
	
		print +("$gen, performance: ", performance(), "\n") unless ($rounds % 25);
	}
	
	%statsMem = ();
}
