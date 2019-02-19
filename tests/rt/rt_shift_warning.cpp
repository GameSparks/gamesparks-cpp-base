#include <cstdint>

static int64_t original_ReadZInt64(uint64_t val)
{
    return (int64_t)(val >> 1) ^ ((int64_t)(val << 63) >> 63);
}

static int64_t fixed_ReadZInt64(uint64_t val)
{
    // https://github.com/google/protobuf/blob/24fe6ba750bf8a247d41fc589460dce1bdd697a1/ruby/ext/google/protobuf_c/upb.h#L7703
    return (val >> 1) ^ -(int64_t)(val & 1);
}

static uint64_t original_WriteZInt64(int64_t val)
{
    return (uint64_t)((val << 1) ^ (val >> 63));
}

static uint64_t fixed_WriteZInt64(int64_t val)
{
    // https://github.com/google/protobuf/blob/24fe6ba750bf8a247d41fc589460dce1bdd697a1/ruby/ext/google/protobuf_c/upb.h#L7703
    return (val << 1) ^ (val >> 63);
}

#include <gtest/gtest.h>
#include <limits>

TEST(Warnings, Shift)
{
    uint64_t test_values[] = {
        0UL,
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::lowest(),
        std::numeric_limits<uint64_t>::max(),
    };
    
    for(const auto x : test_values)
    {
        ASSERT_EQ(x, uint64_t(fixed_ReadZInt64(fixed_WriteZInt64(x))));
        ASSERT_EQ(x, fixed_WriteZInt64(fixed_ReadZInt64(x)));

        ASSERT_EQ(
            original_ReadZInt64(x),
            fixed_ReadZInt64(x)
        );
        ASSERT_EQ(
            original_WriteZInt64(x),
            fixed_WriteZInt64(x)
        );
        
        ASSERT_EQ(x, original_WriteZInt64(original_ReadZInt64(x)));
        ASSERT_EQ(x, fixed_WriteZInt64(fixed_ReadZInt64(x)));

        ASSERT_EQ(x, original_WriteZInt64(fixed_ReadZInt64(x)));
        ASSERT_EQ(x, fixed_WriteZInt64(original_ReadZInt64(x)));
    }
}
