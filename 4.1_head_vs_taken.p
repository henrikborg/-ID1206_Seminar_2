set terminal png
set key right center
set xlabel "length of list"
set ylabel "time [ms]"

# use log scale if we use doubling of number of pages
#set logscale x 2
set logscale y 2
#set xrange {2,4,8,16}


set output "4.1_head_vs_taken.png"
set title "head vs taken"
plot "4.1_timings_head"  u 1:2 w linespoints title "vanilla",\
     "4.1_timings_taken" u 1:2 w linespoints title "merge"
