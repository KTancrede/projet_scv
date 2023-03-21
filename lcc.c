#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lcc.h"
//Initialise la liste
List* initList(){
    List* liste=(List*)malloc(sizeof(List));
    if(liste!=NULL){
        *liste=(Cell*)malloc(sizeof(Cell));
        if(*liste!=NULL){
            (*liste)->data=NULL;
            (*liste)->next=NULL;
        }
    }
    return liste;
}

//Créer une cellule
Cell* buildCell(char* ch){
    Cell* c=(Cell*)malloc(sizeof(Cell));
    if(c!=NULL){
        c->data=strdup(ch);
        c->next=NULL;
    }
    return c;
}

//Insere une cellule à la tête d'une liste
void insertFirst(List *L, Cell* C){
    if(L==NULL){
        printf("fnc insertFirst: La liste donnée en paramètre n'est pas définie\n");
        return;
    }
    else if(C==NULL){
        printf("fnc insertFirst: La cellule a insérer n'est pas définie correctement\n");
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
char* ltos(List* L){
    if(L==NULL){
        printf("fnc ltos: La liste n'est pas définie\n");
        return NULL;
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
    if(L==NULL){
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
    if (L == NULL) {
        printf("fnc searchList: La liste n'est pas définie\n");
        return NULL;
    }
    Cell* c = *L;
    while (c->data != NULL) {
        if (strcmp(str,c->data) == 0) {
            return c;
        }
        c = c->next;
    }
    return NULL; // retourne NULL si l'élément n'est pas trouvé
}

//permet de transformer une chaı̂ne de caractères représentant une liste en une liste chaı̂née
List* stol(char* s){
    if(s==NULL){
        printf("fnc stol: la chaine s n'est pas bien définie\n");
        return NULL;
    }
    List* l=initList();
    int i=0;
    while(s[i]!='\0'){
        i++;
        Cell* c=buildCell(&s[i]);
        insertFirst(l,c);
    }
    return l;
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
    Cell* c = *L;
    while(c!=NULL){
    fprintf(f, "%s\n", c->data);
        c=c->next;
    }
    fclose(f);
}
//permet de lire une liste enregistrée dans un fichier
List* ftol(char* path){
    if(path==NULL){
        printf("fnc ftol: problème de path\n");
        return NULL;
    }

    FILE *f=fopen(path,"r");
    if(f==NULL){
        printf("fnc ftol :Le fichier ne s'est pas ouvert correctement\n");  
        return NULL;
    }
    List *L=initList();

    char c=fgetc(f);
    while(c!=EOF){
        insertFirst(L,buildCell(&c));
    }
    fclose(f);
    return L;
}