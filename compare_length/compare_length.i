%module compare_length
%{
#include "compare_length.h"
%}

%include "typemaps.i"
%include "std_vector.i"

%template(VecInt) std::vector<int>;

int compare(const std::vector<int> vl, const std::vector<int> vr);
