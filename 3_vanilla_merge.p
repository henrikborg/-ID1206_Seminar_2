set terminal png
set key right center
set xlabel "number of runs"
set ylabel "size of free list"

# use log scale if we use doubling of number of pages
#set logscale x 2
set logscale y 2
#set xrange {2,4,8,16}


set output "3_freelist_100.png"
set title "freelist - vanilla vs merge - 100 memory spots"
plot "3.0_vanilla_MEM-SLOTS-100"  u 1:2 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:2 w linespoints title "merge"


set output "3_freelist_1000.png"
set title "freelist - vanilla vs merge - 1000 memory spots"
set ylabel "size of arena"
plot "3.0_vanilla_MEM-SLOTS-1000"  u 1:2 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-1000_x1000" u 1:2 w linespoints title "merge"


set output "3_arena_100.png"
set title "arena - vanilla vs merge - 100 memory spots"
set ylabel "msize of arena"
plot "3.0_vanilla_MEM-SLOTS-100"  u 1:3 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:3 w linespoints title "merge"


set output "3_arena_1000.png"
set title "arena - vanilla vs merge - 1000 memory spots"
set ylabel "size of arena"
plot "3.0_vanilla_MEM-SLOTS-1000"  u 1:3 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:3 w linespoints title "merge"


set output "3_merge_mean_value_100.png"
set title "mean value of free blocks - vanilla vs merge - 100 memory spots"
set ylabel "mean size of free blocks"
set logscale y 2
plot "3.0_vanilla_MEM-SLOTS-100"  u 1:4 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:4 w linespoints title "merge"


set output "3_merge_mean_value_1000.png"
set title "mean value of free blocks - vanilla vs merge - 1000 memory spots"
set ylabel "mean size of free blocks"
set logscale y 2
plot "3.0_vanilla_MEM-SLOTS-1000"  u 1:4 w linespoints title "vanilla",\
     "3.1_merge_MEM-SLOTS-1000_x1000" u 1:4 w linespoints title "merge"
