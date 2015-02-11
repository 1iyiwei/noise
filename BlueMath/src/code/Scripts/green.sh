#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set output_dir = data;
set huge_dir = data/huge;

# first green noise
if(0) then

set dimension = 2;
set domain_size = "1 1";

# blue noise parameters
set blue_domain_option = "uniform";
set r_value = 0.10;
set r_value_half = 0.05;
set r_text = 0p10;
set k_value = 0.70;
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 40;

# white noise parameters
set white_domain_option = "1blob";
set white_domain_size_1d = ${r_value};
set white_domain_size_half_1d = ${r_value_half};
set white_domain_size = "${white_domain_size_1d} ${white_domain_size_1d}";
set white_domain_center_shift = "-${white_domain_size_half_1d} -${white_domain_size_half_1d}";
set num_white_samples = 30;
set white_domain_res_1d = 64;

# other parameters
set dot_radius = 0.005;

set blue_base_name = poisson_${blue_domain_option}_${r_text}_b${boundary};
set white_base_name = white_${white_domain_option}_${num_white_samples};

set green_base_name = ${blue_base_name}_${white_base_name};

if(${white_domain_option} == "1blob") then
echo "1 2 1 0.5 0.5 0.2 0.2" > ${output_dir}/${white_domain_option}.txt

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${white_domain_option}.txt ${output_dir}/${white_domain_option}.pfm ${white_domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2PPM ${output_dir}/${white_domain_option}.pfm ${output_dir}/${white_domain_option}.ppm 1 1 2

./src/Main/Release/PFM2F2M ${output_dir}/${white_domain_option}.pfm ${output_dir}/${white_domain_option}.f2m
else
echo unknown white domain option;
endif

# dart throwing for blue noise
./src/Main/Release/DartThrowing ${blue_domain_option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${blue_base_name}_samples.txt;

# num samples
set blue_samples = `./src/Main/Release/SampleSelector ${output_dir}/${blue_base_name}_samples.txt ${dimension} cout 0`;
@ num_blue_samples = $#blue_samples / ${dimension};

echo ${num_blue_samples}

# green noise
set which_blue_sample = 0;
while(${which_blue_sample} < ${num_blue_samples})

# white noise
sleep 1;
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${white_domain_option}.f2m ${num_white_samples} ${white_domain_size} > ${output_dir}/${white_base_name}_samples.txt

# build blue sample center
set center = "";
set j = 0;
while (${j} < ${dimension})
@ k = ${which_blue_sample} * ${dimension} + ${j} + 1;

set center = "${center} $blue_samples[${k}]";

@ j = ${j} + 1;
end

./src/Main/Release/ShiftSamples ${output_dir}/${white_base_name}_samples.txt ${dimension} ${center} > ${output_dir}/${white_base_name}_shift.txt

./src/Main/Release/ShiftSamples ${output_dir}/${white_base_name}_shift.txt ${dimension} ${white_domain_center_shift} > ${output_dir}/${white_base_name}_samples.txt

if(${which_blue_sample} == 0) then
cp ${output_dir}/${white_base_name}_samples.txt ${output_dir}/${green_base_name}_samples.txt 
else
cat ${output_dir}/${white_base_name}_samples.txt >> ${output_dir}/${green_base_name}_samples.txt
endif

@ which_blue_sample = ${which_blue_sample} + 1;
end

# wrapping around
cp ${output_dir}/${green_base_name}_samples.txt ${output_dir}/${green_base_name}_shift.txt
./src/Main/Release/AdoptSamples ${output_dir}/${green_base_name}_shift.txt ${dimension} "toroidal" ${domain_size} > ${output_dir}/${green_base_name}_samples.txt;

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
./src/Main/Release/DrawSamples ${output_dir}/${green_base_name}_samples.txt ${output_dir}/${green_base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${output_dir}/${green_base_name}_samples.fig
epstopdf ${output_dir}/${green_base_name}_samples.eps --outfile ${output_dir}/${green_base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${green_base_name}_samples.pdf;

endif

# spectrum analysis for green noise
if(0) then

set generate_samples = 1;

set dimension = 2;
set domain_size = "1 1";

# blue noise parameters
set blue_domain_option = "uniform";
set r_value = 0.10;
set r_value_half = 0.05;
set r_text = 0p10;
set k_value = 0.70;
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 40;

# white noise parameters
set white_domain_option = "1blob";
set white_domain_size_1d = ${r_value};
set white_domain_size_half_1d = ${r_value_half};
set white_domain_size = "${white_domain_size_1d} ${white_domain_size_1d}";
set white_domain_center_shift = "-${white_domain_size_half_1d} -${white_domain_size_half_1d}";
set num_white_samples = 30;
set white_domain_res_1d = 64;

# spectrum parameters
set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

# other parameters
set num_runs = 10;
set dot_radius = 0.005;

set blue_base_name = poisson_${blue_domain_option}_${r_text}_b${boundary};
set white_base_name = white_${white_domain_option}_${num_white_samples};

set green_base_name = ${blue_base_name}_${white_base_name};

if(${white_domain_option} == "1blob") then
echo "1 2 1 0.5 0.5 0.2 0.2" > ${output_dir}/${white_domain_option}.txt

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${white_domain_option}.txt ${output_dir}/${white_domain_option}.pfm ${white_domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2PPM ${output_dir}/${white_domain_option}.pfm ${output_dir}/${white_domain_option}.ppm 1 1 2

./src/Main/Release/PFM2F2M ${output_dir}/${white_domain_option}.pfm ${output_dir}/${white_domain_option}.f2m
else
echo unknown white domain option;
endif

# start
set green_pfm_files = "";
set blue_pfm_files = "";
set white_pfm_files = "";
set which_run = 0;
while(${which_run} < ${num_runs})

if(${generate_samples}) then

# dart throwing for blue noise
./src/Main/Release/DartThrowing ${blue_domain_option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${blue_base_name}_${which_run}.txt;

# num samples
set blue_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${blue_base_name}_${which_run}.txt ${dimension} cout 0`;
@ num_blue_samples = $#blue_samples / ${dimension};

# green noise
set which_blue_sample = 0;
while(${which_blue_sample} < ${num_blue_samples})

# white noise
sleep 1;
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${white_domain_option}.f2m ${num_white_samples} ${white_domain_size} > ${huge_dir}/${white_base_name}_${which_run}.txt

# build blue sample center
set center = "";
set j = 0;
while (${j} < ${dimension})
@ k = ${which_blue_sample} * ${dimension} + ${j} + 1;

set center = "${center} $blue_samples[${k}]";

@ j = ${j} + 1;
end

# using which_run instead of which_blue_sample for white noise intentionally for spectrum computation below
./src/Main/Release/ShiftSamples ${huge_dir}/${white_base_name}_${which_run}.txt ${dimension} ${center} > ${huge_dir}/${white_base_name}_shift.txt

./src/Main/Release/ShiftSamples ${huge_dir}/${white_base_name}_shift.txt ${dimension} ${white_domain_center_shift} > ${huge_dir}/${white_base_name}_${which_run}.txt

if(${which_blue_sample} == 0) then
cp ${huge_dir}/${white_base_name}_${which_run}.txt ${huge_dir}/${green_base_name}_${which_run}.txt 
else
cat ${huge_dir}/${white_base_name}_${which_run}.txt >> ${huge_dir}/${green_base_name}_${which_run}.txt
endif

@ which_blue_sample = ${which_blue_sample} + 1;
end

# wrapping around
cp ${huge_dir}/${green_base_name}_${which_run}.txt ${huge_dir}/${green_base_name}_shift.txt
./src/Main/Release/AdoptSamples ${huge_dir}/${green_base_name}_shift.txt ${dimension} "toroidal" ${domain_size} > ${huge_dir}/${green_base_name}_${which_run}.txt;

endif #if(${generate_samples}) 

# compute sft
foreach base_name (${green_base_name} ${blue_base_name} ${white_base_name})

# wrapping around
if(${base_name} == ${white_base_name}) then
cp ${huge_dir}/${base_name}_${which_run}.txt ${huge_dir}/${base_name}_shift.txt
./src/Main/Release/AdoptSamples ${huge_dir}/${base_name}_shift.txt ${dimension} "toroidal" ${domain_size} > ${huge_dir}/${base_name}_${which_run}.txt;
endif

# draw samples
if(${which_run} == 0) then
echo "0 0 0" > ${huge_dir}/palette.txt
./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${which_run}.txt ${huge_dir}/${base_name}_${which_run}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${which_run}.fig
epstopdf ${huge_dir}/${base_name}_${which_run}.eps --outfile ${huge_dir}/${base_name}_${which_run}.pdf
rm ${huge_dir}/palette.txt

echo ${huge_dir}/${base_name}_${which_run}.pdf;
endif

# pft
./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${which_run}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${which_run}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${which_run}_selected.txt ${huge_dir}/${base_name}_${which_run}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

if(${base_name} == ${green_base_name}) then
set green_pfm_files = "${green_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else if(${base_name} == ${blue_base_name}) then
set blue_pfm_files = "${blue_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else if(${base_name} == ${white_base_name}) then
set white_pfm_files = "${white_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else
echo unknown base_name;
exit(1);
endif

end # foreach base_name

@ which_run = ${which_run} + 1;
end

# radial computation
foreach base_name (${green_base_name} ${blue_base_name} ${white_base_name})

if(${base_name} == ${green_base_name}) then
echo ${green_pfm_files} > ${output_dir}/pfm_files_list.txt;
else if(${base_name} == ${blue_base_name}) then
echo ${blue_pfm_files} > ${output_dir}/pfm_files_list.txt;
else if(${base_name} == ${white_base_name}) then
echo ${white_pfm_files} > ${output_dir}/pfm_files_list.txt;
else
echo unknown base_name;
exit(1);
endif

./src/Main/Release/RadialPlot ${output_dir}/pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set x_min = 1;
set x_max = ${sft_freq};
echo plot "[${x_min}:${x_max}]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach base_name

endif

# Gaussian blue noise
if(0) then

set dimension = 2;
set domain_size = "1 1";

# blue noise parameters
set blue_domain_option = "1blob";
set blue_domain_res_1d = 64;
set blue_domain_cell_size = 0.015625; # 1.0/64;

set r_value = 0.02;
set r_text = 0p02;
set k_value = 0.70;
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 40;

# other parameters
set dot_radius = 0.005;

set blob_std = 0.3;
set blob_std_text = 0p3;

set blue_base_name = poisson_${blue_domain_option}_${blob_std_text}_${r_text}_b${boundary};

if(${blue_domain_option} == "1blob") then
echo "1 2 1 0.5 0.5 ${blob_std} ${blob_std}" > ${output_dir}/${blue_domain_option}.txt

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${blue_domain_option}.txt ${output_dir}/${blue_domain_option}.pfm ${blue_domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2PPM ${output_dir}/${blue_domain_option}.pfm ${output_dir}/${blue_domain_option}.ppm 1 1 2

./src/Main/Release/PFM2F2M ${output_dir}/${blue_domain_option}.pfm ${output_dir}/${blue_domain_option}.f2m
else
echo unknown blue domain option;
endif

# dart throwing for blue noise
./src/Main/Release/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${blue_domain_option}.f2m ${blue_domain_cell_size} > ${output_dir}/${blue_base_name}_samples.txt;

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
./src/Main/Release/DrawSamples ${output_dir}/${blue_base_name}_samples.txt ${output_dir}/${blue_base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${output_dir}/${blue_base_name}_samples.fig
epstopdf ${output_dir}/${blue_base_name}_samples.eps --outfile ${output_dir}/${blue_base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${blue_base_name}_samples.pdf;

endif

# Gaussian blue noise spectrum analysis
if(0) then

set dimension = 2;
set domain_size = "1 1";

# blue noise parameters
set blue_domain_option = "1blob";
set blue_domain_res_1d = 64;
set blue_domain_cell_size = 0.015625; # 1.0/64;

set r_value = 0.02;
set r_value_half = 0.01;
set r_text = 0p02;
set k_value = 0.70;
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 40;

# spectrum parameters
set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

# other parameters
set dot_radius = 0.005;

set num_runs = 10;

foreach blob_std (3)

set blob_std_value = 0.${blob_std};
set blob_std_text = 0p${blob_std};

set blue_base_name = poisson_${blue_domain_option}_${blob_std_text}_${r_text}_b${boundary};

if(${blue_domain_option} == "1blob") then
echo "1 2 1 0.5 0.5 ${blob_std_value} ${blob_std_value}" > ${output_dir}/${blue_domain_option}.txt

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${blue_domain_option}.txt ${output_dir}/${blue_domain_option}.pfm ${blue_domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2PPM ${output_dir}/${blue_domain_option}.pfm ${output_dir}/${blue_domain_option}.ppm 1 1 2

./src/Main/Release/PFM2F2M ${output_dir}/${blue_domain_option}.pfm ${output_dir}/${blue_domain_option}.f2m
else
echo unknown blue domain option;
endif

# synthesis and analysis
set which_run = 0;
set blue_pfm_files = "";
while(${which_run} < ${num_runs})

# dart throwing for blue noise
./src/Main/Release/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${blue_domain_option}.f2m ${blue_domain_cell_size} > ${huge_dir}/${blue_base_name}_${which_run}.txt;

# draw samples
if(${which_run} == 0) then
echo "0 0 0" > ${output_dir}/palette.txt
./src/Main/Release/DrawSamples ${huge_dir}/${blue_base_name}_${which_run}.txt ${huge_dir}/${blue_base_name}_${which_run}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${blue_base_name}_${which_run}.fig
epstopdf ${huge_dir}/${blue_base_name}_${which_run}.eps --outfile ${output_dir}/${blue_base_name}_${which_run}.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${blue_base_name}_${which_run}.pdf;
endif

# pft
./src/Main/Release/SampleSelector ${huge_dir}/${blue_base_name}_${which_run}.txt ${dimension} "cout" 0 > ${huge_dir}/${blue_base_name}_${which_run}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${blue_base_name}_${which_run}_selected.txt ${huge_dir}/${blue_base_name}_${which_run}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set blue_pfm_files = "${blue_pfm_files} ${huge_dir}/${blue_base_name}_${which_run}.pfm";

@ which_run = ${which_run} + 1;
end #while(${which_run})

# radial computation
foreach base_name (${blue_base_name})

if(${base_name} == ${blue_base_name}) then
echo ${blue_pfm_files} > ${output_dir}/blue_pfm_files_list.txt
else
echo unknown base_name;
exit(1);
endif

./src/Main/Release/RadialPlot ${output_dir}/blue_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/blue_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set x_min = 1;
set x_max = ${sft_freq};
echo plot "[${x_min}:${x_max}]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach base_name

end # foreach blob_std

endif

# spectrum analysis for green noise
# with flexible spec of distribution for each part (kernel and ground)
if(0) then

set generate_samples = 1;

set dimension = 2;
set domain_size = "1 1";

# ground noise parameters
set ground_domain_option = "uniform";
#set ground_method = "poisson";
#set ground_method = "white";
# ground blue parameters
set ground_r_value = 0.12;
set ground_r_value_half = 0.06;
set ground_r_text = 0p12;
# ground/kernel blue parameters
set k_value = 100.67;
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 40;
# ground white parameters
set num_ground_samples = 35; # match up ground_r_value and k_value above

# kernel noise parameters
set kernel_domain_option = "1blob"; 
set blob_std = 3;
set blob_std_value = 0.${blob_std};
set blob_std_text = 0p${blob_std};
#set kernel_method = "poisson";
#set kernel_method = "white";
set kernel_domain_size_1d = ${ground_r_value};
set kernel_domain_size_half_1d = ${ground_r_value_half};
set kernel_domain_size = "${kernel_domain_size_1d} ${kernel_domain_size_1d}";
set kernel_domain_center_shift = "-${kernel_domain_size_half_1d} -${kernel_domain_size_half_1d}";
# kernel blue parameters
set kernel_r_value = 0.12;
set kernel_r_value_half = 0.06;
set kernel_r_text = 0p12;
# kernle white parameters
set num_kernel_samples = 35;
# for blob
set kernel_domain_res_1d = 64;
set kernel_domain_cell_size = 0.015625; #1.0/64;

# spectrum parameters
set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

# other parameters
set num_runs = 10;
set dot_radius = 0.005;

foreach option (1 2)

if(${option} == 1) then
set ground_method = "poisson";
set kernel_method = "white";
else
set ground_method = "white";
set kernel_method = "poisson";
endif

if(${ground_method} == "poisson") then
set ground_base_name = ${ground_method}_${ground_domain_option}_${ground_r_text}_b${boundary};
else
set ground_base_name = ${ground_method}_${ground_domain_option}_${num_ground_samples};
endif

if(${kernel_method} == "poisson") then
set kernel_base_name = ${kernel_method}_${kernel_domain_option}${blob_std_text}_${kernel_r_text}_b${boundary};
else
set kernel_base_name = ${kernel_method}_${kernel_domain_option}${blob_std_text}_${num_kernel_samples};
endif

set green_base_name = ${ground_base_name}_${kernel_base_name};

if(${kernel_domain_option} == "1blob") then
echo "1 2 1 0.5 0.5 ${blob_std_value} ${blob_std_value}" > ${output_dir}/${kernel_domain_option}.txt

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${kernel_domain_option}.txt ${output_dir}/${kernel_domain_option}.pfm ${kernel_domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2PPM ${output_dir}/${kernel_domain_option}.pfm ${output_dir}/${kernel_domain_option}.ppm 1 1 2

./src/Main/Release/PFM2F2M ${output_dir}/${kernel_domain_option}.pfm ${output_dir}/${kernel_domain_option}.f2m
else
echo unknown kernel domain option;
endif

# start
set green_pfm_files = "";
set ground_pfm_files = "";
set kernel_pfm_files = "";
set which_run = 0;
while(${which_run} < ${num_runs})

if(${generate_samples}) then

# ground noise
if(${ground_method} == "poisson") then
./src/Main/Release/DartThrowing ${ground_domain_option} ${dimension} ${ground_r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${ground_base_name}_${which_run}.txt;
else if(${ground_method} == "white") then
./src/Main/Release/UniformWhiteNoise ${dimension} ${num_ground_samples} ${domain_size} > ${huge_dir}/${ground_base_name}_${which_run}.txt
else
echo unknown ground method;
exit(1);
endif

# num samples
set ground_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${ground_base_name}_${which_run}.txt ${dimension} cout 0`;
@ num_ground_samples = $#ground_samples / ${dimension};

# green noise
set which_ground_sample = 0;
while(${which_ground_sample} < ${num_ground_samples})

# kernel noise
sleep 1;
if(${kernel_method} == "poisson") then
./src/Main/Release/DartThrowing "isotropic" ${dimension} ${kernel_r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${kernel_domain_option}.f2m ${kernel_domain_cell_size} > ${huge_dir}/${kernel_base_name}_${which_run}.txt;
else if(${kernel_method} == "white") then
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${kernel_domain_option}.f2m ${num_kernel_samples} ${domain_size} > ${huge_dir}/${kernel_base_name}_${which_run}.txt
else
echo unknown ground method;
exit(1);
endif

# build ground sample center
set center = "";
set j = 0;
while (${j} < ${dimension})
@ k = ${which_ground_sample} * ${dimension} + ${j} + 1;

set center = "${center} $ground_samples[${k}]";

@ j = ${j} + 1;
end

# using which_run instead of which_ground_sample for kernel noise intentionally for spectrum computation below

./src/Main/Release/ScaleSamples ${huge_dir}/${kernel_base_name}_${which_run}.txt ${dimension} ${kernel_domain_size_1d} > ${huge_dir}/${kernel_base_name}_${which_run}_scale.txt

./src/Main/Release/ShiftSamples ${huge_dir}/${kernel_base_name}_${which_run}_scale.txt ${dimension} ${center} > ${huge_dir}/${kernel_base_name}_shift.txt

./src/Main/Release/ShiftSamples ${huge_dir}/${kernel_base_name}_shift.txt ${dimension} ${kernel_domain_center_shift} > ${huge_dir}/${kernel_base_name}_${which_run}.txt

if(${which_ground_sample} == 0) then
cp ${huge_dir}/${kernel_base_name}_${which_run}.txt ${huge_dir}/${green_base_name}_${which_run}.txt 
else
cat ${huge_dir}/${kernel_base_name}_${which_run}.txt >> ${huge_dir}/${green_base_name}_${which_run}.txt
endif

@ which_ground_sample = ${which_ground_sample} + 1;
end

# wrapping around
cp ${huge_dir}/${green_base_name}_${which_run}.txt ${huge_dir}/${green_base_name}_shift.txt
./src/Main/Release/AdoptSamples ${huge_dir}/${green_base_name}_shift.txt ${dimension} "toroidal" ${domain_size} > ${huge_dir}/${green_base_name}_${which_run}.txt;

endif #if(${generate_samples}) 

# compute sft
foreach base_name (${green_base_name} ${ground_base_name} ${kernel_base_name})

# wrapping around
if(${base_name} == ${kernel_base_name}) then
cp ${huge_dir}/${base_name}_${which_run}.txt ${huge_dir}/${base_name}_shift.txt;
./src/Main/Release/AdoptSamples ${huge_dir}/${base_name}_shift.txt ${dimension} "toroidal" ${domain_size} > ${huge_dir}/${base_name}_${which_run}.txt;
endif

# draw samples
if(${which_run} == 0) then
echo "0 0 0" > ${huge_dir}/palette.txt
./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${which_run}.txt ${huge_dir}/${base_name}_${which_run}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${which_run}.fig
epstopdf ${huge_dir}/${base_name}_${which_run}.eps --outfile ${huge_dir}/${base_name}_${which_run}.pdf
rm ${huge_dir}/palette.txt

echo ${huge_dir}/${base_name}_${which_run}.pdf;
endif

# pft
./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${which_run}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${which_run}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${which_run}_selected.txt ${huge_dir}/${base_name}_${which_run}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

if(${base_name} == ${green_base_name}) then
set green_pfm_files = "${green_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else if(${base_name} == ${ground_base_name}) then
set ground_pfm_files = "${ground_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else if(${base_name} == ${kernel_base_name}) then
set kernel_pfm_files = "${kernel_pfm_files} ${huge_dir}/${base_name}_${which_run}.pfm";
else
echo unknown base_name;
exit(1);
endif

end # foreach base_name

@ which_run = ${which_run} + 1;
end

# radial computation
foreach base_name (${green_base_name} ${ground_base_name} ${kernel_base_name})

if(${base_name} == ${green_base_name}) then
echo ${green_pfm_files} > ${output_dir}/pfm_files_list.txt;
else if(${base_name} == ${ground_base_name}) then
echo ${ground_pfm_files} > ${output_dir}/pfm_files_list.txt;
else if(${base_name} == ${kernel_base_name}) then
echo ${kernel_pfm_files} > ${output_dir}/pfm_files_list.txt;
else
echo unknown base_name;
exit(1);
endif

./src/Main/Release/RadialPlot ${output_dir}/pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set x_min = 1;
set x_max = ${sft_freq};
echo plot "[${x_min}:${x_max}]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach base_name

end # foreach option

endif
