#pragma once

#include <type_traits>
#include <memory>
#include <vector>
#include <array>
#include <iostream>
#include <functional>
#include <immintrin.h>

namespace filtering
{
    template<class T, class Filter>
    struct Match
    {
        constexpr Match() = default;

        inline void operator()(const std::vector<T> &values, std::vector<int> &result, Filter filter) const noexcept
        {
            const int valuesSize = values.size();
#pragma ivdep
#pragma vector always
            for (int i = 0; i < valuesSize; i++)
            {
                result[i] = result[i] & filter(values[i]);
            }
        }
    };

    template<class Filter>
    struct Match<std::array<char, 32>, Filter>
    {
        constexpr Match() = default;

        inline void operator()(const std::vector<std::array<char, 32>> &values,
                               std::vector<int> &result,
                               Filter filter) const noexcept
        {
            const char *pValues   = reinterpret_cast<const char *>(values.data());
            int        counter    = 0;
            const int  valuesSize = values.size();

            // Inner loop is already vectorized, no need for #pragma vector here
            for (int i = 0; i < valuesSize * 32; i += 32, counter++)
            {
                result[counter] = result[counter] & filter(pValues + i);
            }
        }
    };

    template<class T>
    class IFilter
    {
    public:
        using Ptr = std::shared_ptr<IFilter>;

        virtual ~IFilter() = default;

        virtual void match(const std::vector<T> &values, std::vector<int> &result) const = 0;
    };

    template<class T>
    class ISingleValueFilter : public IFilter<T>
    {
    public:
        explicit ISingleValueFilter(const T &value)
                : IFilter<T>(), m_value{value}
        {}

    protected:
        T m_value;
    };

    template<>
    class ISingleValueFilter<std::array<char, 32>> : public IFilter<std::array<char, 32>>
    {
    public:
        explicit ISingleValueFilter(const std::array<char, 32> &value)
                : IFilter(), m_array(value), m_value{m_array.data()}
        {}

    protected:
        const std::array<char, 32> m_array;
        const char                 *m_value;
    };
}

#include "filter_less.cxx"
#include "filter_less_or_equals.cxx"
#include "filter_equals.cxx"
#include "filter_not_equals.cxx"
#include "filter_greater_or_equals.cxx"
#include "filter_greater.cxx"
