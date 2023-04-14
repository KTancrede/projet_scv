#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "merge.h"
#include "wf.h"
#include "hash.h"
#include "unistd.h"
#include "lcc.h"
#include "git.h"
#include "gestCommit.h"
#include "commit.h"

// Fonction pour fusionner deux WorkTrees et créer une liste de conflits
// wt1 et wt2 sont les deux WorkTrees à comparer, conflicts est un pointeur vers une liste de noms de fichiers en conflits
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){
    // Vérifie si les WorkTrees sont définis
    if(wt1 == NULL || wt2 == NULL){
        printf("mergeWorkTrees: Au moins un des WorkTrees est mal défini\n");
        return NULL;
    }
    // Si conflicts est NULL, crée une nouvelle liste
    if(conflicts == NULL){
        conflicts = initList();
    }

    // Crée un nouveau WorkTree pour stocker les fichiers fusionnés
    WorkTree* wt3 = initWorkTree();

    // Parcours tous les fichiers de wt1
    for(int i=0;i<wt1->n;i++){
        // Initialise un flag pour déterminer si un fichier est présent dans les deux WorkTrees
        int flag = 0;
        // Parcours tous les fichiers de wt2
        for(int j=0;j<wt2->n;j++){
            // Si le nom du fichier est le même dans les deux WorkTrees
            if(strcmp(wt1->tab[i].name,wt2->tab[j].name)==0){
                // Le fichier est présent dans les deux WorkTrees, met à jour le flag
                flag = 1;
                // Si les fichiers sont différents
                if(strcmp(wt1->tab[i].hash,wt2->tab[j].hash)!=0){
                    // Ajoute le nom du fichier à la liste de conflits
                    insertFirst(conflicts,buildCell(wt1->tab[i].name));
                }
                // Si les fichiers sont identiques
                else{
                    // Ajoute le fichier au nouveau WorkTree
                    appendWorkTree(wt3,wt1->tab[i].name,wt1->tab[i].hash,wt1->tab[i].mode);
                }
            }
        }
        // Si le fichier n'est présent que dans wt1
        if(!flag){
            // Ajoute le fichier au nouveau WorkTree
            appendWorkTree(wt3,wt1->tab[i].name,wt1->tab[i].hash,wt1->tab[i].mode);
        }
    }

    // Parcours tous les fichiers de wt2
    for(int i=0;i<wt2->n;i++){
        // Initialise un flag pour déterminer si un fichier est présent dans wt3 (le nouveau WorkTree)
        int flag = 0;
        // Parcours tous les fichiers de wt3
        for(int j=0;j<wt3->n;j++){
            // Si le nom du fichier est le même dans wt2 et wt3
            if(strcmp(wt2->tab[i].name,wt3->tab[j].name)==0){
                // Le fichier est déjà dans le nouveau WorkTree, met à jour le flag
                flag = 1;
            }
        }
        // Si le fichier n'est pas déjà dans le nouveau WorkTree
        if(!flag){
            // Ajoute le fichier au nouveau WorkTree
            appendWorkTree(wt3,wt2->tab[i].name,wt2->tab[i].hash,wt2->tab[i].mode);
        }
    }

    // Retourne le nouveau WorkTree
    return wt3;
}



//fusionne la branche courante avec la branche passee en parametre si aucun conflit n’existe.
List* merge(char* remote_branch, char* message){
    //Verification des branches pour vérifier si elles existes
    if(!branchExists(getCurrentBranch())){
        printf("fnc merge: la branche courante n'est pas bien définie\n");
        return NULL;
    }
    if(!branchExists(remote_branch)){
        printf("fnc merge: la branche %s n'existe pas\n", remote_branch);
        return NULL;
    }

    //Verification des conflits (donc qu'il n'y en a pas précisement)
    List* conflicts = initList();
    WorkTree* wt_merge=mergeWorkTrees(getCurrentBranch(),remote_branch,&conflicts);
    if(listSize(conflicts)!=0){
        printf("fnc merge: il y a des conflits\n");
        return conflicts;
    }
    //Récupération de la référence de la branche courante et de la branche à fusionner
    char* current_branch_HEAD = getRef(getCurrentBranch());
    char* remote_branch_HEAD = getRef(remote_branch);

    //Création du nouveau commit
    Commit* new_commit = initCommit();
    commitSet(new_commit,"predecessor", current_branch_HEAD);
    commitSet(new_commit,"merged_predecessor", remote_branch_HEAD);
    commitSet(new_commit, "message", message);

    //Enregistrement instantané du wt de fusion et du nv commit
    char* savedWT = blobWorkTree(wt_merge);
    char* savedCommit = blobCommit(new_commit);

    //Mise a jour des refs de la branche courante et HEAD pour pointer vers le nouveau commit
    createUpdateRef(current_branch_HEAD, savedCommit);
    createUpdateRef("HEAD", savedCommit);

    //suppression de la ref de la branche en parametre
    deleteRef(remote_branch_HEAD);

    //Restauration du projet correspondant au wt de fusion
    restoreCommit(savedCommit);

    return NULL;
}

//cree et ajoute un commit de suppression sur la branche branch, correspondant a la suppression des elements de la liste conflicts
void createDeletionCommit(char* branch, List* conflicts, char* message) {
    char* current_branch = getCurrentBranch();

    // One se deplace sur la branche en question
    myGitCheckoutBranch(branch);

    // On recupere le dernier commit de cette branche et le worktree associe
    char* last_commit_hash = getRef(branch);
    Commit* last_commit = ftc(hashToPathCommit(last_commit_hash));
    char* tree_hash = commitGet(last_commit, "tree");
    WorkTree* wt = ftwt(hashToPath(tree_hash));

    // On vide la zone de preparation
    if (file_exists(".add")) {
        system("rm .add");
    }

    // On ajoute les fichiers/repertoires du worktree qui ne font pas partie de la liste des conflits
    for (int i = 0; i < wt->n; i++) {
        WorkFile wf = wt->tab[i];
        if (searchList(conflicts, wf.name) == NULL) {
            myGitAdd(wf.name);
        }
    }

    // On cree le commit de supression
    myGitCommit(branch, message);

    // On revient sur la branche de depart
    myGitCheckoutBranch(current_branch);
}