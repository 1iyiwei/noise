#!/bin/tcsh -f

foreach c (1 2 4 8 9 13 16 17 32 64 128 256)
./txt2dat data/poisson_2d_${c}c_0p02_mean.txt data/poisson_2d_${c}c_0p02_variance.txt 1300 data/poisson_2d_${c}c_0p02_mean.dat data/poisson_2d_${c}c_0p02_variance.dat 1
end

echo "" > foo.gp

foreach c (1 2 4 8 9 13 16 17 32 64 128 256)

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_${c}c_0p02_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp

if(${c} == 1) then
echo plot "[1:256] [0:1.6]" \"data/poisson_2d_${c}c_0p02_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
else
echo plot "[1:256] [0:1.6]" \"data/poisson_2d_${c}c_0p02_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_1c_0p02_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
endif

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_${c}c_0p02_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/poisson_2d_${c}c_0p02_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end

wgnuplot foo.gp

rm foo.gp
