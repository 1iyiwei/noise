#!/bin/tcsh -f

if ($#argv < 1) then
 echo "please specify the output directory";
 exit(1);
endif

set output_dir = $argv[1]

mkdir -p ${output_dir}

set huge_dir = ${output_dir}/huge;
mkdir -p ${huge_dir}

set exe_dir = ./bin;

# blue noise slicing
if(0) then

# spatial parameters
set option = uniform;

set project_dimension = 2;

set project_domain_size = "1 1";

set dot_radius = 0.005;

set r_value = 0.125;
set r_text = 0p125;

set k_value = 0.68;
# 303 samples in 3D
# 2751 samples in 4D

#set conflict_metric = 0; # mean
set boundary = 1; # toroidal

set projection_dimensions = "0 1";

foreach original_dimension (3 4)

if(${original_dimension} == 3) then
set original_domain_size = "1 1 1";
set sample_grid_resolution_per_unit_domain = 12;
set r2d_value = 0.042;
set r2d_text = 0p042;
set sample_2dgrid_resolution_per_unit_domain = 24;
else if(${original_dimension} == 4) then
set original_domain_size = "1 1 1 1";
set sample_grid_resolution_per_unit_domain = 6;
set r2d_value = 0.014;
set r2d_text = 0p014;
set sample_2dgrid_resolution_per_unit_domain = 70;
else
echo "unknown original dimension";
endif

set original_base_name = ${option}_${original_dimension}d_${r_text};
set project_base_name = ${option}_${original_dimension}d_${project_dimension}d_${r_text};
set reference_base_name = ${option}_${project_dimension}d_${r2d_text};

# spectral parameters
set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

# generate samples
set i = 0;
while(${i} < ${num_runs})

# generation
echo "generate ${original_base_name}_${i}";

${exe_dir}/DartThrowing ${option} ${original_dimension} ${r_value} ${k_value} ${original_domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${original_base_name}_${i}.txt

# projection
echo "project ${project_base_name}_${i}";
${exe_dir}/ProjectSamples ${huge_dir}/${original_base_name}_${i}.txt ${original_dimension} ${projection_dimensions} > ${huge_dir}/${project_base_name}_${i}.txt

# reference
echo "generate ${reference_base_name}_${i}";

${exe_dir}/DartThrowing ${option} ${project_dimension} ${r2d_value} ${k_value} ${project_domain_size} ${boundary} ${sample_2dgrid_resolution_per_unit_domain} > ${huge_dir}/${reference_base_name}_${i}.txt

sleep 1;

# draw samples
if(${i} == 0) then

foreach base_name (${project_base_name} ${reference_base_name})

echo "0 0 0" > ${output_dir}/palette.txt;
${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_${i}.fig ${project_dimension} ${dot_radius} 1 ${project_domain_size} ${output_dir}/palette.txt;
rm -f ${output_dir}/palette.txt;

${exe_dir}/fig2eps -f ${output_dir}/${base_name}_${i}.fig

epstopdf ${output_dir}/${base_name}_${i}.eps --outfile ${output_dir}/${base_name}_${i}.pdf

end # foreach basename

endif

@ i = ${i} + 1;
end

# compute sft
foreach base_name (${original_base_name} ${project_base_name} ${reference_base_name})

if(${base_name} == ${original_base_name}) then
set dimension = ${original_dimension};
else if(${base_name} == ${project_base_name}) then
set dimension = ${project_dimension};
else if(${base_name} == ${reference_base_name}) then
set dimension = ${project_dimension};
else
echo "unknown base_name option";
exit(1);
endif

set pft_spec = "1 1";
set i = 2;
while(${i} < ${dimension})
set pft_spec = "${pft_spec} -${sft_freq}";
@ i = ${i} + 1;
end

echo ${pft_spec}

set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})
echo "sft ${base_name}_${i}";

${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt;
${exe_dir}/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} ${pft_spec} ${sft_size} ${sft_scale} ${normalization_option} >& /dev/null;

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

convert ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png

end # foreach base_name

end # foreach original_dimension

endif

# blue noise sheared slicing
if(0) then

# spatial parameters
set option = uniform;

set original_dimension = 4;
set project_dimension = 2;

set project_domain_size = "1 1";

set dot_radius = 0.005;

set r_value = 0.125;
set r_text = 0p125;

set k_value = 0.68;
# 303 samples in 3D
# 2751 samples in 4D

#set conflict_metric = 0; # mean
set boundary = 1; # toroidal

set projection_dimensions = "0 1";

set original_domain_size = "1 1 1 1";
set sample_grid_resolution_per_unit_domain = 6;
set shift_f_amount = "-0.5 -0.5 -0.5 -0.5";
set shift_b_amount = "0.5 0.5 0.5 0.5";

set r2d_value = 0.014;
set r2d_text = 0p014;
set sample_2dgrid_resolution_per_unit_domain = 70;

# spectral parameters
set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach focal_depth (0.015625 0.03125 0.0625 0.125 0.25 0.5 1 2 4 8)

set focal_depth_text = `echo ${focal_depth} | sed 's/\./p/g' `;

set original_base_name = ${option}_${original_dimension}d_${r_text};
set project_base_name = ${option}_${original_dimension}d_${project_dimension}d_${focal_depth_text}f_${r_text}r;
set shift_base_name = ${original_base_name}_shift;
set shear_base_name = ${project_base_name}_shear;
set reference_base_name = ${option}_${project_dimension}d_${r2d_text};

echo "F2 2 5 5   1 0 ${focal_depth} 0 0    0 1 0 ${focal_depth} 0     0 0 1 0 0    0 0 0 1 0     0 0 0 0 1" > ${output_dir}/warp_matrix.txt;
 
# generate samples
set i = 0;
while(${i} < ${num_runs})

# generation
# echo "generate ${original_base_name}_${i}";

${exe_dir}/DartThrowing ${option} ${original_dimension} ${r_value} ${k_value} ${original_domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${original_base_name}_${i}.txt

# shift
# echo "shift ${shift_base_name}_${i}";
${exe_dir}/ShiftSamples ${huge_dir}/${original_base_name}_${i}.txt ${original_dimension} ${shift_f_amount} > ${huge_dir}/${shift_base_name}_${i}.txt

# warp/shear
# echo "warp ${shear_base_name}_${i}";
${exe_dir}/WarpSamples "matrixfb" 0 ${original_dimension} ${huge_dir}/${shift_base_name}_${i}.txt ${output_dir}/warp_matrix.txt ${output_dir}/warp_matrix.txt > ${huge_dir}/${shear_base_name}_${i}.txt;

# shift back
# echo "shift ${original_base_name}_${i}";
${exe_dir}/ShiftSamples ${huge_dir}/${shear_base_name}_${i}.txt ${original_dimension} ${shift_b_amount} > ${huge_dir}/${shift_base_name}_${i}.txt

# projection
# echo "project ${project_base_name}_${i}";
${exe_dir}/ProjectSamples ${huge_dir}/${shift_base_name}_${i}.txt ${original_dimension} ${projection_dimensions} > ${huge_dir}/${project_base_name}_${i}.txt

# reference
if(0) then
echo "generate ${reference_base_name}_${i}";

${exe_dir}/DartThrowing ${option} ${project_dimension} ${r2d_value} ${k_value} ${project_domain_size} ${boundary} ${sample_2dgrid_resolution_per_unit_domain} > ${huge_dir}/${reference_base_name}_${i}.txt
endif

sleep 1;

# draw samples
if(${i} == 0) then

foreach base_name (${project_base_name})

echo "0 0 0" > ${output_dir}/palette.txt;
${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_${i}.fig ${project_dimension} ${dot_radius} 1 ${project_domain_size} ${output_dir}/palette.txt;
rm -f ${output_dir}/palette.txt;

${exe_dir}/fig2eps -f ${output_dir}/${base_name}_${i}.fig

epstopdf ${output_dir}/${base_name}_${i}.eps --outfile ${output_dir}/${base_name}_${i}.pdf

end # foreach basename

endif

@ i = ${i} + 1;
end

# compute sft
foreach base_name (${original_base_name} ${project_base_name})

if(${base_name} == ${original_base_name}) then
set dimension = ${original_dimension};
else if(${base_name} == ${project_base_name}) then
set dimension = ${project_dimension};
else if(${base_name} == ${reference_base_name}) then
set dimension = ${project_dimension};
else
echo "unknown base_name option";
exit(1);
endif

set pft_spec = "1 1";
set i = 2;
while(${i} < ${dimension})
set pft_spec = "${pft_spec} -${sft_freq}";
@ i = ${i} + 1;
end

echo ${pft_spec}

set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})
echo "sft ${base_name}_${i}";

${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt;
${exe_dir}/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} ${pft_spec} ${sft_size} ${sft_scale} ${normalization_option} >& /dev/null;

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

convert ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png

end # foreach base_name

end # foreach focal_depth

endif

# blue noise sheared slicing, 2D flatland visualization
if(0) then

# spatial parameters
set option = uniform;

set dimension = 2;

set domain_size = "1 1";
set shift_f_amount = "-0.5 -0.5";
set shift_b_amount = "0.5 0.5";

set dot_radius = 0.005;

set r_value = 0.0625;
set r_text = `echo ${r_value} | sed 's/\./p/g' `;

set sample_grid_resolution_per_unit_domain = 17;

set k_value = 0.68;

#set conflict_metric = 0; # mean
set boundary = 1; # toroidal

# spectral parameters
set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach p_value (1 2 4 8 inf)
foreach focal_depth (0.125 0.25 0.5 1 2)
#(0.015625 0.03125 0.0625 0.125 0.25 0.5 1 2 4 8)

set focal_depth_text = `echo ${focal_depth} | sed 's/\./p/g' `;

set original_base_name = ${option}_l${p_value}_${dimension}d_${r_text};
set shift_base_name = ${original_base_name}_shift;
set shear_base_name = ${original_base_name}_${focal_depth_text}f;

echo "F2 2 3 3   1 ${focal_depth} 0   0 1 0   0 0 1" > ${output_dir}/warp_matrix.txt;
 
# generate samples
set i = 0;
while(${i} < ${num_runs})

# generation
# echo "generate ${original_base_name}_${i}";

${exe_dir}/DartThrowing ${option}l${p_value} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${original_base_name}_${i}.txt

# shift
# echo "shift ${shift_base_name}_${i}";
${exe_dir}/ShiftSamples ${huge_dir}/${original_base_name}_${i}.txt ${dimension} ${shift_f_amount} > ${huge_dir}/${shift_base_name}_${i}.txt

# warp/shear
# echo "warp ${shear_base_name}_${i}";
${exe_dir}/WarpSamples "matrixfb" 0 ${dimension} ${huge_dir}/${shift_base_name}_${i}.txt ${output_dir}/warp_matrix.txt ${output_dir}/warp_matrix.txt > ${huge_dir}/${shear_base_name}_${i}.txt;

# shift back
# echo "shift ${original_base_name}_${i}";
${exe_dir}/ShiftSamples ${huge_dir}/${shear_base_name}_${i}.txt ${dimension} ${shift_b_amount} > ${huge_dir}/${shift_base_name}_${i}.txt

cp ${huge_dir}/${shift_base_name}_${i}.txt ${huge_dir}/${shear_base_name}_${i}.txt

sleep 1;

# draw samples
if(${i} == 0) then

foreach base_name (${shear_base_name})

echo "0 0 0" > ${output_dir}/palette.txt;
${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt;
rm -f ${output_dir}/palette.txt;

${exe_dir}/fig2eps -f ${output_dir}/${base_name}_${i}.fig

epstopdf ${output_dir}/${base_name}_${i}.eps --outfile ${output_dir}/${base_name}_${i}.pdf

end # foreach basename

endif

@ i = ${i} + 1;
end

# compute sft
foreach base_name (${shear_base_name})

set pft_spec = "1 1";
set i = 2;
while(${i} < ${dimension})
set pft_spec = "${pft_spec} -${sft_freq}";
@ i = ${i} + 1;
end

echo ${pft_spec}

set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})
echo "sft ${base_name}_${i}";

