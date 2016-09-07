#include <stdio.h>
#include "khash.h"
KHASH_MAP_INIT_INT(m32, int)      // instantiate structs and methods
int main() {
    int absent, is_missing;
    khint_t k;
    khash_t(m32) *h = kh_init(m32);  // allocate a hash table

    k = kh_put(m32, h, 101, &absent);  // insert a key to the hash table
    kh_value(h, k) = 10;             // set the value
    
    int i;
    for (i = 0; i < 20; i++) {
        k = kh_put(m32, h, i, &absent);  // insert a key to the hash table
        if (!absent) {
            kh_del(m32, h, k);
            printf("Key %d existed!\n", i);
            continue;
        }
        printf("put (%d, %d) pair\n", i, i*i);
        kh_value(h, k) = i*i;             // set the value
    }

    int key = 102;
    k = kh_get(m32, h, key);          // query the hash table
    is_missing = (k == kh_end(h));   // test if the key is present
    if (is_missing)
        printf("Key %d does not existed!\n", key);

    k = kh_get(m32, h, 5);
    int v = kh_value(h, k);
    printf("Value is %d\n", v);

    kh_del(m32, h, k);               // remove a key-value pair
    for (k = kh_begin(h); k != kh_end(h); ++k) {  // traverse
        if (kh_exist(h, k)) {            // test if a bucket contains data
            int x = kh_key(h, k);
            int y = kh_value(h, k);
            printf(" value for key %d is %d\n", x, y);
        }

    }
    kh_destroy(m32, h);              // deallocate the hash table
    return 0;
}