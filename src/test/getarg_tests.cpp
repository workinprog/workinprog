#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-wip");
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", false));
    BOOST_CHECK(GetBoolArg("-wip", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-wipo"));
    BOOST_CHECK(!GetBoolArg("-wipo", false));
    BOOST_CHECK(GetBoolArg("-wipo", true));

    ResetArgs("-wip=0");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", false));
    BOOST_CHECK(!GetBoolArg("-wip", true));

    ResetArgs("-wip=1");
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", false));
    BOOST_CHECK(GetBoolArg("-wip", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nowip");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", false));
    BOOST_CHECK(!GetBoolArg("-wip", true));

    ResetArgs("-nowip=1");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", false));
    BOOST_CHECK(!GetBoolArg("-wip", true));

    ResetArgs("-wip -nowip");  // -wip should win
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", false));
    BOOST_CHECK(GetBoolArg("-wip", true));

    ResetArgs("-wip=1 -nowip=1");  // -wip should win
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", false));
    BOOST_CHECK(GetBoolArg("-wip", true));

    ResetArgs("-wip=0 -nowip=0");  // -wip should win
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", false));
    BOOST_CHECK(!GetBoolArg("-wip", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--wip=1");
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", false));
    BOOST_CHECK(GetBoolArg("-wip", true));

    ResetArgs("--nowip=1");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", false));
    BOOST_CHECK(!GetBoolArg("-wip", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-wip", "eleven"), "eleven");

    ResetArgs("-wip -bar");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-wip", "eleven"), "");

    ResetArgs("-wip=");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-wip", "eleven"), "");

    ResetArgs("-wip=11");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-wip", "eleven"), "11");

    ResetArgs("-wip=eleven");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-wip", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-wip", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-wip", 0), 0);

    ResetArgs("-wip -bar");
    BOOST_CHECK_EQUAL(GetArg("-wip", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-wip=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-wip", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-wip=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-wip", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--wip");
    BOOST_CHECK_EQUAL(GetBoolArg("-wip"), true);

    ResetArgs("--wip=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-wip", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nowip");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", true));
    BOOST_CHECK(!GetBoolArg("-wip", false));

    ResetArgs("-nowip=1");
    BOOST_CHECK(!GetBoolArg("-wip"));
    BOOST_CHECK(!GetBoolArg("-wip", true));
    BOOST_CHECK(!GetBoolArg("-wip", false));

    ResetArgs("-nowip=0");
    BOOST_CHECK(GetBoolArg("-wip"));
    BOOST_CHECK(GetBoolArg("-wip", true));
    BOOST_CHECK(GetBoolArg("-wip", false));

    ResetArgs("-wip --nowip");
    BOOST_CHECK(GetBoolArg("-wip"));

    ResetArgs("-nowip -wip"); // wip always wins:
    BOOST_CHECK(GetBoolArg("-wip"));
}

BOOST_AUTO_TEST_SUITE_END()
