#!/bin/tcsh -f

if ($#argv < 1) then
 echo "please specify the input directory"
 exit(1)
endif

set input_dir = $argv[1]

cmake -DCMAKE_BUILD_TYPE=Release ${input_dir}
