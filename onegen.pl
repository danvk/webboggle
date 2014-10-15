#!/usr/bin/perl
use Data::Dumper;
use IPC::Open2;

# Set up the bidirectional pipe
my $exec = "./boggle.exe web2a";
local (*boggleRead, *boggleWrite);
$bogglePid = open2(\*boggleRead, \*boggleWrite, $exec);
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
sub fmtDie($) {
	int(1+$_[0]/4) . "-" . (1+$_[0]%4)
}

# "Mutations" to a board
# Each of these will be applied with some probability

# Pick a random die, and change its value
sub changeRoll($) {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	my $die = int(16*rand);
	my $newRoll = int(6*rand);
	$rolls->[$die] = $newRoll;
	return "rep die ". fmtDie($die)." with '" . $let[$dice->[$die]][$newRoll]. "'";
}

# Swap two dice
sub swapDice($) {
	my $ref = shift;
	my ($dice,$rolls) = @$ref;
	my $die1 = int(16*rand);
	my $die2 = int(16*rand);
	($dice->[$die1],  $dice->[$die2])  = ($dice->[$die2],  $dice->[$die1]);
	($rolls->[$die1], $rolls->[$die2]) = ($rolls->[$die2], $rolls->[$die1]);
	return "swapped dice ". fmtDie($die1)." and ".fmtDie($die2);
}

$reRollFreq = 0.2;
$swapFreq = 0.2;
my @meths = (\&changeRoll, \&swapDice);
sub mutate($) {
	my $bd = shift;
	my $r = 1.0;
	my @muts = ();
	while (rand() < $r) {
		push @muts, $meths[2*rand]->($bd);
		$r *= 0.36;
	}
	return join ", ", @muts;
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
#while (++$rounds) {
print "Initial board:\n";	
	$b = randomBoard();
	$bestSc = getScore($b);
	printBoard($b);
	print "Score: $bestSc\n";

	$lastMut = 0;
	$gen = 2;
	
	while ($gen < $lastMut + 3000) {
		$c = dupBoard($b);
		$desc = mutate($c);
		$s = getScore($c);
		if ($s > $bestSc) {
			$b = $c;
			$bestSc = $s;
			
			print "Mutation: $desc\n";
			print "After $gen generations, found $s\n";
			printBoard($b);
			print "Score: $s\n";
			
			$lastMut = $gen;
			print DATA "$gen\t$s\n";
		}
		$gen++;
	}
	print "Performance: ", performance(), "\n";
