
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts);
List* merge(char* remote_branch, char* message);
void createDeletionCommit(char* branch, List* conflicts, char* message);