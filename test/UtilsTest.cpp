
#include <gtest/gtest.h>
#include "../embedded/main/Utils.hpp"

TEST(Utils, GetSubstr)
{
    String res = Utils::GetSubStr("123456", 0, 3);
    EXPECT_EQ(res, "123");
}
