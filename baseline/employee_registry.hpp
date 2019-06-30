#pragma once

#include "employee.hpp"

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

        virtual std::vector<Employee> filter(const std::vector<Filter>& filters) const = 0;
        virtual void add(Employee employee) = 0;
    };
}
