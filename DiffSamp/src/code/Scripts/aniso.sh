#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set output_dir = data;
set huge_dir = data/huge;

# matrix inversion
if(0) then

foreach warp_option ("identity" "scale" "shear" "perspective")

if(${warp_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${warp_option}.f2m;
else
echo unknown warp option
exit(1);
endif

./src/Main/Release/Matrix "invert" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m

./src/Main/Release/Matrix "multiply" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m ${output_dir}/${warp_option}_identity.f2m

echo "original" 
cat ${output_dir}/${warp_option}.f2m

echo "inverse" 
cat ${output_dir}/${warp_option}_inverse.f2m

echo "identity" 
cat ${output_dir}/${warp_option}_identity.f2m

end # foreach warp_option

endif

# uniform/isotropic/anisotropic dart throwing
if(0) then

set dimension = 2;
set domain_size = "1 1";

set r_value = 0.02;
set r_text = 0p02;
set k_value = 0.70;

set sample_grid_resolution_per_unit_domain = 40;

set warp_cell_size = 0.01;
set aniso_white_noise_radius = 0.01;

set dot_radius = 0.005;

foreach domain_option ("uniform" "1blob" "identity" "scale" "shear" "perspective" "terrain")

if(${domain_option} == "uniform") then

# do nothing

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};
set importance_cell_size = 0.03125; #1/32

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else if(${domain_option} == "identity") then

echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "scale") then

echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "shear") then

echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "perspective") then

echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "terrain") then

# do nothing

else

echo unknown domain option;
exit(1);

endif

if(${domain_option} == "terrain") then

