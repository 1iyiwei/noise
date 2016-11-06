# Code #

Someday I will switch this to cmake......

## Compilation ##

I compiled the code via Visual Studio .net 2003. The code should be compatible with later generation of Visual Studios, but I have not tried any of these.

See rainbow.sln.

## Usage ##

Invoke src/Main/Release/DartThrowing without any arguments will tell you what arguments to use.

Here is a typical usage scenario for 2 dimension, 3 category of samples, and r values (i.e. density) for the 3 categories = (0.04 0.02 0.01).

The k_value determines the number of darts being thrown.

The domain_size specifies the sampling domain size and shape.

The final set of samples will be dumped via stdout. Each sample will occupy one row: category (int) followed by n-dimensional coordinates (float)

(I am using tcsh syntax for clarity but you could easily translate it for windows.)

set dimension = 2;
set num_categories = 3;
set r_values = "0.04 0.02 0.01";
set k_value = 4;
set domain_size = "1 1.5";

src/Main/Release/DartThrowing $dimension $num_categories $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_categories}c_${r_text}.txt

Code:
Main/DartThrowing.cpp demonstrates how to use my code. It should be very simple. I plan to update my paper draft to explain this simple algorithm soon.

For other stuff, take a look at Scripts/regress.sh (regression code per glorious NVIDIA tradition).

## Disclaimer ##
You are on your own if you use my code. I am pretty confident about my code quality, but I cannot guarantee anything.

