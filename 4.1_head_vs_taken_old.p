set terminal png
set key right center
set xlabel "number of runs"
set ylabel "size of free list"

# use log scale if we use doubling of number of pages
#set logscale x 2
set logscale y 2
#set xrange {2,4,8,16}

#
set output "4_freelist_taken_merge_100.png"
set title "freelist - taken vs merge - 100 memory spots"
set ylabel "size of freelist"
plot "4.1_header_MEM-SLOTS-100"  u 1:2 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:2 w linespoints title "merge"

#
set output "4_freelist_taken_merge_1000.png"
set title "freelist - taken vs merge - 1000 memory spots"
set ylabel "size of freelist"
plot "4.1_header_MEM-SLOTS-1000"  u 1:2 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-1000_x1000" u 1:2 w linespoints title "merge"


set output "4_arena_taken_merge_100.png"
set title "arena - taken vs merge - 100 memory spots"
set ylabel "size of arena"
plot "4.1_header_MEM-SLOTS-100"  u 1:3 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:3 w linespoints title "merge"


set output "4_arena_taken_merge_1000.png"
set title "arena - taken vs merge - 1000 memory spots"
set ylabel "size of arena"
plot "4.1_header_MEM-SLOTS-1000"  u 1:3 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-1000_x1000" u 1:3 w linespoints title "merge"


set output "4_merge_mean_value_taken_merge_100.png"
set title "mean value of free blocks - taken vs merge - 100 memory spots"
set ylabel "mean size of free blocks"
set logscale y 2
plot "4.1_header_MEM-SLOTS-100"  u 1:4 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-100_x1000" u 1:4 w linespoints title "merge"


set output "4_merge_mean_value_taken_merge_1000.png"
set title "mean value of free blocks - taken vs merge - 1000 memory spots"
set ylabel "mean size of free blocks"
set logscale y 2
plot "4.1_header_MEM-SLOTS-1000"  u 1:4 w linespoints title "taken",\
     "3.1_merge_MEM-SLOTS-1000_x1000" u 1:4 w linespoints title "merge"
