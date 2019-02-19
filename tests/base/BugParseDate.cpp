//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GSRequestData.h>
#include <cmath>

// --gtest_list_tests
// --gtest_filter=Bugs.GSRequestDataParseNumber
TEST(Bugs, GSDateTimeParse)
{
    // set the locale to de to check if cJSON can cope with that
    std::string old_loc = setlocale(LC_NUMERIC, "");
    //ASSERT_EQ(std::string("de_DE.utf-8"), old_loc);

    GameSparks::Core::GSDateTime date("2015-12-30T12:00Z");

    ASSERT_EQ(2015, date.GetYear());
    ASSERT_EQ(12, date.GetMonth());
    ASSERT_EQ(30, date.GetDay());
    ASSERT_EQ(12, date.GetHour());
    ASSERT_EQ(00, date.GetMinute());
    ASSERT_EQ(00, date.GetSecond());
    ASSERT_FALSE(date.IsLocalTime());
    ASSERT_EQ("2015-12-30T12:00Z", date.ToString());

    // test round-trip conversion
    auto utc_date = date.ToGMTime();
    auto local_date = utc_date.ToLocalTime();
    auto converted_back = local_date.ToGMTime();

    ASSERT_EQ(date, converted_back);

	ASSERT_FALSE(date.IsLocalTime());

    // check if cJSON did screw up the locale
    ASSERT_EQ(setlocale(LC_NUMERIC, ""), old_loc);
}
