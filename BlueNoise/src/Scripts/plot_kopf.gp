set terminal postscript eps
set output 'data/kopf_b2048_4x4_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/kopf_b2048_4x4_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/kopf_b2048_4x4_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/kopf_b2048_4x4_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/adaptive_2dim_0p0044_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/adaptive_2dim_0p0044_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/adaptive_2dim_0p0044_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/adaptive_2dim_0p0044_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/ppentomino_100x100_10x10_s2_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/ppentomino_100x100_10x10_s2_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/ppentomino_100x100_10x10_s2_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/ppentomino_100x100_10x10_s2_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/ppentomino_10x10_10x10_s5_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/ppentomino_10x10_10x10_s5_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/ppentomino_10x10_10x10_s5_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/ppentomino_10x10_10x10_s5_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/ghexomino_subdivision_432x432_s2_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/ghexomino_subdivision_432x432_s2_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/ghexomino_subdivision_432x432_s2_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/ghexomino_subdivision_432x432_s2_variance.dat" with line linetype rgb "blue"

set terminal postscript eps
set output 'data/lagae_33856_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:7.2] "data/lagae_33856_mean.dat" with line linetype rgb "red"
set terminal postscript eps
set output 'data/lagae_33856_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/lagae_33856_variance.dat" with line linetype rgb "blue"

