/*
  convert a cg source file into a c++ string

  Li-Yi Wei
  12/17/2007

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    ifstream input(input_file_name);

    if(! input)
    {
        cerr << "cannot open " << input_file_name << " for read" << endl;
        return 1;
    }

    ofstream output(output_file_name);
    if(! output)
    {
        cerr << "cannot open " << output_file_name << " for write" << endl;
        return 1;
    }
    
    const int BUF_SIZE = 512;

    char buf[BUF_SIZE];

    while(input.good())
    {
        input.getline(buf, BUF_SIZE);
        if(input.good())
        {
            {
                for(int i = 0; (i < strlen(buf)) && (i < BUF_SIZE); i++)
                {
                    if((buf[i] == '/') && (i < strlen(buf)-1) && (i < BUF_SIZE-1) && (buf[i+1] == '/'))
                    {
                        buf[i] = buf[i+1] = '\0';
                        break;
                    }
                }
            }
            
            output << "\"" << buf << "\\n\"" << endl;
        }   
    }
    
    // done
    return 0;
}
