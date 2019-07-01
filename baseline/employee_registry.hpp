#pragma once

#include "employee.hpp"
#include "filter.hpp"

#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace filtering
{
    class IEmployeeRegistry
    {
    public:
        using Ptr = std::unique_ptr<IEmployeeRegistry>;

        static Ptr load(std::string file);

        virtual std::vector<Employee> filter(
            IFilter<std::string>::Ptr nameFilter,
            IFilter<std::string>::Ptr positionFilter,
            IFilter<int>::Ptr ageFilter,
            IFilter<float>::Ptr salaryFilter) const = 0;
        virtual void add(Employee employee) = 0;
    };
}
