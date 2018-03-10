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
  return 0;
}

int deconstruct(void) {
  rocksdb_checkpoint_object_destroy(rocks.cp);
  rocksdb_close(rocks.db);
  return 0;
}

void handle_read(char *key, size_t keylen)
{
  if (!is_open()) {
    return;
  }
  char *err = NULL;
  char *val;
  size_t vallen;
  val = rocksdb_get(rocks.db, rocks.readoptions, key, keylen, &vallen, &err);
  if (err != NULL) {
    fprintf(stdout, "Read Error: %s\n", err);
    return;
  }
  if (val == NULL) {
    printf("Value was not found for key %s\n", key);
    return;
  }
  printf("Found value %s, vallen %zu for key %s\n", val, vallen, key);
  free(val);
}

void handle_delete(char *key, size_t keylen)
{
  if (!is_open()) {
    return;
  }
  char *err = NULL;
  rocksdb_delete(rocks.db, rocks.writeoptions, key, keylen, &err);
  if (err != NULL) {
    fprintf(stdout, "Delete Error: %s\n", err);
    return;
  }
}

void handle_write(char *key, size_t keylen, char *val, size_t vallen)
{
  if (!is_open()) {
    return;
  }
  char *err = NULL;
  rocksdb_put(rocks.db, rocks.writeoptions, key, keylen, val, vallen, &err);
  if (err != NULL) {
    fprintf(stdout, "Write Error: %s\n", err);
  }
}

void handle_checkpoint(char* checkpoint_dir) {
  if (!is_open()) {
    return;
  }
  char *err = NULL;
  uint64_t log_size_for_flush = 8192;
  if (rocks.cp == NULL) {
    create_checkpoint_object();
  }
  rocksdb_checkpoint_create(rocks.cp, checkpoint_dir, log_size_for_flush, &err);
  if (err != NULL) {
    fprintf(stdout, "Checkpoint Error: %s\n", err);
  }
}

int handle_requests(char** argv, int count) {
  if (strcmp("read", argv[0]) == 0) {
    if (count < 2) {
      fprintf(stderr, "Not enough argument for reading\n");
      return -1;
    }
    handle_read(argv[1], strlen(argv[1]));
  }
  else if (strcmp("delete", argv[0]) == 0) {
    if (count < 2) {
      fprintf(stderr, "Not enough argument for deleting\n");
      return -1;
    }
    handle_delete(argv[1], strlen(argv[1]));
  }
  else if (strcmp("write", argv[0]) == 0) {
    if (count < 3) {
      fprintf(stderr, "Not enough argument for writing\n");
      return -2;
    }
    handle_write(argv[1], strlen(argv[1]), argv[2], strlen(argv[2]));
  }
  else if (strcmp("open", argv[0]) == 0) {
    if (count < 2) {
      fprintf(stderr, "Missing arguments for opening database\n");
      return -3;
    }
    return open_db(argv[1]);
  }
  else if (strcmp("close", argv[0]) == 0) {
    return close_db();
  }
  else if (strcmp("checkpoint", argv[0]) == 0) {
    if (count < 2) {
      fprintf(stderr, "Not enough argument for checkpointing\n");
      return -1;
    }
    handle_checkpoint(argv[1]);
  }

  return 0;
}
