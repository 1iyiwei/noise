#!/bin/tcsh -f

set normalized = 1;

set rho_text = 0p671069;
set target_samples = 10400;

foreach k_option (0 1)

if(${k_option} == 0) then
set k_text = ${rho_text}k;
else
set k_text = ${target_samples}k;
endif

foreach r (0p011 0p01sqrt2 0p02 0p04 0p08 0p16)
foreach id (0 1)

if(${k_option} == 0) then

if(${r} == '0p01') then

if(${id} == 0) then
set sample_count = 5200;
if($normalized) then 
set sample_count = 2600;
endif
else
set sample_count = 5200;
if($normalized) then 
set sample_count = 2600;
endif
endif

else if(${r} == '0p011') then

if(${id} == 0) then
set sample_count = 4297;
if($normalized) then 
set sample_count = 1775.40471;
endif
else
set sample_count = 6102;
if($normalized) then 
set sample_count = 3580.23115;
endif
endif

else if(${r} == '0p01sqrt2') then

if(${id} == 0) then
set sample_count = 2600;
if($normalized) then 
set sample_count = 650;
endif
else
set sample_count = 7799;
if($normalized) then 
set sample_count = 5848.5001;
endif
endif

else if(${r} == '0p02') then

if(${id} == 0) then
set sample_count = 1300;
if($normalized) then 
set sample_count = 162.5;
endif
else
set sample_count = 9100;
if($normalized) then 
set sample_count = 7962.5;
endif
endif

else if(${r} == '0p04') then

if(${id} == 0) then
set sample_count = 325;
if($normalized) then 
set sample_count = 10.15625;
endif
else
set sample_count = 10075;
if($normalized) then 
set sample_count = 9760.15625;
endif
endif

else if(${r} == '0p08') then

if(${id} == 0) then
set sample_count = 81;
if($normalized) then 
set sample_count = 0.630865385;
endif
else
set sample_count = 10319;
if($normalized) then 
set sample_count = 10238.6309;
endif
endif

else if(${r} == '0p16') then

if(${id} == 0) then
set sample_count = 20;
if($normalized) then 
set sample_count = 0.0384615385;
endif
else
set sample_count = 10380;
if($normalized) then 
set sample_count = 10360.0385;
endif
endif

endif

else

if(${r} == '0p01') then

if(${id} == 0) then
set sample_count = 5202.9;
if($normalized) then 
set sample_count = 2602.90081;
endif
else
set sample_count = 5197.1;
if($normalized) then 
set sample_count = 2597.10081;
endif
endif

else if(${r} == '0p011') then

if(${id} == 0) then
set sample_count = 4176.4;
if($normalized) then 
set sample_count = 1677.14586;
endif
else
set sample_count = 6223.6;
if($normalized) then 
set sample_count = 3724.34586;
endif
endif

else if(${r} == '0p01sqrt2') then

if(${id} == 0) then
set sample_count = 2553.9;
if($normalized) then 
set sample_count = 627.154347;
endif
else
set sample_count = 7846.1;
if($normalized) then 
set sample_count = 5919.35435;
endif
endif

else if(${r} == '0p02') then

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

else if(${r} == '0p04') then

if(${id} == 0) then
set sample_count = 380.1;
if($normalized) then 
set sample_count = 13.891924;
endif
else
set sample_count = 10019.9;
if($normalized) then 
set sample_count = 9653.69192;
endif
endif

else if(${r} == '0p08') then

if(${id} == 0) then
set sample_count = 100.6;
if($normalized) then 
set sample_count = 0.973111538;
endif
else
set sample_count = 10299.4;
if($normalized) then 
set sample_count = 10199.7731;
endif
endif

else if(${r} == '0p16') then

if(${id} == 0) then
set sample_count = 26.2;
if($normalized) then 
set sample_count = 0.0660038462;
endif
else
set sample_count = 10373.8;
if($normalized) then 
set sample_count = 10347.666;
endif
endif

endif

endif

#@ sample_count = ${sample_count}*${sample_count}/10400;

./txt2dat data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.txt data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_variance.txt ${sample_count} data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.dat data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_variance.dat 1

./txt2dat data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.txt data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected${id}_average10_variance.txt ${sample_count} data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.dat data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected${id}_average10_variance.dat 1

end # foreach id
end # foreach r
end # foreach k_option

# plot individual
echo "" > foo.gp

foreach k_option (0 1)

if(${k_option} == 0) then
set k_text = ${rho_text}k;
else
set k_text = ${target_samples}k;
endif

foreach r (0p011 0p01sqrt2 0p02 0p04 0p08 0p16)

# overlay, all
echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_all_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected0_average10_mean.dat\" with line linetype rgb \"blue\", \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected1_average10_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected1_average10_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

# overlay, truth
echo set terminal postscript eps enhanced >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_truth_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key on >> foo.gp
echo set autoscale y >> foo.gp
if(${r} == '0p02') then
echo set label \"{f_0}\" textcolor rgb \"blue\" at 50,0.25 right >> foo.gp
echo set label \"{f_1}\" textcolor rgb \"red\" at 125,1.325 right >> foo.gp
endif
echo plot "[1:256]" \"data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected0_average10_mean.dat\" title \'class 0\' with line linetype rgb \"blue\", \"data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected1_average10_mean.dat\" title \'class 1\' with line linetype rgb \"red\" >> foo.gp

echo "" >> foo.gp

# individual plots
foreach id (0 1)

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_1c_0_${r}m0p01osqrt2_${k_text}_selected${id}_average10_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/poisson_2d_2c_00_${r}m0p01osqrt2_${k_text}_selected${id}_average10_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end # foreach id
end # foreach r
end # foreach k_option

wgnuplot foo.gp

rm foo.gp
