#include "gfile_reader.h"
//#include "gnode.h"

#define GDATATYPE GDOUBLE

#define FILE_XGRID "data/xgrid.000000.out"
#define FILE_YGRID "data/ygrid.000000.out"
#define FILE_ZGRID "data/zgrid.000000.out"
#define FILE_VAL "data/dtotal.000000.out"

int main()
{
    GFileReader<GDATATYPE> xgrid(FILE_XGRID);
    GFileReader<GDATATYPE> ygrid(FILE_YGRID);
    GFileReader<GDATATYPE> zgrid(FILE_ZGRID);
    GFileReader<GDATATYPE> val(FILE_VAL);

    GHeaderInfo h = xgrid.header();
    GFileReader<GDATATYPE>::printHeader(h);

    /*vector<GNode<GDATATYPE> > nodes;
    for (auto i = 0u; i < h.nNodes; ++i)
    {
        nodes.push_back(GNode<GDATATYPE>(xgrid.data()[i],
                                         ygrid.data()[i], 
                                         zgrid.data()[i],
                                         val.data()[i]));
    }

    for (auto i = 0u; i < h.nNodes; ++i)
    {
        nodes[i].printNode();
    }*/

    return 0;
}
