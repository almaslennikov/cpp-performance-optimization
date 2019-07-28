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
            IFilter<std::array<char, 32>>::Ptr nameFilter,
            IFilter<std::array<char, 32>>::Ptr positionFilter,
            IFilter<int>::Ptr ageFilter,
            IFilter<float>::Ptr salaryFilter) const override;

    private:
        std::vector<std::array<char, 32>> m_names;
        std::vector<std::array<char, 32>> m_positions;
        std::vector<int> m_ages;
        std::vector<float> m_salaries;
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

            std::array<char, 32> name;
            name.fill(0);

            std::array<char, 32> position;
            position.fill(0);

            int age;
            float salary;

            std::getline(lineStream, cell, ',');
            std::move(std::begin(cell), cell.size() < 32 ? cell.end() : std::begin(cell) + 32, name.begin());

            std::getline(lineStream, cell, ',');
            std::move(std::begin(cell), cell.size() < 32 ? cell.end() : std::begin(cell) + 32, position.begin());

            std::getline(lineStream, cell, ',');
            age = std::stoi(std::move(cell));

            std::getline(lineStream, cell, ',');
            salary = std::stof(std::move(cell));

            m_names.emplace_back(std::move(name));
            m_positions.emplace_back(std::move(position));
            m_ages.emplace_back(age);
            m_salaries.emplace_back(salary);
        }
    }

    std::vector<size_t> EmployeeRegistry::filter(
        IFilter<std::array<char, 32>>::Ptr nameFilter,
        IFilter<std::array<char, 32>>::Ptr positionFilter,
        IFilter<int>::Ptr ageFilter,
        IFilter<float>::Ptr salaryFilter) const
    {
        std::vector<bool> matched(m_names.size(), true);

        if (nameFilter)
        {
            nameFilter->match(m_names, matched);
        }

        if (positionFilter)
        {
            positionFilter->match(m_positions, matched);
        }

        if (ageFilter)
        {
            ageFilter->match(m_ages, matched);
        }

        if (salaryFilter)
        {
            salaryFilter->match(m_salaries, matched);
        }

        std::vector<size_t> result;
        result.reserve(matched.size());

        for (size_t i = 0; i < m_names.size(); i++)
        {
            if (matched[i])
            {
                result.push_back(i);
            }
        }

        return result;
    }
}