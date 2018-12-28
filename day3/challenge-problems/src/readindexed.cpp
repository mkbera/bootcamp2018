#include <set>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "crypto.h"
#include "blockchain.h"
#include "transaction.h"
#include "os.h"

#define TDIFF(start, end) ((end.tv_sec - start.tv_sec) * 1000000 + (long)(end.tv_usec - start.tv_usec))

size_t extractHeader(
    uint8_t* data,
    size_t dataSz,
    uint32_t& chunks, 
    std::vector<uint64_t>& chunkOff,
    std::vector<uint64_t>& mapOff,
    std::vector<uint64_t>& mapSz
)
{
    uint8_t *dataStart = data;

    if (dataSz < sizeof(chunks)) return 0;
    chunks = *((uint32_t*)data);
    data += sizeof(chunks);
    dataSz -= sizeof(chunks);

    chunkOff.resize(chunks);
    mapOff.resize(chunks);
    mapSz.resize(chunks);

    const size_t offset_sz = (sizeof(uint64_t)*chunks);
    if (dataSz < offset_sz*3) {
        return 0;
    }

    std::copy(data, data+offset_sz, (uint8_t*) chunkOff.data());
    data += offset_sz;
    std::copy(data, data+offset_sz, (uint8_t*) mapOff.data());
    data += offset_sz;
    std::copy(data, data+offset_sz, (uint8_t*) mapSz.data());
    data += offset_sz;

    const size_t totalSz =  3*offset_sz + sizeof(chunks);
    const size_t ptrSz = (data - dataStart);
    assert (totalSz == ptrSz);
    return totalSz;
}

int readChunk(
    uint8_t* data,
    uint64_t prevMapOff,
    uint64_t chunkStart,
    uint64_t mapStart,
    std::set<std::shared_ptr<block_t> > & heads,
    block_map_t& tails,
    pthread_mutex_t* mutex)
{
    using namespace std;

    balance_map_t init_bal;
    if (prevMapOff != 0) {
        read_balance_map(data + prevMapOff, init_bal);
    }

    balance_map_t final_bal;
    read_balance_map(data + mapStart, final_bal);

    size_t chunkSz = mapStart - chunkStart;
    size_t bytesLeft = chunkSz;
    shared_ptr<block_t> tip;
    shared_ptr<block_t> first_block;
    uint8_t* chunkPtr = data + chunkStart;

    block_map_t bmap;
    int chunk_len = 0;
    for(int i = 0; bytesLeft > 0; i++) {
        chunk_len ++;
        shared_ptr<block_t> blk(new block_t());
        size_t bytesParsed = blk->read(chunkPtr, bytesLeft, bmap);
        bytesLeft -= bytesParsed;
        chunkPtr += bytesParsed;
        if (bytesParsed == 0) break;
        (void)bytesParsed;
        if (i==0)
          first_block = blk;
        if (i==0 && !blk->is_coinbase()) {
          std::shared_ptr<block_t> dummy =  (std::shared_ptr<block_t>)new block_t();
          dummy->set_balances(init_bal);
          blk->set_prev_block(dummy);
        }
        // else {
        //   if (bmap.find(blk->prev_hash) == bmap.end()) return 0;
        //   blk->set_prev_block(bmap[blk->prev_hash]);
        // }
        bool blk_valid = blk->validate();
        bmap[blk->blk_hash] = blk;
        if (blk_valid == false) {
          return 0;
        }
        tip = blk;
    }

    balance_map_t::iterator itr;
    balance_map_t last_balance = tip->get_balances();
    for (itr = last_balance.begin(); itr != last_balance.end(); ++itr) { 
      hash_result_t key = itr->first;
      if (final_bal.find(key) == final_bal.end()){ 
        std::cout << key << " :" << last_balance[key] << "\n";
        // printf("not find key in final bal\n");
        return 0;
      }
      if (final_bal[key] != last_balance[key]){
        // printf("final bal neq last bal\n");
        return 0;
      } 
    } 

    pthread_mutex_lock(mutex);
    tails[tip->blk_hash] = tip;
    

    if (!first_block->is_coinbase()) {
      heads.insert(first_block);    
      
    }
    pthread_mutex_unlock(mutex);
    
    return chunk_len;
    
    /*TODO Insert into the tail map and return the length*/
    // Return 0 on failure
}

void *doReadChunk(void *arg)
{
   struct thread_data *thdata = (struct thread_data *) arg;
   thdata->len = readChunk(thdata->dptr, 
           thdata->prevmap,
           thdata->chunkoff,
           thdata->map,
           *(thdata->heads),
           *(thdata->tails),
           thdata->mutex);
   pthread_exit(NULL);
}   
using namespace os;
int main(int argc, char* argv[])
{
    using namespace std;

    crypto_init();
    if (argc < 2) {
        cerr << "Syntax error. Usage: " << endl;
        cout << "    readchain <filename> [--secure]" << endl;
        return 1;
    }

    block_map_t bmap;

      os::osinstream_t fin(argv[1], true);
      if (!fin) {
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
	return 1;
      }
      // read the chunk header.
      uint32_t chunks;
      vector<uint64_t> chunkOff, mapOff, mapSz;
      size_t hdSz = extractHeader(
        fin.getraw(), fin.getsize(), chunks, chunkOff, mapOff, mapSz);
       // did we fail to read header?
       if (hdSz == 0) {
            cerr << "Unable to read header." << endl;
            return 1;
         }
    // How many chunks
    std::cout << "chunks = " << chunks << std::endl;
    // now read each of the chunks.
    set< shared_ptr<block_t> > heads;
    block_map_t tails;
    size_t total_len = 0;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    struct thread_data *thdata = new thread_data[chunks];
    struct thmgr_t *tmgr = NULL;
    tmgr = new thmgr_t(chunks);   // Not using TSEC
       
    struct timeval start, end;
    assert(gettimeofday(&start, NULL) == 0);
    for (uint32_t i=0; i < chunks; i++) {
           thdata[i].dptr = fin.getraw(); 
           thdata[i].prevmap = (i == 0 ? 0 : mapOff[i-1]);
           thdata[i].chunkoff = chunkOff[i];
           thdata[i].map = mapOff[i];
           thdata[i].heads = &heads;
           thdata[i].tails = &tails;
           thdata[i].mutex = &mutex,
           thdata[i].len = 0;
           
           assert(tmgr->create(doReadChunk, &thdata[i]) == i);
           assert(tmgr->launch(i) == 0);
    }
     
    for (uint32_t i=0; i < chunks; i++){ 
               tmgr->wait(i);
               total_len += thdata[i].len;
    }
    assert(gettimeofday(&end, NULL) == 0);
    delete tmgr;
    
    pthread_mutex_destroy(&mutex);

    bool valid = true;
    // std:: cout << "heads " << heads.size() << " " << tails.size() << "\n";
    for (auto p : heads) {
        auto pos = tails.find(p->prev_hash);
        if (pos == tails.end()) {
            cout << "unable to find pred for: " << p->blk_hash << std::endl;
            valid = false;
            break;
        } else {
            p->set_prev_block(pos->second);
        }
    }

    unsigned length = 0;
    shared_ptr<block_t> tip;
    for (auto p : tails) {
        p.second->recompute_length();
        if (length < p.second->length) {
            length = p.second->length;
            tip = p.second;
        }
    }
    if (valid && tip) {
        cout << "blk: " << tip->blk_hash << "; len: " << tip->length << endl;
    } else {
        cout << "invalid." << endl;
    }
    cout << "Time taken = " << TDIFF(start, end) << " microsecs" << endl;
    crypto_finalize();
    return 0;
}
