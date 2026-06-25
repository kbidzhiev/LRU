set terminal pngcairo size 800,600 enhanced font "Helvetica,12"
set output 'cache_hit_statistics.png'

set title "Cache statistics for uniform random integers [0:100)"
set xlabel "Cache size"
set ylabel "Cache hits"
set key bottom right

plot "cache_hits.dat" u 1:2 title "LRU",\
      "" u 1:3 title "LFU",\
      "" u 1:4 title "Belady"\
