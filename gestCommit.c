#include "commit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wf.h"
#include "hash.h"
#include "lcc.h"
#include "git.h"
#include "gestCommit.h"


//Crée le répertoire caché .refs (s’il n’existe pas déjà), puis crée les fichiers master et HEAD (vides)
void initRefs () {
    if (!file_exists(".refs")){
        system ( "mkdir .refs" ) ;
        system ( "touch .refs/master");
        system ( "touch .refs/HEAD");
    }
}

//met à jour une référence en remplaçant son contenu par hash.
void createUpdateRef(char* ref_name, char* hash){
    char buff [100];
    sprintf(buff, "echo %s > .refs/%s" , hash , ref_name);
    system(buff);
}

//Permet de supprimer une référence
void deleteRef(char *ref_name ) {
    char buff [256];
    sprintf ( buff , ".refs/%s" , ref_name ) ;
    if (! file_exists ( buff ) ) {
        printf ( "fnc deleteRef: The reference %s does not exist\n", ref_name );
    } else {
    sprintf(buff , "rm .refs/%s" , ref_name ) ;
    system(buff ) ;
    }
}
//récupère vers quoi pointe une référence (c-à-d le hash contenu dans le fichier)
char* getRef(char* ref_name){
    FILE *fp;
    char *result = malloc(sizeof(char)*256) ;
    char buff[256];
    sprintf(buff ,".refs/%s",ref_name);
    if (!file_exists(buff)){
        printf ( "fnc getRef: The reference %s does not exist\n" , ref_name ) ;
        return NULL ;
    }
    fp = fopen( buff ,"r" ) ;
    if ( fp == NULL ) {
        printf ( "fnc getRef: Error opening file \n") ;
        return NULL ;
    }
    fgets (result,256,fp) ;
    fclose (fp) ;
    return result ;
}
//Permet de créer un fichier
void createFile(char * file){
    char buff [100];
    sprintf(buff , "touch %s" , file ) ;
    system(buff);
}

//permet à un utilisateur d’ajouter un fichier ou un répertoire dans le WorkTree correspondant à la zone de préparation.
void myGitAdd(char* file_or_folder){
    WorkTree * wt ;
    if (! file_exists (".add")){
        createFile ( ".add") ;
        wt = initWorkTree () ;
    } else {
        wt = ftwt (".add") ;
    }
    if ( file_exists ( file_or_folder ) ) {
        appendWorkTree ( wt , file_or_folder, NULL ,0) ;
        wttf ( wt , ".add" ) ;
    } else {
        printf ( "file or folder %s does not exist \n" , file_or_folder ) ;
    }
}
//Simulation de la commande git commit
void myGitCommit(char* branch_name, char* message){
    //printf("On rentre dans gitCommit\n");
    if (!file_exists(".refs" ) ) {
        printf ( "Il faut d'abord initaliser le refs du projet\n");
        return;
    }
    char buff [256];
    sprintf (buff,".refs/%s" , branch_name);
    if (!file_exists(buff)){
        printf ("La branche n'existe pas\n") ;
        return ;
    }
    char * last_hash = getRef(branch_name);
    char * head_hash = getRef("HEAD");
    if(last_hash==NULL &&head_hash==NULL){
        printf("Problème de définition des hash\n");
    }
    if (strcmp (last_hash ,head_hash)!=0) {
        printf ("HEAD doit pointer sur le dernier commit de la branche\n") ;
        return ;
    }
    WorkTree * wt = ftwt(".add");
    char * hashwt = saveWorkTree(wt,".");
    Commit * c = createCommit (hashwt);
    if ( strlen ( last_hash ) !=0) {
        commitSet (c, "predecessor" ,last_hash ) ;
    }
    if (message!=NULL) {
        commitSet (c,"message",message ) ;
    }
    char * hashc = blobCommit(c);
    createUpdateRef(branch_name,hashc);
    createUpdateRef("HEAD",hashc);
    system ( "rm .add") ;
}
//void initBranch() qui crée le fichier caché .current branch contenant la chaı̂ne de caractères ”master”.
void initBranch(){
    FILE * f = fopen ( ".current_branch" ,"w");
    fputs("master",f ) ;
    fclose(f);
}
//vérifie l’existence d’une branche
int branchExists(char *branch){
    List * refs = listdir(".refs");
    return searchList(refs,branch )!= NULL ;
}
//crée une référence appelée branch, qui pointe vers le même commit que la référence HEAD
void createBranch(char* branch){
    char * hash = getRef("HEAD");
     if (hash == NULL) {
        printf("Error getting hash from HEAD reference");
        //return;
    }
    createUpdateRef(branch,hash);
}

