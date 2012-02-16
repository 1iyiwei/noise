#!/bin/tcsh -f

if(1) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";

set r_value = 04;

set num_runs = 10;
set num_pairs = 52003;
@ total_num_pairs = ${num_pairs} * ${num_runs};

# for DFT
set dft_freq = 128;
@ dft_size = ${dft_freq} * 2;
set dft_scale = ${dft_size};

# for histogram
set which_dimension = "0 1";
set hist_freq = 1;
set num_bins = 800;

# ground truth
set k_value = 0.67;

set num_classes = 1;
set priority = "0";
set r_values = 0.${r_value};
set r_text = 0p${r_value};

set exe_name = DartThrowing;
set output_name = grid;

set ground_truth_name = ${output_name}_${dimension}d_${num_classes}c_${r_text};
set ground_truth_pair_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${num_pairs};

foreach r_variance (004 4 8)

# diff generator

set option = "hybrid";

set diff_synth_pair_name_all = "";

foreach length_sign ('-' '+')
foreach variance_sign ('-' '+')

set diff_synth_pair_name = diff_${option}_${dimension}d_${length_sign}0p${r_value}_${variance_sign}0p${r_variance}_${num_pairs}

set diff_synth_pair_name_all = "${diff_synth_pair_name_all} ${diff_synth_pair_name}";

end # foreach variance_sign
end # foreach length_sign

# comparison
foreach diff_file_base_name (${ground_truth_pair_name} ${diff_synth_pair_name_all})

./txt2dat ${outputdir}/${diff_file_base_name}_mean.txt ${outputdir}/${diff_file_base_name}_variance.txt ${total_num_pairs} ${outputdir}/${diff_file_base_name}_mean.dat ${outputdir}/${diff_file_base_name}_variance.dat 1

end # foreach diff_file_base_name

# plot stuff
foreach diff_synth_pair_name (${diff_synth_pair_name_all})

set output_base_name = truth_${diff_synth_pair_name};

# radial plot
set output_name = ${output_base_name}_mean;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${dft_freq}] [0.995:1.005]" \"${outputdir}/${diff_synth_pair_name}_mean.dat\" title \'synth\' with line linetype rgb \"red\", \"${outputdir}/${ground_truth_pair_name}_mean.dat\" title \'truth\' with line linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

echo ${outputdir}/${output_name}.pdf;

# histogram plot
set output_name = ${output_base_name}_histogram;

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
echo set ylabel \"density\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
echo plot "[0:1.41245] [0:3.5]" \"${outputdir}/${diff_synth_pair_name}_histogram.txt\" title \'synth\' with lines linetype rgb \"red\", \"${outputdir}/${ground_truth_pair_name}_histogram.txt\" title \'truth\' with lines linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

echo ${outputdir}/${output_name}.pdf;

end # foreach diff_synth_pair_name 

end # foreach r_variance

endif
