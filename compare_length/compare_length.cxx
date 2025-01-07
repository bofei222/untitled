#include <vector>
#include "l2.h"
#include "compare_length.h"

int compare(const std::vector<int>& vl, const std::vector<int>& vr)
{
    int l2_l = l2(vl);
    int l2_r = l2(vr);
    return l2_l - l2_r;
}