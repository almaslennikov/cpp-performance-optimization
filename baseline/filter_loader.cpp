#include "filter_loader.hpp"

#include <fstream>
#include <sstream>

namespace filtering
{
    template<class T>
    T parseString(std::string str)
    {
        static_assert("Function is not implemented");
    }

    template<>
    std::string parseString(std::string str)
    {
        return str;
    }

    template<>
    int parseString(std::string str)
    {
        return std::stoi(str);
    }

    template<>
    float parseString(std::string str)
    {
        return std::stof(str);
    }

    template<class T>
    std::shared_ptr<IFilter<T>> parseFilter(std::string filterStr)
    {
        if (filterStr.size() <= 1) return nullptr;
        if (filterStr[0] == '>')
        {
            if (filterStr[1] == '=' && filterStr.size() > 2)
            {
                auto value = parseString<T>({ filterStr.begin() + 2, filterStr.end() });
                auto greaterFilter = std::make_shared<GreaterFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
                auto equalsFilter = std::make_shared<EqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
                return std::shared_ptr<IFilter<T>>(new AnyFilter<T>({ greaterFilter, equalsFilter }));
                //return std::make_shared<AnyFilter<T>>({ greaterFilter, equalsFilter });
            }
            else
            {
                return std::make_shared<GreaterFilter<T>>(parseString<T>({ filterStr.begin() + 1, filterStr.end() }));
            }
        }
        else if (filterStr[0] == '<')
        {
            if (filterStr[1] == '=' && filterStr.size() > 2)
            {
                auto value = parseString<T>({ filterStr.begin() + 2, filterStr.end() });
                auto lessFilter = std::make_shared<LessFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
                auto equalsFilter = std::make_shared<EqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
                return std::shared_ptr<IFilter<T>>(new AnyFilter<T>({ lessFilter, equalsFilter }));
            }
            else
            {
                return std::make_shared<LessFilter<T>>(parseString<T>({ filterStr.begin() + 1, filterStr.end() }));
            }
        }
        else if (filterStr.size() <= 2) return nullptr;
        else if (filterStr[0] == '=' && filterStr[1] == '=')
        {
            return std::make_shared<EqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
        }
        else if (filterStr[0] == '!' && filterStr[1] == '=')
        {
            return std::make_shared<NotEqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
        }

        return nullptr;
    }

    FilterLoader::FilterLoader(std::unordered_map<std::string, Types> fields)
        :m_fields(fields) {}

    std::list<std::unordered_map<std::string, FilterType>> FilterLoader::load(std::string filepath)
    {
        std::ifstream csv(std::move(filepath));
        std::list<std::unordered_map<std::string, FilterType>> result;

        for (std::string line; std::getline(csv, line);)
        {
            std::unordered_map<std::string, FilterType> query;

            std::stringstream lineStream(line);
            std::string cell;

            while (std::getline(lineStream, cell, ','))
            {
                // If encountered next field section
                if (cell.size() > 2 && cell.front() == '"' && cell.back() == '"')
                {
                    std::string fieldName{ cell.begin() + 1, cell.end() - 1 };
                    FilterType filter;

                    auto it = m_fields.find(fieldName);
                    if (it != m_fields.end())
                    {
                        auto type = it->second;

                        std::getline(lineStream, cell, ',');

                        bool anyFilter = cell.find('|') != std::string::npos;
                        bool allFilter = cell.find('&') != std::string::npos;
                        if (anyFilter && allFilter)
                        {
                            // Unsupported filter type
                            continue;
                        }
                        else if (!anyFilter && !allFilter)
                        {
                            if (type == Types::Int)
                            {
                                filter = parseFilter<int>(cell);
                            }
                            else if (type == Types::Float)
                            {
                                filter = parseFilter<float>(cell);
                            }
                            else if (type == Types::String)
                            {
                                filter = parseFilter<std::string>(cell);
                            }
                            query[fieldName] = filter;
                        }
                        else
                        {
                            // TODO
                        }
                    }
                    else
                    {
                        // Unknown field, continue
                        continue;
                    }
                }
            }

            result.push_back(query);
        }
        return result;
    }
    //std::vector<GroupFilter> loadFilters(std::string file)
    //{
    //    std::ifstream csv(std::move(file));
    //    std::vector<GroupFilter> result;

    //    for (std::string line; std::getline(csv, line);)
    //    {
    //        std::stringstream lineStream(line);
    //        std::string cell;

    //        std::vector<IFilter<std::string>::Ptr> nameFilters;
    //        std::vector<IFilter<std::string>::Ptr> positionFilters;
    //        std::vector<IFilter<int>::Ptr> ageFilters;
    //        std::vector<IFilter<float>::Ptr> salaryFilters;

