#pragma once

#include "employee.hpp"
#include "filter.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <array>

namespace filtering
{
    class IEmployeeRegistry
    {
    public:
        using Ptr = std::unique_ptr<IEmployeeRegistry>;

        static Ptr load(std::string file);

        virtual std::vector<size_t> filter(
            IFilter<std::array<char, 32>>::Ptr nameFilter,
            IFilter<std::array<char, 32>>::Ptr positionFilter,
            IFilter<int>::Ptr ageFilter,
            IFilter<float>::Ptr salaryFilter) const = 0;
    };
}
