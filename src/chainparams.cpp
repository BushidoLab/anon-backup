// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "crypto/equihash.h"
#include "util.h"
#include "utilstrencodings.h"
#include <assert.h>
#include <boost/assign/list_of.hpp>
#include "base58.h"
using namespace std;
#include "chainparamsseeds.h"

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        strCurrencyUnits = "ANON";
        consensus.fCoinbaseMustBeProtected = true;

        //TODO To be decided
        // consensus.nSubsidySlowStartInterval = 2;
        consensus.nSubsidyHalvingInterval = 105000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 4000;
        consensus.prePowLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit = uint256S("07ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 2016; //diffuculty adjusts every 2 weeks
        // assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPowTargetSpacing = 10 * 60; // time between blocks (sec)
        consensus.fPowAllowMinDifficultyBlocks = false;

        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        pchMessageStart[0] = 0x83;
        pchMessageStart[1] = 0xd8;
        pchMessageStart[2] = 0x47;
        pchMessageStart[3] = 0xa7;
        vAlertPubKey = ParseHex("04f2cd746e629ffd320a81287474c98c2ad15d15b0a210b0144edcd8f3e1301c6311fd751fa34ba17d88090374cfec7cd9aaca55a5a0c4456511acc01b922005de");
        nDefaultPort = 33130;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;
        const size_t N = 144, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database (and is in any case of zero value).
         *
         * >>> from pyblake2 import blake2s
         * >>> 'Zclassic' + blake2s(b'No taxation without representation. ANON #437541 - 00000000000000000397f175a94dd3f530b957182eb2a9f7b79a44a94a5e0450').hexdigest()
         */
        const char* pszTimestamp = "Anonymousd8e0ce2abad2015a525f42479b7b0b324d6bb69e5b4d4310489d51c4cc22e32e";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 4;
        genesis.nTime    = 1532704413;
        genesis.nBits    = 0x1f07ffff;
        // genesis.nBits    = 0x1d00ffff;

        genesis.nNonce   = uint256S("0x000000000000000000000000000000000000000000000000000000000000068f");
        genesis.nSolution = ParseHex("18469d916cd847f65e7013d622db5a95991165544d31874eb6216cdca2a04d50c927fdc24a57c3a1c4eede522cc197ed23262078e9b69154af0ce938a35689e6f25da872a2fa0099acef372395d79c6874ae9c61d8aa1b24706c7b89507ede0c85825d17");

        consensus.hashGenesisBlock = genesis.GetHash();
   
        assert(consensus.hashGenesisBlock == uint256S("0x0006e2f1792ad5a97c2a266c2d65bbd53b0f44b153db1f35d40acbb7d24ae93b"));
        assert(genesis.hashMerkleRoot == uint256S("0xe50058ee0a974a2e8562fe45782bc1a104af045df1b3d5c0de4aea2e972d1d59"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // TODO: setup a DNSSeed

        // guarantees the first 2 characters, when base58 encoded, are "An"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x05,0x82};

        // guarantees the first 2 characters, when base58 encoded, are "3Z"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x53,0x89};

        // the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0x80};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x88,0xB2,0x1E};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x88,0xAD,0xE4};
        // guarantees the first 2 characters, when base58 encoded, are "zk"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0xA8};
        // guarantees the first 2 characters, when base58 encoded, are "SK"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAB,0x36};


        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        nFulfilledRequestExpireTime = 60*60; // fulfilled requests expire in 1 hour
        checkpointData = {
            {
                {      0, consensus.hashGenesisBlock }
                // {  30000, uint256S("0x000000005c2ad200c3c7c8e627f67b306659efca1268c9bb014335fdadc0c392") },
                // { 160000, uint256S("0x000000065093005a1a46ee95d6d66c2b07008220ca64dd3b3a93bbd1945480c0") },
                // { 272992, uint256S("0x0102a4e1353149c9221ebd7c45b3170ea4b8a7d1cf4075b3bc50c440fb9ebdd6") },
                // { 273992, uint256S("0x000822e42b2128a296b704c273a4c3f0e707b06305d2be521cf7577ef67e476a") },
                // { 274992, uint256S("0x03e91186991b31044179b82e88ee06c2205e3304a525d130070153fde4aaf414") },
                // { 275992, uint256S("0x02bc3f08cec58ac79b35a29161b43cd301696e6e6421d704314d08a35b2169db") },
                // { 276992, uint256S("0x0046665bbb64b609935ed0fd184d6c42657dc5c36b16ed1e59ecc2fcb0508d32") },
                // { 277992, uint256S("0x0111d3ffc37fb3474ed2a6e3e9bfc53ee0d7cc0413a732f19d59061700f6b842") },
                // { 278458, uint256S("0x0747cecedfd30754323c5afdaef547d5e90fd50485370ebf12dd33bbac61faaa") },
                // { 279500, uint256S("0x0000000082ece76130c82337903b14a2109fab2dcd2d153fcdbf48a0054a694b") },
            },
            0,     // * UNIX timestamp of last checkpoint block
            0,       // * total number of transactions between genesis and last checkpoint
                            //   (the tx=... number in the SetBestChain debug.log lines)
            50            // * estimated number of transactions per day after checkpoint
                            //   total number of tx / (checkpoint block height / (24 * 24))
        };

        nForkStartHeight = 0;
        nForkHeightRange = 0;
        zUtxoMiningStartBlock = 0;

    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strCurrencyUnits = "ANONT";
        consensus.fCoinbaseMustBeProtected = true;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 400;
        consensus.powLimit = uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        consensus.prePowLimit = consensus.powLimit;
        // assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.fPowAllowMinDifficultyBlocks = true;

        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008


        pchMessageStart[0] = 0x7a;
        pchMessageStart[1] = 0x74;
        pchMessageStart[2] = 0x8d;
        pchMessageStart[3] = 0x38;

        vAlertPubKey = ParseHex("048679fb891b15d0cada9692047fd0ae26ad8bfb83fabddbb50334ee5bc0683294deb410be20513c5af6e7b9cec717ade82b27080ee6ef9a245c36a795ab044bb3");
        nDefaultPort = 33127;
        nPruneAfterHeight = 1000;


        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1532704413;
        genesis.nBits = 0x2007ffff;
        genesis.nNonce = uint256S("0x0000000000000000000000000000000000000000000000000000000000000694");
        genesis.nSolution = ParseHex("00405e6159d18c3534879797c5af4d84eb81c1937d6cdebf6b507f9eee5b21d75cf17d507df6acb0a77e76da7b8ccbf0a80e07ae333024a5989ccbd31e6be38aea6dd56701d9a6c9399ecd08a2cbbaa3fa50718a782cd023f84c9a7b7e4a2d92cdb6c950");
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x04cb00a7d6b71b20dd9e28673c5726bddc1eaafca27403509c86ed2750470d7d"));

        vFixedSeeds.clear();
        vSeeds.clear();

        // guarantees the first 2 characters, when base58 encoded, are "tA"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1C,0xCE};
        // guarantees the first 2 characters, when base58 encoded, are "t2"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        // the first character, when base58 encoded, is "9" or "c" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        // guarantees the first 2 characters, when base58 encoded, are "zt"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0xB6};
        // guarantees the first 2 characters, when base58 encoded, are "ST"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            ( 0, consensus.hashGenesisBlock),
            genesis.nTime,
            0,
            0
        };

        nForkStartHeight = 7;
        nForkHeightRange = 1724;
        zUtxoMiningStartBlock = 1675;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strCurrencyUnits = "REG";
        consensus.fCoinbaseMustBeProtected = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nSubsidyHalvingInterval = 105000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0x0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        consensus.prePowLimit = consensus.powLimit;
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up

        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;

        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xe8;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x5f;
        nMaxTipAge = 24 * 60 * 60;

        
        const size_t N = 48, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;
        genesis.nTime = 1532704413;
        genesis.nBits = 0x200f0f0f;
        genesis.nNonce = uint256S("0x0000000000000000000000000000000000000000000000000000000000000025");
        genesis.nSolution = ParseHex("06d18c2e52bace73990bdaa3fd64ff2d6323178f74ddf6db7217671e71ce2ea24f11c59c");
        consensus.hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 3130;
        assert(consensus.hashGenesisBlock == uint256S("0x0d7dd7d17fb3790fa94170e32502c1db54e32cc6c6246b3699b677d33bd389a4"));
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
        nFulfilledRequestExpireTime = 5*60; // fulfilled requests expire in 5 minutes

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x0d7dd7d17fb3790fa94170e32502c1db54e32cc6c6246b3699b677d33bd389a4")),
            0,
            0,
            0
        };

        nForkStartHeight = 0;
        nForkHeightRange = 0;
        zUtxoMiningStartBlock = 0;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);

    // Some python qa rpc tests need to enforce the coinbase consensus rule
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestprotectcoinbase")) {
        regTestParams.SetRegTestCoinbaseMustBeProtected();
    }
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}




