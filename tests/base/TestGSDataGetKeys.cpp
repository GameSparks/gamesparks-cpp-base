#include <gtest/gtest.h>
#include <GameSparks/GSData.h>

TEST(GSData, GetKeys)
{
	using namespace GameSparks;
	using namespace GameSparks::Core;

	GSData data;
	cJSON* json = data.GetBaseData();
	ASSERT_TRUE(json != nullptr);
	ASSERT_EQ(json->type, cJSON_Object);

	cJSON_AddBoolToObject(json, "hello", true);
	cJSON_AddBoolToObject(json, "world", true);
	cJSON_AddBoolToObject(json, "foo", true);
	cJSON_AddBoolToObject(json, "bar", true);
	cJSON_AddBoolToObject(json, "baz", true);
	cJSON_AddBoolToObject(json, "z", true);
	cJSON_AddBoolToObject(json, "y", true);
	cJSON_AddBoolToObject(json, "x", true);
	cJSON_AddBoolToObject(json, "c", true);
	cJSON_AddBoolToObject(json, "b", true);
	cJSON_AddBoolToObject(json, "a", true);

	gsstl::vector<gsstl::string> keys = data.GetKeys();
	ASSERT_EQ(keys.size(), 11);

	// test that keys are sorted
	ASSERT_EQ(keys.front(), "a");
	ASSERT_EQ(keys.back(), "z");

	for (std::vector<std::string>::const_iterator i = keys.begin(); i != keys.end(); ++i)
	{
		std::clog << "key: " << *i << std::endl;
	}
}
