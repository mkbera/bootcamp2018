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
