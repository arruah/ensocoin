// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2015-2017 The Bitcoin Unlimited developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "arith_uint256.h"
#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

#if 0 //BU: never used this incarnation of the function
static CBlock CreateGenesisBlock(const  CScript& genesisInputScript, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = genesisInputScript;
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}
#endif


/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Times 6/Jul/2017 US fires response to North Korea 'slap in the face'";
    const CScript genesisOutputScript = CScript() << ParseHex("04c075cf0d0efdf24000f3b864c4a2c9c8477a590d0006c07f4f2d1c793318710092daf3764073e9c27fc0f6cb620cd45139f5c614218e560d4ab22ac68a998c96") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static void MineGenesisBlock(CBlock genesis, Consensus::Params consensus, std::string netID)
{
    // Create genesis block
    if (true && genesis.GetHash() != consensus.hashGenesisBlock) {
        std::cout << "recalculating params for " + netID + "net." << std::endl;
        std::cout << "old testnet genesis nonce: " << genesis.nNonce << std::endl;
        std::cout << "old testnet genesis hash: " << consensus.hashGenesisBlock.ToString().c_str() << std::endl;
        std::cout << "genesis.GetHash(): " << genesis.GetHash().ToString().c_str() << std::endl;

        std::cout << "Mining of genesis block started" << std::flush;

        bool fNegative;
        bool fOverflow;
        arith_uint256 bnTarget;

        bnTarget.SetCompact(genesis.nBits, &fNegative, &fOverflow);
        // Check range
        if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(consensus.powLimit)) {
            std::cout << std::endl << "CheckProofOfWork(): nBits below minimum work" << std::endl;
            return;
        }

        genesis.nNonce = 0;
        while (UintToArith256(genesis.GetHash()) > bnTarget) {
            ++genesis.nNonce;
            if (!genesis.nNonce) {
                // Nonce wrapped, incrementing time
                genesis.nTime++;
            }

            if (!(genesis.nNonce & 0xFFFF)) {
                std::cout << "\r" << "Nonce " << genesis.nNonce << " hash " << genesis.GetHash().ToString().c_str() << std::flush;
            }
        }

        std::cout << std::endl;
        std::cout << "new " + netID + "net genesis merkle root: " << genesis.hashMerkleRoot.ToString().c_str() << std::endl;
        std::cout << "new " + netID + "net genesis nonce: " << genesis.nNonce << std::endl;
        std::cout << "new " + netID + "net genesis nTime: " << genesis.nTime << std::endl;
        std::cout << "new " + netID + "net genesis hash: " << genesis.GetHash().ToString().c_str() << std::endl;
        std::cout << "generated with difficulty: " << consensus.powLimit.ToString().c_str() << std::endl;
    }
}

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

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 227931;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        //consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit = uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800; // May 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xd5;
        pchMessageStart[1] = 0x28;
        pchMessageStart[2] = 0x48;
        pchMessageStart[3] = 0x60;
        vAlertPubKey = ParseHex("04edb7cf11014f48acb50f36b665fef69553459b557ec0bf9967189881e63575fbd86003809283c7f246c04d8252555062b33a15359c1267303cd3f93fc8c7122e");
        nDefaultPort = 7993;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1499212800, 1879025204, 0x207fffff, 1, 50 * COIN);

        // MineGenesisBlock(genesis, consensus, strNetworkID);

        consensus.hashGenesisBlock = genesis.GetHash();
        // TODO uncomment
        assert(consensus.hashGenesisBlock == uint256S("0x6b7fa32e7c689504a0318c2b3ddf91a55ded9c693d4cd78107b7b35397cdba2f"));
        assert(genesis.hashMerkleRoot == uint256S("0xb234efecd627a509c2ae8e1b6ed4d62bd5cad192164b1c2e2d92afea4261f61f"));

        // BITCOINUNLIMITED START
        // vSeeds.push_back(CDNSSeedData("seed.ensocoin.org", "seed.ensocoin.org"));
        // BITCOINUNLIMITED END

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,8);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,172);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x21)(0x35)(0x3F).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x5E)(0xCD)(0x9A).convert_to_container<std::vector<unsigned char> >();

        // BITCOINUNLIMITED START
        vFixedSeeds = std::vector<SeedSpec6>();
        // BITCOINUNLIMITED END

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x6b7fa32e7c689504a0318c2b3ddf91a55ded9c693d4cd78107b7b35397cdba2f")),
            1499212800, // * UNIX timestamp of last checkpoint block
            0,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0     // * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

