#include "filter_loader.hpp"

#include <fstream>
#include <sstream>

namespace filtering
{
    std::vector<GroupFilter> loadFilters(std::string file)
    {
        std::ifstream csv(std::move(file));
        std::vector<GroupFilter> result;

        for (std::string line; std::getline(csv, line);)
        {
            std::stringstream lineStream(line);
            std::string cell;

            std::vector<IFilter<std::string>::Ptr> nameFilters;
            std::vector<IFilter<std::string>::Ptr> positionFilters;
            std::vector<IFilter<int>::Ptr> ageFilters;
            std::vector<IFilter<float>::Ptr> salaryFilters;

            std::getline(lineStream, cell, ',');
            if (cell == "name")
            {
                std::vector<std::string> names;
                for (; std::getline(lineStream, cell, ',');)
                {
                    if (cell == "position") break;
                    names.emplace_back(std::move(cell));
                }

                if (names.size() == 1)
                {
                    nameFilters.emplace_back(std::make_shared<EqualsFilter<std::string>>(names.front()));
                }
                if (names.size() > 1)
                {

                    nameFilters.emplace_back(createMultiValueFilter<EqualsAnyFilter<std::string>, std::string>(names.begin(), names.end()));
                }
            }

            if (cell == "position")
            {
                std::vector<std::string> positions;
                for (; std::getline(lineStream, cell, ',');)
                {
                    if (cell == "age") break;
                    positions.emplace_back(std::move(cell));
                }

                if (positions.size() == 1)
                {
                    positionFilters.emplace_back(std::make_shared<EqualsFilter<std::string>>(positions.front()));
                }
                if (positions.size() > 1)
                {

                    positionFilters.emplace_back(createMultiValueFilter<EqualsAnyFilter<std::string>, std::string>(positions.begin(), positions.end()));
                }
            }

            if (cell == "age")
            {
                for (; std::getline(lineStream, cell, ',');)
                {
                    if (cell == "salary") break;
                    if (cell.size() <= 1) continue;
                    if (cell[0] == '>')
                    {
                        if (cell[1] == '=' && cell.size() > 2)
                        {
                            // TODO implement GreaterOrEqualsFilter
                            // ageFilters.push_back(createSingleValueFilter<GreaterOrEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 2, cell.end()))));
                        }
                        else
                        {
                            ageFilters.push_back(createSingleValueFilter<GreaterFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
                        }
                    }
                    else if (cell[0] == '<')
                    {
                        if (cell[1] == '=' && cell.size() > 2)
                        {
                            // TODO implement LessOrEqualsFilter
                            // ageFilters.push_back(createSingleValueFilter<LessrOrEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 2, cell.end()))));
                        }
                        else
                        {
                            ageFilters.push_back(createSingleValueFilter<LessFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
                        }
                    }
                    else if (cell.size() <= 2) continue;
                    else if (cell[1] == '=' && cell[2] == '=')
                    {
                        ageFilters.push_back(createSingleValueFilter<EqualsFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
                    }
                    else if (cell[1] == '!' && cell[2] == '=')
                    {
                        ageFilters.push_back(createSingleValueFilter<NotEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
                    }
                }
            }

            if (cell == "salary")
            {
                for (; std::getline(lineStream, cell, ',');)
                {
                    if (cell.size() <= 1) continue;
                    if (cell[0] == '>')
                    {
                        if (cell[1] == '=' && cell.size() > 2)
                        {
                            // TODO implement GreaterOrEqualsFilter
                            // ageFilters.push_back(createSingleValueFilter<GreaterOrEqualsFilter<float>>(std::stof(std::string(cell.begin() + 2, cell.end()))));
                        }
                        else
                        {
                            salaryFilters.push_back(createSingleValueFilter<GreaterFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
                        }
                    }
                    else if (cell[0] == '<')
                    {
                        if (cell[1] == '=' && cell.size() > 2)
                        {
                            // TODO implement LessOrEqualsFilter
                            // ageFilters.push_back(createSingleValueFilter<LessrOrEqualsFilter<float>>(std::stof(std::string(cell.begin() + 2, cell.end()))));
                        }
                        else
                        {
                            salaryFilters.push_back(createSingleValueFilter<LessFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
                        }
                    }
                    else if (cell.size() <= 2) continue;
                    else if (cell[1] == '=' && cell[2] == '=')
                    {
                        salaryFilters.push_back(createSingleValueFilter<EqualsFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
                    }
                    else if (cell[1] == '!' && cell[2] == '=')
                    {
                        salaryFilters.push_back(createSingleValueFilter<NotEqualsFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
                    }
                }
            }

            GroupFilter filter{ nameFilters, positionFilters, ageFilters, salaryFilters };

            result.emplace_back(std::move(filter));
        }

        return result;
    }
}