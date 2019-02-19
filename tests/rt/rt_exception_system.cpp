//
// Created by Benjamin Schulz on 13/05/16.
//

#include <gtest/gtest.h>
#include "../../src/System/Failable.hpp"
#include "../../src/System/IO/IOException.hpp"
#include <System/Nullable.hpp>

using namespace System;

Failable<int> f()
{
    GS_THROW(System::IO::IOException("f throws"));
    return 0;
}

Failable<int> g()
{
    //GS_THROW(f().GetException());
    GS_ASSIGN_OR_THROW(tmp, f());
	(void)tmp;
    return 0;
}

Failable<int> i_dont_throw()
{
    return 42;
}


Failable<char> you_need_to_check_the_result()
{
    return '@';
}


TEST(ExceptionSystem, Basic)
{
    auto result = g();

    ASSERT_FALSE(result.isOK());

    std::clog << result.GetException() << std::endl;

    bool catch_was_called = false;
    bool line_was_executed = false;
    GS_TRY
    {
        GS_ASSIGN_OR_CATCH(number, i_dont_throw());
        ASSERT_EQ(number, 42);
        GS_CALL_OR_CATCH(g());
        line_was_executed = true;
    }
    GS_CATCH(e)
    {
        std::clog << e << std::endl;
        catch_was_called = true;
    }
    ASSERT_TRUE(catch_was_called);
    ASSERT_FALSE(line_was_executed);

    //you_need_to_check_the_result();
}
