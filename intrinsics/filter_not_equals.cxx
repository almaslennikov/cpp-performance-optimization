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
            m_match(values, result, [this](const char *value)
            {
#if 0
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
#else
                auto source  = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(value));
                auto pattern = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(m_value));
                auto result  = _mm256_cmpeq_epi8(source, pattern);

                return _mm256_movemask_epi8(result) != -1 ? 1 : 0;
#endif
            });
        }

    private:
        Match <std::array<char, 32>, StringComparator> m_match;
    };
}
