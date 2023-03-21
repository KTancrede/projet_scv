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
//permet de convertir un élément kvp en une chaı̂ne de caractères de la forme ”clé :valeur”
char* kvts(kvp* k){
    if(k==NULL){
        printf("fnc kvts: le kvp est mal défini");
        return NULL;
    }
    int longueur=strlen(k->key)+strlen(k->value)+3;
    char *r=malloc(longueur*sizeof(char));
    r[0]='\0';
    snprintf(r,longueur,"%s :%s",k->key,k->value);
    r[longueur]='\0';
    return r;
}
//Créer un kvp depuis une string de la forme ”clé :valeur”
kvp* stkv(char* str){
    char* k = (char*)malloc(strlen(str)*sizeof(char)); 
    k[0] = '\0'; 
    char* v = (char*)malloc(strlen(str)*sizeof(char)); 
    v[0] = '\0'; 
    
    return createKeyVal(k, v); // Appel à la fonction createKeyVal avec les chaînes k et v en paramètres
}

//Initialise un commint
Commit* initCommit(){
    Commit* c=(Commit *)malloc(sizeof(Commit));
    c->n=0;
    c->size=SIZE_MAX;
    c->T=malloc(c->size*sizeof(kvp*));
    for(int i=0;i<c->size;i++){
        c->T[i]=NULL;
    }
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
void commitSet(Commit* c,char* key, char* value){ // A RETRAVAILLER
    if(c==NULL){
        printf("fnc commitSet: le commit est mal défini\n");
        return;
    }
    unsigned char *ukey=(unsigned char*)key;
    unsigned long hash_valeur=hash(ukey)%SIZE_MAX;
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
//cherche dans la table s’il existe un élément dont la clé est key retourne la valeur de l’élément s’il existe, et NULL sinon
char* commitGet(Commit* c, char* key){
    if(c==NULL){
        printf("fnc commitGet: le commit est mal défini\n");
        return NULL;
    }
    unsigned char *ukey=(unsigned char*)key;
    unsigned long hash_valeur=hash(ukey)%SIZE_MAX;
    int i=0;
    while(c->T[hash_valeur]!=NULL && i<c->size){
        if(strcmp(c->T[hash_valeur]->key,key)==0){
            return c->T[hash_valeur]->value; // on a trouvé
        }
        i++;
        hash_valeur=(hash_valeur+i)%SIZE_MAX;
    }
    return NULL;
}
//convertit un commit en une chaı̂ne de caractères
char* cts(Commit* c){
    if(c==NULL){
        printf("fnc cts: le commit est mal défini\n");
        return NULL;
    }
    int longeur=0;
    for(int i=0;i<c->size;i++){
        if(c->T[i]!=NULL){
            longeur+=strlen(kvts(c->T[i]))+1;
        }
    }

    char*r=malloc(longeur+1*sizeof(char));
    r[0]='\0';
    for(int i=0;i<c->size;i++){
        if(c->T[i]!=NULL){
            printf("%s\n",kvts(c->T[i]));
            snprintf(r,strlen(kvts(c->T[i]))+2,"%s\n",kvts(c->T[i]));
        }
    }
    return r;
}
//Retourne un commit depuis une string
Commit* stc(char* ch){
    if(ch==NULL){
        printf("fnc stc: erreur de d'argument de string\n");
    }
    Commit* c=initCommit();

    char* tok=strtok(ch,"\n");
    while(tok!=NULL){
        kvp *k=stkv(tok);
        if(k!=NULL){
            commitSet(c,k->key,k->value);
            freeKeyVal(k);
        }
        tok=strtok(NULL,"\n");
    }
    return c;
}

//Ecrit dans un fichier file la chaine représentant le commit c
void ctf(Commit* c, char* file){
    FILE* f=fopen(file,"w");
    if(f==NULL){
        printf("fnc ctf: erreur d'ouverture du fichier");
        return;
    }
    fputs(cts(c),f);
    fclose(f);
}

//charger un Commit depuis un fichier le représentant
Commit* ftc(char* file){
    FILE* f=fopen(file,"r");
    if(f==NULL){
        printf("fnc ftc: erreur de lecture du fichier");
    }
    Commit *c=initCommit();
    char ligne[255];
    while(fgets(ligne,255,f)){
        if(strlen(ligne)>1){//Si la ligne n'est pas vide
            kvp*k=stkv(ligne);
            commitSet(c,k->key,k->value);
            freeKeyVal(k);
        }
    }
    return c;
}