./src/Main/Release/Warp2JacobianField ${domain_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

else if((${domain_option} != "uniform") && (${domain_option} != "1blob")) then

./src/Main/Release/Warp2JacobianField ${output_dir}/${domain_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

endif

foreach boundary (0 1)

set base_name = poisson_${domain_option}_${r_text}_b${boundary};

if(${domain_option} == "uniform") then

./src/Main/Release/DartThrowing ${domain_option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${base_name}_samples.txt;

else if(${domain_option} == "1blob") then

./src/Main/Release/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${importance_cell_size} > ${output_dir}/${base_name}_samples.txt;

else

./src/Main/Release/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${warp_cell_size} ${aniso_white_noise_radius} > ${output_dir}/${base_name}_samples.txt;
endif

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
${rainbow_dir}/src/Main/Release/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${base_name}_samples.pdf;

end # foreach boundary
end # foreach domain_option

endif

# anisotropic noise based on warp
# plus inverse warp and clip
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_samples = 4000;

set dot_radius = 0.005;

set radius = 0.01;
set warp_cell_size = 0.01;

foreach warp_option ("identity" "scale" "shear" "perspective" "terrain")

if(${warp_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "terrain") then
# do nothing
else
else
echo unknown warp option
exit(1);
endif

if(${warp_option} == "terrain") then
./src/Main/Release/Warp2JacobianField ${warp_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
else
./src/Main/Release/Warp2JacobianField ${output_dir}/${warp_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
endif

#./src/Main/Release/Matrix "invert" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m

set base_name = aniso_white_${warp_option};

./src/Main/Release/AnisotropicWhiteNoise ${output_dir}/${warp_option}_jacobian_field.f2m ${dimension} ${num_samples} ${domain_size} ${radius} ${warp_cell_size} > ${output_dir}/${base_name}_samples.txt;

set inverse_name = ${base_name}_inverse;

if(${warp_option} == "terrain") then
./src/Main/Release/WarpSamples ${warp_option} 1 ${dimension} ${output_dir}/${base_name}_samples.txt > ${output_dir}/${inverse_name}_samples.txt
else
./src/Main/Release/WarpSamples ${output_dir}/${warp_option}.f2m 1 ${dimension} ${output_dir}/${base_name}_samples.txt > ${output_dir}/${inverse_name}_samples.txt
endif

if(0) then
./src/Main/Release/ClipSamples ${output_dir}/${inverse_name}_samples.txt ${dimension} ${domain_size} > ${output_dir}/${inverse_name}_clipped_samples.txt
mv ${output_dir}/${inverse_name}_clipped_samples.txt ${output_dir}/${inverse_name}_samples.txt
endif

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt

foreach draw_name (${base_name} ${inverse_name}) # ${uniform_name} ${uniform_forward_name} ${uniform_forward_inverse_name})

${rainbow_dir}/src/Main/Release/DrawSamples ${output_dir}/${draw_name}_samples.txt ${output_dir}/${draw_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${output_dir}/${draw_name}_samples.fig
epstopdf ${output_dir}/${draw_name}_samples.eps --outfile ${output_dir}/${draw_name}_samples.pdf

end #foreach draw_name

rm -f ${output_dir}/palette.txt

end # foreach warp_option

endif

# spectrum analysis for anisotropic white/blue noise
if(0) then

set dimension = 2;
set domain_size = "1 1";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.70;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;

# white
#set num_original_samples = 1000;
set white_radius = 0.01;
set warp_cell_size = 0.01;

set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

set dot_radius = 0.005;

foreach warp_option ("identity" "scale" "shear" "perspective" "terrain")

if(${warp_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "terrain") then
# do nothing
else
echo unknown warp option
exit(1);
endif

#./src/Main/Release/Matrix "invert" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m

if(${warp_option} == "terrain") then
./src/Main/Release/Warp2JacobianField ${warp_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
else
./src/Main/Release/Warp2JacobianField ${output_dir}/${warp_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
endif

foreach method ("poisson" "white") # poisson must preceed white for sample count

set base_name = aniso_${warp_option}_${method}_${r_text};
set inverse_name = ${base_name}_inverse;

# generate samples
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then

./src/Main/Release/AnisotropicWhiteNoise ${output_dir}/${warp_option}_jacobian_field.f2m ${dimension} ${num_original_samples} ${domain_size} ${white_radius} ${warp_cell_size} > ${huge_dir}/${base_name}_${i}.txt ;

else if(${method} == "poisson") then

./src/Main/Release/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${warp_option}_jacobian_field.f2m ${warp_cell_size} ${white_radius} > ${huge_dir}/${base_name}_${i}.txt;

# num samples
set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0`;
@ num_original_samples = $#final_samples / ${dimension};

else

echo unknown method
exit(1);

endif

sleep 1;

if(${warp_option} == "terrain") then
./src/Main/Release/WarpSamples ${warp_option} 1 ${dimension} ${huge_dir}/${base_name}_${i}.txt > ${huge_dir}/${inverse_name}_${i}.txt
else
./src/Main/Release/WarpSamples ${output_dir}/${warp_option}.f2m 1 ${dimension} ${huge_dir}/${base_name}_${i}.txt > ${huge_dir}/${inverse_name}_${i}.txt
endif

if(1) then
./src/Main/Release/ClipSamples ${huge_dir}/${inverse_name}_${i}.txt ${dimension} ${domain_size} > ${huge_dir}/${inverse_name}_clipped_${i}.txt
mv ${huge_dir}/${inverse_name}_clipped_${i}.txt ${huge_dir}/${inverse_name}_${i}.txt
endif

# draw samples
if(${i} == -1) then
echo "0 0 0" > ${output_dir}/palette.txt

foreach draw_name (${base_name} ${inverse_name})

${rainbow_dir}/src/Main/Release/DrawSamples ${huge_dir}/${draw_name}_${i}.txt ${huge_dir}/${draw_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${draw_name}_${i}.fig
epstopdf ${huge_dir}/${draw_name}_${i}.eps --outfile ${huge_dir}/${draw_name}_${i}.pdf

end #foreach draw_name

rm -f ${output_dir}/palette.txt
endif

@ i = ${i} + 1;
end # while

# spectrum
set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${inverse_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${inverse_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${inverse_name}_${i}_selected.txt ${huge_dir}/${inverse_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${inverse_name}_${i}.pfm";

set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${inverse_name}_${i}.txt ${dimension} 0`;
@ num_output_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_output_samples};
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

end # foreach method 
end # foreach warp_option

endif

# spectrum analysis for anisotropic white noise *in the forward domain*
if(0) then

set dimension = 2;
set domain_size = "1 1";

# poisson numbers 
set r_value = 0.03;
set r_text = 0p03;

# white
set num_original_samples = 1000;

set dot_radius = 0.005;

set white_radius = 0.01;
set warp_cell_size = 0.01;

set analysis_domain = "forward"; #"inverse"

set domain_option = "uniform";

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 10;
set diff_norm = "unit";
set diff_weight = "unit";
set max_cache_size = 1;

set sim_infinite_domain = 1; # via 3x3 tiling

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
endif

set diff_max = 0.1;
set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_size = 1;
set diffxform_hist_normalization_const = 0.33; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

# sft stuff
set sft_freq = 64; #output_resolution * diff_max
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

set num_runs = 10;

if(${domain_option} == "uniform") then

set domain_res = 1x1;
set importance_cell_size = 3;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else

echo unknown domain option;
exit(1);

endif

foreach warp_option ("scale" "shear" "perspective")

if(${warp_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${warp_option}.f2m;
else if(${warp_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${warp_option}.f2m;
else
echo unknown warp option
exit(1);
endif

./src/Main/Release/Matrix "invert" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m

./src/Main/Release/Warp2JacobianField ${output_dir}/${warp_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m

set head_name = aniso_white_${warp_option};;
set forward_name = ${head_name}_forward;
set inverse_name = ${head_name}_inverse;

if(${analysis_domain} == "forward") then
set analysis_name = ${forward_name};
else
set analysis_name = ${inverse_name};
endif

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

./src/Main/Release/AnisotropicWhiteNoise ${output_dir}/${warp_option}_jacobian_field.f2m ${dimension} ${num_original_samples} ${domain_size} ${white_radius} ${warp_cell_size} > ${huge_dir}/${forward_name}_${i}.txt ;

sleep 1;

if(${analysis_domain} == "inverse") then
./src/Main/Release/WarpSamples ${output_dir}/${warp_option}.f2m 1 ${dimension} ${huge_dir}/${forward_name}_${i}.txt > ${huge_dir}/${inverse_name}_${i}.txt

./src/Main/Release/ClipSamples ${huge_dir}/${inverse_name}_${i}.txt ${dimension} ${domain_size} > ${huge_dir}/${inverse_name}_clipped_${i}.txt
mv ${huge_dir}/${inverse_name}_clipped_${i}.txt ${huge_dir}/${inverse_name}_${i}.txt
endif

# draw samples
if(${i} == -1) then
echo "0 0 0" > ${output_dir}/palette.txt

foreach draw_name (${forward_name} ${inverse_name})

${rainbow_dir}/src/Main/Release/DrawSamples ${huge_dir}/${draw_name}_${i}.txt ${huge_dir}/${draw_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${draw_name}_${i}.fig
epstopdf ${huge_dir}/${draw_name}_${i}.eps --outfile ${huge_dir}/${draw_name}_${i}.pdf

end #foreach draw_name

rm -f ${output_dir}/palette.txt
endif

set all_sample_files = "${all_sample_files} ${huge_dir}/${analysis_name}_${i}.txt";

# tiling
if(${sim_infinite_domain}) then

${rainbow_dir}/src/Main/Release/TileSamples ${huge_dir}/${analysis_name}_${i}.txt ${dimension} 1 3 3 ${domain_size} > ${huge_dir}/${analysis_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${analysis_name}_tiled_${i}.txt";

endif

@ i = ${i} + 1;
end # while

# spectrum / diff transform
foreach kernel_option ("gaussian" "sft")

if(${kernel_option} == "gaussian") then
set keep_dc = 0;
set clamp_radius = 0;
else
set keep_dc = 1;
set clamp_radius = 1;
endif

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${analysis_name}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${analysis_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${analysis_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${analysis_name}_${i}_selected.txt ${huge_dir}/${analysis_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${analysis_name}_${i}.pfm";

set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${analysis_name}_${i}.txt ${dimension} 0`;
@ num_output_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

./src/Main/Release/IsotropicDiffXform ${output_dir}/${domain_option}.f2m ${importance_cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif

# radial computation
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot
if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:1.8]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
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

end # foreach kernel_option 

end # foreach warp_option

endif
