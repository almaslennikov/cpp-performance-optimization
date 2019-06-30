#include "employee_registry.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace filtering
{
    class EmployeeRegistry : public IEmployeeRegistry
    {
    public:
        EmployeeRegistry(std::string file);

        std::vector<Employee> filter(
            const std::vector<IFilter<std::string>::Ptr>& nameFilters,
            const std::vector<IFilter<std::string>::Ptr>& positionFilters,
            const std::vector<IFilter<int>::Ptr>& ageFilters,
            const std::vector<IFilter<float>::Ptr>& salaryFilters) const override;
        void add(Employee employee) override;

    private:
        std::vector<Employee> m_employees;
    };

    IEmployeeRegistry::Ptr IEmployeeRegistry::load(std::string file)
    {
        return std::make_unique<EmployeeRegistry>(std::move(file));
    }

    EmployeeRegistry::EmployeeRegistry(std::string file)
    {
        std::ifstream csv(file);

        for (std::string line; std::getline(csv, line);)
        {
            std::stringstream lineStream(line);
            std::string cell;

            std::string name;
            std::string position;
            int age;
            float salary;

            std::getline(lineStream, cell, ',');
            name = std::move(cell);

            std::getline(lineStream, cell, ',');
            position = std::move(cell);

            std::getline(lineStream, cell, ',');
            age = std::stoi(std::move(cell));

            std::getline(lineStream, cell, ',');
            salary = std::stof(std::move(cell));

            m_employees.emplace_back(name, position, age, salary);
        }
    }

    std::vector<Employee> EmployeeRegistry::filter(
        const std::vector<IFilter<std::string>::Ptr>& nameFilters,
        const std::vector<IFilter<std::string>::Ptr>& positionFilters,
        const std::vector<IFilter<int>::Ptr>& ageFilters,
        const std::vector<IFilter<float>::Ptr>& salaryFilters) const
    {
        std::vector<Employee> result;

        std::copy_if(
            m_employees.begin(),
            m_employees.end(),
            std::back_inserter(result),
            [&](const Employee& employee)
        {
            bool match = true;

            for (auto& filter : nameFilters)
            {
                match &= filter->match(employee.name);
            }
            for (auto& filter : positionFilters)
            {
                match &= filter->match(employee.position);
            }
            for (auto& filter : ageFilters)
            {
                match &= filter->match(employee.age);
            }
            for (auto& filter : salaryFilters)
            {
                match &= filter->match(employee.salary);
            }

            return match;
        });

        return result;
    }

    void EmployeeRegistry::add(Employee employee)
    {
        m_employees.push_back(std::move(employee));
    }
}