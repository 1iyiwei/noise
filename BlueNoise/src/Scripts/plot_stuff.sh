#!/bin/tcsh -f

foreach i (dart bridson gpu white)
 ./txt2dat.exe data/${i}_mean.txt data/${i}_variance.txt data/white_mean.txt data/white_variance.txt data/${i}_mean.dat data/${i}_variance.dat
end

wgnuplot plot_stuff.gp
