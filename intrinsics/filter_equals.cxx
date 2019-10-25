namespace filtering
{
    template<class T>
    class EqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;

    public:
        explicit EqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<int> &result) const override
        {
#if 0
            m_match(values, result, [this](auto value)
            {
                return value == this->m_value;
            });
#else
            if constexpr (std::is_same<T, int>::value)
            {
                auto sourceLength         = values.size();
                auto vectorizedIterations = sourceLength / 8;
                auto remainder            = sourceLength % 8;

                const int *const pSource      = values.data();
                int       *const pDestination = result.data();

                auto pattern = _mm256_set_epi32(ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value,
                                                ISingleValueFilter<int>::m_value);

                // Vectorized loop
                for (uint32_t i = 0; i < vectorizedIterations; i++)
                {
                    auto source      = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 8)));
                    auto destination = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pDestination + (i * 8)));
                    auto resultMask  = _mm256_cmpeq_epi32(source, pattern);

                    _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 8)),
                                        _mm256_and_si256(resultMask, destination));
                }

                // Remainder
                if (remainder)
                {
                    for (int i = vectorizedIterations * 8; i < sourceLength; i++)
                    {
                        pDestination[i] = pDestination[i] & (pSource[i] == ISingleValueFilter<int>::m_value);
                    }
                }
            } else
            {
                m_match(values, result, [this](auto value)
                {
                    return value == this->m_value;
                });
            }
#endif
        }

    private:
        Match <T, Comparator> m_match;
    };

    template<>
    class EqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
        using StringComparator = std::function<int(const char *)>;

    public:
        explicit EqualsFilter(const std::array<char, 32> &value)
                : ISingleValueFilter(value)
        {}

        void match(const std::vector<std::array<char, 32>> &values, std::vector<int> &result) const override
        {
            m_match(values, result, [this](const char *value) -> int
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

                return _mm256_movemask_epi8(result) == -1 ? 1 : 0;
#endif
            });
        }

    private:
        Match <std::array<char, 32>, StringComparator> m_match{};
    };
}
