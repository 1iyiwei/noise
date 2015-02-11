This is my code for SIGGRAPH 2011 paper "Differential Domain Analysis for Non-uniform Sampling", co-authored by Li-Yi Wei (me) and Rui Wang.

Basically, Rui did the surface sampling code and I did the rest. And you can find my portion of the code here.

**** Access ****
I put the code under a public svn (subversion) repository. This is easier for me to maintain than a binary zip file. Simply perform the following to get my source code:

svn co http://svn.liyiwei.org/public/BlueMath

**** Compilation ****
I compiled the code via Cmake, just run cnmake.bat (for Windows) or cgmake.sh (for linux) under the build directory.

**** Run ****
Take a look at src/Scripts/regress.sh to see examples on how to use the code.
I am using cygwin + windows.

**** Disclaimer ****
You are on your own if you use my code. I am pretty confident about my code quality, but I cannot guarantee anything.



