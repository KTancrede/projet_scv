#define SIZE_MAX 200
typedef struct key_value_pair {
    char* key;
    char* value;
} kvp;
typedef struct hash_table {
    kvp** T;
    int n;
    int size;
} HashTable;
typedef HashTable Commit;

kvp* createKeyVal(char* key, char* val);
void freeKeyVal(kvp* kv);
char* kvts(kvp* k);
kvp* stkv(char* str);
Commit* initCommit();
unsigned long hash(unsigned char *str);
void commitSet(Commit* c, char* key, char* value);
Commit* createCommit(char* hash);