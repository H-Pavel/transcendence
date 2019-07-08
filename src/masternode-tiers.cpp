// Copyright (c) 2017-2019 The Transcendence developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternode-tiers.h"
#include "amount.h"

bool IsMasternodeOutput(const CWalletTx* tx, int index)
{
    bool bFound = false;

    for (auto tier = MasternodeTiers::TIER_1K; tier != MasternodeTiers::TIER_NONE; tier++) {
        if (tx->vout[index].nValue == MASTERNODE_TIER_COINS[tier] * COIN) {
            bFound = true;
            break;
        }
    }
    return bFound;
}

int GetMasternodeTierFromOutput(COutput& out)
{
    int tierRet = MasternodeTiers::TIER_NONE;

    for (auto tier = MasternodeTiers::TIER_1K; tier != MasternodeTiers::TIER_NONE; tier++) {
        if (out.tx->vout[out.i].nValue == MASTERNODE_TIER_COINS[tier] * COIN) {
            tierRet = tier;
            break;
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