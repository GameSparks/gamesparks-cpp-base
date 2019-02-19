#include <gtest/gtest.h>
#include <cstdint>

union Integer
{
	char bytes[4];
	uint32_t integer;
};

TEST(RT, LittleEndian)
{
	Integer integer;
	integer.integer = 0x00000001;
	ASSERT_EQ(integer.bytes[0], 0x01) << "Only little endian architectures are currently supported.";
}