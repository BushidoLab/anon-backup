#!/usr/bin/env python2
# Copyright (c) 2014 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.


from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
from time import *

MASTERNODE_COLLATERAL = 500

class MasternodeInfo:
    def __init__(self, mn_key, collateral_id, collateral_out):
        self.mn_key = mn_key
        self.collateral_id = collateral_id
        self.collateral_out = collateral_out

class MasternodeTest (BitcoinTestFramework):
    def __init__(self):
        self.mninfo = []
        self.mn_count = 10
        self.is_network_split = False
        self.num_nodes = self.mn_count + 4       

    def create_simple_node(self):
        idx = len(self.nodes)
        self.nodes.append(start_node(idx, self.options.tmpdir,
                                     ["-debug"]))
        for i in range(0, idx):
            connect_nodes(self.nodes[i], idx)

    def get_mnconf_file(self, n):
        datadir = os.path.join("cache", "node"+str(n))
        return os.path.join(datadir, "regtest/masternode.conf")

    def prepare_masternodes(self):
        for idx in range(0, self.mn_count):
            mn_key = self.nodes[0].masternode("genkey")
            address = self.nodes[0].getnewaddress()
            txid = self.nodes[0].sendtoaddress(address, MASTERNODE_COLLATERAL)
            masternode_ouputs = self.nodes[0].masternode("outputs")
            collateral_vout = 0
            for key, value in masternode_ouputs.iteritems():
                if key == txid:
                    collateral_vout = value
            self.mninfo.append(MasternodeInfo(mn_key, txid, collateral_vout))

    def write_mn_config(self):
        # f = open(conf, 'a')
        for idx in range(0, self.mn_count):
            conf = self.get_mnconf_file(idx)
            with open(conf, 'a') as f:
                f.write(("mn"+str(idx+1))+" 127.0.0.1:"+str(p2p_port(idx + 1))+str(" "))
                f.write(self.mninfo[idx].mn_key+" ")
                f.write(self.mninfo[idx].collateral_id+" ")
                f.write(self.mninfo[idx].collateral_out+"\n")
            return conf

    def create_masternodes(self):
        for idx in range(0, self.mn_count):
            self.nodes.append(start_node(idx + 1, self.options.tmpdir,
                                         ['-debug=masternode', '-externalip=127.0.0.1',
                                          '-masternode=1',
                                          '-masternodeprivkey=%s' % self.mninfo[idx].mn_key
                                          ]))
            for i in range(0, idx + 1):
                connect_nodes(self.nodes[i], idx + 1)        

    def setup_network(self, split=False):
        self.nodes = []
        self.nodes.append(start_node(0, self.options.tmpdir, ["-debug"]))
        for i in range(0,150):
            set_mocktime(get_mocktime() + 1)
            self.nodes[0].generate(1)
        self.prepare_masternodes()
        self.write_mn_config()
        stop_node(self.nodes[0], 0)
        self.nodes[0] = start_node(0, self.options.tmpdir, ["-debug"])
        self.create_masternodes()
        # create connected simple nodes
        for i in range(0, self.num_nodes - self.mn_count - 1):
            self.create_simple_node()        
        #DASH
        for i in range(0, 2):
            set_mocktime(get_mocktime() + 1)
            self.nodes[0].generate(1)
        # sync all the nodes
        self.sync_all()
        # set_mocktime(get_mocktime() + 1)
        sync_masternodes(self.nodes)
        res = self.nodes[0].masternode("start-alias", "mn%d" % i)
        assert(res["result"] == 'successful')
        # for i in range(1, self.mn_count + 1):
        #     # res = self.nodes[0].masternode("start-alias", "mn%d" % i)
        #     res = self.nodes[0].masternode("start-all")
        #     assert(res["result"] == 'successful')
        # for i in range(0,4):
        #     self.nodes[0].mnsync("next")
        # self.nodes[0].masternode("start-all")
        # sync_masternodes(self.nodes)

    # def run_test (self):
    #     mn_info = self.nodes[0].masternodelist("status")
    #     assert(len(mn_info) == self.mn_count)
    #     for status in mn_info.values():
    #         assert(status == 'ENABLED')

if __name__ == '__main__':
    MasternodeTest ().main ()