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
//wt1 et wt2 sont les deux WorkTrees a comparer et conflicts est la liste des noms de fichiers en conflits
//PAS FINI!!!
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){
    if(wt1 == NULL || wt2 == NULL){
        printf("mergeWorkTrees: Au moins un des WorkTrees est mal défini\n");
        return NULL;
    }
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
    return wt3;
}

//fusionne la branche courante avec la branche pass ́ee en parametre si aucun conflit n’existe.
List* merge(char* remote_branch, char* message){


}

//cree et ajoute un commit de suppression sur la branche branch, correspondant a la suppression des elements de la liste conflicts
void createDeletionCommit(char* branch, List* conflicts, char* message){

}