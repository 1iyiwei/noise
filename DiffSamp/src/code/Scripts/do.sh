#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set output_dir = data;
set huge_dir = data/huge;

# 2D dart throwing
if(0) then

set dimension = 2;

set exe_name = DartThrowing;
set output_name = grid;

if(0) then
set exe_name = WhiteNoiseSamples;
set output_name = white;
endif

set num_class = 1;
set priority = "0";

set r_values = "0.04";
set r_text = 0p04;

set k_value = 0.75;

if(${exe_name} == WhiteNoiseSamples) then
set num_samples = 405; # derived from dart throwing result
endif

set domain_size = "1 1";

set num_runs = 10;

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# sample set generation
if(${exe_name} == DartThrowing) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $priority $r_values $k_value $domain_size > ${huge_dir}/${base_name}_${i}.txt
else if(${exe_name} == WhiteNoiseSamples) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $num_samples $domain_size > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown exe name;
exit(1);
endif

sleep 1;

# compute sft
${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${base_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

# num samples
set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_samples};
else if(${normalization_option} == 'sqrtsamplecount') then
set hist_normalization_const = 1;
else
echo unknown normalization option;
exit(1);
endif

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:1.8]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-20:20]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 1
echo ${output_dir}/${base_name}_average${num_runs}.ppm

endif

# synthetic PFT
if(0) then

set dimension = 2;
set domain_size = "1 1";
set domain_size_string = "1x1";

set num_samples = 405;
set num_runs = "infty";

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach domain_option ("uniform" "1blob")

if(${domain_option} == "uniform") then

set domain_res = 1x1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else if(${domain_option} == "debug") then

set domain_res = 2x1;
echo "F2 2 2 1 1 3" > ${output_dir}/${domain_option}.f2m;

else

echo unknown domain option;
exit(1);

endif

set base_name = ${domain_option}_${domain_res};

./src/Main/Release/SyntheticPFT ${num_samples} ${output_dir}/${domain_option}.f2m ${domain_size_string} ${output_dir}/${base_name}_average${num_runs}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm
end #foreach domain_option

endif

# visualize 2D adaptive white noise
if(0) then

set dimension = 2;

set num_samples = 10000; 
set dot_radius = 0.002;

set domain_size = "1 1";

foreach domain_option ("uniform" "1blob")

if(${domain_option} == "uniform") then

set domain_res = 1x1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

set base_name = ${domain_option}_${dimension}d_white;

# sample set generation
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m $num_samples $domain_size > ${output_dir}/${base_name}_samples.txt

# draw samples
${rainbow_dir}/src/Main/Release/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf

end #foreach domain_option

endif

# 2D adaptive white noise
if(0) then

set dimension = 2;

set exe_name = IsotropicWhiteNoise;

set num_samples = 405; 

set domain_size = "1 1";

set num_runs = 10;

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach domain_option ("uniform" "1blob")

if(${domain_option} == "uniform") then

set domain_res = 1x1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

set base_name = ${domain_option}_${dimension}d_white;

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# sample set generation
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m $num_samples $domain_size > ${huge_dir}/${base_name}_${i}.txt

sleep 1;

# compute sft
${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${base_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

# num samples
set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_samples};
else if(${normalization_option} == 'sqrtsamplecount') then
set hist_normalization_const = 1;
else
echo unknown normalization option;
exit(1);
endif

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:1.8]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-20:20]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm

end #foreach domain_option

endif

# uniform white noise
if(0) then

set dimension = 2;

set domain_size = "1 1";
set num_samples = 400;
set sub_domain_scale = 0.05;

set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

set base_name = uniform_white;

# generate samples
set i = 0;
while(${i} < ${num_runs})

./src/Main/Release/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt

sleep 1
@ i = ${i} + 1;
end

# spectrum
set all_pfm_files = "";
set i = 0;
while (${i} < ${num_runs})

# compute sft
${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_samples};
else if(${normalization_option} == 'sqrtsamplecount') then
set hist_normalization_const = 1;
else
echo unknown normalization option;
exit(1);
endif

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:1.8]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-20:20]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm

endif

# uniform diff transform
if(0) then

set dimension = 2;

set num_samples = 256; # white noise
set sample_rate_1d = 16;
set sample_rate = "${sample_rate_1d} ${sample_rate_1d}"; # regular grid

set domain_size = "1 1";

set sub_domain_min = "0 0";
set sub_domain_max = "1 1";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;

set num_runs = 10;

foreach method ("white" "grid" "hex")

set base_name = ${method}_diffxform;

# generate samples
set all_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then
./src/Main/Release/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "grid") then
./src/Main/Release/RegularGrid ${dimension} ${domain_size} ${sample_rate} 0 > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "hex") then
./src/Main/Release/HexagonalGrid ${dimension} ${domain_size} ${sample_rate_1d} > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown sampling method;
exit(1);
endif

sleep 1;

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

@ i = ${i} + 1;
end

# diff transform
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial mean (todo)

end # foreach method

endif

# convert Balzer plant image
if(0) then

set name = "plant"; #991x932

convert -size 1024x1024 xc:none -fill white -draw "rectangle 1, 1, 1024, 1024" ${output_dir}/background.png;

composite -gravity center ${output_dir}/${name}.png ${output_dir}/background.png ${output_dir}/${name}_square.png

convert ${output_dir}/${name}_square.png -negate -compress none -resize 128x128 ${output_dir}/${name}.pgm
endif
