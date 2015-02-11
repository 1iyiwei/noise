#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set blue_dir = ../BlueNoise;
set output_dir = data;
set huge_dir = data/huge;

set surface_output_dir = ${output_dir}/surface;

# conversion from rui file
if(0) then

foreach method (wn pd apd)

if(${method} == "pd") then
set models = "bunny eight genus3 hand max venus";
else if(${method} == "wn") then
set models = "bunny eight hand";
else if(${method} == "apd") then
set models = "genus3 max venus";
else
echo unknown method;
exit(1);
endif

foreach model (${models})

echo ${model}_${method};

./src/Main/Release/Rui2F2M ${surface_output_dir}/${model}_${method}.pfm ${surface_output_dir}/${model}_${method}.f2m

./src/Main/Release/F2M2PFM ${surface_output_dir}/${model}_${method}.f2m ${surface_output_dir}/${model}_${method}.f3m

./src/Main/Release/PFM2PPM ${surface_output_dir}/${model}_${method}.f3m ${surface_output_dir}/${model}_${method}.ppm 1 1 0

end
end

endif
