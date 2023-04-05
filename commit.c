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
    char * buff = malloc ( sizeof ( char ) *100) ;
    sprintf ( buff , "%s : %s" , k -> key , k -> value ) ;
    return buff ;
}
//Créer un kvp depuis une string de la forme ”clé :valeur”
kvp* stkv(char* str){
    char k [100] , v [100];
    sscanf ( str , "%s : %s",k,v);
    return createKeyVal(k, v); // Appel à la fonction createKeyVal avec les chaînes k et v en paramètres
}

//Initialise un commint
Commit* initCommit(){
    Commit* c=(Commit *)malloc(sizeof(Commit));
    c->n=0;
    c->size=N_COMMIT;
    c->T=malloc(c->size*sizeof(kvp*));
    for(int i=0;i<c->size;i++){
        c->T[i]=NULL;
    }
    return c;
}

//Fonction de hash
unsigned long hash(char *str){
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
//insère la paire (key, value) dans la table, en gérant les collisions par adressage ouvert et probing linéaire.
void commitSet(Commit* c,char* key, char* value){
    int p = hash(key)%c-> size ;  //JE COMPRENDS PAS CA VIENT DU PDF
    while (c->T[p]!= NULL ) {
        p = (p+1)%c-> size ; //probing lineaire
    }
    c->T[p] = createKeyVal(key,value);
    c->n ++;   
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
    int p = hash( key )%c-> size ;
    int i=0;
    while(c->T[p]!=NULL && i<c->size){
        if(strcmp(c->T[p]->key,key)==0){
            return c->T[p]->value; // on a trouvé
        }
        p=(p+i)%SIZE_MAX;
        i++;
        }
    return NULL;
}
//convertit un commit en une chaı̂ne de caractères
char* cts(Commit* c){
    if(c==NULL){
        printf("fnc cts: le commit est mal défini\n");
        return NULL;
    }
    char*r=malloc(c->n*sizeof(char)*100);
    r[0]='\0';

    for(int i=0;i<c->size;i++){
        if(c->T[i]!=NULL){
            strcat (r, kvts(c->T[i]));
            strcat (r,"\n") ;
        }
    }
    return r;
}
//Retourne un commit depuis une string
Commit* stc(char* ch){ //Different du pdf mais semble bien fonctionner
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

//charge un Commit depuis un fichier le représentant
Commit * ftc ( char * file ) {
    char buff [ MESSAGE_SIZE ];
    char * all = malloc ( sizeof ( char ) * MESSAGE_SIZE ) ;
    FILE * f = fopen ( file ,"r");
    if (f==NULL){
        printf("ERROR: f i l e d o e s n o t e x i s t \n" ) ;
        return NULL ;
    }
    while(fgets(buff,N,f)!= NULL){
        strcat ( all , buff ) ;
    }
    Commit * c = stc (all) ;
    return c ;
}

char * blobCommit ( Commit * c ) {
    char fname [100] = "/tmp/myfileXXXXXX" ;
    int fd = mkstemp(fname);
    if (fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }
    ctf (c , fname ) ;
    char * hash = sha256file ( fname ) ;
    char * ch =hashToFile(hash);
    strcat ( ch , ".c" ) ;
    cp(ch,fname);
    return hash;
}