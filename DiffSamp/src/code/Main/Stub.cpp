/*
  Stub.cpp

  doing nothing except for source project initialization

  Li-Yi Wei
  11/12/2008

*/

#include <iostream>
using namespace std;

#include "Exception.hpp"

int Main(int argc, char **argv)
{
    throw Exception("doing nothing!");
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
