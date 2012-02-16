#!/bin/tcsh -f

set outputdir = data;

if(0) then

set dimension = 2;
set num_classes = 1;

set r_text = 0p02;
set num_bins = 800;
set hist_freq = 1;
set all_pfm_files_count = 10;

set target_samples = 1680;

foreach distribution ('poisson' 'white')

./txt2dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt ${target_samples} data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.dat 1

end

# histogram
foreach normalized (0 1)

if(${normalized}) then
set output_name = ${outputdir}/poisson_white_${dimension}d_${num_classes}c_${r_text}_histogram_normalized_freq${hist_freq}

./normalize_hist ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_histogram.txt ${outputdir}/white_${dimension}d_${num_classes}c_${r_text}_histogram.txt ${output_name}.txt
else
set output_name = ${outputdir}/poisson_white_${dimension}d_${num_classes}c_${r_text}_histogram_freq${hist_freq}
endif

echo plot ${output_name}.eps

#echo plot data/poisson_white_${dimension}d_${num_classes}c_${r_text}_histogram_freq${hist_freq}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalized}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
if(${normalized}) then
echo plot "[0:1]" \"data/poisson_white_${dimension}d_${num_classes}c_${r_text}_histogram_normalized_freq${hist_freq}.txt\" title \'Poisson disk\' with lines linetype rgb \"red\" >> foo.gp
else
echo set autoscale y >> foo.gp
echo plot "[0:1]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_name}_zoom.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalized}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
if(${normalized}) then
echo plot "[0:0.2]" \"data/poisson_white_${dimension}d_${num_classes}c_${r_text}_histogram_normalized_freq${hist_freq}.txt\" title \'Poisson disk\' with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:0.2]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp;

end #foreach normalized

# poisson white
echo plot data/poisson_white_${dimension}d_${num_classes}c_${r_text}_mean.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_white_${dimension}d_${num_classes}c_${r_text}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat\" title \'Poisson disk\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

endif

if(1) then

foreach distribution ('poisson' 'discreterelax' 'discreterandom') # 'poisson' must proceed all others

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set ids = 0;

set target_num_samples = 2600;

set k_value = 100;

set r_values = "0.04 0.01511858";
set r_text = 0p04m0p02osqrt2;

set num_bins = 800;
set num_runs = 1280;

set hist_freq = 1; # 50 75 100 125), the rest not seem useful at all

if(${distribution} == 'poisson') then
 set distribution_title = 'Poisson disk';
else if(${distribution} == 'discreterelax') then
 set distribution_title = 'discrete relaxation';
else if(${distribution} == 'discreterandom') then
 set distribution_title = 'discrete randomization';
else
echo unknown distribution;
exit(1);
endif

# radial plots

if(1) then

set all_pfm_files_count = 10;

foreach id (0 1)

if(${id} == 0) then
set num_samples = 336.6;
set num_samples = 43.5767538; # 336.6^2/2600;
else
set num_samples = 2263.4;
set num_samples = 1970.37675; # 2263.4^2/2600
endif


#echo data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_mean
./txt2dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_variance.txt ${num_samples} data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_mean.dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_variance.dat 1

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_mean.dat\" title \'${distribution_title}\' with lines linetype rgb \"red\" >> foo.gp
#, \"data/white_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat\"  title \'uniform random\' with lines linetype rgb \"blue\"
wgnuplot foo.gp
rm foo.gp;

end # foreach id

endif

# plot histogram, old
if(0) then

foreach id (0 1)
echo plot data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_histogram_freq${hist_freq}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_histogram_freq${hist_freq}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
echo set ylabel \"density\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
echo plot "[t=0:0.2]" "[1.55:2.05]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_histogram.txt\" title \'${distribution_title}\' with lines linetype rgb \"red\", "2*(1-t)" title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp
#, \"data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\"
wgnuplot foo.gp
rm foo.gp;
end

endif

# plot histogram
if(1) then

foreach which_dimension_text ('0' '01')

if(${which_dimension_text} == '0') then
set x_max = 1;
set y_min = 0.75;
set y_max = 1.25;
set y_min = 0;
set y_max = 2.5;
else
set x_max = 1.41245;
set y_min = 0;
set y_max = 2.5;
endif

foreach id (0 1)

foreach normalization (0 1)

if(${normalization}) then
set histogram = 'histogram_normalized';
./normalize_hist data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram_normalized.txt
else
set histogram = 'histogram';
endif

echo plot data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}_freq${hist_freq}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}_freq${hist_freq}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalization}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
if(${normalization}) then
echo plot "[0:${x_max}]" "[${y_min}:${y_max}]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'${distribution_title}\' with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:${x_max}]" "[0:2.5]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'${distribution_title}\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'uniform random\' with lines linetype rgb \"\#009900\" >> foo.gp
endif
wgnuplot foo.gp
rm foo.gp;

end # foreach normalization
end # foreach id
end # foreach which_dimension_text

endif

end #foreach distribution

endif

# plot 2D histogram, old
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 1;

set r_text = 0p02

set which_dimension = "0 1";
set which_dimension_text = "01";

set hist_freq = 1;

foreach normalized (0 1)

if(${normalized}) then
set output_name = ${outputdir}/poisson_white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram_normalized_freq${hist_freq};

./normalize_hist ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt ${outputdir}/white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram_normalized.txt
else
set output_name = ${outputdir}/poisson_white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram_freq${hist_freq};
endif

echo plot ${output_name}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalized}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
if(${normalized}) then
echo plot "[0:1.41245] [0:2.5]" \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram_normalized.txt\" title \'Poisson disk\' with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:1.41245] [0:2.5]" \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", \"${outputdir}/white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp;
end # foreach normalized

endif
