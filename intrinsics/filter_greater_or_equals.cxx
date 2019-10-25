namespace filtering
{
    template<class T>
    class GreaterOrEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit GreaterOrEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<int> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Greater or equals filter is not supported for string type");
            } else
            {
#if 0
                m_match(values, result, [this](auto value)
                {
                    return value >= this->m_value;
                });
#else
                if constexpr (std::is_same<T, float>::value)
                {
                    auto sourceLength         = values.size();
                    auto vectorizedIterations = sourceLength / 8;
                    auto remainder            = sourceLength % 8;

                    const float *const pSource      = values.data();
                    int         *const pDestination = result.data();

                    auto pattern = _mm256_set_ps(ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value,
                                                 ISingleValueFilter<float>::m_value);

                    // Vectorized loop
                    for (uint32_t i = 0; i < vectorizedIterations; i++)
                    {
                        auto source      = _mm256_loadu_ps(pSource + (i * 8));
                        auto destination = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 8)));
                        auto resultMask  = _mm256_srli_epi32(
                                _mm256_cvtps_epi32(
                                        _mm256_cmp_ps(source, pattern, 13)),
                                31);

                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 8)),
                                            _mm256_and_si256(resultMask, destination));
                    }

                    // Remainder
                    if (remainder)
                    {
                        for (int i = vectorizedIterations * 8; i < sourceLength; i++)
                        {
                            pDestination[i] = pDestination[i] & (pSource[i] > ISingleValueFilter<float>::m_value);
                        }
                    }
                } else
                {
                    m_match(values, result, [this](auto value)
                    {
                        return value >= this->m_value;
                    });
                }
#endif
            }
        }

    private:
        Match <T, Comparator> m_match;
    };
}
