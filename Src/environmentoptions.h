#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H
#include "gl_const.h"

class EnvironmentOptions
{
public:
    EnvironmentOptions(bool AS, bool AD, bool CC, int MT = CN_SP_MT_EUCL, double HW = 1., int BT = 1, int ST = CN_SP_ST_DIJK);
    EnvironmentOptions();
    int     metrictype;     //Can be chosen Euclidean, Manhattan, Chebyshev and Diagonal distance
    bool    allowsqueeze;   //Option that allows to move throught "bottleneck"
    bool    allowdiagonal;  //Option that allows to make diagonal moves
    bool    cutcorners;     //Option that allows to make diagonal moves, when one adjacent cell is untraversable
    double  hweight;        // Can be >= 1
    int     breakingties;    // Can be g-min, g-max
    int     searchtype;
};

#endif // ENVIRONMENTOPTIONS_H
