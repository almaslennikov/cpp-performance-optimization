#pragma once

#include <type_traits>
#include <memory>
#include <vector>

namespace filtering
{
    template<class T, class Filter>
    void match(const std::vector<T>& values, std::vector<bool>& result, Filter filter)
    {
        for (int i = 0; i < values.size(); i++)
        {
            result[i] = result[i] & filter(values[i]);
        }
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
