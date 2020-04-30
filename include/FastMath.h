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

inline intptr_t* getMaskCombinations(intptr_t* seedMasks, const int32_t maskCount, int32_t& combCount)
{
    // Calculate number of possible combinations
    combCount = (1u << maskCount) - 1 /*Exclude empty set*/;
    int32_t* compIt = new int32_t[maskCount];
    intptr_t* combs = new intptr_t[combCount];

    // Process all combinations, from (N,N) to (N,1)
    int32_t combIt = 0;
    for (int32_t compCount = maskCount; compCount > 0; compCount--)
    {
        // Setup iterators and sizes for (N,compCount)
        for (int32_t i = 0; i < compCount; i++)
        {
            compIt[i] = i;
        }

        // Calcualte every combination of (N,compCount)
        int32_t curIncIt = compCount - 1;
        do
        {
            // Compute current Combinations
            combs[combIt] = seedMasks[compIt[0]];
            for (int32_t i = 1; i < compCount; i++)
            {
                combs[combIt] ^= seedMasks[compIt[i]];
            }

            // Find current incrementing iterator
            for (int32_t i = compCount - 1; i >= 1; i--)
            {
                const int32_t limit = maskCount - (compCount - i);
                curIncIt -= 1 - signMask(compIt[curIncIt] - limit);
            }

            // Update component iterators
            int32_t lastComp = ++compIt[curIncIt];
            for (int32_t i = curIncIt + 1; i < compCount; i++)
            {
                compIt[i] = ++lastComp;
            }
            // Reset curent incrementing cursor
            curIncIt = compCount - 1;

            // Increment combination iterator
            combIt++;
        } while (compIt[0] <= (maskCount - compCount));
    }

    delete[] compIt;
    return combs;
}

} // namespace rv

#endif