${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt;
${exe_dir}/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} ${pft_spec} ${sft_size} ${sft_scale} ${normalization_option} >& /dev/null;

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

convert ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png

end # foreach base_name

end # foreach focal_depth
end # foreach p_value 
endif


if(0) then

echo "Lloyd 2D + coverage";

set dimension = 2;
set domain_size = "1 1";
set very_small = 0;
set num_samples = 300;
set dot_radius = 0.01;

set num_iterations = 32;
set boundary = 1;

set vdpower_values = (inf 32 16 14 12 10 8 6 4 2);
#(2 4 6 8 10 12 14 16 32 inf);

set root_name = uniform_${num_samples};

set before_name = ${root_name}_before;
set after_name = ${root_name}_after;

set compute = 1;

if(${compute}) then
# init
${exe_dir}/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${output_dir}/${before_name}_samples.txt;

set answers = ();

# iterations
set i = 0;
while(${i} < ${num_iterations})

echo iteration ${i};

(${exe_dir}/LloydRelaxation2D ${output_dir}/${before_name}_samples.txt ${dimension} ${boundary} ${domain_size} 1 > ${output_dir}/${after_name}_samples.txt) >& /dev/null;

foreach normalization (0 1)

set base_name = ${root_name}_n${normalization};

foreach vdpower (${vdpower_values})

