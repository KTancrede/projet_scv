#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lcc.h"
//Initialise la liste
List* initList(){
    /*
    List* liste=(List*)malloc(sizeof(List));
    if(liste!=NULL){
        *liste=(Cell*)malloc(sizeof(Cell));
        if(*liste!=NULL){
            (*liste)->data=NULL;
            (*liste)->next=NULL;
        }
    }
    return liste;*/
    //Correction
    List * L = malloc(sizeof(List));
    * L = NULL ;
    return L ;
}

//Créer une cellule
Cell* buildCell(char* ch){
    Cell* c=(Cell*)malloc(sizeof(Cell));
    c->data=strdup(ch);
    c->next=NULL;
    return c;
}

//Insere une cellule à la tête d'une liste
void insertFirst(List *L, Cell* C){
    if(L==NULL){
        printf("fnc insertFirst: La liste donnée en paramètre n'est pas définie\n");
        return;
    }
    else if(C==NULL){
        printf("fnc insertFirst: La cellule à insérer n'est pas définie correctement\n");
        return;
    }
    else{
        C->next=*L;
        *L=C;
    }
}

//retourne la chaı̂ne de caractères qu'une cellule représente
char* ctos(Cell* c){
    if(c==NULL){
        printf("fnc ctos: La cellule donnée n'existe pas\n");
        return NULL;
    }
    if(c->data == NULL) {
        return "";
    }
    return c->data;
}
//retourne la chaı̂ne de caractères qu'une liste représente
char* ltos(List* L){ //Differente du corrigée
    if(*L==NULL){
        printf("fnc ltos: La liste n'est pas définie\n");
        return "";
    }
    char * r=(char*)malloc(1024*sizeof(char));
    r[0]='\0';
    Cell* c = *L;
    while(c!=NULL){
        snprintf(r + strlen(r), 1024 - strlen(r), "%s|", ctos(c));
        c=c->next;
    }
    r[strlen(r) - 1] = '\0';
    return r;
}

//Cherche l'élément i d'une liste
Cell* listGet(List* L, int i){
    if(*L==NULL){
        printf("fnc listGet: La liste n'est pas définie\n");
        return NULL;
    }
    int n=0;
    Cell* c = *L;
    while(c!=NULL){
        if(i<n++){
            return c;
        }
        c=c->next;
    }
    printf("fnc listGet: l'élément n'a pas été trouvé\n");
    return NULL;
}

//recherche un élément dans une liste à partir de son contenu et renvoie une référence vers lui ou NULL s’il n’est pas dans la liste
Cell* searchList(List* L, char* str) {
    if (*L == NULL) {
        printf("fnc searchList: La liste n'est pas définie\n");
        return NULL;
    }
    List ptr = *L;
    while (ptr->data != NULL && strcmp(str,ptr->data)!=0) {
        ptr = ptr->next;
    }
    return ptr; // retourne NULL si l'élément n'est pas trouvé
}

//permet de transformer une chaı̂ne de caractères représentant une liste en une liste chaı̂née
List * stol ( char * s ) {
    int pos = 0;
    int n_pos = 0;
    //int size = strlen ( s ) ;
    int sep = '|';
    char * ptr ;
    char * result = malloc ( sizeof ( char ) *1000) ;
    //int end = 0;
    List * L = initList () ;
    while ( pos < strlen ( s ) ) {
        ptr = strchr ( s + pos , sep ) ;
        if ( ptr == NULL )
            n_pos = strlen ( s ) +1;
        else {
            n_pos = ptr - s + 1;
        }
        memcpy ( result , s + pos , n_pos - pos - 1) ;
        result[n_pos-pos-1] ='\0';
        pos = n_pos ;
        insertFirst (L , buildCell ( result ) ) ;
    }
    return L ;
}
//permet d’écrire une liste dans un fichier
void ltof(List* L, char* path){
    if(L==NULL ||path==NULL){
        printf("fnc ltof : problème de liste ou de path\n");
        return;
    }
    FILE *f=fopen(path,"w");
    if(f==NULL){
        printf("fnc ltof :Le fichier ne s'est pas ouvert correctement\n");
        return;
    }
    fputs(ltos(L),f);
    fclose(f);
}
//permet de lire une liste enregistrée dans un fichier
List* ftol(char* path){
    if(path==NULL){
        printf("fnc ftol: problème de path\n");
        return NULL;
    }
    char buff [ N * MAX_FILES ];
    FILE *f=fopen(path,"r");
    if(f==NULL){
        printf("fnc ftol :Le fichier ne s'est pas ouvert correctement\n");
        return NULL;
    }
    fgets ( buff , N * MAX_FILES , f ) ;
    return stol ( buff ) ;
}