//lit le fichier caché .current branch pour retourner le nom de la branche courante
char* getCurrentBranch(){
    FILE * f = fopen (".current_branch","r");
    char * buff = malloc(sizeof(char)*100);
    fscanf (f, "%s",buff);
    return buff;
}
char *hashToPathCommit(char *hash) {
    char *buff = malloc(sizeof(char) * 150);
    char *path = hashToPath(hash);
    path[strcspn(path, "\n")] = '\0'; // supprimer le caractère de saut de ligne à la fin de la chaîne
    sprintf(buff, "%s.c", path);
    free(path);
    return buff;
}


//parcourt la branche appelee branch, et pour chacun de ses commits, affiche son hash et son message descriptif
void printBranch(char* branch){
    char * commit_hash = getRef(branch);
    //printf("commit_hash: %s\n",commit_hash);
    //printf("hashToPathCommit(commit_hash): %s\n",hashToPathCommit(commit_hash));
    Commit * c = ftc(hashToPathCommit(commit_hash));
    while (c !=NULL){
        if(commitGet(c,"message") != NULL )
            printf( "%s -> %s \n",commit_hash,commitGet(c,"message"));
        else
            printf("%s \n",commit_hash);
        if (commitGet(c ,"predecessor") != NULL){
            commit_hash=commitGet(c,"predecessor");
            c=ftc(hashToPathCommit(commit_hash));
        }else{
            c=NULL;
        }
    }
}
//qui contruit et retourne une liste  chaı̂née contenant le hash de tous les commits de la branche.
List* branchList(char* branch){
    List * L = initList();
    char * commit_hash = getRef(branch);
    Commit * c = ftc(hashToPathCommit(commit_hash));
    while ( c != NULL ) {
        insertFirst (L,buildCell(commit_hash));
        if ( commitGet (c ,"predecessor") != NULL ) {
            commit_hash = commitGet (c ,"predecessor");
            c = ftc(hashToPathCommit(commit_hash));
        }else{
           c = NULL ;
        }
    }
    return L;
}
//renvoie la liste des hash des commits de toutes les branches (sans répétition)
List *getAllCommits (){
    List * L = initList();
    List * content = listdir (".refs");
    for ( Cell * ptr = * content ; ptr != NULL ; ptr = ptr-> next ) {
        if ( ptr-> data [0] == '.' )
            continue;
        List * list = branchList ( ptr -> data ) ;
        Cell * cell = * list ;
        while ( cell != NULL ) {
            if (searchList(L,cell -> data ) == NULL ) {
                insertFirst (L,buildCell(cell->data));
            }
        cell = cell -> next ;
        }
    }
    return L ;
}
//permet de restaurer le worktree associé à un commit dont le hash est donné en paramètre
void restoreCommit(char* hash_commit){
    char * commit_path = hashToPathCommit ( hash_commit ) ;
    Commit * c = ftc(commit_path);
    char * tree_hash = strcat(hashToPath(commitGet(c ,"tree")) ,".t");
    WorkTree * wt = ftwt(tree_hash);
    restoreWorkTree ( wt ,".");
}

void myGitCheckoutBranch(char* branch){
    //Change current_branch
    FILE * f = fopen(".current_branch","w");
    fprintf (f,"%s", branch ) ;
    fclose(f);

    char * hash_commit = getRef(branch);
    createUpdateRef("HEAD",hash_commit);
    restoreCommit(hash_commit);
}
//retourne une nouvelle liste contenant uniquement les éléments de L qui commencent par la chaı̂ne de caractères pattern
List* filterList(List* L, char* pattern){
    List * filtered =initList();
    for( Cell * ptr = * L ; ptr != NULL ; ptr = ptr -> next ) {
        char * c = strdup ( ptr->data);
        c [strlen(pattern)] ='\0';
        if ( strcmp (c , pattern ) == 0) {
            insertFirst ( filtered , buildCell ( ptr -> data ) ) ;
        }
        free ( c ) ;
    }
    return filtered ;
}

//Compte le nombre d'élément dans la liste
int listSize(List *l) {
    int count = 0;
    Cell* current = *l;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}



void myGitCheckoutCommit(char* pattern){
    List * L = getAllCommits();
    List * filtred_list = filterList(L,pattern);
    if (listSize(filtred_list)==1){
        char * commit_hash = (listGet(filtred_list , 0))-> data ;
        createUpdateRef ("HEAD", commit_hash ) ;
        restoreCommit ( commit_hash ) ;
    } else {
        if(listSize(filtred_list)==0){
            printf ( "No pattern matching\n");
        } else {
            printf ( "Multipkle matching found: \n");
            for ( Cell *ptr=*filtred_list;ptr!=NULL;ptr=ptr->next ) {
                printf ( "-> %s \n" , ptr -> data);
            }
        }
    }
}