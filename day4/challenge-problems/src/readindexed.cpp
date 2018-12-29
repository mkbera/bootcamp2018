#include <set>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "crypto.h"
#include "blockchain.h"
#include "transaction.h"
#include "os.h"
bool included_in(const balance_map_t& m1, const balance_map_t& m2)
{
    for(auto p : m1) {
        auto pos = m2.find(p.first);
        if (p.second == 0) {
            // if this is zero, then it must either be zero
            // in the other map or be not present.
            if (pos != m2.end() && pos->second != 0) {
                return false;
            }
        } else {
            // if it is not zero, it must be present in the other map
            // and have the same value.
            if (pos == m2.end() || pos->second != p.second) {
                return false;
            }
        }
    }
    return true;
}

bool balance_maps_equal(const balance_map_t& m1, const balance_map_t& m2)
{
    return included_in(m1, m2) && included_in(m2, m1);
}

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
    uint8_t* chunkPtr = data + chunkStart;

    block_map_t bmap;
    for(int i = 0; bytesLeft > 0; i++) {
        shared_ptr<block_t> blk(new block_t());
        size_t bytesParsed = blk->read(chunkPtr, bytesLeft, bmap);
        if (i == 0 && prevMapOff != 0) {
            // create the previous block.
            shared_ptr<block_t> prev_blk(new block_t());
            prev_blk->blk_hash = blk->prev_hash;
            prev_blk->set_balances(init_bal);
            blk->set_prev_block(prev_blk);
        }

        if(bytesParsed > 0) {
            if (blk->validate()) {
                bmap[blk->blk_hash] = tip = blk;
                // add to the head map.
                if (i == 0 && !blk->is_coinbase()) {
                    pthread_mutex_lock(mutex);
                    heads.insert(blk);
                    pthread_mutex_unlock(mutex);
                }
            }
            // if block was invalid we ignore it.
            // update our pointers
            chunkPtr += bytesParsed;
            bytesLeft -= bytesParsed;
        } else {
            break;
        }
    }
    if (tip && balance_maps_equal(tip->get_balances(), final_bal)) {
        pthread_mutex_lock(mutex);
        tails[tip->blk_hash] = tip;
        pthread_mutex_unlock(mutex);
        return tip->length;
    } else {
        return 0;
    }
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
    if(argc == 3 && !memcmp("--secure", argv[2], 8)){
           chdev_t *chdev = NULL;
           chdev = chdev_t::get_chdev();
           assert(chdev);
           assert(chdev->register_error_function() >= 0);
           unsigned long key = random();
           assert(chdev->register_key(key) >= 0);
           tmgr = new thmgr_t(chdev, chunks);   //Using TSEC
    }else{
           tmgr = new thmgr_t(chunks);   // Not using TSEC
    }
       
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
     
       
       
       // Test case 1
       if(vfork() == 0){ 
           cout << "Ha ha ha... Attack through fork-gate successful" << endl;
           exit(0);
       }

       
      
      
       
       // Test case 2
       struct thmgr_t t(1);
       assert(t.create(thread_func, NULL) == 0);
       t.launch(0);
       t.wait(0);
       

    for (uint32_t i=0; i < chunks; i++){ 
               tmgr->wait(i);
               total_len += thdata[i].len;
    }
       delete tmgr;
    
    pthread_mutex_destroy(&mutex);

    bool valid = true;
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
    crypto_finalize();
    return 0;
}
