#!/bin/tcsh -f

# Figure~\ref{fig:priority_comparison} and Figure~\ref{fig:patient_killing_comparison}
if(1) then

set normalize = 1;

set dimension = 2;

set num_classes = 3;
set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set ids = "0 1 2";

set target_samples = 6801;
set k_value = 100;
set domain_size = "1 1";

set num_runs = 10;

foreach output_name ('poisson' 'gree' 'tree')

if(${output_name} == 'poisson') then
set num_p_options = 2;
set num_sign_options = 2;
else if(${output_name} == 'gree') then
set num_p_options = 1;
set num_sign_options = 1;
else if(${output_name} == 'tree') then
set num_p_options = 1;
set num_sign_options = 1;
else
echo unknown output name;
exit(1);
endif

set p_option = 0;
while(${p_option} < ${num_p_options})
#foreach p_option (0 1)

if(${p_option} == 0) then
set priority = "0 0 0";
set p_text = 000;
else
set priority = "0 1 2";
set p_text = 012;
endif

set sign_option = 0;
while(${sign_option} < ${num_sign_options})
#foreach sign_option (0 1)

if(${sign_option} == 1) then
set sign = '-';
else if (${sign_option} == 0) then
set sign = '+';
else
echo unknown sign option
exit(1);
endif

if((${sign_option} == 0) && (${p_option} == 0)) then

if(${output_name} == 'poisson') then
set patience = ('' '_1p' '_100p');
else if(${output_name} == 'gree') then
set patience = ('_0p' '_1p' '_100p');
else if(${output_name} == 'tree') then
set patience = ('_0p' '_1p' '_100p');
else
echo unknown output_option;
exit(1);
endif

set patience_size = 3;

else

set patience = ('');
set patience_size = 1;

endif

set id = 0;
while(${id} < ${num_classes})

if(${id} == 0) then
set num_samples = 323;
if(${normalize}) then
set num_samples = 15.3402441; #323^2/6801
endif
else if(${id} == 1) then
set num_samples = 1295;
if(${normalize}) then
set num_samples = 246.585061;
endif
else if(${id} == 2) then
set num_samples = 5183;
if(${normalize}) then
set num_samples = 3949.93222;
endif
else
echo unknown id
exit(1);
endif

#wc -l ../../data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_?_selected${id}.txt

set which_patience = 0;
while(${which_patience} < ${patience_size})

@ which_patience = ${which_patience} + 1;

set patience_text = $patience[$which_patience];

./txt2dat data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_mean.txt data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_variance.txt ${num_samples} data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_mean.dat data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_variance.dat 1

./txt2dat data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.txt data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_variance.txt ${num_samples} data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_variance.dat 1

if(${id} == 0) then
set y_range = "[0:0.09]";
else if(${id} == 1) then
set y_range = "[0:0.35]";
else if(${id} == 2) then
set y_range = "[0:1.4]";
else
echo unknown id;
exit(1);
endif

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:256] ${y_range}" \"data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_mean.dat\" with line linetype rgb \"red\", \"data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;

end # which_patience

# plot different patience values together
if(1) then

if((${sign_option} == 0) && (${p_option} == 0)) then
set patience_text = '_allp';

set patience1 = $patience[1];
set patience2 = $patience[2];
set patience3 = $patience[3];

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.676 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:256] ${y_range}" \"data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience1}_selected${id}_average${num_runs}_mean.dat\" title \'${patience1}\' with line linetype rgb \"\#ff9900\", \"data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience2}_selected${id}_average${num_runs}_mean.dat\" title \'${patience2}\' with line linetype rgb \"blue\", \"data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience3}_selected${id}_average${num_runs}_mean.dat\" title \'${patience3}\' with line linetype rgb \"red\", \"data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'truth\' with line linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;
endif

endif

@ id = ${id} + 1;
end

@ sign_option = ${sign_option} + 1;
end # foreach sign

@ p_option = ${p_option} + 1;
end # foreach p_option

end # foreach output_name

# aggregate plots
if(1) then

set id = 0;
while(${id} < ${num_classes})

if(${id} == 0) then
set y_range = "[0:0.09]";
else if(${id} == 1) then
set y_range = "[0:0.35]";
else if(${id} == 2) then
set y_range = "[0:1.4]";
else
echo unknown id;
exit(1);
endif

# plot different output_names together
set output_name = allo;

set p_option = 0;
set priority = "0 0 0";
set p_text = 000;

set sign_option = 0;
set sign = '+';

foreach patience_text ('_0p' '_1p' '_100p')

