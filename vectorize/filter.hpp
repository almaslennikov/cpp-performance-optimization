#pragma once

#include <type_traits>
#include <memory>
#include <vector>
#include <array>

namespace filtering
{
    template<class T, class Filter>
    void match(const std::vector<T>& values, std::vector<bool>& result, Filter filter)
    {
        int mainLoop = values.size() / 8; //TODO calculate correct delimeter (based on uarch)
#pragma ivdep
#pragma vector always
        for (int i = 0; i < mainLoop * 8; i++) //Maybe make a point about having signed counter
        {
            result[i] = result[i] & filter(values[i]);
        }

        for (int i = mainLoop * 8; i < values.size(); i++) //Process remainder scalar
        {
            result[i] = result[i] & filter(values[i]);
        }
    }

    template<class Filter>
    void match<std::array<char, 32>, Filter>(const std::vector<std::array<char, 32>>& values, std::vector<bool>& result, Filter filter)
    {
        const char* p_values = reinterpret_cast<const char*>(values.data());
        int counter = 0;
        int mainLoop = values.size() / 8; //TODO calculate correct delimeter (based on uarch)
        for (int i = 0; i < values.size() * 32; i += 32, counter++)
        {
            result[counter] = result[counter] & filter(p_values + i);
        }
//#pragma ivdep
//#pragma vector always
//        for (int i = 0; i < mainLoop * 8 * 32; i += 32, counter++) //Maybe make a point about having signed counter
//        {
//            result[counter] = result[counter] & filter(p_values + i);
//        }
//
//        for (int i = mainLoop * 8 * 32; i < values.size() * 32; i += 32, counter++) //Process remainder scalar
//        {
//            result[counter] = result[counter] & filter(p_values + i);
//        }
    }

    template<class T>
    class IFilter
    {
    public:
        using Ptr = std::shared_ptr<IFilter>;

        virtual ~IFilter() = default;

        virtual void match(const std::vector<T>& values, std::vector<bool>& result) const = 0;
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

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
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

        void match(const std::vector<std::array<char, 32>>& values, std::vector<bool>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
#pragma ivdep
#pragma vector always
                for (int i = 0; i < 32; i++)
                {
                    if (value[i] != m_value[i])
                    {
                        return false;
                    }
                }
                return true;
            });
        }
    };

    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        NotEqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
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

        void match(const std::vector<std::array<char, 32>>& values, std::vector<bool>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
#pragma ivdep
#pragma vector always
                for (int i = 0; i < 32; i++)
                {
                    if (value[i] != m_value[i])
                    {
                        return true;
                    }
                }

                return false;
            });
        }
    };

    template<class T>
    class GreaterFilter final : public ISingleValueFilter<T>
    {
    public:
        GreaterFilter(const T& value)
            : ISingleValueFilter(value) {}

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
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

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
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

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
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

        void match(const std::vector<T>& values, std::vector<bool>& result) const override
        {
            filtering::match(values, result, [this](auto value)
            {
                return value <= m_value;
            });
        }
    };

}
