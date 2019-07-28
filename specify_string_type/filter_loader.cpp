#include "filter_loader.hpp"

#include <fstream>
#include <sstream>
#include <array>

namespace filtering
{
    template<class T>
    T parseString(std::string str)
    {
        static_assert("Function is not implemented");
    }

    template<>
    std::array<char, 32> parseString(std::string str)
    {
        std::array<char, 32> result;
        result.fill(0);
        std::move(std::begin(str), str.size() < 32 ? str.end() : std::begin(str) + 32, result.begin());
        return result;
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
                return std::make_shared<GreaterOrEqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
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
                return std::make_shared<LessOrEqualsFilter<T>>(parseString<T>({ filterStr.begin() + 2, filterStr.end() }));
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
                                filter = parseFilter<std::array<char, 32>>(cell);
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
}