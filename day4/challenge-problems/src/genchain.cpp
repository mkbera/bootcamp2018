#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <cstdlib>

#include "crypto.h"
#include "blockchain.h"
#include "transaction.h"

struct utxo_t
{
    std::unique_ptr<rsa_private_key_t> privKey;
    std::unique_ptr<rsa_public_key_t> pubKey;
    hash_result_t addr;
    uint64_t amount;

    utxo_t();
};

utxo_t::utxo_t()
{
    keypair_t p = generate_rsa_keys();
    privKey.reset(p.first);
    pubKey.reset(p.second);
    addr.set_hash_from_data(pubKey->keyDER);
    amount = 0;
}

typedef std::unordered_map<hash_result_t, std::shared_ptr<utxo_t> > utxo_map_t;

utxo_map_t utxo_map;

std::shared_ptr<block_t> create_coinbase(const hash_result_t& addr)
{
    std::shared_ptr<block_t> coinbase(new block_t());
    coinbase->reward_addr = addr;
    coinbase->set_hash();
    assert(coinbase->validate());
    return coinbase;
}

utxo_map_t::iterator find_spendable(utxo_map_t& utxos, const hash_result_t& reward_addr)
{
    for (auto pos = utxos.begin(); pos != utxos.end(); pos++) {
        if (pos->second->amount > 0 && pos->first != reward_addr) {
            return pos;
        }
    }
    return utxos.end();
}

std::shared_ptr<txn_t> create_txn(
    utxo_t& snd, utxo_t& rcv, utxo_t& chng, uint64_t amt)
{
    hash_result_t snd_addr = snd.addr;
    hash_result_t rcv_addr = rcv.addr;
    hash_result_t chng_addr = chng.addr;

    std::shared_ptr<txn_t> txn(
          new txn_t(*snd.privKey, snd.pubKey->keyDER, 
          snd_addr, rcv_addr, chng_addr, amt));
    return txn;
}

void ensure_min_utxos(utxo_map_t& utxos, unsigned int min_utxos)
{
    while (utxos.size() < min_utxos) {
        std::shared_ptr<utxo_t> utxo(new utxo_t());
        assert(utxos.find(utxo->addr) == utxos.end());
        utxos[utxo->addr] = utxo;
    }
}

void create_chain_and_write(
  std::ostream& out, unsigned len,
  utxo_map_t& utxos, int num_txns)
{
    utxos.clear();

    std::shared_ptr<block_t> prev_block;

    for(unsigned blockIndex = 0;blockIndex < len; blockIndex++) {
        ensure_min_utxos(utxos, num_txns);

        // find a random reward address
        int cnt = rand() % utxos.size();
        auto reward_pos = utxos.begin();
        for(int i = 0; i < cnt; i++, reward_pos++);
        auto reward_addr = reward_pos->first;

        std::shared_ptr<block_t> block;
        if (blockIndex == 0) {
            // just create coinbase
            block = create_coinbase(reward_addr);
            prev_block = block;
        } else {
            block = std::shared_ptr<block_t>(new block_t(prev_block));
            block->reward_addr = reward_addr;

            // create some transactions
            for (int txIndex = 0; txIndex < num_txns; txIndex++) {
                // try to find a utxo to spend.
                auto spend_pos = find_spendable(utxos, reward_addr);
                if (spend_pos == utxos.end()) break;
                std::shared_ptr<utxo_t> spend(spend_pos->second);
                // find receiver.
                std::shared_ptr<utxo_t> recv;
                for (auto pos = utxos.begin(); pos != utxos.end(); pos++) {
                    if (pos->first != spend->addr) {
                        recv = pos->second;
                        break;
                    }
                }
                assert (recv);
                // find change address.
                std::shared_ptr<utxo_t> change;
                for (auto pos = utxos.begin(); pos != utxos.end(); pos++) {
                    if (pos->first != spend->addr && pos->first != recv->addr) {
                        change = pos->second;
                        break;
                    }
                }
                assert (change);
                // randomly choose amount.
                uint64_t amount = (rand() % spend->amount) + 1;
                while (amount == 0) { amount = (rand() % spend->amount) + 1; }
                // create transaction.
                auto txn = create_txn(*spend, *recv, *change, amount);
                txn->validate();

                block->add_transaction(txn, true);
                // update receiver state.
                recv->amount += amount;
                // update change addr.
                change->amount = spend->amount - amount;
                // clear out spender.
                spend->amount = 0;

                // add to utxo map.
                utxos[recv->addr] = recv;
                utxos[change->addr] = change;
                utxos.erase(spend->addr);

                // ensure we have a minimum number of utxos.
                ensure_min_utxos(utxos, num_txns);
            }
            block->set_hash();
            assert (!block->is_coinbase());
            assert (block->validate());
        }
        std::cout << "[" << std::setw(5) << blockIndex << "] "
                  << "blk: " << block->blk_hash 
                  << "; #txns: " << block->transactions.size()
                  << std::endl;
        block->write(out);
        prev_block = block;
        utxos[reward_addr]->amount += BLOCK_REWARD;
    }
}

int main(int argc, char* argv[])
{
    crypto_init();

    if (argc != 4) {
        std::cerr << "Syntax error. Usage: " << std::endl;
        std::cerr 
          << "  genchain <num_blocks> <max_txns_per_block> <outfilename>"
          << std::endl;
        return 1;
    }

    std::list< std::shared_ptr<block_t> > chain;
    utxo_map_t utxos;

    const unsigned chain_length = std::atoi(argv[1]);
    const unsigned num_txns = std::atoi(argv[2]);

    std::ofstream fout(argv[3]);
    if (fout.good()) {
        create_chain_and_write(fout, chain_length, utxos, num_txns);
    }
    fout.close();

    crypto_finalize();
}
