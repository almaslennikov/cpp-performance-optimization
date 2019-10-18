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
    class IGroupFilter : public IFilter<T>
    {
    public:
        IGroupFilter(std::initializer_list<IFilter<T>::Ptr> filters)
            : m_filters{filters} {}

    protected:
        std::vector<IFilter<T>::Ptr> m_filters;
    };


    template<class T>
    class AnyFilter final : public IGroupFilter<T>
    {
    public:
        AnyFilter(std::initializer_list<IFilter<T>::Ptr> filters)
            : IGroupFilter(filters) {}

        bool match(const T& value) const
        {
            bool res = false;
            for (const auto& filter : m_filters)
            {
                res |= filter->match(value);
            }
            return res;
        }
    };

    template<class T>
    class AllFilter final : public IGroupFilter<T>
    {
    public:
        AllFilter(std::initializer_list<IFilter<T>::Ptr> filters)
            : IGroupFilter(filters) {}

        bool match(const T& value) const
        {
            bool res = false;
            for (const auto& filter : m_filters)
            {
                res &= filter->match(value);
            }
            return res;
        }
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

        bool match(const T& value) const override
        {
            return value == m_value;
        }
    };

    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
    public:
        NotEqualsFilter(const T& value)
            : ISingleValueFilter(value) {}

        bool match(const T& value) const override
        {
            return value != m_value;
        }
    };

    template<class T>
    class GreaterFilter final : public ISingleValueFilter<T>
    {
    public:
        GreaterFilter(const T& value)
            : ISingleValueFilter(value) {}

        bool match(const T& value) const override
        {
            return value > m_value;
        }
    };

    template<class T>
    class LessFilter final : public ISingleValueFilter<T>
    {
    public:
        LessFilter(const T& value)
            : ISingleValueFilter(value) {}

        bool match(const T& value) const override
        {
            return value < m_value;
        }
    };

}
