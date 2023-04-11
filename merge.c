#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "merge.h"
#include "wf.h"
#include "hash.h"
#include "unistd.h"
#include "lcc.h"
#include "git.h"

//Cree une liste des noms de fichiers qui sont en conflits et un Worktree compose des fichiers qui ne sont pas en conflits
//PAS FINI!!!
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){ 
    if(conflicts == NULL){
        conflicts = initList();
    }
    if(wt1 == NULL || wt2 == NULL){
        printf("fnc inWorkTree: Au moins un des WorkTrees est mal défini");
        return -1;
    }
    WorkTree* wt3 = initWorkTree();
    for(int i=0;i<wt1->n;i++){
        for(int j=0;j<wt2->n;j++){
            if(strcmp(wt1->tab[i].name,wt2->tab[j].name)==0){
                if(strcmp(wt1->tab[i].hash,wt2->tab[j].hash)!=0){
                    insertFirst(conflicts,buildCell(wt1->tab[i].name));
                }
                else{
                    appendWorkTree(wt3,wt1->tab[i].name,wt1->tab[i].hash,wt1->tab[i].mode);
                }
            }
        }
    }
}