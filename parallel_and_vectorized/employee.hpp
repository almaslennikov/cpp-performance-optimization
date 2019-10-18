#pragma once

#include <string>

namespace filtering
{
    struct Employee
    {
        std::string name;
        std::string position;
        int age;
        float salary;

        Employee(std::string p_name, std::string p_position, int p_age, float p_salary)
            : name(std::move(p_name)),
            position(std::move(p_position)),
            age(p_age),
            salary(p_salary)
        {}
    };
}