if(${patience_text} == '_0p') then
set patience_text_p = '';
else
set patience_text_p = ${patience_text};
endif

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.676 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:256] ${y_range}" \"data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text_p}_selected${id}_average${num_runs}_mean.dat\" title \'grid\' with line linetype rgb \"\#ff9900\", \"data/gree_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_mean.dat\" title \'gree\' with line linetype rgb \"blue\", \"data/tree_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}${patience_text}_selected${id}_average${num_runs}_mean.dat\" title \'tree\' with line linetype rgb \"red\", \"data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'truth\' with line linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;
end # foreach patience

# aggregate plots for Figure~\ref{fig:priority_comparison}
if(1) then

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/priority_${dimension}d_${num_classes}c_${r_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.676 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:256] ${y_range}" \"data/poisson_${dimension}d_-${num_classes}c_000_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 1\' with line linetype rgb \"\#ff9900\", \"data/tree_${dimension}d_+${num_classes}c_000_${r_text}_1p_selected${id}_average${num_runs}_mean.dat\" title \'case 2\' with line linetype rgb \"blue\", \"data/poisson_${dimension}d_+${num_classes}c_012_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 3\' with line linetype rgb \"red\", \"data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'truth\' with line linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;
endif

# zoom in, aggregate plots for Figure~\ref{fig:priority_comparison}

if(1) then
if(${id} == 0) then
set x_range_zoom = "[1:256]";
set y_range_zoom = "[0:0.09]";
else if(${id} == 1) then
set x_range_zoom = "[1:74]"; #"[1:256]";
set y_range_zoom = "[0:0.1]"; #"[0:0.35]";
else if(${id} == 2) then
set x_range_zoom = "[1:92]"; #"[1:256]";
set y_range_zoom = "[0:0.5]"; #"[0:1.4]";
else
echo unknown id;
exit(1);
endif

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/priority_${dimension}d_${num_classes}c_${r_text}_selected${id}_mean_zoom.eps\' >> foo.gp
echo set size 0.5, 0.676 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
if(${id} == 1) then
echo set key right top >> foo.gp
else if(${id} == 2) then
echo set key left top >> foo.gp
endif
#echo set autoscale y >> foo.gp
#echo plot "${x_range_zoom} ${y_range_zoom}" \"data/poisson_${dimension}d_-${num_classes}c_000_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 1\' with line linetype rgb \"\#ff9900\", \"data/tree_${dimension}d_+${num_classes}c_000_${r_text}_1p_selected${id}_average${num_runs}_mean.dat\" title \'case 2\' with line linetype rgb \"blue\", \"data/poisson_${dimension}d_+${num_classes}c_012_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 3\' with line linetype rgb \"red\", \"data/poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'truth\' with line linetype rgb \"\#009900\" >> foo.gp
echo plot "${x_range_zoom} ${y_range_zoom}" \"data/poisson_${dimension}d_-${num_classes}c_000_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 1\' with line linetype rgb \"red\", \"data/tree_${dimension}d_+${num_classes}c_000_${r_text}_1p_selected${id}_average${num_runs}_mean.dat\" title \'case 2\' with line linetype rgb \"\#009900\", \"data/poisson_${dimension}d_+${num_classes}c_012_${r_text}_selected${id}_average${num_runs}_mean.dat\" title \'case 3\' with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp;
rm foo.gp;
endif

@ id = ${id} + 1;
end

endif

endif


####################
# old stuff below
####################
if(0) then
foreach selection (0 1 2)
foreach priority (000 210)
foreach rtext (0p010p020p04full)

#wc -l ../../data/poisson_2d_3c_${priority}_${rtext}_?_selected${selection}.txt

if(${selection} == 0 && ${priority} == 000) then
set num_samples = 5778.7
else if(${selection} == 0 && ${priority} == 210) then
set num_samples = 5418.2
else if(${selection} == 1 && ${priority} == 000) then
set num_samples = 1187.0
else if(${selection} == 1 && ${priority} == 210) then
set num_samples = 1386.7
else if(${selection} == 2 && ${priority} == 000) then
set num_samples = 316.3
else if(${selection} == 2 && ${priority} == 210) then
set num_samples = 429.1
endif

./txt2dat data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_mean.txt data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_variance.txt ${num_samples} data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_mean.dat data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_variance.dat 1

end
end
end

echo "" > foo.gp

foreach selection (0 1 2)
foreach priority (000 210)
foreach rtext (0p010p020p04full)

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [0:2.0]" \"data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:20]" \"data/poisson_2d_3c_${priority}_${rtext}_selected${selection}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end
end
end

wgnuplot foo.gp

rm foo.gp
endif
