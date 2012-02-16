#!/bin/tcsh -f

# Lenna and Mandril
# both from http://en.wikipedia.org/wiki/Standard_test_image
# monnier2 from
# http://scienceblogs.com/cognitivedaily/upload/2009/02/visual_illusion_may_explain_th/monnier2.jpg
# i.e. the upper left corner of Seurat's Sunday Afternoon at La Grande Jatte
# Seurat_SundayAfternoonGrandJatte from
# http://www.ibm.com/developerworks/java/library/j-j2d/
if(0) then

set outputdir = data;

set generate_samples = 0;

set dimension = 2;

#set num_classes = 3;

set r_value = 02; 
set dot_radius = 0.004; # about 1/5 #0.0${r_value};
set dot_text = "";

if(0) then
set r_value = 01;
set dot_radius = 0.002;
endif

if(0) then
set r_value = 005;
set dot_radius = 0.001;
endif

if(0) then # for 3 class
set r_value = 002;
set dot_radius = 0.00051;
#set dot_radius = 0.001;
endif

if(0) then # for 6 and 7 class
set r_value = 003;
set dot_radius = 0.0006;
set dot_text = 0p0006;
#set dot_radius = 0.0008;
#set dot_text = 0p0008;
set dot_radius = 0.001;
set dot_text = 0p001;
#set dot_radius = 0.00051;
endif

if(0) then # for 6 and 7 class
set r_value = 004;
set dot_radius = 0.0008;
set dot_text = 0p0008;
#set dot_radius = 0.001;
#set dot_radius = 0.00051;
endif

set k_value = 64.67; #16;
set domain_size = "1 1";

set conflict_metric = 0; # mean

#set field_name = "Lenna";
#set field_name = "Mandril";
#set field_name = "lena";

foreach field_name ("Monarch") #("Seurat_SundayAfternoonGrandJatte") # ("monnier2") #("Lenna") # ("Mandril") ("Monarch") ("Pepper") ("van_gogh_bandaged")

if(${field_name} == "s2010_logo_ctr") then
set domain_size = "1 1.44676806";
echo s2010_logo_ctr is a dangerous image;
exit(1);
else if(${field_name} == "Seurat_SundayAfternoonGrandJatte") then
set domain_size = "1 1.5";
endif

foreach negation (0 1) #(1 0)

foreach num_classes (7) #(6 7 3 4) 

set priority = "";
set r_values = "";
set field_names = "";

set class = 0;
while(${class} < ${num_classes})
set priority = "${priority} 0";
set r_values = "${r_values} 0.${r_value}";
set field_names = "${field_names} ${outputdir}/${field_name}_${class}.pfm";

@ class = ${class} + 1;
end

set r_text = 0p${r_value};

if((${num_classes} == 3) || (${num_classes} == 6) || (${num_classes} == 7)) then

if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
convert ${outputdir}/${field_name}.ppm -negate -compress none ${conversion_source}
endif

else if(${num_classes} == 4) then

if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -colorspace CMYK ${conversion_source}
else
set conversion_source = ${outputdir}/${field_name}_negate.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -negate -colorspace CMYK ${conversion_source}
endif

else
echo unknown number of classes
exit(1);
endif

set class = 0;
while(${class} < ${num_classes})

if((${num_classes} == 3) || (${num_classes} == 6) || (${num_classes} == 7)) then

if(${class} == 0) then
set channel = 'R';
else if(${class} == 1) then
set channel = 'G';
else if(${class} == 2) then
set channel = 'B';
else
set channel = '';
endif

else if(${num_classes} == 4) then

if(${class} == 0) then
set channel = 'C';
else if(${class} == 1) then
set channel = 'M';
else if(${class} == 2) then
set channel = 'Y';
else if(${class} == 3) then
set channel = 'K';
else
set channel = '';
endif

else
echo unknown number of classes;
exit(1);
endif

if(${channel} != '') then
convert -separate -channel ${channel} -compress none ${conversion_source} ${outputdir}/${field_name}_${class}.ppm
./src/Sain/Release/PPM2PFM ${outputdir}/${field_name}_${class}.ppm ${outputdir}/${field_name}_${class}.pfm
endif

@ class = ${class} + 1;
end # while

if(${num_classes} > 4) then

if(${num_classes} == 6) then
if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0";
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
set output_basis = "0 1 1  1 0 1  1 1 0  1 0 0  0 1 0  0 0 1";
endif
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0";
else if(${num_classes} == 7) then
if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0  1 1 1";
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
set output_basis = "0 1 1  1 0 1  1 1 0  1 0 0  0 1 0  0 0 1  0 0 0";
endif
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0  1 1 1";
else
echo unknown number of classes > 4;
exit(1);
endif

./src/Sain/Release/PPM2PFM ${conversion_source} ${outputdir}/${field_name}.pfm;
./src/Main/Release/ColorTransform ${outputdir}/${field_name}.pfm ${num_classes} ${output_basis} ${field_names}
endif
# echo debug; exit(1);

set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;

set output_name = adaptive_${field_name}_${dimension}d_${num_classes}c_${r_text}_n${negation};

set output_name_all = ${output_name_exe}_${output_name}

if(${dot_text} == "") then
set output_name_plot = ${output_name_all};
else
set output_name_plot = ${output_name_exe}_adaptive_${field_name}_${dimension}d_${num_classes}c_${r_text}_dot${dot_text}_n${negation};
endif

if(${generate_samples}) then
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names}
time src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names} > ${outputdir}/${output_name_all}.txt
endif

set color_palette = ${outputdir}/${field_name}_color_palette.txt

if(${num_classes} == 3) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 4) then

if(${negation} == 0) then
echo "0 255 255  255 0 255  255 255 0  0 0 0" > ${color_palette}
set background = 1;
else
echo "255 0 0  0 255 0  0 0 255  255 255 255" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 6) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255  0 255 255  255 0 255  255 255 0" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0  255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 7) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255  0 255 255  255 0 255  255 255 0  255 255 255" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0  255 0 0  0 255 0  0 0 255  0 0 0" > ${color_palette}
set background = 1;
endif

else
echo unknown number of classes;
exit(1);
endif

set id = -1;
#while(${id} < ${num_classes})
while(${id} < 0)
if(${id} == -1) then
set output_name_final = ${output_name_all};
else
set output_name_final = ${output_name_all}_${id};
grep "^${id}" ${outputdir}/${output_name_all}.txt > ${outputdir}/${output_name_final}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_name_final}.txt ${outputdir}/${output_name_plot}.fig ${dimension} ${dot_radius} ${background} ${domain_size} ${color_palette}
src/Scripts/fig2eps -f ${outputdir}/${output_name_plot}.fig
epstopdf ${outputdir}/${output_name_plot}.eps --outfile ${outputdir}/${output_name_plot}.pdf

@ id = ${id} + 1; 
end # while(${id}

end # foreach num_classes
end # foreach negation 
end # foreach field_name

endif
