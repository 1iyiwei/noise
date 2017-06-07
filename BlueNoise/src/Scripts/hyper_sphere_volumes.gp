set terminal postscript eps
set output 'data/hyper_sphere_volumes.eps'
set size 0.5, 0.675
set title ""
set xlabel "dimension"
set ylabel "log10 volume"
set key off
plot [2:100] [0:110] "data/hyper_sphere_volumes.dat" with line linetype rgb "red"