    //        std::getline(lineStream, cell, ',');
    //        if (cell == "name")
    //        {
    //            std::vector<std::string> names;
    //            for (; std::getline(lineStream, cell, ',');)
    //            {
    //                if (cell == "position") break;
    //                names.emplace_back(std::move(cell));
    //            }

    //            if (names.size() == 1)
    //            {
    //                nameFilters.emplace_back(std::make_shared<EqualsFilter<std::string>>(names.front()));
    //            }
    //            if (names.size() > 1)
    //            {

    //                nameFilters.emplace_back(createMultiValueFilter<EqualsAnyFilter<std::string>, std::string>(names.begin(), names.end()));
    //            }
    //        }

    //        if (cell == "position")
    //        {
    //            std::vector<std::string> positions;
    //            for (; std::getline(lineStream, cell, ',');)
    //            {
    //                if (cell == "age") break;
    //                positions.emplace_back(std::move(cell));
    //            }

    //            if (positions.size() == 1)
    //            {
    //                positionFilters.emplace_back(std::make_shared<EqualsFilter<std::string>>(positions.front()));
    //            }
    //            if (positions.size() > 1)
    //            {

    //                positionFilters.emplace_back(createMultiValueFilter<EqualsAnyFilter<std::string>, std::string>(positions.begin(), positions.end()));
    //            }
    //        }

    //        if (cell == "age")
    //        {
    //            for (; std::getline(lineStream, cell, ',');)
    //            {
    //                if (cell == "salary") break;
    //                if (cell.size() <= 1) continue;
    //                if (cell[0] == '>')
    //                {
    //                    if (cell[1] == '=' && cell.size() > 2)
    //                    {
    //                        // TODO implement GreaterOrEqualsFilter
    //                        // ageFilters.push_back(createSingleValueFilter<GreaterOrEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 2, cell.end()))));
    //                    }
    //                    else
    //                    {
    //                        ageFilters.push_back(createSingleValueFilter<GreaterFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
    //                    }
    //                }
    //                else if (cell[0] == '<')
    //                {
    //                    if (cell[1] == '=' && cell.size() > 2)
    //                    {
    //                        // TODO implement LessOrEqualsFilter
    //                        // ageFilters.push_back(createSingleValueFilter<LessrOrEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 2, cell.end()))));
    //                    }
    //                    else
    //                    {
    //                        ageFilters.push_back(createSingleValueFilter<LessFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
    //                    }
    //                }
    //                else if (cell.size() <= 2) continue;
    //                else if (cell[1] == '=' && cell[2] == '=')
    //                {
    //                    ageFilters.push_back(createSingleValueFilter<EqualsFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
    //                }
    //                else if (cell[1] == '!' && cell[2] == '=')
    //                {
    //                    ageFilters.push_back(createSingleValueFilter<NotEqualsFilter<int>>(std::stoi(std::string(cell.begin() + 1, cell.end()))));
    //                }
    //            }
    //        }

    //        if (cell == "salary")
    //        {
    //            for (; std::getline(lineStream, cell, ',');)
    //            {
    //                if (cell.size() <= 1) continue;
    //                if (cell[0] == '>')
    //                {
    //                    if (cell[1] == '=' && cell.size() > 2)
    //                    {
    //                        // TODO implement GreaterOrEqualsFilter
    //                        // ageFilters.push_back(createSingleValueFilter<GreaterOrEqualsFilter<float>>(std::stof(std::string(cell.begin() + 2, cell.end()))));
    //                    }
    //                    else
    //                    {
    //                        salaryFilters.push_back(createSingleValueFilter<GreaterFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
    //                    }
    //                }
    //                else if (cell[0] == '<')
    //                {
    //                    if (cell[1] == '=' && cell.size() > 2)
    //                    {
    //                        // TODO implement LessOrEqualsFilter
    //                        // ageFilters.push_back(createSingleValueFilter<LessrOrEqualsFilter<float>>(std::stof(std::string(cell.begin() + 2, cell.end()))));
    //                    }
    //                    else
    //                    {
    //                        salaryFilters.push_back(createSingleValueFilter<LessFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
    //                    }
    //                }
    //                else if (cell.size() <= 2) continue;
    //                else if (cell[1] == '=' && cell[2] == '=')
    //                {
    //                    salaryFilters.push_back(createSingleValueFilter<EqualsFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
    //                }
    //                else if (cell[1] == '!' && cell[2] == '=')
    //                {
    //                    salaryFilters.push_back(createSingleValueFilter<NotEqualsFilter<float>>(std::stof(std::string(cell.begin() + 1, cell.end()))));
    //                }
    //            }
    //        }

    //        GroupFilter filter{ nameFilters, positionFilters, ageFilters, salaryFilters };

    //        result.emplace_back(std::move(filter));
    //    }

    //    return result;
    //}
}