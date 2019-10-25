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

        inline void operator()(const std::vector<T> &values, std::vector<char> &result, Filter filter) const
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
                               std::vector<char> &result,
                               Filter filter) const
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

        virtual void match(const std::vector<T> &values, std::vector<char> &result) const = 0;
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

    template<class T>
    class EqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;

    public:
        explicit EqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            m_match(values, result, [this](auto value)
            {
                return value == this->m_value;
            });
        }

    private:
        Match<T, Comparator> m_match;
    };

    template<>
    class EqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
        using StringComparator = std::function<bool(const char *)>;

    public:
        explicit EqualsFilter(const std::array<char, 32> &value)
                : ISingleValueFilter(value)
        {}

        void match(const std::vector<std::array<char, 32>> &values, std::vector<char> &result) const override
        {
            m_match(values, result, [this](const char *value) -> bool
            {
#if 0
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
#else
                auto source  = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(value));
                auto pattern = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(m_value));
                auto result  = _mm256_cmpeq_epi8(source, pattern);

                return _mm256_movemask_epi8(result) == 0;
#endif
            });
        }

    private:
        Match<std::array<char, 32>, StringComparator> m_match{};
    };

    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit NotEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            m_match(values, result, [this](auto value)
            {
                return value != this->m_value;
            });
        }

    private:
        Match<T, Comparator> m_match;
    };

    template<>
    class NotEqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
        using StringComparator = std::function<bool(const char *)>;
    public:
        explicit NotEqualsFilter(const std::array<char, 32> &value)
                : ISingleValueFilter(value)
        {}

        void match(const std::vector<std::array<char, 32>> &values, std::vector<char> &result) const override
        {
            m_match(values, result, [this](const char *value)
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

    private:
        Match<std::array<char, 32>, StringComparator> m_match;
    };

    template<class T>
    class GreaterFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit GreaterFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Greater filter is not supported for string type");
            } else
            {
                m_match(values, result, [this](auto value)
                {
                    return value > this->m_value;
                });
            }
        }

    private:
        Match<T, Comparator> m_match;
    };

    template<class T>
    class GreaterOrEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit GreaterOrEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Greater or equals filter is not supported for string type");
            } else
            {
                m_match(values, result, [this](auto value)
                {
                    return value >= this->m_value;
                });
            }
        }

    private:
        Match<T, Comparator> m_match;
    };

    template<class T>
    class LessFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit LessFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Less filter is not supported for string type");
            } else
            {
                m_match(values, result, [this](auto value)
                {
                    return value < this->m_value;
                });
            }
        }

    private:
        Match<T, Comparator> m_match;
    };

    template<class T>
    class LessOrEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit LessOrEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<char> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Less or equals filter is not supported for string type");
            } else
            {
                m_match(values, result, [this](auto value)
                {
                    return value <= this->m_value;
                });
            }
        }

    private:
        Match<T, Comparator> m_match;
    };
}
