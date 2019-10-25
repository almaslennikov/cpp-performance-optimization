namespace filtering
{
    template<class T>
    class NotEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit NotEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<int> &result) const override
        {
            m_match(values, result, [this](auto value)
            {
                return value != this->m_value;
            });
        }

    private:
        Match <T, Comparator> m_match;
    };

    template<>
    class NotEqualsFilter<std::array<char, 32>> final : public ISingleValueFilter<std::array<char, 32>>
    {
        using StringComparator = std::function<bool(const char *)>;
    public:
        explicit NotEqualsFilter(const std::array<char, 32> &value)
                : ISingleValueFilter(value)
        {}

        void match(const std::vector<std::array<char, 32>> &values, std::vector<int> &result) const override
        {
#if 0

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

                pDestination[i + 0] = pDestination[i + 0] & (_mm256_movemask_epi8(mask1) != -1 ? 1 : 0);
                pDestination[i + 1] = pDestination[i + 1] & (_mm256_movemask_epi8(mask2) != -1 ? 1 : 0);
                pDestination[i + 2] = pDestination[i + 2] & (_mm256_movemask_epi8(mask3) != -1 ? 1 : 0);
                pDestination[i + 3] = pDestination[i + 3] & (_mm256_movemask_epi8(mask4) != -1 ? 1 : 0);
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
        Match <std::array<char, 32>, StringComparator> m_match;
    };
}
