#include "gfile_reader.h"

#define TEST_FILE "xgrid.000000.out"

int main()
{
    GFileReader<GDOUBLE> xgridFile(TEST_FILE);
    xgridFile.printHeader();
    xgridFile.printData();

    return 0;
}
