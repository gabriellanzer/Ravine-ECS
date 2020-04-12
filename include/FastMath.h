#ifndef FASTMATH_H
#define FASTMATH_H
#include <stdint.h>

namespace rv
{

/**
 * @brief Returns the bit mask for any int32_t value
 *
 * @param val Value whose mask will be returned
 * @return constexpr int32_t Mask for the sign value.
 */
constexpr int32_t signMask(const int32_t val) { return (static_cast<uint32_t>(val) >> 31 & 0x1); }

/**
 * @brief Return the minimum value between 'a' and 'b'
 *
 * @param a Value to be compared
 * @param b Value to be compared
 * @return constexpr int32_t 'a' or 'b', whichever is smaller
 */
constexpr int32_t min(const int32_t a, const int32_t b)
{
    const int32_t mask = signMask(a - b);
    return a * mask + b * (1 - mask);
}

/**
 * @brief Return the maximum value between 'a' and 'b'
 *
 * @param a Value to be compared
 * @param b Value to be compared
 * @return constexpr int32_t 'a' or 'b', whichever is bigger
 */
constexpr int32_t max(const int32_t a, const int32_t b)
{
    const int32_t mask = signMask(a - b);
    return a * (1 - mask) + b * mask;
}

} // namespace rv

#endif