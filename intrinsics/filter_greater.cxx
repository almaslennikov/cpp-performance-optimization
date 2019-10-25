namespace filtering
{
    template<class T>
    class GreaterFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit GreaterFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<int> &result) const override
        {
            if constexpr (std::is_same<T, std::array<char, 32>>::value)
            {
                throw std::invalid_argument("Greater filter is not supported for string type");
            } else
            {
#if 0
                m_match(values, result, [this](auto value)
                {
                    return value > this->m_value;
                });
#else
                if constexpr (std::is_same<T, int>::value)
                {
                    auto sourceLength         = values.size();
                    auto vectorizedIterations = sourceLength / 32;
                    auto remainder            = sourceLength % 32;

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
                        auto source1 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 32) + 0));
                        auto source2 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 32) + 8));
                        auto source3 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 32) + 16));
                        auto source4 = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(pSource + (i * 32) + 24));

                        auto destination1 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 0));
                        auto destination2 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 8));
                        auto destination3 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 16));
                        auto destination4 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 24));

                        auto resultMask1 = _mm256_cmpgt_epi32(source1, pattern);
                        auto resultMask2 = _mm256_cmpgt_epi32(source2, pattern);
                        auto resultMask3 = _mm256_cmpgt_epi32(source3, pattern);
                        auto resultMask4 = _mm256_cmpgt_epi32(source4, pattern);

                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 0),
                                            _mm256_and_si256(resultMask1, destination1));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 8),
                                            _mm256_and_si256(resultMask2, destination2));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 16),
                                            _mm256_and_si256(resultMask3, destination3));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 24),
                                            _mm256_and_si256(resultMask4, destination4));
                    }

                    // Remainder
                    if (remainder)
                    {
                        for (int i = vectorizedIterations * 32; i < sourceLength; i++)
                        {
                            pDestination[i] = pDestination[i] & (pSource[i] > ISingleValueFilter<int>::m_value);
                        }
                    }
                } else if constexpr (std::is_same<T, float>::value)
                {
                    auto sourceLength         = values.size();
                    auto vectorizedIterations = sourceLength / 32;
                    auto remainder            = sourceLength % 32;

                    const float *const pSource      = values.data();
                    int         *const pDestination = result.data();

                    const auto pattern = _mm256_set_ps(ISingleValueFilter<float>::m_value,
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
                        auto source1 = _mm256_loadu_ps(pSource + (i * 32) + 0);
                        auto source2 = _mm256_loadu_ps(pSource + (i * 32) + 8);
                        auto source3 = _mm256_loadu_ps(pSource + (i * 32) + 16);
                        auto source4 = _mm256_loadu_ps(pSource + (i * 32) + 24);

                        auto destination1 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 0));
                        auto destination2 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 8));
                        auto destination3 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 16));
                        auto destination4 = _mm256_loadu_si256(
                                reinterpret_cast<const __m256i *>(pDestination + (i * 32) + 24));

                        auto resultMask1 = _mm256_srli_epi32(_mm256_cvtps_epi32(_mm256_cmp_ps(source1, pattern, 14)),
                                                             31);
                        auto resultMask2 = _mm256_srli_epi32(_mm256_cvtps_epi32(_mm256_cmp_ps(source2, pattern, 14)),
                                                             31);
                        auto resultMask3 = _mm256_srli_epi32(_mm256_cvtps_epi32(_mm256_cmp_ps(source3, pattern, 14)),
                                                             31);
                        auto resultMask4 = _mm256_srli_epi32(_mm256_cvtps_epi32(_mm256_cmp_ps(source4, pattern, 14)),
                                                             31);

                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 0),
                                            _mm256_and_si256(resultMask1, destination1));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 8),
                                            _mm256_and_si256(resultMask2, destination2));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 16),
                                            _mm256_and_si256(resultMask3, destination3));
                        _mm256_storeu_si256(reinterpret_cast<__m256i *>(pDestination + (i * 32) + 24),
                                            _mm256_and_si256(resultMask4, destination4));
                    }

                    // Remainder
                    if (remainder)
                    {
                        for (int i = vectorizedIterations * 32; i < sourceLength; i++)
                        {
                            pDestination[i] = pDestination[i] & (pSource[i] > ISingleValueFilter<float>::m_value);
                        }
                    }
                } else
                {
                    m_match(values, result, [this](auto value)
                    {
                        return value > this->m_value;
                    });
                }
#endif
            }
        }

    private:
        Match <T, Comparator> m_match;
    };
}
