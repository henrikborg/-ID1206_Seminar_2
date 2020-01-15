set terminal png
set output "3.0_vanilla_freelist.png"
set title "Vanilla implementation - free list"
set key right center
set xlabel "number of runs"
set ylabel "size of free list"

# use log scale if we use doubling of number of pages
#set logscale x 2
#set logscale y 2
#set xrange {2,4,8,16}

plot "3.0_vanilla_MEM-SLOTS-100"  u 1:2 w linespoints title " 100 memory spots",\
     "3.0_vanilla_MEM-SLOTS-1000" u 1:2 w linespoints title "1000 memory spots"


set output "3.0_vanilla_arena.png"
set title "Vanilla implementation - arena"
set ylabel "size of arena"

plot "3.0_vanilla_MEM-SLOTS-100"  u 1:3 w linespoints title " 100 memory spots",\
     "3.0_vanilla_MEM-SLOTS-1000" u 1:3 w linespoints title "1000 memory spots"


set output "3.0_vanilla_mean_value.png"
set title "Vanilla implementation - mean value of blocks in free list"
set ylabel "mean size of free blocks"
set logscale y 2

plot "3.0_vanilla_MEM-SLOTS-100"  u 1:4 w linespoints title " 100 memory spots",\
     "3.0_vanilla_MEM-SLOTS-1000" u 1:4 w linespoints title "1000 memory spots"

