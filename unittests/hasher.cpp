#include "gtest/gtest.h"

#include <unordered_set>
#include "Hasher.h"

TEST(Table, CaseInsensitive)
{
    std::unordered_set<std::string, case_insensitive_hash, case_insensitive_equal> set;
    set.insert({"LUI", "LuI", "lui"});
    EXPECT_EQ(set.size(), 1);
    auto it = set.find("lui");
    EXPECT_TRUE(it != set.end());
    it = set.find("Lui");
    EXPECT_TRUE(it != set.end());
}
