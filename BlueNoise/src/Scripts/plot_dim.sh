#!/bin/tcsh -f

foreach j ('' dart_ white_)
foreach i (3d 4d 5d 6d)
 ./txt2dat.exe data/${j}${i}_mean.txt data/${j}${i}_variance.txt data/white_${i}_mean.txt data/white_${i}_variance.txt data/${j}${i}_mean.dat data/${j}${i}_variance.dat
end
end

wgnuplot plot_dim.gp
