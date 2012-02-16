/*
  FillPenroseHole.cpp

  a hack to fill in the empty corners produced by the PenroseColoring

  Li-Yi Wei
  10/08/2008

*/

#include <iostream>
using namespace std;

#include "Exception.hpp"

int Main(int argc, char **argv)
{
    throw Exception("not needed; use the fill_by_flip flag in PenroseColoring!");
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
