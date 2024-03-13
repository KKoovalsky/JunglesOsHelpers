/**
 * @file        enum_to_bits.hpp
 * @brief       Converts enums to bits, aka: enum class { one = 0, two = 1, ... } -> { 0b01, 0b10, ... } (powers of 2).
 */
#ifndef ENUM_TO_BITS_HPP
#define ENUM_TO_BITS_HPP

#include <algorithm>
#include <array>
#include <tuple>
#include <type_traits>

namespace jungles::utils
{

namespace detail
{
template<typename T, typename U>
struct is_same_type : std::false_type
{
};

template<typename T>
struct is_same_type<T, T> : std::true_type
{
};

template<typename... Types>
struct are_all_same;

template<typename T, typename... Rest>
struct are_all_same<T, Rest...> : std::conjunction<is_same_type<T, Rest>...>
{
};

template<auto... Vs>
struct unique_values
{
  private:
    static inline constexpr unsigned get_num_unique_values()
    {
        std::array vs{Vs...};
        auto new_end{std::unique(std::begin(vs), std::end(vs))};
        return std::distance(std::begin(vs), new_end);
    }

  public:
    static inline constexpr auto value{get_num_unique_values()};
};

template<typename... Vs>
struct first_type
{
    using value = typename std::tuple_element_t<0, std::tuple<Vs...>>;
};

}; // namespace detail

template<typename UnderlyingType = unsigned, auto... Events>
struct EnumToBits
{
  private:
    static inline constexpr auto MinEnumValue{std::min({static_cast<int>(Events)...})};
    static inline constexpr auto MaxEnumValue{std::max({static_cast<UnderlyingType>(Events)...})};

  public:
    static_assert(std::is_trivial_v<UnderlyingType>);
    static_assert((std::is_enum_v<decltype(Events)> or ...), "Use 'enum' or 'enum class' to define the events");
    static_assert(detail::are_all_same<decltype(Events)...>::value, "The enumerations must have the same type");
    static_assert(detail::unique_values<Events...>::value <= sizeof(UnderlyingType) * 8,
                  "Maximum of sizeof(UnderlyingType) events are supported");
    static_assert(MaxEnumValue <= sizeof(UnderlyingType) * 8, "The maximum enumeration value is too high");
    static_assert(MinEnumValue >= 0, "Only non-negative values are supported");

    using value_type = typename detail::first_type<decltype(Events)...>::value;

    template<typename... Ts>
    static inline constexpr UnderlyingType to_bits(Ts... events)
    {
        static_assert(detail::are_all_same<Ts...>::value);
        static_assert(std::is_same_v<typename detail::first_type<Ts...>::value, value_type>);

        std::array evts{events...};
        UnderlyingType bits{0};
        for (auto e : evts)
        {
            auto underlying_value{static_cast<unsigned>(e)};
            bits |= 1 << underlying_value;
        }

        return bits;
    }

    template<auto... Vs>
    static inline constexpr UnderlyingType to_bits()
    {
        static_assert(detail::are_all_same<decltype(Vs)...>::value);
        static_assert(std::is_same_v<typename detail::first_type<decltype(Vs)...>::value, value_type>);
        static_assert((is_valid<Vs>() and ...), "Invalid value provided");

        UnderlyingType bits{0};
        constexpr std::array evts{Vs...};
        for (auto e : evts)
        {
            auto underlying_value{static_cast<unsigned>(e)};
            bits |= 1 << underlying_value;
        }

        return bits;
    }

  private:
    static inline constexpr std::array events{Events...};

    template<auto V>
    static inline constexpr bool is_valid()
    {
        return std::find(std::begin(events), std::end(events), V) != std::end(events);
    }
};

}; // namespace jungles::utils

#endif /* ENUM_TO_BITS_HPP */
