/*
  SampleRecord.hpp

  Li-Yi Wei
  06/24/2008

*/

#include <vector>
using namespace std;

struct SampleRecord
{
    typedef enum {UNKNOWN, OUTSIDE, KILLED, REJECTED, ACCEPTED} Status;

    SampleRecord(void) : class_id(-1), status(REJECTED) {};
    SampleRecord(const int class_id_i, const Status status_i) : class_id(class_id_i), status(status_i) {};

    int class_id;
    Status status;
};

int ReadSampleHistory(const char * input_file_name, vector<SampleRecord> & result);
int WriteSampleHistory(const char * output_file_name, const vector<SampleRecord> & result);
