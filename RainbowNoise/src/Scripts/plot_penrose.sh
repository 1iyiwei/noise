#!/bin/tcsh -f

set penrose_sample_count = 1542.7;
set grid_min_sample_count = 646;
set grid_max_sample_count = 1543;

foreach base_name (penrose_color_uniform40000 grid_min_uniform40000 grid_max_uniform40000)

if(${base_name} == 'penrose_color_uniform40000') then
set sample_count = ${penrose_sample_count};
else if(${base_name} == 'grid_min_uniform40000') then
set sample_count = ${grid_min_sample_count};
else if(${base_name} == 'grid_max_uniform40000') then
set sample_count = ${grid_max_sample_count};
else
echo unknown base_name;
exit(1);
endif

./txt2dat data/${base_name}_average10_mean.txt data/${base_name}_average10_variance.txt ${sample_count} data/${base_name}_average10_mean.dat data/${base_name}_average10_variance.dat 1

echo "" > foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_average10_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
# echo set autoscale y >> foo.gp
echo plot "[1:256] [0:4.5]" \"data/${base_name}_average10_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_average10_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:20]" \"data/${base_name}_average10_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp
rm foo.gp

end #foreach base_name
