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
        using Filter = std::function<bool(const Employee&)>;

        static Ptr load(std::string file);

        virtual std::vector<Employee> filter(
            const std::vector<IFilter<std::string>::Ptr>& nameFilters,
            const std::vector<IFilter<std::string>::Ptr>& positionFilters,
            const std::vector<IFilter<int>::Ptr>& ageFilters,
            const std::vector<IFilter<float>::Ptr>& salaryFilters) const = 0;
        virtual void add(Employee employee) = 0;
    };
}
