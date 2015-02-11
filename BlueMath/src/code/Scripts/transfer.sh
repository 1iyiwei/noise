#!/bin/tcsh -f

set rainbow_src_dir = ../../RainbowNoise/src/;

# Library
foreach file () #(PFT SFT Array FrameBuffer Counter SequentialCounter)
foreach ext (hpp cpp)

svn cp ${rainbow_src_dir}/Library/${file}.${ext} Library/

end
end

# Spectrum
foreach file () #(PFT SFT)
foreach ext (hpp cpp)

svn cp ${rainbow_src_dir}/Spectrum/${file}.${ext} Library/

end
end

# Sain
foreach file () #(SFT RadialPlot PFM2PPM)
foreach ext (cpp)

svn cp ${rainbow_src_dir}/Sain/${file}.${ext} Main/

end
end
 
