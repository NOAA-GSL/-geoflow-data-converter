#include "gfile_reader.h"

#define TEST_FILE "xgrid.000000.out"

int main()
{
    GFileReader xgridFile(TEST_FILE);
    xgridFile.readData(TEST_FILE);
    xgridFile.printHeader();
    //xgridFile.printData();

    /*GHeaderInfo ygridHeader;
    GFileReader::readHeader("ygrid.000000.out", ygridHeader);
    GFileReader::printHeader(ygridHeader);*/

    /*ygridHeader = GFileReader::readHeader("ygrid.000000.ou");
    GFileReader::printHeader(ygridHeader);

    ygridHeader = GFileReader::readHeader("ygrid.000000.ou");
    ygridHeader.printHeader();*/

    return 0;
}
