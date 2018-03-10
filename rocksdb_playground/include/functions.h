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
int parse_lineinput(char *lineinput, char* *array, int *count);
int handle_requests(char** array, int count);

extern struct rocksdb_params rocks;