set coverage = `${exe_dir}/Coverage ${output_dir}/${before_name}_samples.txt ${dimension} ${vdpower} ${boundary} ${normalization} ${domain_size} ${very_small}`;

# set answers = (${answers} ${coverage});

if(${i} == 0) then
echo ${coverage} > ${output_dir}/${base_name}_${vdpower}.dat;
else
echo ${coverage} >> ${output_dir}/${base_name}_${vdpower}.dat;
endif

end #foreach vdpower
end #foreach normalization

cp ${output_dir}/${after_name}_samples.txt ${output_dir}/${before_name}_samples.txt;

@ i = ${i} + 1;

end #while(${i} < ${num_iterations})

endif

# plot

# echo ${answers}

foreach normalization (0 1)

set base_name = ${root_name}_n${normalization};

if(${normalization}) then
set ylabel = "normalized E";
else
set ylabel = "E";
endif

echo set terminal postscript eps enhanced > ${output_dir}/foo.gp
echo set output \'${output_dir}/${base_name}_plot.eps\' >> ${output_dir}/foo.gp
echo set size 0.75, 0.75 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"iteration\" >> ${output_dir}/foo.gp
echo set ylabel \"${ylabel}\" >> ${output_dir}/foo.gp

@ num_iterations_1 = ${num_iterations} - 1;
#echo plot "[1:${num_iterations}] [0:1.05]" "\\" >> ${output_dir}/foo.gp
echo plot "[0:${num_iterations_1}]" "\\" >> ${output_dir}/foo.gp

