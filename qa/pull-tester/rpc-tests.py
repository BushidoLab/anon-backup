#!/usr/bin/env python2
# Copyright (c) 2014-2015 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""
Run Regression Test Suite

This module calls down into individual test cases via subprocess. It will
forward all unrecognized arguments onto the individual test scripts, other
than:

    - `-extended`: run the "extended" test suite in addition to the basic one.
    - `-win`: signal that this is running in a Windows environment, and we
      should run the tests.
    - `--coverage`: this generates a basic coverage report for the RPC
      interface.

For a description of arguments recognized by test scripts, see
`qa/pull-tester/test_framework/test_framework.py:BitcoinTestFramework.main`.

"""

import os
import time
import shutil
import sys
import subprocess
import tempfile
import re

from tests_config import *

#If imported values are not defined then set to zero (or disabled)
if 'ENABLE_WALLET' not in vars():
    ENABLE_WALLET=0
if 'ENABLE_BITCOIND' not in vars():
    ENABLE_BITCOIND=0
if 'ENABLE_UTILS' not in vars():
    ENABLE_UTILS=0
if 'ENABLE_ZMQ' not in vars():
    ENABLE_ZMQ=0

ENABLE_COVERAGE=0

#Create a set to store arguments and create the passOn string
opts = set()
passOn = ""
p = re.compile("^--")

bold = ("","")
if (os.name == 'posix'):
    bold = ('\033[0m', '\033[1m')

for arg in sys.argv[1:]:
    if arg == '--coverage':
        ENABLE_COVERAGE = 1
    elif (p.match(arg) or arg == "-h"):
        passOn += " " + arg
    else:
        opts.add(arg)

#Set env vars
buildDir = BUILDDIR
if "ANOND" not in os.environ:
    os.environ["ANOND"] = buildDir + '/src/anond' + EXEEXT
if "DASHCLI" not in os.environ:
    os.environ["ANONCLI"] = buildDir + '/src/anon-cli' + EXEEXT

if EXEEXT == ".exe" and "-win" not in opts:
    # https://github.com/bitcoin/bitcoin/commit/d52802551752140cf41f0d9a225a43e84404d3e9
    # https://github.com/bitcoin/bitcoin/pull/5677#issuecomment-136646964
    print( "Win tests currently disabled by default.  Use -win option to enable")
    sys.exit(0)

if not (ENABLE_WALLET == 1 and ENABLE_UTILS == 1 and ENABLE_BITCOIND == 1):
    print( "No rpc tests to run. Wallet, utils, and bitcoind must all be enabled")
    sys.exit(0)

# python-zmq may not be installed. Handle this gracefully and with some helpful info
if ENABLE_ZMQ:
    try:
        import zmq
    except ImportError as e:
        print("ERROR: \"import zmq\" failed. Set ENABLE_ZMQ=0 or " \
            "to run zmq tests, see dependency info in /qa/README.md.")
        raise e

#Tests
testScripts=[
    'prioritisetransaction.py'
    'wallet_treestate.py', #PASSES
    'wallet_protectcoinbase.py', #FAILING
    'wallet_shieldcoinbase.py', #PASSES
    'wallet.py', #FAILING
    'wallet_nullifiers.py', #PASSES
    'wallet_1941.py', #PASSES
    'listtransactions.py', #PASSING
    'mempool_resurrect_test.py', #PASSES
    'txn_doublespend.py', #PASSES PYTHON TEST HANDLER IS NOT ACCEPTING THE PARAMTER
    'txn_doublespend.py --mineblock' #FAILING
    'getchaintips.py', #PASSING
    'rawtransactions.py', #PASSING
    'rest.py',#PASSING
    'mempool_spendcoinbase.py', #PASSES
    'mempool_coinbase_spends.py', #PASSES Doesn't work on mac
    'mempool_tx_input_limit.py', #PASSES
    'httpbasics.py', #PASSING
    'zapwallettxes.py', #PASSING
    'proxy_test.py', #PASSING
    'merkle_blocks.py', #PASSES
    'fundrawtransaction.py', #FAILING
    'signrawtransactions.py',#PASSING
    'walletbackup.py', #PASSING
    'nodehandling.py', #PASSING
    'reindex.py', #PASSING
    'decodescript.py',#PASSING
    'disablewallet.py', #PASSING
    'zcjoinsplit.py', #PASSES
    'zcjoinsplitdoublespend.py', #PASSES
    'getblocktemplate.py', #PASSES
    'bip65-cltv-p2p.py',#PASSING
    'bipdersig-p2p.py',#PASSING
    # 'p2p-masternodes.py',
];
if ENABLE_ZMQ:
    testScripts.append('zmq_test.py')

testScriptsExt=[
    'getblocktemplate_longpoll.py',
    'getblocktemplate_proposals.py',
    'pruning.py',
    'forknotify.py',
    'hardforkdetection.py',
    'invalidateblock.py',
    'keypool.py',
    'receivedby.py',
    'rpcbind_test.py',
#   'script_test.py',
    'smartfees.py',
    'maxblocksinflight.py',
    'invalidblockrequest.py',
    'p2p-versionbits-warning.py',
#    'forknotify.py',
    'p2p-acceptblock.py',
];

def runtests():
    coverage = None

    if ENABLE_COVERAGE:
        coverage = RPCCoverage()
        print("Initializing coverage directory at %s\n" % coverage.dir)

    rpcTestDir = buildDir + '/qa/rpc-tests/'
    run_extended = '-extended' in opts
    cov_flag = coverage.flag if coverage else ''
    flags = " --srcdir %s/src %s %s" % (buildDir, cov_flag, passOn)

    #Run Tests
    for i in range(len(testScripts)):
        if (len(opts) == 0
                or (len(opts) == 1 and "-win" in opts )
                or run_extended
                or testScripts[i] in opts
                or re.sub(".py$", "", testScripts[i]) in opts ):

            print("Running testscript %s%s%s ..." % (bold[1], testScripts[i], bold[0]))
            time0 = time.time()
            subprocess.check_call(
                rpcTestDir + testScripts[i] + flags, shell=True)
            print("Duration: %s s\n" % (int(time.time() - time0)))

            # exit if help is called so we print just one set of
            # instructions
            p = re.compile(" -h| --help")
            if p.match(passOn):
                sys.exit(0)

    # Run Extended Tests
    for i in range(len(testScriptsExt)):
        if (run_extended or testScriptsExt[i] in opts
                or re.sub(".py$", "", testScriptsExt[i]) in opts):

            print(
                "Running 2nd level testscript "
                + "%s%s%s ..." % (bold[1], testScriptsExt[i], bold[0]))
            time0 = time.time()
            subprocess.check_call(
                rpcTestDir + testScriptsExt[i] + flags, shell=True)
            print("Duration: %s s\n" % (int(time.time() - time0)))

    if coverage:
        coverage.report_rpc_coverage()

        print("Cleaning up coverage data")
        coverage.cleanup()


class RPCCoverage(object):
    """
    Coverage reporting utilities for pull-tester.

    Coverage calculation works by having each test script subprocess write
    coverage files into a particular directory. These files contain the RPC
    commands invoked during testing, as well as a complete listing of RPC
    commands per `bitcoin-cli help` (`rpc_interface.txt`).

    After all tests complete, the commands run are combined and diff'd against
    the complete list to calculate uncovered RPC commands.

    See also: qa/rpc-tests/test_framework/coverage.py

    """
    def __init__(self):
        self.dir = tempfile.mkdtemp(prefix="coverage")
        self.flag = '--coveragedir %s' % self.dir

    def report_rpc_coverage(self):
        """
        Print out RPC commands that were unexercised by tests.

        """
        uncovered = self._get_uncovered_rpc_commands()

        if uncovered:
            print("Uncovered RPC commands:")
            print("".join(("  - %s\n" % i) for i in sorted(uncovered)))
        else:
            print("All RPC commands covered.")

    def cleanup(self):
        return shutil.rmtree(self.dir)

    def _get_uncovered_rpc_commands(self):
        """
        Return a set of currently untested RPC commands.

        """
        # This is shared from `qa/rpc-tests/test-framework/coverage.py`
        REFERENCE_FILENAME = 'rpc_interface.txt'
        COVERAGE_FILE_PREFIX = 'coverage.'

        coverage_ref_filename = os.path.join(self.dir, REFERENCE_FILENAME)
        coverage_filenames = set()
        all_cmds = set()
        covered_cmds = set()

        if not os.path.isfile(coverage_ref_filename):
            raise RuntimeError("No coverage reference found")

        with open(coverage_ref_filename, 'r') as f:
            all_cmds.update([i.strip() for i in f.readlines()])

        for root, dirs, files in os.walk(self.dir):
            for filename in files:
                if filename.startswith(COVERAGE_FILE_PREFIX):
                    coverage_filenames.add(os.path.join(root, filename))

        for filename in coverage_filenames:
            with open(filename, 'r') as f:
                covered_cmds.update([i.strip() for i in f.readlines()])

        return all_cmds - covered_cmds


if __name__ == '__main__':
    runtests()
