#ifndef PLOT_H
#define PLOT_H

#include "common.h"

#include <string>

struct PlotPoint {
    int64_t x_axis;
    int64_t y_axis;
};

struct PlotData {
    std::vector<PlotPoint> data;
    std::string x_name;
    std::string x_unit;
    std::string y_name;
    std::string y_unit;
};

class Plotter {
public:
    void do_plot(PlotData const&);
};

#endif