set vdpower_size = $#vdpower_values;
set vdpower_last = $vdpower_values[${vdpower_size}];

set i = 1;
foreach vdpower (${vdpower_values})

if(${vdpower} == ${vdpower_last}) then
set separator = "";
else
set separator = ", \\";
endif

if(${vdpower} == inf) then
set title = "{/Symbol \245}";
else
set title = ${vdpower};
endif

echo \"${output_dir}/${base_name}_${vdpower}.dat\" "title '${title}'" with line linetype -1 linecolor ${i} ${separator} >> ${output_dir}/foo.gp

@ i = ${i} + 1;
end #foreach vdpower

wgnuplot ${output_dir}/foo.gp;
# rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${base_name}_plot.eps --outfile ${output_dir}/${base_name}_plot_noembed.pdf;
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/${base_name}_plot_noembed.pdf ${output_dir}/${base_name}_plot.pdf;

end # foreach normalization (0 1)

endif

if(0) then
echo "Generate ideal Lytro sample set"

set aberration_values = (0 140);
foreach aberration (${aberration_values})
    set sub_folder = ${output_dir}/sample_a${aberration}
    mkdir -p ${sub_folder}

    set lambda_int_min = -200;
    set lambda_int_max = 200;
    set lambda_int_inc = 5;

    set lambda_int = ${lambda_int_min};
    while(${lambda_int} <= ${lambda_int_max})
    
        @ lambda_int_value = ${lambda_int} / 10;
        @ lambda_fra_value = ${lambda_int} % 10;
        if(${lambda_fra_value} < 0) then
            @ lambda_fra_value = ${lambda_fra_value} * -1;
            if(${lambda_int_value} == 0) then
                set lambda_int_value = -${lambda_int_value};
            endif
        endif
    
        set lambda = ${lambda_int_value}.${lambda_fra_value}00000;

        ${exe_dir}/GenerateIdealSample.exe ${lambda} ${aberration} ${sub_folder}/sample_${lambda}.txt
        
        @ lambda_int = ${lambda_int} + ${lambda_int_inc};
    end # while(${lambda_int} <= ${lambda_int_max})
