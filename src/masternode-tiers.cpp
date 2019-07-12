// Copyright (c) 2017-2019 The Transcendence developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternode-tiers.h"
#include "amount.h"

bool IsMasternodeOutput(const CWalletTx* tx, int index, int blockHeight)
{
    return (GetMasternodeTierFromOutput(tx, index, blockHeight) != MasternodeTiers::TIER_NONE);
}

int GetMasternodeTierFromOutput(const CWalletTx* tx, int index, int blockHeight)
{
    int tierRet = MasternodeTiers::TIER_NONE;

    if (index < 0 || index >= tx->vout.size())
        return MasternodeTiers::TIER_NONE;

    if (blockHeight < TIER_BLOCK_HEIGHT) {
        if (tx->vout[index].nValue == MASTERNODE_TIER_COINS[MasternodeTiers::TIER_1K] * COIN) {
            tierRet = MasternodeTiers::TIER_1K;
        }
    }
    else {
        for (int tier = MasternodeTiers::TIER_1K; tier != MasternodeTiers::TIER_NONE; tier++) {
            if (tx->vout[index].nValue == MASTERNODE_TIER_COINS[tier] * COIN) {
                tierRet = tier;
                break;
            }
        }
    }
    return tierRet;
}

double GetObfuscationValueForTier(int nTier)
{
    if (nTier >= MasternodeTiers::TIER_NONE || nTier < MasternodeTiers::TIER_1K) {
        return 0;
    }
    else {
        return TIER_OBFUSCATION_VALUES[nTier];
    }
}

unsigned int CalculateWinningTier(std::vector<size_t>& vecTierSizes, uint256 blockHash)
{
    const unsigned int distribution[MasternodeTiers::TIER_NONE] = {1, 3, 10, 30, 100};
    double nDenominator = 0; // Summ( distribution[i]*count[i] )
    unsigned int nMod = 0; // modulus = Summ( distribution[i] )

    //Contains pairs <tier number, weighted value>
    std::vector<std::pair<size_t, unsigned int>> weightedDistribution;

    //Select tiers which contain nodes
    for (auto i = 0; i < MasternodeTiers::TIER_NONE; i++) {
        if (vecTierSizes[i] > 0) {
            nMod += distribution[i];
            nDenominator += distribution[i] * vecTierSizes[i];
            weightedDistribution.push_back(make_pair(i, 0));
        }
    }

    //Stop calculation if there are no nodes or the only single tier is presented in the network
    if (weightedDistribution.size() == 0) {
        return MasternodeTiers::TIER_NONE;
    }
    else if (weightedDistribution.size() == 1) {
        return weightedDistribution[0].first;
    }

    unsigned int nPreviousWeight = 0;
    for (auto j = 0; j < weightedDistribution.size() - 1; j++) {
        auto curTier = weightedDistribution[j].first;

        //Calculate weighted distribution: WD = modulus * (distribution*count) / ( Summ(distribution[i]*count[i]) )
        unsigned int weightedValue = round(vecTierSizes[curTier] * distribution[curTier] * nMod * 1.0 / nDenominator);
        if (weightedValue == 0) {
            weightedValue = 1;
        }
        //Convert weighted distribution to the percent of the modulus
        weightedDistribution[j].second = nPreviousWeight;
        weightedDistribution[j].second += weightedValue;
        nPreviousWeight = weightedDistribution[j].second;
    }
    weightedDistribution[weightedDistribution.size() - 1].second = nMod;
    //Now distribution is converted from values [1,3,10,30,100] to the weighted percents, e.g. [1, 4, 14, 44, 144] for modulus = 144

    int nWinningTier = MasternodeTiers::TIER_NONE;
    unsigned int nCheckNumber = (unsigned long long)(blockHash.getdouble()) % nMod;
    for (auto k = 0; k < weightedDistribution.size(); k++) {
        if (nCheckNumber < weightedDistribution[k].second) {
            nWinningTier = weightedDistribution[k].first;
            break;
        }
    }

    return nWinningTier;
}
