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

        std::vector<size_t> filter(
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

    std::vector<size_t> EmployeeRegistry::filter(
        IFilter<std::string>::Ptr nameFilter,
        IFilter<std::string>::Ptr positionFilter,
        IFilter<int>::Ptr ageFilter,
        IFilter<float>::Ptr salaryFilter) const
    {
        std::vector<size_t> result;
        result.reserve(m_employees.size());

        for (size_t i = 0; i < m_employees.size(); i++)
        {
            bool match = true;

            if (nameFilter)
            {
                match &= nameFilter->match(m_employees[i].name);
            }
            if (positionFilter)
            {
                match &= positionFilter->match(m_employees[i].position);
            }
            if (ageFilter)
            {
                match &= ageFilter->match(m_employees[i].age);
            }
            if (salaryFilter)
            {
                match &= salaryFilter->match(m_employees[i].salary);
            }

            if (match)
            {
                result.push_back(i);
            }
        }

        return result;
    }

    void EmployeeRegistry::add(Employee employee)
    {
        m_employees.push_back(std::move(employee));
    }
}