end #foreach aberration
endif

if(0) then

echo "plot Lytro sample sets"

if ($#argv < 2) then
    echo "please specify the input directory";
    exit(1);
endif

set input_dir = $argv[2]
set dimension = 2;
set domain_size = "1 0.867";
set dot_radius = 0.002;
set root_name = ideal_lytro;

set lambda_int_min = -200;
set lambda_int_max = 200;
set lambda_int_inc = 5;

set lambda_int = ${lambda_int_min};

while(${lambda_int} <= ${lambda_int_max})

    @ lambda_int_value = ${lambda_int} / 10;
    @ lambda_fra_value = ${lambda_int} % 10;
    if(${lambda_fra_value} < 0) then
        @ lambda_fra_value = ${lambda_fra_value} * -1;
        if(${lambda_int_value} == 0) then
            set lambda_int_value = -${lambda_int_value};
        endif
    endif

    set lambda = ${lambda_int_value}.${lambda_fra_value}00000;

    set draw_name = ${root_name}_${lambda_int}

    # draw commands
    ${exe_dir}/DrawSamples ${input_dir}/sample_${lambda}.txt ${output_dir}/${draw_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}

    ${exe_dir}/fig2eps -f ${output_dir}/${draw_name}.fig

    epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf

    rm -f ${output_dir}/${draw_name}.fig ${output_dir}/${draw_name}.eps
    @ lambda_int = ${lambda_int} + ${lambda_int_inc};
end # while(${lambda_int} <= ${lambda_int_max})
endif

if(0) then

echo "ideal lytro + coverage";

if ($#argv < 2) then
 echo "please specify the input directory";
 exit(1);
endif

set input_dir = $argv[2]

set dimension = 2;
set domain_size = "1 0.867";
set very_small = 0;
set jitter_type = "gaussian";
#set jitter_amount = ${very_small};
set normalization = 1;

set boundary_values = (0 1);

set vdpower_values = (inf 32 16 14 12 10 8 6 4 2);

set jitter_amounts = (000);

set lambda_int_min = -200;
set lambda_int_max = 200;
set lambda_int_inc = 5;

set root_name = ideal_lytro;

set compute = 1;

foreach jitter_amount_string (${jitter_amounts})

foreach boundary (${boundary_values})

set base_name = ${root_name}_j${jitter_amount_string}_n${normalization}_b${boundary};

set jitter_amount = 0.${jitter_amount_string};

if(${compute}) then

foreach vdpower (${vdpower_values})

echo -n "" > ${output_dir}/${base_name}_${vdpower}.dat;

end # foreach vdpower

set lambda_int = ${lambda_int_min};

while(${lambda_int} <= ${lambda_int_max})

@ lambda_int_value = ${lambda_int} / 10;
@ lambda_fra_value = ${lambda_int} % 10;
if(${lambda_fra_value} < 0) then
@ lambda_fra_value = ${lambda_fra_value} * -1;
if(${lambda_int_value} == 0) then
set lambda_int_value = -${lambda_int_value};
endif
endif

set lambda = ${lambda_int_value}.${lambda_fra_value}00000;

${exe_dir}/JitterSamples ${input_dir}/sample_${lambda}.txt ${dimension} ${jitter_type} ${jitter_amount} > ${output_dir}/sample_${lambda}.txt

foreach vdpower (${vdpower_values})

echo jitter ${jitter_amount} vdpower ${vdpower} lambda ${lambda};

# set coverage =  `wc -l ${input_dir}/sample_${lambda}.txt`;
set coverage = `${exe_dir}/Coverage ${output_dir}/sample_${lambda}.txt ${dimension} ${vdpower} ${boundary} ${normalization} ${domain_size} ${very_small}`;

echo "${lambda} ${coverage}" >> ${output_dir}/${base_name}_${vdpower}.dat;

end # foreach vdpower

@ lambda_int = ${lambda_int} + ${lambda_int_inc};
end # while(${lambda_int} <= ${lambda_int_max})

endif

# plot

set xlabel = "{/Symbol l}";

if(${normalization}) then
set ylabel = "normalized E";
else
set ylabel = "E";
endif

echo set terminal postscript eps enhanced > ${output_dir}/foo.gp
echo set output \'${output_dir}/${base_name}_plot.eps\' >> ${output_dir}/foo.gp
echo set size 0.75, 0.75 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel "'${xlabel}'" >> ${output_dir}/foo.gp
echo set ylabel \"${ylabel}\" >> ${output_dir}/foo.gp
echo \"plot [-20:20][0:20] \" >> ${output_dir}/foo.gp

set vdpower_size = $#vdpower_values;
set vdpower_last = $vdpower_values[${vdpower_size}];

set i = 0;

foreach vdpower (${vdpower_values})

if(${vdpower} == ${vdpower_last}) then
set separator = "";
else
set separator = ", \\";
endif

if(${vdpower} == inf) then
set title = "{/Symbol \245}";
else
set title = ${vdpower};
endif

echo \"${output_dir}/${base_name}_${vdpower}.dat\" "using 1:2" "title '${title}'" with line linetype -1 linecolor ${i} ${separator} >> ${output_dir}/foo.gp
echo \"${output_dir}/${base_name}_${vdpower}.dat\" "using 1:2" "title '${title}'" with line linetype -1 linecolor ${i} ${separator}

@ i = ${i} + 1;
end #foreach vdpower

# use gunplot in cygwin instead of wgunplot
gnuplot ${output_dir}/foo.gp;
# rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${base_name}_plot.eps --outfile ${output_dir}/${base_name}_plot_noembed.pdf;
# ps2pdf13 causes strange error
#ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/${base_name}_plot_noembed.pdf ${output_dir}/${base_name}_plot.pdf;

end # foreach boundary

end # foreach jitter_amount_string

endif

if(0) then

echo "hexagonal grid + coverage";

set dimension = 2;
set domain_size = "1 0.867";
set sample_rate = 50;
set dot_radius = 0.002;
set very_small = 0;
set normalization = 1;

set boundary = 1;

set jitter_type = "gaussian";
set jitter_amounts = (000 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015 016);

set vdpower_values = (inf 32 16 14 12 10 8 6 4 2 0);

set root_name = hexagonal_s${sample_rate}_n${normalization}_b${boundary};

${exe_dir}/HexagonalGrid ${dimension} ${domain_size} ${sample_rate} > ${output_dir}/${root_name}.txt

foreach vdpower (${vdpower_values})

echo -n "" > ${output_dir}/${root_name}_${vdpower}.dat;

end #foreach vdpower

foreach jitter_amount_string (${jitter_amounts})

set base_name = ${root_name}_j${jitter_amount_string};

set jitter_amount = 0.${jitter_amount_string};

${exe_dir}/JitterSamples ${output_dir}/${root_name}.txt ${dimension} ${jitter_type} ${jitter_amount} > ${output_dir}/${base_name}.txt

foreach vdpower (${vdpower_values})

set coverage = `${exe_dir}/Coverage ${output_dir}/${base_name}.txt ${dimension} ${vdpower} ${boundary} ${normalization} ${domain_size} ${very_small}`;

echo "${jitter_amount} ${coverage}" >> ${output_dir}/${root_name}_${vdpower}.dat;

end # foreach vdpower

# draw samples
if(1) then
foreach draw_option ("voronoi_b1")

if(${draw_option} == "stipple") then

set radius_sign = "";
set domain_sign = "";

else

set radius_sign = "-";
if(${draw_option} == "voronoi_b0") then
set domain_sign = "-";
else
set domain_sign = "";
endif

endif

set draw_name = ${base_name}_${draw_option};

${exe_dir}/DrawSamples ${output_dir}/${base_name}.txt ${output_dir}/${draw_name}_samples.fig ${dimension} ${radius_sign}${dot_radius} 1 ${domain_sign}${domain_size}

${exe_dir}/fig2eps -f ${output_dir}/${draw_name}_samples.fig

epstopdf ${output_dir}/${draw_name}_samples.eps --outfile ${output_dir}/${draw_name}_samples.pdf

end # foreach draw_option
endif

end # foreach jitter_amount

# plot

set xlabel = "jitter amount";

if(${normalization}) then
set ylabel = "normalized E";
else
set ylabel = "E";
endif

echo set terminal postscript eps enhanced > ${output_dir}/foo.gp
echo set output \'${output_dir}/${root_name}_plot.eps\' >> ${output_dir}/foo.gp
echo set size 0.75, 0.75 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel "'${xlabel}'" >> ${output_dir}/foo.gp
echo set ylabel \"${ylabel}\" >> ${output_dir}/foo.gp

echo plot "\\" >> ${output_dir}/foo.gp

set vdpower_size = $#vdpower_values;
set vdpower_last = $vdpower_values[${vdpower_size}];

set i = 0;

foreach vdpower (${vdpower_values})

if(${vdpower} == ${vdpower_last}) then
set separator = "";
else
set separator = ", \\";
endif

if(${vdpower} == inf) then
set title = "{/Symbol \245}";
else
set title = ${vdpower};
endif

echo \"${output_dir}/${root_name}_${vdpower}.dat\" "using 1:2" "title '${title}'" with line linetype -1 linecolor ${i} ${separator} >> ${output_dir}/foo.gp

@ i = ${i} + 1;
end #foreach vdpower

wgnuplot ${output_dir}/foo.gp;
# rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${root_name}_plot.eps --outfile ${output_dir}/${root_name}_plot_noembed.pdf;
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/${root_name}_plot_noembed.pdf ${output_dir}/${root_name}_plot.pdf;

endif

if(0) then

echo "cross product noise - flatland";

set dart_option = "uniform";
set r_value_0 = 0.01;
set r_text_0 = 0p01;
set r_value_1 = 0.03;
set r_text_1 = 0p03;
set sample_grid_resolution_per_unit_domain = 100;
set k_value = 0.70;
set boundary = 1;

set original_dimension = 1;
set original_domain_size = "1";

set project_dimension = 2;
set project_domain_size = "1 1"

set dot_radius = 0.005;

set num_runs = 10;

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

set samp_0_options = ("white" "blue" "regular");
set samp_1_options = ("white" "blue" "regular");
set conv_schemes = ("all" "nearest");

foreach samp_0_option (${samp_0_options})
foreach samp_1_option (${samp_1_options})

set original_base_name_0 = ${samp_0_option}_${original_dimension}d_row_${r_text_0};
set project_base_name_0 = ${samp_0_option}_${original_dimension}d_row_${project_dimension}d_${r_text_0};
set original_base_name_1 = ${samp_1_option}_${original_dimension}d_col_${r_text_1};
set project_base_name_1 = ${samp_1_option}_${original_dimension}d_col_${project_dimension}d_${r_text_1};

set project_base_name_row = ${project_base_name_0};
set project_base_name_col = ${project_base_name_1};

set project_base_name = ${samp_0_option}_${samp_1_option}_${original_dimension}d_${project_dimension}d_${r_text_0}_${r_text_1};

foreach conv_scheme (${conv_schemes})

set conv_base_name = ${project_base_name}_${conv_scheme};

if(${conv_scheme} == "nearest") then
set conv_extra = "0 1 1 0 ${boundary} ${project_domain_size}";
else
set conv_extra = "";
endif

# generate samples
set i = 0;
while(${i} < ${num_runs})

set conv_sample_set = "";

foreach k (0 1)

if(${k} == 0) then
set projection_dimensions = "0 1";
set project_target_name = ${project_base_name_row};
set original_base_name = ${original_base_name_0};
set r_value = ${r_value_0};
set samp_option = ${samp_0_option};
else
set projection_dimensions = "1 0";
set project_target_name = ${project_base_name_col};
set original_base_name = ${original_base_name_1};
set r_value = ${r_value_1};
set samp_option = ${samp_1_option};
endif

# generation
echo "generate ${original_base_name}_${k}_${i}";

if(${samp_option} == "blue") then

set gen_command = "${exe_dir}/DartThrowing ${dart_option} ${original_dimension} ${r_value} ${k_value} ${original_domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain}";

else if(${samp_option} == "white") then

set num_samples = `${exe_dir}/Math samplenum ${original_dimension} ${r_value} ${k_value}`;

set gen_command = "${exe_dir}/UniformWhiteNoise ${original_dimension} ${num_samples} ${original_domain_size}";

else if(${samp_option} == "regular") then

set num_samples = `${exe_dir}/Math samplenum ${original_dimension} ${r_value} ${k_value}`;

set gen_command = "${exe_dir}/RegularGrid ${original_dimension} ${original_domain_size} ${num_samples} 0";

else

echo "unknown sample option";
exit(1);

endif

if(! -f ${huge_dir}/${original_base_name}_${k}_${i}.txt) then
${gen_command} > ${huge_dir}/${original_base_name}_${k}_${i}.txt
endif

# projection
echo "project ${project_target_name}_${i}";
${exe_dir}/ProjectSamples ${huge_dir}/${original_base_name}_${k}_${i}.txt ${original_dimension} ${projection_dimensions} > ${huge_dir}/${project_target_name}_${i}.txt

sleep 1;

set conv_sample_set = "${conv_sample_set} ${huge_dir}/${project_target_name}_${i}.txt";
end #forech k

# convolve

echo "convolve ${conv_base_name}_${i}";
${exe_dir}/ConvolveSamples ${conv_sample_set} ${project_dimension} ${conv_scheme} ${conv_extra} > ${huge_dir}/${conv_base_name}_${i}.txt

# draw samples
if(${i} == 0) then

foreach base_name (${conv_base_name})

echo "0 0 0" > ${output_dir}/palette.txt;
${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_${i}.fig ${project_dimension} ${dot_radius} 1 ${project_domain_size} ${output_dir}/palette.txt;
rm -f ${output_dir}/palette.txt;

${exe_dir}/fig2eps -f ${output_dir}/${base_name}_${i}.fig

epstopdf ${output_dir}/${base_name}_${i}.eps --outfile ${output_dir}/${base_name}_${i}.pdf

end # foreach basename

endif

@ i = ${i} + 1;
end

# compute sft

if(${samp_0_option} == $samp_0_options[1]) then
    set sft_set = (${project_base_name_row} ${project_base_name_col} ${conv_base_name})
else
    set sft_set = (${project_base_name_row} ${conv_base_name})
endif

foreach base_name (${sft_set})

if(${base_name} == ${original_base_name}) then
set dimension = ${original_dimension};
else 
set dimension = ${project_dimension};
endif

set pft_spec = "1 1";
set i = 2;
while(${i} < ${dimension})
set pft_spec = "${pft_spec} -${sft_freq}";
@ i = ${i} + 1;
end

echo ${pft_spec}

set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})
echo "sft ${base_name}_${i}";

${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt;
${exe_dir}/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} ${pft_spec} ${sft_size} ${sft_scale} ${normalization_option} >& /dev/null;

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# num samples
set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_0.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

if((${base_name} == ${conv_base_name}) && (${conv_scheme} == "all")) then
    set display_option = "sqrt_axis";
else
    set display_option = "sqrt_dc";
endif
set display_option = "sqrt_dc";

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 ${display_option}

convert ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png

end # foreach base_name

end # foreach conv_scheme
end # foreach samp_1_option
end # foreach samp_0_option

endif
