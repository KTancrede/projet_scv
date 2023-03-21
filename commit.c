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
//Initialise un commint
Commit* initCommit(){
    Commit* c=(Commit *)malloc(sizeof(Commit));
    c->T=malloc(c->size*sizeof(kvp*));
    for(int i=0;i<c->size;i++){
        c->T[i]=NULL;
    }
    c->n=0;
    c->size=SIZE_MAX;
    return c;
}

//Fonction de hash
unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
//insère la paire (key, value) dans la table, en gérant les collisions par adressage ouvert et probing linéaire.
void commitSet(Commit* c, char* key, char* value){ // A RETRAVAILLER
    unsigned long hash_valeur=hash(key)%SIZE_MAX;
    //Si la case du commit est vide
    if(c->T[hash_valeur]==NULL){
        kvp* k=createKeyVal(key,value);
        c->T[hash_valeur]=k;
        c->n++;
    }
    //SI la case du commit est déjà allouée
    int i=0;
    while(c->T[hash_valeur]!=NULL && i<c->size){ 
        if(strcmp(c->T[hash_valeur]->key,key)==0){ //Si la clé existe déjà on la met à jour avec +i
            c->T[hash_valeur]->value=value;
            return; //On a finit
        }
        i++;
        hash_valeur=(hash_valeur+i)%SIZE_MAX;
    }    
}

//alloue et initialise un Commit, puis ajoute l’élément obligatoire correspondant à la clé "tree"
Commit* createCommit(char* hash){
    Commit *c=initCommit();
    commitSet(c,"tree",hash);
    return c;
}