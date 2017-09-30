#!/bin/tcsh -f

#echo $#argv

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set output_dir = $argv[1]

mkdir -p ${output_dir}

set huge_dir = ${output_dir}/huge;
mkdir -p ${huge_dir}

set exe_dir = ./bin;

set gnuplot = "gnuplot";

# synthetic PFT
if(1) then

set dimension = 2;
set domain_size = "1 1";
set domain_size_string = "1x1";

set num_samples = 888;
set num_runs = "infty";

set sft_freq = 64;
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

${exe_dir}/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

${exe_dir}/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

set base_name = ${domain_option}_${domain_res};

${exe_dir}/SyntheticPFT ${num_samples} ${output_dir}/${domain_option}.f2m ${domain_size_string} ${output_dir}/${base_name}_average${num_runs}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm
end #foreach domain_option

endif

# diff transform
if(1) then

set dimension = 2;

set num_samples = 1024; # white noise
set sample_rate_1d = 10; # hex
set sample_rate = "32 32"; # regular grid

set domain_size = "3 3";

set sub_domain_min = "1 1";
set sub_domain_max = "2 2";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;

set num_runs = 1;

foreach method ("white" "grid" "hex")

set base_name = ${method}_diffxform;

# generate samples
set all_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then
${exe_dir}/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "grid") then
${exe_dir}/RegularGrid ${dimension} ${domain_size} ${sample_rate} 0 > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "hex") then
${exe_dir}/HexagonalGrid ${dimension} ${domain_size} ${sample_rate_1d} > ${huge_dir}/${base_name}_${i}.txt
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

${exe_dir}/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial mean (todo)

end # foreach method

endif

# isotropic/adaptive diff transform
if(1) then

set dimension = 2;

set domain_size = "1 1";

set r_value = 0.04;
set r_text = 0p04;
set k_value = 0.70;
#set conflict_metric = 0; # mean
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;
set max_importance = 1;

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 5;
#set diff_norm = "symmetry";
#set diff_weight = "unit";
set max_cache_size = 4;

set sub_domain_min = "0 0";
set sub_domain_max = "1 1";

set diff_max = 0.1;
set diff_domain_max = "${diff_max} ${diff_max}";

#set kernel_option = "gaussian";
set kernel_size = 1;
set hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

set num_runs = 5;

foreach domain_option ("uniform" "1blob") #"random" 

if(${domain_option} == "uniform") then

set domain_res = 1x1;
set importance_cell_size = 1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

${exe_dir}/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "random") then

set domain_res = 2x2;
set importance_cell_size = 0.5;
echo "F2 2 2 2 0.1 0.2 0.3 0.4" > ${output_dir}/${domain_option}.f2m;

${exe_dir}/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set importance_cell_size = 0.03125; # 1.0/32

set domain_res_1d = 8;
set importance_cell_size = 0.125; # 1.0/8

set domain_res = ${domain_res_1d}x${domain_res_1d};

${exe_dir}/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

${exe_dir}/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

foreach measure ("isotropic" "anisoiso")

foreach method ("poisson" "white")# "poissondivwhite") # poisson must preceed white to count sample, and both must preceed poissondivwhite

set head_name = ${domain_option}_${r_text};

set base_name = ${head_name}_${measure}_${method};

# generate samples
set all_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then
${exe_dir}/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "poisson") then

if(${measure} == "anisoiso") then
set max_importance_arg = ${max_importance}
else
set max_importance_arg = "";
endif

${exe_dir}/DartThrowing ${measure} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${max_importance_arg} ${importance_cell_size} > ${huge_dir}/${base_name}_${i}.txt

else if(${method} == "poissondivwhite") then
# do nothing
else
echo unknown sampling method;
exit(1);
endif

sleep 1;

if(${method} !=  "poissondivwhite") then
set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};
endif

@ i = ${i} + 1;
end

# diff transform
foreach kernel_option ("gaussian") # "cos") # cos is too slow

if(${kernel_option} == "cos") then
set keep_dc = 1;
else
set keep_dc = 0;
endif

foreach diff_norm ("symmetry")
foreach diff_weight ("unit")

set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
set base_name = ${head_name}_${measure}_${method}_${tail_name};

if(${method} ==  "poissondivwhite") then

${exe_dir}/NormF2M "div" 1 ${output_dir}/${head_name}_poisson_${tail_name}_average${num_runs}.f2m ${output_dir}/${head_name}_white_${tail_name}_average${num_runs}.f2m ${output_dir}/${head_name}_${method}_${tail_name}_average${num_runs}.f2m;

else

echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;

if(${measure} == "isotropic") then
set exe_name = IsotropicDiffXform;
set max_importance_arg = "";
else if(${measure} == "anisoiso") then
set exe_name = AnisoIsoDiffXform;
set max_importance_arg = ${max_importance};
else
echo unknown measure;
exit(1);
endif

${exe_dir}/${exe_name} ${output_dir}/${domain_option}.f2m ${max_importance_arg} ${importance_cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

endif #(${method} ==  "poissondivwhite") then

${exe_dir}/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial computation
echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot

if(${method} ==  "poissondivwhite") then
set hist_norm_const = 1;
else
set hist_norm_const = ${hist_normalization_const};
endif

./${exe_dir}/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_norm_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${diff_scale}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [0:7]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

${gnuplot} foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach diff_weight
end # foreach diff_norm
end # foreach kernel_option

end # foreach method

end # foreach measure

end # foreach domain_option

endif

# anisotropic noise based on warp
if(1) then

set dimension = 2;
set domain_size = "1 1";

set num_samples = 4000;

set dot_radius = 0.005;

set radius = 0.01;
set warp_cell_size = 0.01;

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

set base_name = aniso_white_${warp_option};

${exe_dir}/Warp2JacobianField ${output_dir}/${warp_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m

${exe_dir}/AnisotropicWhiteNoise ${output_dir}/${warp_option}_jacobian_field.f2m ${dimension} ${num_samples} ${domain_size} ${radius} ${warp_cell_size} > ${output_dir}/${base_name}_samples.txt ;

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt

${exe_dir}/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${exe_dir}/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt

end # foreach warp_option

endif

# uniform/isotropic/anisotropic dart throwing
if(1) then

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

${exe_dir}/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

${exe_dir}/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

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

${exe_dir}/Warp2JacobianField ${domain_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

else if((${domain_option} != "uniform") && (${domain_option} != "1blob")) then

${exe_dir}/Warp2JacobianField ${output_dir}/${domain_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

endif

foreach boundary (0 1)

set base_name = poisson_${domain_option}_${r_text}_b${boundary};

if(${domain_option} == "uniform") then

${exe_dir}/DartThrowing ${domain_option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${base_name}_samples.txt;

else if(${domain_option} == "1blob") then

${exe_dir}/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${importance_cell_size} > ${output_dir}/${base_name}_samples.txt;

else

${exe_dir}/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${warp_cell_size} ${aniso_white_noise_radius} > ${output_dir}/${base_name}_samples.txt;
endif

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
${exe_dir}/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${exe_dir}/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${base_name}_samples.pdf;

end # foreach boundary
end # foreach domain_option

endif

# spectrum analysis for anisotropic white/blue noise
if(1) then

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

#${exe_dir}/Matrix "invert" ${output_dir}/${warp_option}.f2m ${output_dir}/${warp_option}_inverse.f2m

if(${warp_option} == "terrain") then
${exe_dir}/Warp2JacobianField ${warp_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
else
${exe_dir}/Warp2JacobianField ${output_dir}/${warp_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${warp_option}_jacobian_field.f2m
endif

foreach method ("poisson" "white") # poisson must preceed white for sample count

set base_name = aniso_${warp_option}_${method}_${r_text};
set inverse_name = ${base_name}_inverse;

# generate samples
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then

${exe_dir}/AnisotropicWhiteNoise ${output_dir}/${warp_option}_jacobian_field.f2m ${dimension} ${num_original_samples} ${domain_size} ${white_radius} ${warp_cell_size} > ${huge_dir}/${base_name}_${i}.txt ;

else if(${method} == "poisson") then

${exe_dir}/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${warp_option}_jacobian_field.f2m ${warp_cell_size} ${white_radius} > ${huge_dir}/${base_name}_${i}.txt;

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_original_samples = $#final_samples / ${dimension};

else

echo unknown method
exit(1);

endif

sleep 1;

if(${warp_option} == "terrain") then
${exe_dir}/WarpSamples ${warp_option} 1 ${dimension} ${huge_dir}/${base_name}_${i}.txt > ${huge_dir}/${inverse_name}_${i}.txt
else
${exe_dir}/WarpSamples ${output_dir}/${warp_option}.f2m 1 ${dimension} ${huge_dir}/${base_name}_${i}.txt > ${huge_dir}/${inverse_name}_${i}.txt
endif

if(1) then
${exe_dir}/ClipSamples ${huge_dir}/${inverse_name}_${i}.txt ${dimension} ${domain_size} > ${huge_dir}/${inverse_name}_clipped_${i}.txt
mv ${huge_dir}/${inverse_name}_clipped_${i}.txt ${huge_dir}/${inverse_name}_${i}.txt
endif

# draw samples
if(${i} == -1) then
echo "0 0 0" > ${output_dir}/palette.txt

foreach draw_name (${base_name} ${inverse_name})

${exe_dir}/DrawSamples ${huge_dir}/${draw_name}_${i}.txt ${huge_dir}/${draw_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
${exe_dir}/fig2eps -f ${huge_dir}/${draw_name}_${i}.fig
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

${exe_dir}/SampleSelector ${huge_dir}/${inverse_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${inverse_name}_${i}_selected.txt
${exe_dir}/SamplePFT ${huge_dir}/${inverse_name}_${i}_selected.txt ${huge_dir}/${inverse_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${inverse_name}_${i}.pfm";

set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${inverse_name}_${i}.txt ${dimension} cout 0`;
@ num_output_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
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

./${exe_dir}/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

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

${gnuplot} foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm

end # foreach method 
end # foreach warp_option

endif

# anisotropic diff transform
if(1) then

set dimension = 2;

set domain_size = "1 1";

# poisson
set r_value = 0.04;
set r_text = 0p04;
set k_value = 0.70;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;

# white
#set num_samples = 400;
set white_radius = 0.01;
set warp_cell_size = 0.05;

set anisotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 5;
#set diff_norm = "symmetry";
#set diff_weight = "unit";
set max_cache_size = 4;

set sim_infinite_domain = 1; # via 3x3 tiling
set sim_infinite_tiling = "3 3";

if(${sim_infinite_domain}) then
set default_sub_domain_min = "1 1";
set default_sub_domain_max = "2 2";
else
set default_sub_domain_min = "0 0";
set default_sub_domain_max = "1 1";
endif

set diff_max = 0.1;
set diff_domain_max = "${diff_max} ${diff_max}";

#set kernel_option = "gaussian";
set kernel_size = 1;
set hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

set num_runs = 5;

foreach domain_option ("identity" "scale" "shear" "perspective" "terrain")

set sub_domain_min = "${default_sub_domain_min}";
set sub_domain_max = "${default_sub_domain_max}";

if(${domain_option} == "identity") then
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
echo unknown domain option
exit(1);
endif

if(${domain_option} == "terrain") then

${exe_dir}/Warp2JacobianField ${domain_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

else

${exe_dir}/Warp2JacobianField ${output_dir}/${domain_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

endif

if(${sim_infinite_domain}) then
${exe_dir}/TileF2M ${output_dir}/${domain_option}.f2m ${sim_infinite_tiling} ${output_dir}/${domain_option}_tiled.f2m;
endif


foreach method ("poisson" "white")# "poissondivwhite") # poisson must preceed white to count sample, and both must preceed poissondivwhite

set head_name = ${domain_option}_${r_text};

set base_name = ${head_name}_${method};

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then

${exe_dir}/AnisotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${dimension} ${num_samples} ${domain_size} ${white_radius} ${warp_cell_size} > ${huge_dir}/${base_name}_${i}.txt ;

else if(${method} == "poisson") then

${exe_dir}/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${warp_cell_size} ${white_radius} > ${huge_dir}/${base_name}_${i}.txt;

else

echo unknown sampling method;
exit(1);

endif

sleep 1;

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then

${exe_dir}/TileSamples ${huge_dir}/${base_name}_${i}.txt ${dimension} 1 ${sim_infinite_tiling} ${domain_size} > ${huge_dir}/${base_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.txt";

endif

@ i = ${i} + 1;
end

# diff transform
foreach kernel_option ("gaussian") # "cos") # cos is too slow

if(${kernel_option} == "cos") then
set keep_dc = 1;
else
set keep_dc = 0;
endif

foreach diff_norm ("symmetry")
foreach diff_weight ("unit")

set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
set base_name = ${head_name}_${method}_${tail_name};

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

if(${sim_infinite_domain}) then
set domain_option_file = ${output_dir}/${domain_option}_tiled.f2m 
else
set domain_option_file = ${output_dir}/${domain_option}.f2m 
endif

${exe_dir}/AnisotropicDiffXform ${domain_option_file} ${warp_cell_size} ${anisotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial computation
echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
set hist_norm_const = ${hist_normalization_const};

./${exe_dir}/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_norm_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${diff_scale}

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [0:7]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

${gnuplot} foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach diff_weight
end # foreach diff_norm
end # foreach kernel_option

end # foreach method

end # foreach domain_option

endif
