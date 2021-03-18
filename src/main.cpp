#include "gfile_reader.h"

int main()
{
    GHeaderInfo header;
    
    GSIZET ret = GFileReader::readHeader("xgrid.000000.out", header);
    GFileReader::printHeader(header);

    return 0;
}
