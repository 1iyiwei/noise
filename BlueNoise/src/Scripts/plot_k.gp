set terminal postscript eps
set output 'data/poisson_2dim_0p01_k4_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k4_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k4_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k4_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k5_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k5_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k5_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k5_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k6_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k6_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k6_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k6_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k7_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k7_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k7_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k7_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k8_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k8_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k8_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k8_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k9_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k9_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k9_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k9_gpu_variance.dat" with line linetype rgb "blue"


set terminal postscript eps
set output 'data/poisson_2dim_0p01_k10_gpu_mean.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "power"
set key off
plot [1:256] [0:2.0] "data/poisson_2dim_0p01_k10_gpu_mean.dat" with line linetype rgb "red"

set terminal postscript eps
set output 'data/poisson_2dim_0p01_k10_gpu_variance.eps'
set size 0.5, 0.675
set title ""
set xlabel "frequency"
set ylabel "anisotrophy"
set key off
plot [1:256] [-20:20] "data/poisson_2dim_0p01_k10_gpu_variance.dat" with line linetype rgb "blue"


