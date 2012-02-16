#!/bin/tcsh -f

set halftone_dir = ../Halftoning;
set output_dir = data;
set huge_dir = data/huge;

# color error diffusion for constant image, i.e. uniform sampling
if(0) then

set num_class = 1; # 1 or 3

# maintain intensity consistency
set image_size = 128;
set intensity = 0625; # roughly 1000 samples per class
set intensity = 025; # roughly 400 samples per class

if(0) then
set image_size = 512;
set intensity = 0015625; # roughly 400 samples per class
endif

if(0) then # this will leave a large piece of empty area in the beginning
set image_size = 1024;
set intensity = 0009765625; # roughly 1000 samples per class
set intensity = 000390625; # roughly 400 samples per class
endif

# color space
set palette_root_name = palette;
set draw_palette_root_name = draw_palette;
set background_color = "0 0 0";
if(${num_class} == 1) then
echo "0 0 0   1 1 1" > ${output_dir}/${palette_root_name}.txt;
echo "0 0 0" > ${output_dir}/${draw_palette_root_name}.txt;
else if(${num_class} == 3) then
echo "0 0 0   1 0 0   0 1 0   0 0 1" > ${output_dir}/${palette_root_name}.txt;
echo "1 0 0   0 1 0   0 0 1" > ${output_dir}/${draw_palette_root_name}.txt;
else
echo unknown number of class
exit(1);
endif

set palette_file_name = ${output_dir}/${palette_root_name}.txt;
#set palette_file_name = "null";

set input_root_name = g${intensity};

foreach diffuser_type (zhou knuthzhou) #(varcoeff zhou knuth knuthzhou)
foreach scanner_type (serpentine hilbert) #(serpentine random hilbert)

set output_root_name = ${input_root_name}_${num_class}c_${diffuser_type}_${scanner_type};

${halftone_dir}/src/Main/Release/ErrorDiffuse -${image_size}.${intensity} ${output_dir}/${output_root_name}.ppm ${diffuser_type} ${scanner_type} ${palette_file_name};

set sample_root_name = ${output_root_name}_samples;

./src/Main/Release/ExtractDiscreteSamples ${output_dir}/${output_root_name}.ppm ${image_size} ${background_color} > ${output_dir}/${sample_root_name}.txt;

set plot_root_name = ${sample_root_name};
set domain_size = "1 1";
set dot_radius = 0.005;

src/Main/Release/DrawSamples ${output_dir}/${sample_root_name}.txt ${output_dir}/${plot_root_name}.fig 2 ${dot_radius} 1 ${domain_size} ${output_dir}/${draw_palette_root_name}.txt;
src/Scripts/fig2eps -f ${output_dir}/${plot_root_name}.fig
epstopdf ${output_dir}/${plot_root_name}.eps --outfile ${output_dir}/${plot_root_name}.pdf

end # foreach diffuser_type
end # foreach scanner_type

endif

# void and cluster, visual samples
if(0) then

set grid_size = 128;
set num_classes = 3;

set num_samples = "1000 1000 1000"; # match error diffusion above

set num_samples = "400 400 400"; # friendly for 1in paper image
set num_samples_text = 1200;

# set num_samples = "42 42 42"; # debug
set relative_splat_size = 1.0;
set init_swap_size = 32;
set min_num_iterations = 100;

if(0) then
set grid_size = 256;
set num_samples = "100 200 400"; # heterogeneous
set num_samples_text = 700;
set init_swap_size = 32;
set min_num_iterations = 400;
endif

if(0) then
set grid_size = 512;
set num_samples = "308 1118 5351"; # match teaser figure
set num_samples_text = 6777;
set init_swap_size = 64;
set min_num_iterations = 400;
endif

set background_color = "0 0 0";

set palette_root_name = palette;
echo "255 0 0   0 255 0   0 0 255" > ${output_dir}/${palette_root_name}.txt;

set palette_file_name = ${output_dir}/${palette_root_name}.txt;
#set palette_file_name = "null";

foreach weighting_scheme (fair rainbow)
set output_root_name = voidcluster_${weighting_scheme}_${num_classes}c_${num_samples_text}s;

