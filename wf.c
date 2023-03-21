#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wf.h"
#include <sys/stat.h>
#include "hash.h"
#include "unistd.h"
#include "lcc.h"
#include "git.h"
#include "errno.h"


//Permet de créer un WorkFile
WorkFile* createWorkFile(char* name){
    WorkFile* wf=(WorkFile*)malloc(sizeof(WorkFile));
    wf->name=strdup(name);
    wf->hash=NULL;
    wf->mode=0;
    return wf;
}
//Permet de convertir un WorkFile en chaine de caractère de la forme nom\thash\t\mode avec \t une tabulation
char* wfts(WorkFile* wf){
    //re : valeur de retour
    char * re=(char*)malloc(1024*sizeof(char));
    snprintf(re,1024,"%s\t%s\t%d\t",wf->name,wf->hash==NULL ? "":wf->hash,wf->mode); //On vérifie si le hash n'est pas nul, si c'est le cas on remplace par les guillemets
    return re;
}
//Permet de convertir une chaine de caractère en un WorkFile
WorkFile* stwf(char* ch) {
    //Comme on sait qu'il y a 3 éléments dans un workfile nous allons travailler avec 3 boucles 
    if (ch == NULL) {
        printf("fnc stwf: La chaine de caractère est mal définie\n");
        return NULL;
    }
    WorkFile* wf = (WorkFile*) malloc(sizeof(WorkFile));
    //On initialise les variables que l'on va utiliser
    char nom[256] = "";
    char hash[256] = "";
    char mode[4] = "";

    int i = 0;
    int j = 0;
    //NOM
    while (ch[i] != '\t') {
        nom[j] = ch[i];
        i++;
        j++;
    }
    nom[j] = '\0';
    i++;

    //HASH
    j = 0;
    while (ch[i] != '\t') {
        hash[j] = ch[i];
        i++;
        j++;
    }
    hash[j] = '\0';
    i++;

    //MODE
    j = 0;
    while (ch[i] != '\t' && ch[i] != '\0') {
        mode[j] = ch[i];
        i++;
        j++;
    }
    mode[j] = '\0';
    //On copie nos données dans la structure de données
    wf->name = strdup(nom);
    wf->hash = strdup(hash);
    wf->mode = atoi(mode);
    return wf;
}
//Initialise un Worktree avec pour taille max d'élémént donc son tab SIZE_MAX
WorkTree* initWorkTree(){
    WorkTree* wt=(WorkTree*)malloc(sizeof(WorkTree));
    if(wt==NULL){
        printf("fnc initWorkTree: erreur de l'allocation mémoire");
        return NULL; 
    }
    wt->tab=(WorkFile*)malloc(SIZE_MAX*sizeof(WorkFile));
    wt->size=SIZE_MAX;
    wt->n=0;
    return wt;
}
//vérifie la présence d’un fichier ou répertoire dans un WorkTree. Cette fonction doit retourner la position du fichier dans le tableau s’il est présent, et -1 sinon.
int inWorkTree(WorkTree* wt, char* name){
    if(wt==NULL){
        printf("fnc inWorkTree: Le WorkTree est mal défini");
        return -1;
    }
    for(int i=0;i<wt->n;i++){
        if(strcmp(name,wt->tab[i].name)==0){
            return i;
        }
    }
    return -1;
}
//ajoute un fichier ou répertoire au WorkTree (s’il n’existe pas déjà)
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
    if(wt==NULL){
        printf("fnc appendWorkTree: Le WorkTree est mal défini");
        return -1;
    }
    if(inWorkTree(wt,name)!=-1){
        //printf("fnc appendWorkTree: Le fichier est déjà présent dans le WT");
        return 0;
    }
    if(wt->n>=wt->size){
        printf("fnc appendWorkTree: le worktree est rempli");
        return -1;
    }
    WorkFile* wf=createWorkFile(name);
    wf->hash=strdup(hash);
    wf->mode=mode;
    wt->tab[wt->n++]=*wf;
    return 1;
}
//convertit un WorkTree en une chaı̂ne de caractères composée des représentations des WorkFile séparées par un saut de ligne (caractère ’\n’).
char* wtts(WorkTree* wt){
    if(wt==NULL){
        printf("fnc wtts: Le WorkTree est mal défini");
        return NULL;
    }
    char* rendu=(char*)malloc(SIZE_MAX*256*sizeof(char));
    rendu[0]='\0';
    for(int i=0;i<wt->n;i++){
        strcat(rendu,wfts(&wt->tab[i]));
        strcat(rendu,"\n");
    }
    return rendu;
}
//convertit une chaı̂ne de caractères représentant un WorkTree en un WorkTree
WorkTree* str_to_wt(char* ch){
    if(ch == NULL){
        printf("fnc str_to_wt: La chaîne de caractères est mal définie");
        return NULL;
    }
    WorkTree* wt = initWorkTree();
    int i = 0;
    while(ch[i] != '\0'){
        char* ligne = strtok(&ch[i], "\n");
        if(ligne != NULL){
            WorkFile* wf = stwf(ligne);
            appendWorkTree(wt, wf->name, wf->hash, wf->mode);
            free(wf);
            i += strlen(ligne) + 1;
        }
    }
    return wt;
}
//écrit dans le fichier file la chaı̂ne de caractères représentant un WorkTree
int wttf(WorkTree* wt, char* file){
    FILE* f=fopen(file,"w");
    if(f==NULL){
        printf("fnc wttf: erreur d'ouverture du fichier");
        return -1;
    }
    char *r=wtts(wt);
    fputs(r,f);
    fclose(f);
    return 0;
}
//construit un WorkTree à partir d’un fichier qui contient sa représentation en chaı̂ne de caractères.
WorkTree* ftwt(char* file){
    FILE *f=fopen(file,"r");
    if(f==NULL){
        printf("fnc ftwt: erreur 'ouverture du fichier");
        return NULL;
    }
    WorkTree* wt=initWorkTree();
    char nom[256];
    char hash[256];
    int mode;
    char ligne[256];
    while(fgets(ligne,256,f)!=NULL){
        sscanf(ligne,"%s\t%s\t%d\t",nom,hash,&mode);
        appendWorkTree(wt,nom,hash,mode);
    }
    return wt;
}
//Permet de recuperer l'entier des permissions pour un fichier
int getChmod(const char *path){
    struct stat ret;
    if ( stat ( path , &ret ) == -1) {
        return -1;
    }
    return(ret.st_mode & S_IRUSR ) |( ret.st_mode & S_IWUSR ) |( ret.st_mode & S_IXUSR ) | /*owner*/
    (ret.st_mode & S_IRGRP ) |( ret.st_mode & S_IWGRP ) |( ret.st_mode & S_IXGRP ) | /*group*/
    (ret.st_mode & S_IROTH ) |( ret.st_mode & S_IWOTH ) |( ret.st_mode & S_IXOTH ) ; /*other*/
}

