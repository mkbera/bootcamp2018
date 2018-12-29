#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <assert.h>

#include <iostream>
#include <cstdlib>

#include "crypto.h"

std::string gen_random_string(size_t n)
{
    static const int range = 'z' - '0' + 1;
    std::string text;
    text.resize(n);
    for (unsigned i = 0; i != n; i++) {
        text[i] = (rand() % range) + '0';
    }
    return text;
}

std::string mutate(const std::string& text)
{
    std::string tp(text);
    unsigned pos = rand() % tp.length();
    if((rand() & 15) < 11) {
        if (islower(tp[pos])) {
            tp[pos] = toupper(tp[pos]);
        } else {
            tp[pos] = tolower(tp[pos]);
        }
    }
    return tp;
}

const int MAX_KEY_FILES = 10000;
int keys_gen = 0;

int check_signature()
{
    std::string text = gen_random_string(64);
    std::string textp = mutate(text);

    bool equal = text == textp;

    uint8_t h1[SHA256_DIGEST_LENGTH], h2[SHA256_DIGEST_LENGTH];
    sha256((const uint8_t*) text.data(), text.length(), h1);
    sha256((const uint8_t*) textp.data(), textp.length(), h2);

    bool heq = true;
    for(unsigned i=0; i != SHA256_DIGEST_LENGTH; i++) {
        heq = heq && (h1[i] == h2[i]);
    }

    int n = (rand() % MAX_KEY_FILES) + 1;
    std::unique_ptr<rsa_private_key_t> priKey;
    std::unique_ptr<rsa_public_key_t> pubKey;

    int rnd = (rand() % 3);
    if (0 == rnd) {
        char priPEMFile[32], pubPEMFile[32];
        sprintf(priPEMFile, "keys/k%05d.pri.pem", n);
        sprintf(pubPEMFile, "keys/k%05d.pub.pem", n);

        // set private key.
        priKey.reset(rsa_private_key_t::create_from_PEM(priPEMFile));
        // create one private key object from the file.
        std::unique_ptr<rsa_public_key_t> pubkey1(
            rsa_public_key_t::create_from_PEM(pubPEMFile));
        // create a new private key from the DER representation of pubkey1
        std::unique_ptr<rsa_public_key_t> pubkey2(
            new rsa_public_key_t(pubkey1->keyDER.data(),
                                 pubkey1->keyDER.size()));
        // now set the result
        pubKey = std::move(pubkey2);
    } else if (1 == rnd) {
        char priDERFile[32], pubDERFile[32];
        sprintf(priDERFile, "keys/k%05d.pri.der", n);
        sprintf(pubDERFile, "keys/k%05d.pub.der", n);

        // set private key.
        priKey.reset(rsa_private_key_t::create_from_DER(priDERFile));
        // create one private key object from the file.
        std::unique_ptr<rsa_public_key_t> pubkey1(
            rsa_public_key_t::create_from_DER(pubDERFile));
        // create a new private key from the DER representation of pubkey1
        std::unique_ptr<rsa_public_key_t> pubkey2(
            new rsa_public_key_t(pubkey1->keyDER.data(),
                                 pubkey1->keyDER.size()));
        // now set the result
        pubKey = std::move(pubkey2);
    } else {
        keypair_t p = generate_rsa_keys();
        priKey.reset(p.first);
        pubKey.reset(p.second);
        keys_gen += 1;
    }


    std::vector<uint8_t> sign;
    priKey->sign((const uint8_t*) text.data(), text.length(), sign);
    bool ver = pubKey->verify((const uint8_t*) textp.data(), textp.length(), sign.data(), sign.size());

    assert (equal == heq);
    assert (equal == ver);

    return ((int) equal);
}

int main()
{
    crypto_init();

    int cnt = 0;
    const int NTESTS = 100;
    for (int i = 0; i < NTESTS; i++) {
        int v = check_signature();
        if (v) cnt += 1;
    }
    std::cout << keys_gen << "/" << cnt << "/" << NTESTS << std::endl;

    crypto_finalize();
    return 0;
}
