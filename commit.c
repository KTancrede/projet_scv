#include "commit.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wf.h"
#include "hash.h"
#include "lcc.h"
#include "git.h"

//allouer et d’initialiser un élément
kvp* createKeyVal(char* key, char* val){
    kvp * k=(kvp*)malloc(sizeof(kvp));
    k->key=strdup(key);
    k->value=strdup(val);
    return k;
}
//libérer la mémoire associé à un élément.
void freeKeyVal(kvp* kv){
    free(kv->key);
    free(kv->value);
    free(kv);
}
//permet de convertir un élément en une chaı̂ne de caractères de la forme ”clé :valeur”
char* kvts(kvp* k){
    char *r=(char*)malloc(1024*sizeof(char));
    r[0]='\0';
    snprintf(r,1024,"%s :%s",k->key,k->value);
    return r;
}
//Créer un kvp depuis une string de la forme ”clé :valeur”
kvp* stkv(char* str){
    char* k = (char*)malloc(1024/2*sizeof(char)); 
    k[0] = '\0'; 
    char* v = (char*)malloc(1024/2*sizeof(char)); 
    v[0] = '\0'; 
    int i = 0;
    // Extraction de la première partie de la chaîne avant ":"
    while(str[i] != ':' && str[i] != '\0'){
        if(str[i]==' '){ //gestion des espaces
            i++;
        }
        k[i] = str[i]; 
        //printf("%s\n",&k[i]);
        i++;
    }
    k[i] = '\0'; 
    // Extraction de la deuxième partie de la chaîne après ":"
    if(str[i] == ':'){
        i++;
        int j = 0;
        while(str[i] != '\0'){
            if(str[i]==' '){ // gestion des espaces
                i++;
            }
            v[j] = str[i]; 
            i++; 
            j++;
        }
        v[j] = '\0'; 
    }
    return createKeyVal(k, v); // Appel à la fonction createKeyVal avec les chaînes k et v en paramètres
}

Commit* initCommit(){
    Commit* c=(Commit *)malloc(sizeof(Commit));
    c->T=NULL;
    c->n=0;
    c->size=SIZE_MAX;
    return c;
}

unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}