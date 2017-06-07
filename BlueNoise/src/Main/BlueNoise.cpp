/*
  BlueNoise.cpp

  Li-Yi Wei
  06/15/2007
  07/08/2007 : content migration to Main/SFT.cpp

*/

#include <iostream>
using namespace std;

#include "Exception.hpp"

int Main(int argc, char **argv)
{
    throw Exception("not yet done!");
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