${halftone_dir}/src/Main/Release/VoidCluster ${grid_size} ${num_classes} ${num_samples} ${weighting_scheme} ${relative_splat_size} ${init_swap_size} ${min_num_iterations} ${output_dir}/${output_root_name}.ppm ${output_dir}/${palette_root_name}.txt;

set sample_root_name = ${output_root_name}_samples;

./src/Main/Release/ExtractDiscreteSamples ${output_dir}/${output_root_name}.ppm ${grid_size} ${background_color} ${output_dir}/${palette_root_name}.txt > ${output_dir}/${sample_root_name}.txt;

set domain_size = "1 1";
set dot_radius = 0.005;

foreach id (0 1 2 all)

set sample_partial_root_name = ${output_root_name}_${id}_samples;

set plot_root_name = ${sample_partial_root_name};

if(${id} == 'all') then
cp ${output_dir}/${sample_root_name}.txt ${output_dir}/${sample_partial_root_name}.txt
else
grep "^${id}" ${output_dir}/${sample_root_name}.txt > ${output_dir}/${sample_partial_root_name}.txt
endif

src/Main/Release/DrawSamples ${output_dir}/${sample_partial_root_name}.txt ${output_dir}/${plot_root_name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${plot_root_name}.fig
epstopdf ${output_dir}/${plot_root_name}.eps --outfile ${output_dir}/${plot_root_name}.pdf

end # foreach id

end # foreach weighting_scheme

endif

# void and cluster, spectrum
if(0) then

set dimension = 2;
set grid_size = 256; # 128 or 256

set num_classes = 3;
set num_samples = "400 400 400"; # friendly for 1in paper image
set num_samples_array = (400 400 400);
set total_num_samples = 1200;
set num_samples_text = 1200;

if(0) then
set num_classes = 2;
set num_samples = "150 1050"; # 1:7 ratio for worst case hump
set num_samples_array = (150 1050);
set total_num_samples = 1200;
set num_samples_text = 1200;
endif

set relative_splat_size = 1.0;
set init_swap_size = 32;
set min_num_iterations = 200;

set background_color = "0 0 0";

set palette_root_name = palette;
echo "255 0 0   0 255 0   0 0 255" > ${output_dir}/${palette_root_name}.txt;

set palette_file_name = ${output_dir}/${palette_root_name}.txt;
#set palette_file_name = "null";

set num_runs = 10;

# sft
set sft_freq = 256;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set domain_size = "1 1";
set dot_radius = 0.0025;

foreach weighting_scheme (fair)

set output_base_name = voidcluster_${weighting_scheme}_${grid_size}g_${num_classes}c_${num_samples_text}s;

# generation
set which_run = 0;
while(${which_run} < ${num_runs})

set output_root_name = ${output_base_name}_${which_run};

${halftone_dir}/src/Main/Release/VoidCluster ${grid_size} ${num_classes} ${num_samples} ${weighting_scheme} ${relative_splat_size} ${init_swap_size} ${min_num_iterations} ${huge_dir}/${output_root_name}.ppm ${output_dir}/${palette_root_name}.txt;

sleep 1;

set sample_root_name = ${output_root_name}_samples;

./src/Main/Release/ExtractDiscreteSamples ${huge_dir}/${output_root_name}.ppm ${grid_size} ${background_color} ${output_dir}/${palette_root_name}.txt > ${huge_dir}/${sample_root_name}.txt;

@ which_run = ${which_run} + 1;
end # while 

# spectrum
foreach id (0 1)

set sft_base_name = ${output_base_name}_${id};

set all_pfm_files = "";
set which_run = 0;
while(${which_run} < ${num_runs})

set output_root_name = ${output_base_name}_${which_run};
set sft_root_name = ${sft_base_name}_${which_run};

set input_sample_root_name = ${output_root_name}_samples;
set output_sample_root_name = ${sft_root_name}_samples;

if(${id} == 'all') then
set ids = "0 1 2";
else
set ids = "${id}";
@ id_plus_one = ${id} + 1;
endif

src/Main/Release/SampleSelector ${huge_dir}/${input_sample_root_name}.txt ${dimension} ${ids} > ${huge_dir}/${output_sample_root_name}_selected.txt
src/Sain/Release/SFT ${huge_dir}/${output_sample_root_name}_selected.txt ${huge_dir}/${output_sample_root_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "$all_pfm_files ${huge_dir}/${output_sample_root_name}.pfm";

@ which_run = ${which_run} + 1;
end # while

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${sft_base_name}_average${num_runs}.pfm ${output_dir}/${sft_base_name}_average${num_runs}_mean.txt ${output_dir}/${sft_base_name}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

src/Sain/Release/PFM2PPM ${output_dir}/${sft_base_name}_average${num_runs}.pfm ${output_dir}/${sft_base_name}_average${num_runs}.ppm 1 1 1

echo ${output_dir}/${sft_base_name}_average${num_runs}.ppm

if(${id} == 'all') then
set num_class_samples = ${total_num_samples};
else
set num_class_samples = $num_samples_array[$id_plus_one];
endif

src/Scripts/txt2dat ${output_dir}/${sft_base_name}_average${num_runs}_mean.txt ${output_dir}/${sft_base_name}_average${num_runs}_variance.txt ${num_class_samples} ${output_dir}/${sft_base_name}_average${num_runs}_mean.dat ${output_dir}/${sft_base_name}_average${num_runs}_variance.dat 1

# mean
echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${sft_base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}]" \"${output_dir}/${sft_base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${sft_base_name}_average${num_runs}_mean;
epstopdf ${output_dir}/${output_name}.eps --outfile ${output_dir}/${output_name}.pdf;
echo ${output_dir}/${output_name}.pdf;

# variance
echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${sft_base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-20:20]" \"${output_dir}/${sft_base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${sft_base_name}_average${num_runs}_variance;
epstopdf ${output_dir}/${output_name}.eps --outfile ${output_dir}/${output_name}.pdf;
echo ${output_dir}/${output_name}.pdf;

end # foreach id

end # foreach weighting_scheme

endif

# multi-class PD, try to match 3x400 samples
if(0) then

set dimension = 2;
set num_classes = 3;
set priority = "0 0 0";
set r_values = "0.036 0.036 0.036";
set r_text = 400;

set dot_radius = 0.005;

set k_value = 0.67;
set domain_size = "1 1";

set exe_name = DartThrowing;
set output_name = rainbow;

set output_root_name = ${output_name}_${dimension}d_${num_classes}c_${r_text};

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${output_dir}/${output_root_name}.txt

if(1) then
src/Main/Release/DrawSamples ${output_dir}/${output_root_name}.txt ${output_dir}/${output_root_name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${output_root_name}.fig
epstopdf ${output_dir}/${output_root_name}.eps --outfile ${output_dir}/${output_root_name}.pdf
endif

echo ${output_root_name}
endif

# R1G2B1 sensor layout
if(0) then

set grid_size_1d = 20;
# set num_samples = "100 200 100";
set intensity_r = "25";
set intensity_g = "50";
set intensity_b = "25";

set diffuser_type = zhou;
set scanner_type = serpentine;

set input_root_name = r1g2b1_ed;

set palette_root_name = palette;
echo "1 0 0   0 1 0   0 0 1" > ${output_dir}/${palette_root_name}.txt;

set palette_file_name = ${output_dir}/${palette_root_name}.txt;
#set palette_file_name = "null";

set output_root_name = ${input_root_name}_${diffuser_type}_${scanner_type}_${grid_size_1d}x${grid_size_1d};

${halftone_dir}/src/Main/Release/ErrorDiffuse "${grid_size_1d}.${intensity_r} ${grid_size_1d}.${intensity_g} ${grid_size_1d}.${intensity_b}" ${output_dir}/${output_root_name}.ppm ${diffuser_type} ${scanner_type} ${palette_file_name};

set background_color = "0 0 0";
set dimension = 2;
set domain_size = "1 1";
set dot_radius = 0.02;

./src/Main/Release/ExtractDiscreteSamples ${output_dir}/${output_root_name}.ppm ${grid_size_1d} ${background_color} ${palette_file_name} > ${output_dir}/${output_root_name}.txt;

foreach name (${output_root_name})

src/Main/Release/DrawSamples ${output_dir}/${name}.txt ${output_dir}/${name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${name}.fig
epstopdf ${output_dir}/${name}.eps --outfile ${output_dir}/${name}.pdf

end # foreach name

endif

# R1G2B1 zoneplate
if(0) then

set grid_size_1d = 40;
set num_samples = "400 800 400";

set intensity_r = "25";
set intensity_g = "50";
set intensity_b = "25";

set diffuser_type = zhou;
set scanner_type = serpentine;

set input_root_name = r1g2b1_ed;

set palette_root_name = palette;
echo "1 0 0   0 1 0   0 0 1" > ${output_dir}/${palette_root_name}.txt;

set palette_file_name = ${output_dir}/${palette_root_name}.txt;
#set palette_file_name = "null";

set output_root_name = ${input_root_name}_${diffuser_type}_${scanner_type}_${grid_size_1d}x${grid_size_1d};

${halftone_dir}/src/Main/Release/ErrorDiffuse "${grid_size_1d}.${intensity_r} ${grid_size_1d}.${intensity_g} ${grid_size_1d}.${intensity_b}" ${output_dir}/${output_root_name}.ppm ${diffuser_type} ${scanner_type} ${palette_file_name};

set background_color = "0 0 0";
set dimension = 2;
set domain_size = "1 1";

./src/Main/Release/ExtractDiscreteSamples ${output_dir}/${output_root_name}.ppm ${grid_size_1d} ${background_color} ${palette_file_name} > ${output_dir}/${output_root_name}.txt;

# for tiling, must match grid_size_1d and num_samples above
if(${grid_size_1d} == 80) then
set num_tile_per_dim = 4;
set scaling = 0.25;
else if(${grid_size_1d} == 40) then
set num_tile_per_dim = 8;
set scaling = 0.125;
else
echo unknown grid_size_1d;
exit(1);
endif

set num_tiles = "${num_tile_per_dim} ${num_tile_per_dim}";

# for zone plate
set function_offset = 0;
set function_scale = 10;
set output_resolution = 320;
set filter_gaussian_std = 1.5;

set grid_size = "${grid_size_1d} ${grid_size_1d}";

foreach base_name (${output_root_name})

set output_name = ${base_name};
set output_tiling_name = ${base_name}_tiling${num_tile_per_dim};

src/Main/Release/TileSamples ${output_dir}/${output_name}.txt ${dimension} ${scaling} ${num_tiles} ${domain_size} > ${output_dir}/${output_tiling_name}.txt;

foreach class (0 1 2)
set select_ids = ${class};

src/Main/Release/SampleSelector ${output_dir}/${output_tiling_name}.txt ${dimension} ${select_ids} > ${output_dir}/${output_tiling_name}_${class}.txt

src/Sain/Release/ZonePlate ${dimension} ${output_dir}/${output_tiling_name}_${class}.txt ${output_dir}/${output_tiling_name}_${class}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

../BlueNoise/src/Main/Release/PPM2PFM ${output_dir}/${output_tiling_name}_${class}_zoneplate.ppm ${output_dir}/${output_tiling_name}_${class}_zoneplate.pfm
end

# merge
src/Sain/Release/Merge3PFM ${output_dir}/${output_tiling_name}_0_zoneplate.pfm ${output_dir}/${output_tiling_name}_1_zoneplate.pfm ${output_dir}/${output_tiling_name}_2_zoneplate.pfm ${output_dir}/${output_tiling_name}_merge_zoneplate.pfm 

echo ${output_tiling_name}_merge_zoneplate.ppm
src/Sain/Release/PFM2PPM ${output_dir}/${output_tiling_name}_merge_zoneplate.pfm ${output_dir}/${output_tiling_name}_merge_zoneplate.ppm 1 1 2

end # foreach base_name

endif
