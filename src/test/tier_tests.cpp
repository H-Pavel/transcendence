// Copyright (c) 2017-2019 The Transcendence developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include <vector>
#include <cstdlib>

#include "masternode-tiers.h"
#include "amount.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(tier_tests)

BOOST_AUTO_TEST_CASE(test_tier_from_outputs)
{
    CWallet wallet;
    int nextLockTime = 0;
    int nInput = 0;
    unsigned int tiers[MasternodeTiers::TIER_NONE] = {MasternodeTiers::TIER_1K, MasternodeTiers::TIER_3K, MasternodeTiers::TIER_10K, MasternodeTiers::TIER_30K, MasternodeTiers::TIER_100K}

    LOCK(wallet.cs_wallet);

    for (auto i = 0; i < MasternodeTiers::TIER_NONE; i++) {
        CMutableTransaction tx;
        tx.nLockTime = nextLockTime++;
        tx.vout.resize(nInput + 1);
        tx.vout[nInput].nValue = MASTERNODE_TIER_COINS[i] * COIN;

        CWalletTx wtx(&wallet, tx);

        BOOST_CHECK(IsMasternodeOutput(&tx, nInput));
        BOOST_CHECK(!IsMasternodeOutput(&tx, tx.vout.size() + 2));
        BOOST_CHECK_EQUAL(GetMasternodeTierFromOutput(&tx, nInput), tiers[i]);
        BOOST_CHECK_EQUAL(GetMasternodeTierFromOutput(&tx, nInput + 2), MasternodeTiers::TIER_NONE);
    }
    CMutableTransaction txNotMn;
    txNotMn.nLockTime = nextLockTime++;
    txNotMn.vout.resize(nInput + 1);
    txNotMn.vout[nInput].nValue = 1234 * COIN;

    CWalletTx wtxNotMn(&wallet, txNotMn);

    BOOST_CHECK(IsMasternodeOutput(&txNotMn, nInput));
    BOOST_CHECK(!IsMasternodeOutput(&txNotMn, txNotMn.vout.size() + 2));
    BOOST_CHECK_EQUAL(GetMasternodeTierFromOutput(&txNotMn, nInput), MasternodeTiers::TIER_NONE);
}

BOOST_AUTO_TEST_CASE(test_obfuscation_value)
{
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_1K) == 999.99, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_3K) == 2999.99, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_10K) == 9999.99, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_30K) == 29999.99, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_100K) == 99999.99, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(MasternodeTiers::TIER_NONE) == 0, boost::test_tools::tolerance(0.0001));
    BOOST_TEST(GetObfuscationValueForTier(-2) == 0, boost::test_tools::tolerance(0.0001));
}

BOOST_AUTO_TEST_CASE(test_intial_distribution)
{
    std::vector<size_t> vecTierSizes;
    std::vector<int> vecBlockNumbers = {0, 1, 3, 4, 13, 14, 43, 44, 143, 144};
    std::vector<unsigned int> vecTiers = {MasternodeTiers::TIER_1K, MasternodeTiers::TIER_3K, MasternodeTiers::TIER_3K,
                                          MasternodeTiers::TIER_10K, MasternodeTiers::TIER_10K, MasternodeTiers::TIER_30K,
                                          MasternodeTiers::TIER_30K, MasternodeTiers::TIER_100K, MasternodeTiers::TIER_100K, MasternodeTiers::TIER_1K};

    for (auto i = 0; i < MasternodeTiers::TIER_NONE; i++) {
        vecTierSizes.push_back(1);
    }

    for (auto j = 0; j < vecBlockNumbers.size(); j++) {
        uint256 tmpHash(vecBlockNumbers[j]);
        auto nTier = CalculateWinningTier(vecTierSizes, tmpHash);
        BOOST_CHECK_EQUAL(nTier, vecTiers[j]);
    }
}

BOOST_AUTO_TEST_CASE(test_winning_tier)
{


}


BOOST_AUTO_TEST_SUITE_END()
