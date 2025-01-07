#include <vector>
#include <math.h>

#include "l2.h"

using namespace std;

int l2(const vector<int>& elements)
{
    int sum = 0;
    for (vector<int>::const_iterator iter = elements.begin();
            iter != elements.end();
            iter++)
    {
        sum += (*iter) * (*iter);
    }
    float sq = sqrt(sum);
    int l2 = (int)sq;
    return l2;
}
