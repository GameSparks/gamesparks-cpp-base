//
// Created by Benjamin Schulz on 22/04/15.
//


#include <gtest/gtest.h>
#include <GameSparks/GSRequestData.h>
#include <cmath>

//static const char *parse_number(cJSON *item,const char *num)
//{
//    double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;
//
//    if (*num=='-') sign=-1,num++;	/* Has sign? */
//    if (*num=='0') num++;			/* is zero */
//    if (*num>='1' && *num<='9')	do	n=(n*10.0)+(*num++ -'0');	while (*num>='0' && *num<='9');	/* Number? */
//    if (*num=='.' && num[1]>='0' && num[1]<='9') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */
//    if (*num=='e' || *num=='E')		/* Exponent? */
//    {	num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
//        while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
//    }
//
//    n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
//
//    item->valuedouble=n;
//    item->valueint=(int)n;
//    item->type=cJSON_Number;
//    return num;
//}

static const char* parse_number(GameSparks::cJSON* item, const char *num)
{
    char* end;
    item->valuedouble = std::strtod(num, &end);
    item->valueint = int(round(item->valuedouble));
    item->type = cJSON_Number;
    return end;
}

// --gtest_list_tests
// --gtest_filter=Bugs.GSRequestDataParseNumber
TEST(Bugs, GSRequestDataParseNumber)
{
	//ASSERT_EQ(9007199254740992L, static_cast<long long>(round(9007199254740992.0)));
	//ASSERT_EQ(2147483648.0, round(2147483648.0));

    // set the locale to de to check if cJSON can cope with that
    std::string old_loc = setlocale(LC_NUMERIC, "");
    //ASSERT_EQ(std::string("de_DE.utf-8"), old_loc);

    GameSparks::Core::GSRequestData data;
    data.AddNumber("double", 720.1);
    data.AddNumber("int", 720);
	data.AddNumber("long", 9007199254740992L);			// 2^53 max value
	data.AddNumber("long2", -9007199254740992L);

    // convert to JSON and back again
    data = GameSparks::Core::GSRequestData(GameSparks::cJSON_Parse(data.GetJSON().c_str()));
    ASSERT_TRUE(data.GetBaseData() != nullptr);

    ASSERT_EQ(720.1, data.GetDouble("double").GetValue());
    ASSERT_EQ(720  , data.GetInt("int").GetValue());
	ASSERT_EQ(9007199254740992L, data.GetLongLong("long").GetValue());
	ASSERT_EQ(-9007199254740992L, data.GetLongLong("long2").GetValue());

    // check if cJSON did screw up the locale
    ASSERT_EQ(setlocale(LC_NUMERIC, ""), old_loc);
}

TEST(Bugs, cJsonParseNumber)
{
    auto number = GameSparks::cJSON_CreateNumber(-1);
    parse_number(number, "720.0\0foobar");
    ASSERT_EQ(number->valuedouble, 720.0);
    ASSERT_EQ(number->valueint, 720);
    parse_number(number, "720.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    ASSERT_EQ(number->valuedouble, 720.0);
    ASSERT_EQ(number->valueint, 720);
}
