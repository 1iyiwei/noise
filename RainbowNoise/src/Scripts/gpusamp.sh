#!/bin/tcsh -f

set output_dir = data/balzer;

# 10 runs from Diego to verify spectrum quality
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;
set sft_half_size = 256;
set sft_scale = ${sft_size};

set num_samples = 1019;
set root_name = balzer-1019s-2p18p;

set all_pfm_files = "";

# power spectrums
set i = 0;
while(${i} < ${num_runs})

./src/Sain/Release/SFT ${output_dir}/${root_name}_${i}.txt ${output_dir}/${root_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${output_dir}/${root_name}_${i}.pfm";

@ i = ${i} + 1;
end

set base_name = ${root_name}_average${num_runs};

# radial summary
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Sain/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}.pfm ${output_dir}/${base_name}_mean.txt ${output_dir}/${base_name}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# spectrum image
./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}.pfm ${output_dir}/${base_name}.ppm 1 1 1

# spectrum plots
./src/Scripts/txt2dat ${output_dir}/${base_name}_mean.txt ${output_dir}/${base_name}_variance.txt ${num_samples} ${output_dir}/${base_name}_mean.dat ${output_dir}/${base_name}_variance.dat 1

echo set terminal postscript eps >> ${output_dir}/foo.gp
echo set output \'${output_dir}/${base_name}_mean.eps\' >> ${output_dir}/foo.gp
echo set size 0.5, 0.675 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"frequency\" >> ${output_dir}/foo.gp
echo set ylabel \"power\" >> ${output_dir}/foo.gp
echo set key off >> ${output_dir}/foo.gp

echo plot "[1:${sft_half_size}] [0:3.0]" \"${output_dir}/${base_name}_mean.dat\" with line linetype rgb \"red\" >> ${output_dir}/foo.gp

echo "" >> ${output_dir}/foo.gp

echo set terminal postscript eps >> ${output_dir}/foo.gp
echo set output \'${output_dir}/${base_name}_variance.eps\' >> ${output_dir}/foo.gp
echo set size 0.5, 0.675 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"frequency\" >> ${output_dir}/foo.gp
echo set ylabel \"anisotrophy\" >> ${output_dir}/foo.gp
echo set key off >> ${output_dir}/foo.gp
echo plot "[1:${sft_half_size}] [-20:20]" \"${output_dir}/${base_name}_variance.dat\" with line linetype rgb \"blue\" >> ${output_dir}/foo.gp

echo "" >> ${output_dir}/foo.gp

wgnuplot ${output_dir}/foo.gp
rm ${output_dir}/foo.gp

foreach type (mean variance)
epstopdf ${output_dir}/${base_name}_${type}.eps
cp ${output_dir}/${base_name}_${type}.pdf ${output_dir}/foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/foo.pdf ${output_dir}/${base_name}_${type}.pdf
rm -f ${output_dir}/foo.pdf
end

endif
