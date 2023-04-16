#define SIZE_MAX 200
typedef struct {
    char * name ;
    char * hash ;
    int mode ;
} WorkFile ;

typedef struct {
    WorkFile * tab ;
    int size ;
    int n ;
} WorkTree ;

WorkFile* createWorkFile(char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
int wttf(WorkTree* wt, char* file);
WorkTree* str_to_wt(char* ch);
WorkTree* ftwt(char* file);
int getChmod(const char *path);
void setMode ( int mode , char * path );
char* saveWorkTree(WorkTree* wt, char* path);
char* blobWorkTree(WorkTree* wt);
int isFile(const char* path);
int isDir(const char* path);
char * concat_paths ( char * path1 , char * path2 );
int isWorkTree ( char * hash );
void restoreWorkTree ( WorkTree * wt , char * path );
char * hashToFile ( char * hash );
WorkTree* getWorkTreeFromBranch(char* branch);