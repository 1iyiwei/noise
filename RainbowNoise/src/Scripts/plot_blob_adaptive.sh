#!/bin/tcsh -f

if(1) then

set dimension = 2;

set num_runs = 10;

foreach r_value (02 04)

if(${r_value} == '02') then
set num_samples = 1529.6;
else if(${r_value} == '04') then
set num_samples = 364.6;
else
echo unknown r_value;
exit(1);
endif

foreach evaluator ('sft' 'lft' 'lft_shiftinvariant')

if((${r_value} == '04') && (${evaluator} == 'lft_shiftinvariant')) then
set num_samples = 381.5;
endif

set root_name = blob_${dimension}d_0p${r_value}_average${num_runs}_${evaluator};

./txt2dat data/${root_name}_mean.txt data/${root_name}_variance.txt ${num_samples} data/${root_name}_mean.dat data/${root_name}_variance.dat 1

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/${root_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:64] [0:2.0]" \"data/${root_name}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${root_name}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:64] [-20:20]" \"data/${root_name}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;

end # foreach evaluator

end # foreach r_value

endif
