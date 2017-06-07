set terminal postscript eps
set output 'data/3d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:64] [0:1.6] "data/3d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/3d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:64] [-20:20] "data/3d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/4d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:32] [0:1.6] "data/4d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/4d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:32] [-20:20] "data/4d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/5d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:16] [0:1.6] "data/5d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/5d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:16] [-20:20] "data/5d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/6d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:8] [0:1.6] "data/6d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/6d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:8] [-20:20] "data/6d_variance.dat" with line linetype rgb "blue"



set terminal postscript eps
set output 'data/dart_3d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:64] [0:1.6] "data/dart_3d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/dart_3d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:64] [-20:20] "data/dart_3d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/dart_4d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:32] [0:1.6] "data/dart_4d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/dart_4d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:32] [-20:20] "data/dart_4d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/dart_5d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:16] [0:1.6] "data/dart_5d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/dart_5d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:16] [-20:20] "data/dart_5d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/dart_6d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:8] [0:1.6] "data/dart_6d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/dart_6d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:8] [-20:20] "data/dart_6d_variance.dat" with line linetype rgb "blue"



set terminal postscript eps
set output 'data/white_3d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:64] [0:1.6] "data/white_3d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/white_3d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:64] [-20:20] "data/white_3d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/white_4d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:32] [0:1.6] "data/white_4d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/white_4d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:32] [-20:20] "data/white_4d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/white_5d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:16] [0:1.6] "data/white_5d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/white_5d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:16] [-20:20] "data/white_5d_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/white_6d_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:8] [0:1.6] "data/white_6d_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/white_6d_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:8] [-20:20] "data/white_6d_variance.dat" with line linetype rgb "blue"
