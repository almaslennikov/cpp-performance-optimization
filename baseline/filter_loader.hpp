#pragma once

#include "filter.hpp"

#include <vector>
#include <string>

namespace filtering
{
    struct GroupFilter
    {
        std::vector<IFilter<std::string>::Ptr> nameFilters;
        std::vector<IFilter<std::string>::Ptr> positionFilters;
        std::vector<IFilter<int>::Ptr> ageFilters;
        std::vector<IFilter<float>::Ptr> salaryFilters;
    };

    std::vector<GroupFilter> loadFilters(std::string file);
}