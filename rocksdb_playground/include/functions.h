#include <rocksdb/c.h>


struct rocksdb_params {
  rocksdb_t *db;
  rocksdb_options_t *options;
  rocksdb_writeoptions_t *writeoptions;
  rocksdb_readoptions_t *readoptions;
  rocksdb_checkpoint_t *cp;
};

void init_rocksdb(void);
int deconstruct(void);
int open_db(char* db_path);
int close_db(void);
int handle_read(char *key, size_t keylen);
int handle_write(char *key, size_t keylen, char *val, size_t vallen);
int handle_delete(char *key, size_t keylen);
int handle_checkpoint(char* checkpoint_dir);
int handle_list(void);

extern struct rocksdb_params rocks;
