#pragma once

#include <type_traits>
#include <memory>
#include <vector>
#include <array>
#include <iostream>

namespace filtering
{
    template<class T, class Filter>
    void match(const std::vector<T>& values, std::vector<char>& result, Filter filter)
    {
        const int valuesSize = values.size();
#pragma ivdep
#pragma vector always
        for (int i = 0; i < valuesSize; i++)
        {
            result[i] = result[i] & filter(values[i]);
        }
    }

    template<class Filter>
    void match<std::array<char, 32>, Filter>(const std::vector<std::array<char, 32>>& values, std::vector<char>& result, Filter filter)
    {
        const char* pValues = reinterpret_cast<const char*>(values.data());
        int counter = 0;
        const int valuesSize = values.size();
        // Inner loop is already vectorized, no need for #pragma vector here
        for (int i = 0; i < valuesSize * 32; i += 32, counter++)
        {
            result[counter] = result[counter] & filter(pValues + i);
        }
    }

    template<class T>
    class IFilter
    {
    public:
        using Ptr = std::shared_ptr<IFilter>;

        virtual ~IFilter() = default;

        virtual void match(const std::vector<T>& values, std::vector<char>& result) const = 0;
    };

    template<class T>
    class ISingleValueFilter : public IFilter<T>
    {
    public:
        ISingleValueFilter(const T& value)
            : IFilter(), m_value{ value } {}

    protected:
        T m_value;
    };

    template<>
    class ISingleValueFilter<std::array<char, 32>> : public IFilter<std::array<char, 32>>
    {
    public:
        ISingleValueFilter(const std::array<char, 32>& value)
            : IFilter(), m_array(value), m_value{ m_array.data() } {}

    protected:
        const std::array<char, 32> m_array;
        const char* m_value;
    };

    template<class T>
    class EqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        EqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value == m_value;
            });
        }
    };

    template<>
    class EqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
    public:
        EqualsFilter(const std::array<char, 32>& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<std::array<char, 32>>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                bool result = true;
#pragma ivdep
#pragma vector always
                for (int i = 0; i < 32; i++)
                {
                    if (value[i] != m_value[i])
                    {
                        result = false;
                    }
                }

                return result;
            });
        }
    };

    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        NotEqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value != m_value;
            });
        }
    };

    template<>
    class NotEqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
    public:
        NotEqualsFilter(const std::array<char, 32>& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<std::array<char, 32>>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                bool result = false;
#pragma ivdep
#pragma vector always
                for (int i = 0; i < 32; i++)
                {
                    if (value[i] != m_value[i])
                    {
                        result = true;
                    }
                }

                return result;
            });
        }
    };

    template<class T>
    class GreaterFilter final : public ISingleValueFilter<T>
    {
    public:
        GreaterFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value > m_value;
            });
        }
    };

    template<class T>
    class GreaterOrEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        GreaterOrEqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value >= m_value;
            });
        }
    };

    template<class T>
    class LessFilter final : public ISingleValueFilter<T>
    {
    public:
        LessFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value < m_value;
            });
        }
    };

    template<class T>
    class LessOrEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        LessOrEqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<char>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value <= m_value;
            });
        }
    };

}
