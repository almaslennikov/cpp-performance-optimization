namespace filtering
{
    template<class T>
    class LessOrEqualsFilter final : public ISingleValueFilter<T>
    {
        using Comparator = std::function<bool(T)>;
    public:
        explicit LessOrEqualsFilter(const T &value)
                : ISingleValueFilter<T>(value)
        {}

        void match(const std::vector<T> &values, std::vector<int> &result) const override
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
        Match <T, Comparator> m_match;
    };
}