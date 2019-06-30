#pragma once

#include <type_traits>
#include <memory>
#include <vector>

namespace filtering
{
    template<class T>
    class IFilter
    {
    public:
        using Ptr = std::shared_ptr<IFilter>;

        virtual ~IFilter() = default;

        virtual bool match(const T& value) const = 0;
    };

    template<class T>
    class ISingleValueFilter : public IFilter<T>
    {
    public:
        ISingleValueFilter(const T& value)
            : m_value(value) {}
    protected:
        T m_value;
    };

    template<class T>
    class IMultiValueFilter : public IFilter<T>
    {
    public:
        template<class Iterator>
        IMultiValueFilter(Iterator begin, Iterator end)
            : m_values(begin, end) {}
    protected:
        std::vector<T> m_values;
    };

    template<class Filter, class T>
    std::shared_ptr<IFilter<T>> createSingleValueFilter(const T& value)
    {
        return std::make_shared<Filter>(value);
    }

    template<class Filter, class T, class Iterator>
    std::shared_ptr<IFilter<T>> createMultiValueFilter(Iterator begin, Iterator end)
    {
        return std::make_shared<Filter>(begin, end);
    }

    template<class T>
    class EqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        EqualsFilter(const T& value)
            : ISingleValueFilter(value)
        {
        }

        bool match(const T& value) const override
        {
            return value == m_value;
        }
    };

    template<class T>
    class EqualsAnyFilter final : public IMultiValueFilter<T>
    {
    public:

        template<class Iterator>
        EqualsAnyFilter(Iterator begin, Iterator end)
            : IMultiValueFilter(begin, end) {}

        bool match(const T& value) const override
        {
            bool result = false;
            for (auto& predicateValue : m_values)
            {
                result |= value == predicateValue;
            }
            return result;
        }
    };

    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        NotEqualsFilter(const T& value)
            : ISingleValueFilter(value)
        {
        }

        bool match(const T& value) const override
        {
            return value == m_value;
        }
    };

    template<class T>
    class IArithmeticFilter : public ISingleValueFilter<T>
    {
    public:
        IArithmeticFilter(const T& value)
            : ISingleValueFilter(value)
        {
        }
    };

    template<class T>
    class GreaterFilter final : public IArithmeticFilter<T>
    {
    public:
        GreaterFilter(const T& value)
            : IArithmeticFilter(value)
        {
        }

        bool match(const T& value) const override
        {
            return value > m_value;
        }
    };

    template<class T>
    class LessFilter final : public IArithmeticFilter<T>
    {
    public:
        LessFilter(const T& value)
            : IArithmeticFilter(value)
        {
        }

        bool match(const T& value) const override
        {
            return value < m_value;
        }
    };

}
