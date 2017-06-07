set terminal postscript eps
set output 'data/dart_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:255] [0:1.8] "data/dart_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/dart_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:255] [-20:20] "data/dart_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:255] [0:1.8] "data/gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:255] [-20:20] "data/gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/bridson_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:255] [0:1.8] "data/bridson_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/bridson_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:255] [-20:20] "data/bridson_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/white_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:255] [0:1.8] "data/white_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/white_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:255] [-20:20] "data/white_variance.dat" with line linetype rgb "blue"