//Permet de modifier la valeur d'un mode d'un fichier
void setMode ( int mode , char * path ) {
    char buff [100];
    sprintf(buff,"chmod %d %s ",mode,path);
    system(buff);
}
//Renvoie 1 si le path correspond à un fichier, 0 sinon
int isFile(const char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
//Renvoie 1 si le path correspond à un repertoire, 0 sinon
int isDir(const char* path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

//crée un fichier temporaire représentant le WorkTree pour pouvoir ensuite créer l’enregistrement instantané du WorkTree (+ l’extension ”.t”) retourne le hash du fichier temporaire
char* blobWorkTree(WorkTree* wt){
    //Analyse du WT
    if(wt==NULL){
        printf("fnc blobWorkTree: le WorkTree n'est pas bien définie");
        return NULL;
    }
    //Création du fichier temporaire et gestion des erreurs
    char template[]="/tmp/worktreeXXXXXX";
    char fname[1000];
    strcpy(fname,template);
    int fd=mkstemp(fname);
    if(fd<0){
        printf("fnc blobWorkTree: problème de création du fichier temporaire %s : %s\n", fname, strerror(errno));
        return NULL;
    }
    //Ouverture du fichier
    FILE* f=fopen(fname,"w");
    if(f==NULL){
        printf("fnc blobWorkTree: problème d'ouverture du ficher temporaire");
        return NULL;
    }

    //Ecriture du WT dans le fichier temp
    char* str=wtts(wt);
    fwrite(str, strlen(str), 1, f);
    free(str);
    fclose(f);

    //Calcul du hash du fichier temporaire
    char *hash=sha256file(fname);
    //Creation du chemin
    char chemin[1024];
    chemin[0]='\0';
    strcat(chemin, hashToPath(hash));
    char commande[1024];
    snprintf(commande, 1024 + 10, "mkdir -p %s", chemin);
    system(commande);
    
    //Copie du fichier temporaire dans le répertoire de hachage
    char dest[1024];
    dest[0] = '\0';
    strcat(dest, chemin);
    strcat(dest, "/");
    char* fname_only = strrchr(fname, '/');
    strcat(dest, fname_only + 1);
    strcat(dest,".t");
    rename(fname, dest);

    return hash;
}



//WorkFile wt dont le chemin est donné en paramètre, crée un enregistrement instantané de tout son contenu (de manière récursive), puis de lui même.
char* saveWorkTree(WorkTree* wt, char* path){
    if(wt==NULL){
        printf("fnc saveWorkTree: le WorkTree n'est pas bien définie");
        return NULL;
    }
    if(path==NULL){
        printf("fnc saveWorkTree: le path n'est pas bien définie");
        return NULL;
    }
    //On crée un nouveau wt pour le répertoire courant 
    WorkTree* newWT=initWorkTree();
    for(int i=0;i<wt->n;i++){
        WorkFile* wf=&wt->tab[i];
        if(S_ISREG(wf->mode)){
            //Si wf est un fichier 
            //blobFile();
        }
        //else if()
    
    return "a";
}
}