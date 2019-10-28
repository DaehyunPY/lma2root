#ifndef _helperfunctionsforDetSorter_h_
#define _helperfunctionsforDetSorter_h_

#include <vector>

class MyLayerProperty;
class MySignalAnalyzedEvent;
class MyPeak;

void extractTimes(MySignalAnalyzedEvent &, const MyLayerProperty &, std::vector<MyPeak *> &, std::vector<double> &);

#endif