class CUnlParams : public CChainParams {
public:
    CUnlParams() {
        strNetworkID = "nol";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 10;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        consensus.powLimit = uint256S("3fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        //consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xce;
        pchMessageStart[2] = 0xc4;
        pchMessageStart[3] = 0xe9;
        vAlertPubKey = ParseHex("04fc9702847840aaf195de8442ebecedf5b095cdbb9bc716bda9110971b28a49e0ead8564ff0db22209e0374782c093bb899692d524e9d6a6956e7c5ecbcd68284");
        nDefaultPort = 7996;
        nMaxTipAge = 30 * 24 * 60 * 60;  // this chain is not consistently mined
        nPruneAfterHeight = 100000;


        // replace with generated genesis block
        // CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
        const CScript genesisOutputScript = CScript() << ParseHex("76a91472ecf500e25c73965301f43ee94fbec073cd8eed88ac");
        genesis = CreateGenesisBlock("Bigger blocks FTW (for the world)", genesisOutputScript, 1467923406, 131338110, 486604799, 536870912, CAmount(5000000000));
        consensus.hashGenesisBlock = genesis.GetHash();

        //assert(consensus.hashGenesisBlock == uint256S("0x000000009551bf7a1012675b991320a5bac0ca4090b53fc4e92555ada4776c49"));
        //assert(genesis.hashMerkleRoot == uint256S("0xca020d64c30bddfc728d66b141752bddbd03adb6c72bb945e78e51a5cd7613c5"));

        // BITCOINUNLIMITED START
        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("ensocoin.org", "nolnet-seed.ensocoin.org", true));
        //vSeeds.push_back(CDNSSeedData("bitnodes.io", "seed.bitnodes.io"));      // Bitnodes (Addy Yeow)
        //vSeeds.push_back(CDNSSeedData("bitcoin.sipa.be", "seed.bitcoin.sipa.be")); // Pieter Wuille
        // BITCOINUNLIMITED END

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        // BITCOINUNLIMITED START
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        // BITCOINUNLIMITED END

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0000000000000000000000000000000000000000000000000000000000000000")),
            0,
            0,
            0
        };
    }
};
CUnlParams unlParams;


/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256S("0x0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8");
        //consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit = uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1456790400; // March 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        pchMessageStart[0] = 0x6f;
        pchMessageStart[1] = 0xdd;
        pchMessageStart[2] = 0xe7;
        pchMessageStart[3] = 0x4a;
        vAlertPubKey = ParseHex("04a1de58b0bd170376ed2d2e6c3fe86729ea5d01caffdfe706b0df40f9bcd45fd18ba7836f4ebe771c507ec7204cef7e849f5825fdb683cb1be6dc2767691a8354");
        nDefaultPort = 17993;
        nMaxTipAge = 0x7fffffff;
        nPruneAfterHeight = 1000;

        // args: nTime, nNonce, nBits, nVersion, genesisReward
        genesis = CreateGenesisBlock(1499212800, 2848149976, 0x207fffff, 1, 50 * COIN);

        // MineGenesisBlock(genesis, consensus, strNetworkID);

        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x119569bb4d8d2fa6f053adda21c0371e071c41604df83f9c88a590086ee42c2f"));
        assert(genesis.hashMerkleRoot == uint256S("0xb234efecd627a509c2ae8e1b6ed4d62bd5cad192164b1c2e2d92afea4261f61f"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        //vSeeds.push_back(CDNSSeedData("testseed.ensocoin.org", "testseed.ensocoin.org"));

        // List of address prefixes for TestNet
        // Testnet pubkey hash
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,118);
        // Testnet script hash
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,237);
        // Testnet Private key
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,201);
        // Testnet BIP32 pubkey
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x74)(0x65)(0x75).convert_to_container<std::vector<unsigned char> >();
        // Testnet BIP32 private key
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x74)(0x65)(0x72).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x119569bb4d8d2fa6f053adda21c0371e071c41604df83f9c88a590086ee42c2f")),
            1499212800,
            0,
            0
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMaxTipAge = 24 * 60 * 60;
        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1490008897, 686125357, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        // TODO uncomment
        //assert(consensus.hashGenesisBlock == uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
        //assert(genesis.hashMerkleRoot == uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("00000000bfc98493cb340dce8f73e87c63ffa578af91e5deb24169a8bab597b1")),
            0,
            0,
            0
        };
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }
};
static CRegTestParams regTestParams;

       CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else if (chain == CBaseChainParams::UNL)
            return unlParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}
