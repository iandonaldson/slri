#!/usr/bin/perl
# keep track of pattern set's precision and recall
# tested on a training set that contains curated interaction sentences
# bash-2.05$ perl training.pl [pattern file] [interaction-sentences] 


open(RE, $ARGV[0]); 		#pattern file
open(INPUT,$ARGV[1]); 		#int_sentences file
@a = <RE>;
close(RE);
@b = <INPUT>;
close(INPUT);
@matrix = (
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9]
); 
@sum=([0,0,0,0],[0,0,0,0],[0,0,0,0],[0,0,0,0]);
@stat=([0,0],[0,0]);

$precison =0.0; 	#correct-match/match
$recall =0.0;
$fmeasure = 0.0;
$accuracy = 0.0;

$total =0;			#sentence total
$match=0;			#sentence count
$cm=0;				#correct match

foreach $line(@b){		
	if($line =~/\|/){
		++$total;
		chomp($line);
		($pmid, $truth,$gis, $text)=split(/\|/,$line);
		($gia, $gib) = split(/ /, $gis);
		
		$scoren=0;
		$score0=0;
		$score1=0.323+0.115;
		$score2=0.438;
		$maxscore=0.51;
		$predict=0;
		
		@list = split(/[.;]/, $text);
		foreach $sent(@list){
		print"$sent\n";
			foreach $rule(@a){	
				if($rule =~ /^\s{0,24}(-1|0|1|2)\|([^|]*)\|([^|]*)\|(\d)\|(\d)\|"(.*)".*/){
					$class = 0; 
					$class=$1;
					$ID =$2;
					$score=0.0;
					$score=$3;
					$style = $4;
					$option=$5;
					$pattern=$6;
					chomp($pattern); 	#remove the ending newline from $pattern
	
					$predict=0;
					$correct=0;
					
					#substitude A,B by $gia,$gib
					$pattern1 = $pattern;
					$pattern1=~s/B/$gib/e;
					$pattern1=~s/A/$gia/e;
	
					$pattern2 = $pattern;
					$pattern2=~s/B/$gia/e;
					$pattern2=~s/A/$gib/e;
	
					
					if($sent=~/$pattern1/i || $sent=~/$pattern2/i){
						++$match;
						if($class==-1&&$score>$scoren){$scoren=$score;}
						if($class==0&&$score>$score0){$score0=$score;}
						if($class==2&&$score>$score2){$score2=$score;}
						if($class==1&&$score>$score1){$score1=$score;}
					}
				}
			}
		}				
		if($maxscore<=$score1){$maxscore=$score1; $predict = 1;}
		if($maxscore<=$score2){$maxscore=$score2; $predict = 2;}
		if($maxscore<=$score0){$maxscore=$score0; $predict = 0;}
		if($maxscore<=$scoren){$maxscore=$scoren; $predict = -1;}
			
		$sum[$truth+1][$predict+1]++;
		if($truth==$predict ){
			$correct =1;
			++$cm;
		}			
		else{
			$correct =0;
		}
		print"$pmid|$maxscore|$truth|$predict|$correct|$gia $gib|$text\n";
	}
}

print "---- statistics ----\n";
if($match!=0 && $total!=0){
	$accuracy = 1.0*$cm/$total;
}
else{
	$accuracy=0;
}
print "[$match] all matches ; [$total] sentences: [$cm] correct predictions \n";
print "accuracy:  [$accuracy]\n\n";
print "---- sum[4][4] ----\n";
for($i=0;$i<4;$i++){
	for($j=0;$j<4;$j++){
		$stat[$i/2][$j/2]+=$sum[$i][$j];
		print "$sum[$i][$j]\t";
	}
	print "\n";
}
print "---- stat[2][2] ----\n";
for($i=0;$i<2;$i++){
	for($j=0;$j<2;$j++){
		print "$stat[$i][$j]\t";
	}
	print "\n";
}
print "---- is this interaction real ? ----\n";
$precision = $stat[1][1]/($stat[0][1]+$stat[1][1]);
$recall = $stat[1][1]/($stat[1][0]+$stat[1][1]);
$fmeasure = 2*$precision*$recall/($precision+$recall);
print"precision= [$precision]; recall = [$recall]; fmeasure = [$fmeasure];"
