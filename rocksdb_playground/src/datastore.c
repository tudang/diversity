#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "functions.h"

void init_rocksdb(void)
{
    uint64_t mem_budget = 1048576;
    rocks.options = rocksdb_options_create();
    long cpus = sysconf(_SC_NPROCESSORS_ONLN);
    rocksdb_options_increase_parallelism(rocks.options, (int)(cpus));
    rocksdb_options_optimize_level_style_compaction(rocks.options, mem_budget);
    rocksdb_options_set_create_if_missing(rocks.options, 1);
    rocks.writeoptions = rocksdb_writeoptions_create();
    rocksdb_writeoptions_disable_WAL(rocks.writeoptions, 1);
    rocksdb_writeoptions_set_sync(rocks.writeoptions, 0);
    rocks.readoptions = rocksdb_readoptions_create();
}

inline int create_checkpoint_object() {
    char *err = NULL;
    rocks.cp = rocksdb_checkpoint_object_create(rocks.db, &err);
    if (err != NULL) {
        printf("Cannot create checkpoint object: %s\n", err);
        return -2;
    }
    return 0;
}

inline int is_open(void) {
    if (rocks.db == NULL) {
        fprintf(stderr, "DB is not openned\n");
        return 0;
    }
    return 1;
}

int open_db(char* db_path) {
    char* err = NULL;
    rocks.db = rocksdb_open(rocks.options, db_path, &err);
    if (err != NULL) {
        printf("Cannot open DB: %s\n", err);
        return -1;
    }
    return 0;
}

int close_db(void) {
    if (!is_open()) {
        return 0;
    }
    rocksdb_close(rocks.db);
    rocks.db = NULL;
    return 0;
}

int deconstruct(void) {
    if (rocks.cp)
        rocksdb_checkpoint_object_destroy(rocks.cp);

    if (rocks.db)
        rocksdb_close(rocks.db);
  return 0;
}

int handle_read(char *key, size_t keylen)
{
    if (!is_open()) {
        return -1;
    }
    char *err = NULL;
    char *val;
    size_t vallen;

    val = rocksdb_get(rocks.db, rocks.readoptions, key, keylen, &vallen, &err);
    if (err != NULL) {
        fprintf(stdout, "Read Error: %s\n", err);
        return -1;
    }
    if (val == NULL) {
        printf("Value was not found for key %s\n", key);
        return 0;
    }
    printf("Found value %s, vallen %lu for key %s\n", val, vallen, key);
    free(val);
    return 0;
}

int handle_list(void)
{
    if (!is_open())
        return -1;

    rocksdb_iterator_t* iterator = rocksdb_create_iterator(
        rocks.db, rocks.readoptions);
    if (!iterator) {
        fprintf(stderr, "Create Iterator has an error\n");
        return -1;
    }

    printf("%-20s | %-20s\n", "Key", "Value");
    printf("------------------------------------------------\n");
    for (rocksdb_iter_seek_to_first(iterator);
            rocksdb_iter_valid(iterator);
            rocksdb_iter_next(iterator))
    {
        size_t klen;
        const char *key = rocksdb_iter_key(iterator, &klen);
        size_t vlen;
        const char *value = rocksdb_iter_value(iterator, &vlen);
        printf("%-20s | %-20s\n", key, value);
    }
    rocksdb_iter_destroy(iterator);
    return 0;
}

int handle_delete(char *key, size_t keylen)
{
    if (!is_open()) {
        return -1;
    }
    char *err = NULL;
    rocksdb_delete(rocks.db, rocks.writeoptions, key, keylen, &err);
    if (err != NULL) {
        fprintf(stdout, "Delete Error: %s\n", err);
        return -1;
    }

    fprintf(stdout, "Deleted key %s\n", key);
    return 0;
}

int handle_write(char *key, size_t keylen, char *val, size_t vallen)
{
    if (!is_open()) {
        return -1;
    }
    char *err = NULL;
    rocksdb_put(rocks.db, rocks.writeoptions, key, keylen, val, vallen, &err);
    if (err != NULL) {
        fprintf(stdout, "Write Error: %s\n", err);
        return -1;
    }
    fprintf(stdout, "Write <%s,%s> OK\n", key, val);
    return 0;
}

int handle_checkpoint(char* checkpoint_dir) {
    if (!is_open()) {
        return -1;
    }
    char *err = NULL;
    uint64_t log_size_for_flush = 8192;
    if (rocks.cp == NULL) {
        create_checkpoint_object();
    }
    rocksdb_checkpoint_create(rocks.cp, checkpoint_dir, log_size_for_flush, &err);
    if (err != NULL) {
        fprintf(stdout, "Checkpoint Error: %s\n", err);
        return -1;
    }
    fprintf(stdout, "Checkpoint OK\n");
    return 0;
}
