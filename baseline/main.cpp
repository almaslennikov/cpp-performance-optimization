#include "employee_registry.hpp"

#include <chrono>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace filtering;

    auto start = std::chrono::high_resolution_clock::now();

    auto registry = IEmployeeRegistry::load("C:\\Users\\Alexander-PC\\Documents\\cppconf-piter-2019-optimization\\employees.csv");

    auto beforeFiltering = std::chrono::high_resolution_clock::now();
    std::cout << "Initialization finished. Time: " << 
        std::chrono::duration_cast<std::chrono::milliseconds>(beforeFiltering - start).count() << "\n";

    auto interns = registry->filter({ 
        [](const Employee& employee)
    {
        return employee.position == "Intern";
    } 
    });

    auto managers = registry->filter({
        [](const Employee& employee)
    {
        return employee.position == "Manager";
    }
    });

    auto oldPeople = registry->filter({
        [](const Employee& employee)
    {
        return employee.age > 55;
    }
    });

    auto youngPeople = registry->filter({
        [](const Employee& employee)
    {
        return employee.age < 30;
    }
    });

    auto highIncome = registry->filter({
        [](const Employee& employee)
    {
        return employee.salary > 250000.0;
    }
    });

    auto mediumIncome = registry->filter({
        [](const Employee& employee)
    {
        return employee.salary < 250000.0 && employee.salary > 100000.0;
    }
    });

    auto lowIncome = registry->filter({
        [](const Employee& employee)
    {
        return employee.salary < 100000.0;
    }
    });

    auto finish = std::chrono::high_resolution_clock::now();

    std::cout << "Filtering finished. Time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - beforeFiltering).count() << "\n";
    std::cout << "Total time: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "\n";
}