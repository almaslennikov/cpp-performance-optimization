#pragma once

#include "filter.hpp"

#include <list>
#include <string>
#include <variant>
#include <unordered_map>

namespace filtering
{
    enum class Types
    {
        String,
        Int,
        Float
    };
    using FilterType = std::variant<IFilter<std::string>::Ptr, IFilter<int>::Ptr, IFilter<float>::Ptr>;

    class FilterLoader final
    {
    public:
        FilterLoader(std::unordered_map<std::string, Types>);

        std::list<std::unordered_map<std::string, FilterType>> load(std::string filepath);
    
    private:
        std::unordered_map<std::string, Types> m_fields;
    };
}