#!/bin/tcsh -f

set normalized = 1;

foreach method (randomized discreterelax)
foreach r (0p02)
foreach id (0 1)

if(${r} == '0p02') then

if(${id} == 0) then
set sample_count = 1346.2;
if($normalized) then 
set sample_count = 174.255235;
endif
else
set sample_count = 9053.8;
if($normalized) then 
set sample_count = 7881.85523;
endif
endif

else

echo unknown r value
exit(1);
endif

# ground truth
./txt2dat data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_mean.txt data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_variance.txt ${sample_count} data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_mean.dat data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_variance.dat 1

# method 
./txt2dat data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_mean.txt data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_variance.txt ${sample_count} data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_mean.dat data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_variance.dat 1

# plot
echo "" > foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
if(${id} == 1) then
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
else
echo plot "[1:256] [0:0.45]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_2c_00_${r}m0p01osqrt2_10400k_selected${id}_average10_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
endif

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${method}_selected${id}_average10_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp
rm foo.gp

end
end
end
