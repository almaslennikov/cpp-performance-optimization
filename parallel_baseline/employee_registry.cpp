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
            IFilter<std::string>::Ptr nameFilter,
            IFilter<std::string>::Ptr positionFilter,
            IFilter<int>::Ptr ageFilter,
            IFilter<float>::Ptr salaryFilter) const override;
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
        IFilter<std::string>::Ptr nameFilter,
        IFilter<std::string>::Ptr positionFilter,
        IFilter<int>::Ptr ageFilter,
        IFilter<float>::Ptr salaryFilter) const
    {
        std::vector<Employee> result;

        std::copy_if(
            m_employees.begin(),
            m_employees.end(),
            std::back_inserter(result),
            [&](const Employee& employee)
        {
            bool match = true;

            if (nameFilter)
            {
                match &= nameFilter->match(employee.name);
            }
            if (positionFilter)
            {
                match &= positionFilter->match(employee.position);
            }
            if (ageFilter)
            {
                match &= ageFilter->match(employee.age);
            }
            if (salaryFilter)
            {
                match &= salaryFilter->match(employee.salary);
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