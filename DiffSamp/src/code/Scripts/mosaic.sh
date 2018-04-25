#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set blue_dir = ../BlueNoise;
set output_dir = data;
set huge_dir = data/huge;

# mosaic representative image
# 6x5 of 256x256 images
if(0) then

set output_name = mosaic;

set image_height = 256;
set image_width = 256;

set num_rows = 5;
set num_cols = 6;

@ mosaic_height = ${num_rows} * ${image_height};
@ mosaic_width = ${num_cols} * ${image_width};

convert -size ${mosaic_width}x${mosaic_height} xc:none -fill white -draw "rectangle 1, 1, ${mosaic_width}, ${mosaic_height}" ${output_dir}/${output_name}.png;

set images = "";

# uniform
foreach method (white grid0 grid1 poisson lloyd balzer ppentomino)
foreach kernel (sft diffxform_gaussian_10r_infdd_unit_unit)
set images = "${images} uniform_0p03_${method}_${kernel}_average10";
end
end

# hidden
foreach pattern (grid0 hex)
foreach spacing (5 6 7 8)
foreach kernel (sft diffxform_gaussian_10r_infdd_unit_unit)
set images = "${images} uniform_${pattern}_${spacing}_poisson_0p03_${kernel}_average10";
end
end
end

set row = 0;
set col = 0;
foreach image (${images})

@ row_offset = ${row} * ${image_height};
@ col_offset = ${col} * ${image_width};

composite -geometry ${image_width}x${image_height}+${col_offset}+${row_offset} ${output_dir}/${image}.png ${output_dir}/${output_name}.png ${output_dir}/${output_name}.png

@ col = ${col} + 1;
if(${col} >= ${num_cols}) then
@ row = ${row} + 1;
set col = 0;
endif

end

convert -quality 100 ${output_dir}/${output_name}.png ${output_dir}/${output_name}.jpg
endif
