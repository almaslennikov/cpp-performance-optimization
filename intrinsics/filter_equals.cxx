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

#if 0
            m_match(values, result, [this](const char *value) -> int
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
#else
            const int size           = values.size();
            const int vectorizedSize = size / 4;
            const int remainder      = size % 4;
            const char *const pSource      = reinterpret_cast<const char *>(values.data());
            int        *const pDestination = result.data();

            auto pattern = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(m_value));

            // Vectorized loop
#pragma ivdep
            for (int i = 0; i < (vectorizedSize * 4); i += 4)
            {
                auto source1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + ((i + 0) * 32)));
                auto source2 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + ((i + 1) * 32)));
                auto source3 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + ((i + 2) * 32)));
                auto source4 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + ((i + 3) * 32)));

                auto mask1 = _mm256_cmpeq_epi8(source1, pattern);
                auto mask2 = _mm256_cmpeq_epi8(source2, pattern);
                auto mask3 = _mm256_cmpeq_epi8(source3, pattern);
                auto mask4 = _mm256_cmpeq_epi8(source4, pattern);

                pDestination[i + 0] = pDestination[i + 0] & (_mm256_movemask_epi8(mask1) == -1 ? 1 : 0);
                pDestination[i + 1] = pDestination[i + 1] & (_mm256_movemask_epi8(mask2) == -1 ? 1 : 0);
                pDestination[i + 2] = pDestination[i + 2] & (_mm256_movemask_epi8(mask3) == -1 ? 1 : 0);
                pDestination[i + 3] = pDestination[i + 3] & (_mm256_movemask_epi8(mask4) == -1 ? 1 : 0);
            }

            if (remainder)
            {
#pragma ivdep
                for (int i = vectorizedSize * 4; i < size; i++)
                {
                    auto source = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 32)));
                    auto mask   = _mm256_cmpeq_epi8(source, pattern);

                    pDestination[i] = pDestination[i] & (_mm256_movemask_epi8(mask) == -1 ? 1 : 0);
                }
            }
#endif
        }

    private:
        Match <std::array<char, 32>, StringComparator> m_match{};
    };
}
