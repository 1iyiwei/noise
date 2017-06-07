/*
  summarize the k-statistics table for paper

  Li-Yi Wei
  09/09/2007

*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>

string FileName(const string & root_dir, const int dim, const int k)
{
    string result;
    
    {
        ostringstream file_name;

        file_name << root_dir << "/adaptive_uniform_" << dim << "dim_k" << k << "_statistics.txt";
        result = file_name.str();
    }

    return result;
}

double Read(const string & root_dir, const int dim, const int k)
{
    double total = 0;
    int count = 0;
            
    const string file_name = FileName(root_dir, dim, k);

    ifstream input(file_name.c_str());

    if(!input)
    {
        cerr << "cannot open " << file_name << endl;
        return 1;
    }

    while(input.good())
    {
        int foo;
        string bar1, bar2, bar3, bar4;
        // input >> "total # of samples " >> foo;
        input >> bar1 >> bar2 >> bar3 >> bar4 >> foo;
        
        if(input.good())
        {
            // cerr << file_name << " : " << bar1 << " " << bar2 << " " << bar3 << " " << bar4 << " " << foo << endl;
            
            total += foo;
            count += 1;
        }
    }

    return total/max(count, 1);
}

int main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " root_dir dim_max k_max k_inf" << endl;
        return 1;
    }

    int argCtr = 0;
    const string root_dir = argv[++argCtr];
    const int dim_max = atoi(argv[++argCtr]);
    const int k_max = atoi(argv[++argCtr]);
    const int k_inf = atoi(argv[++argCtr]);

    cerr << "root_dir: " << root_dir << endl;
    
    vector< vector<double> > statistics(dim_max+1);
    for(unsigned int i = 0; i < statistics.size(); i++)
    {
        statistics[i] = vector<double>(k_max+1);

        for(unsigned int j = 0; j < statistics[i].size(); j++)
        {
            statistics[i][j] = 0;
        }
    }

    for(int dim = 2; dim <= dim_max; dim++)
    {
        for(int k = 1; k <= k_max; k++)
        {
            statistics[dim][k] = Read(root_dir, dim, k)/Read(root_dir, dim, k_inf);
        }
    }
 
    for(int dim = 2; dim <= dim_max; dim++)
    {
        cout << dim << "D"; 
        for(int k = 1; k <= k_max; k++)
        {
            cout << " & " << floor(statistics[dim][k]*100+0.5); // << "\\%";
        }
        cout << "\\\\" << endl;
        cout << "\\hline" << endl;
    }
    
    // done
    cerr << "done" << endl;
    return 0;
}
