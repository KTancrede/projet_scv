void initRefs();
void createUpdateRef(char* ref_name, char* hash);
void deleteRef(char* ref_name);
char* getRef(char* ref_name);
void createFile(char * file);
void myGitAdd(char* file_or_folder);
void myGitCommit(char* branch_name, char* message);
void initBranch();
int branchExists(char* branch);
void createBranch(char* branch);
char* getCurrentBranch();
char * hashToPathCommit(char * hash);
int listSize(List l);
List* branchList(char* branch);
List* getAllCommits();
void restoreCommit(char* hash_commit);
void myGitCheckoutBranch(char* branch);
List* filterList(List* L, char* pattern);
void myGitCheckoutCommit(char* pattern);