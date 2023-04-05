#include "gestCommit.h"
#include "commit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wf.h"
#include "hash.h"
#include "lcc.h"
#include "git.h"

//Crée le répertoire caché .refs (s’il n’existe pas déjà), puis crée les fichiers master et HEAD (vides)
void initRefs () {
    if (! file_exists (".refs")){
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
void deleteRef (char *ref_name ) {
    char buff [256];
    sprintf ( buff , ".refs/%s" , ref_name ) ;
    if (! file_exists ( buff ) ) {
        printf ( "The reference %s does not exist", ref_name );
    } else {
    sprintf(buff , "rm . ref/%s" , ref_name ) ;
    system(buff ) ;
    }
}
//récupère vers quoi pointe une référence (c-à-d le hash contenu dans le fichier)
char* getRef(char* ref_name){
    FILE *fp;
    char *result = malloc ( sizeof ( char ) *256) ;
    char buff [256];
    sprintf ( buff , ". refs/%s", ref_name ) ;
    if (! file_exists ( buff ) ) {
        printf ( "The reference %s does not exist" , ref_name ) ;
        return NULL ;
    }
    fp = fopen ( buff ,"r" ) ;
    if ( fp == NULL ) {
        printf ( "Error opening file \n") ;
        return NULL ;
    }
    fgets ( result , 256 , fp ) ;
    fclose ( fp ) ;
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

void myGitCommit(char* branch_name, char* message){
    if (!file_exists(".refs" ) ) {
        printf ( "Il faut d'abord initaliser le refs du projet");
        return;
    }
    char buff [256];
    sprintf (buff,".refs/%s" , branch_name);
    if (!file_exists(buff)){
        printf ("La branche n'existe pas") ;
        return ;
    }
    char * last_hash = getRef(branch_name);
    char * head_hash = getRef("HEAD");
    if (strcmp ( last_hash , head_hash ) !=0) {
        printf ("HEAD doit pointer sur le dernier commit de la branche") ;
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