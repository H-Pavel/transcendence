// Copyright (c) 2017-2019 The Transcendence developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MASTERNODE_TIERS_H
#define MASTERNODE_TIERS_H

#include "wallet.h"

enum MasternodeTiers { TIER_1K = 0, TIER_3K, TIER_10K, TIER_30K, TIER_100K, TIER_NONE};

const int MASTERNODE_TIER_COINS[MasternodeTiers::TIER_NONE] = {1000, 3000, 10000, 30000, 100000};
const double TIER_OBFUSCATION_VALUES[MasternodeTiers::TIER_NONE] = {999.99, 2999.99, 9999.99, 29999.99, 99999.99};
const int TIER_BLOCK_HEIGHT = 100;


bool IsMasternodeOutput(const CWalletTx* tx, int index, int blockHeight);
int GetMasternodeTierFromOutput(const CWalletTx* tx, int index, int blockHeight);
double GetObfuscationValueForTier(int nTier);
unsigned int CalculateWinningTier(std::vector<size_t>& vecTierSizes, uint256 blockHash);

#endif
