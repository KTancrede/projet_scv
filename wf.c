#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "wf.h"
#include "hash.h"
#include "unistd.h"
#include "lcc.h"
#include "git.h"
#include "errno.h"
#include <sys/stat.h>


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
    char * re=malloc(1024*sizeof(char));
    sprintf ( re,"%s\t%s\t%d",wf->name , wf->hash ,wf->mode ) ; //On vérifie si le hash n'est pas nul, si c'est le cas on remplace par les guillemets
    return re;
}
//Permet de convertir une chaine de caractère en un WorkFile
WorkFile* stwf(char* ch) {
    //Comme on sait qu'il y a 3 éléments dans un workfile nous allons travailler avec 3 boucles
    if (ch == NULL) {
        printf("fnc stwf: La chaine de caractère est mal définie\n");
        return NULL;
    }
    char* name = malloc(1024*sizeof(char));
    char* hash= malloc(1024*sizeof(char));
    int mode;
    sscanf ( ch,"%s\t%s\t%d",name,hash,&mode) ;
    WorkFile* wf =createWorkFile(name);
    //On copie nos données dans la structure de données
    wf->hash = hash;
    wf->mode = mode;
    return wf;
}
//Initialise un Worktree avec pour taille max d'élémént donc son tab SIZE_MAX
WorkTree* initWorkTree(){
    WorkTree* wt=(WorkTree*)malloc(sizeof(WorkTree));
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
    if(inWorkTree(wt,name)!=-1){
        printf("fnc appendWorkTree: Le fichier  %s est déjà présent dans le WT\n",name);
        return 0;
    }
    if(wt->n>=wt->size){
        printf("fnc appendWorkTree: le worktree est rempli");
        return -1;
    }
    if (wt->size > wt-> n ) {
        wt->tab[wt->n].mode =mode ;
        wt->tab[wt->n].name =strdup( name ) ;
        if ( hash!= NULL )
            wt->tab[wt->n++].hash = strdup ( hash ) ;
        else
            wt->tab[wt->n++].hash = NULL ;
        return 0;
    }
    return 0;
}
//convertit un WorkTree en une chaı̂ne de caractères composée des représentations des WorkFile séparées par un saut de ligne (caractère ’\n’).
char* wtts(WorkTree* wt){
    if(wt==NULL){
        printf("fnc wtts: Le WorkTree est mal défini");
        return NULL;
    }
    char* rendu=(char*)malloc(wt->size*1000*sizeof(char));
    rendu[0]='\0';
    for(int i=0;i<wt->n;i++){
        strcat(rendu,wfts(wt->tab+i));
        strcat(rendu,"\n");
    }
    return rendu;
}
//convertit une chaı̂ne de caractères représentant un WorkTree en un WorkTree
WorkTree* str_to_wt(char* ch){
    int pos = 0;
    int n_pos = 0;
    int sep = '\n' ;
    char * ptr ;
    char * result = malloc ( sizeof ( char ) *10000) ;
    WorkTree * wt = initWorkTree () ;
    while ( pos < strlen(ch)){
        ptr = strchr ( ch + pos , sep ) ;
        if (ptr== NULL )
            n_pos = strlen ( ch ) +1;
        else {
            n_pos = ptr - ch + 1;
        }
        memcpy ( result , ch + pos , n_pos - pos - 1) ;
        result [ n_pos - pos - 1]= '\0' ;
        pos = n_pos ;
        WorkFile * wf = stwf (result) ;
        appendWorkTree ( wt , wf -> name , wf -> hash , wf -> mode ) ;
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
    fputs(wtts(wt),f);
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
    char * buff = malloc (sizeof( char)*N ) ;
    char * all_wf = malloc ( sizeof ( char ) * N * MAX_FILES ) ;
    while ( fgets ( buff , N , f ) != NULL ){
        strcat ( all_wf , buff ) ;
    }
    return str_to_wt(all_wf);
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

char * hashToFile ( char * hash ){
    char * ch2 = strdup ( hash ) ;
    ch2 [2] = '\0';
    struct stat st;
    if( stat(ch2 ,&st)==-1){
        mkdir ( ch2 , 0700) ;
    }
return hashToPath ( hash );
}

//crée un fichier temporaire représentant le WorkTree pour pouvoir ensuite créer l’enregistrement instantané du WorkTree (+ l’extension ”.t”) retourne le hash du fichier temporaire
char* blobWorkTree( WorkTree * wt ) {
    char fname [100] = "/tmp/myfileXXXXXX" ;
    int fd=mkstemp(fname);
    if (fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }
    wttf ( wt , fname ) ;
    char * hash = sha256file( fname ) ;
    char * ch = hashToFile(hash) ;
    strcat(ch,".t") ;
    cp(ch, fname) ;
    return hash ;
}
//Concatene deux paths
char * concat_paths ( char * path1 , char * path2 ){
    char * result = malloc ( strlen ( path1 ) + strlen ( path2 ) + 1) ;
    if ( result == NULL ) {
    printf ( "E r r o r : u n a b l e t o a l l o c a t e memory\n");
    return NULL ;
    }
    strcpy( result , path1 ) ; // Copy path1 to result
    strcat( result , "/" ) ; // Append a slash to result
    strcat( result , path2 ) ; // Append path2 to result
    return result ;
}

//WorkFile wt dont le chemin est donné en paramètre, crée un enregistrement instantané de tout son contenu (de manière récursive), puis de lui même.
char* saveWorkTree(WorkTree* wt, char* path){
   for ( int i =0; i < wt -> n ; i ++) {
        char * absPath = concat_paths ( path , wt->tab[i].name ) ;
        if ( isFile ( absPath ) == 1) {
            blobFile(absPath);
            wt->tab[i].hash = sha256file( absPath ) ;
            wt->tab[i].mode = getChmod( absPath ) ;
        }
        else{
            WorkTree * wt2 = initWorkTree () ;
            List * L = listdir ( absPath ) ;
            for ( Cell * ptr = * L ; ptr != NULL ; ptr = ptr -> next ) {
                if ( ptr -> data[0]== '.' )
                    continue ;
                appendWorkTree(wt2,ptr->data,NULL,0) ;
                }
                wt -> tab[i].hash = saveWorkTree ( wt2 , absPath ) ;
                wt -> tab[i].mode = getChmod ( absPath ) ;
        }
    }
    return blobWorkTree ( wt ) ;
}

int isWorkTree ( char * hash ) {
    if ( file_exists(strcat(hashToPath( hash ),".t"))) {
        return 1;
    }
    if ( file_exists(hashToPath( hash))){
        return 0;
    }
    return -1;
}

void restoreWorkTree ( WorkTree * wt , char * path ) {
    for ( int i =0; i < wt -> n ; i ++) {
        char * absPath = concat_paths ( path , wt -> tab [ i ]. name ) ;
        char * copyPath = hashToPath ( wt -> tab [ i ]. hash ) ;
        char * hash = wt -> tab [ i ]. hash ;
        if ( isWorkTree ( hash ) == 0) { //si c’est un fichier
            cp ( absPath , copyPath ) ;
            setMode ( getChmod ( copyPath ) , absPath ) ;
        } else {
            if ( isWorkTree ( hash ) == 1) { //si c’est un repertoire
            strcat ( copyPath , ".t" ) ;
            WorkTree * nwt = ftwt ( copyPath ) ;
            restoreWorkTree ( nwt , absPath ) ;
            setMode ( getChmod ( copyPath ) , absPath ) ;
            }
        }
    }
}