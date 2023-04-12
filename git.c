#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "hash.h"
#include "lcc.h"
#include <sys/stat.h>

//prend en paramètre une adresse et renvoie une liste contenant le noms des fichiers et répertoires qui s’y trouvent
List * listdir ( char * root_dir ) {
    DIR * dp ;
    struct dirent * ep ;
    List * L = initList () ;
    * L = NULL ;
    Cell * temp_cell ;
    dp = opendir ( root_dir ) ;
    if ( dp != NULL ){
        while (( ep = readdir ( dp ) ) != NULL ){
            temp_cell = buildCell ( ep -> d_name ) ;
            insertFirst (L , temp_cell ) ;
            List ptr = * L ;
                while ( ptr != NULL ) {
                    ptr = ptr -> next ;
                }
        }
        (void)closedir ( dp ) ;
    }
    else
    {
        perror( "fnc listdir: Couldn ’ t open the directory" ) ;
        return NULL ;
    }
    return L;
}

//retourne 1 si le fichier existe dans le répertoire courant et 0 sinon
int file_exists ( char * file ){
    struct stat buffer;
    return (int)( stat ( file , &buffer ) == 0) ;
}


//copie le contenu d’un fichier vers un autre, en faisant une lecture ligne par ligne du fichier source
void cp(char *to, char *from){
    if(file_exists(from)==1){
        printf("Le fichier existe et est copié\n");
        FILE *f_from=fopen(from,"r");
        if(f_from==NULL){
            printf("fnc cp: Le fichier source ne s'est pas ouvert correctement\n");
        }
        FILE *f_to=fopen(to,"w");
        if(f_to==NULL){
            printf("fnc cp: Le fichier destination ne s'est pas ouvert correctement\n");
        }
        char c;
        c=fgetc(f_from);
        while(c!=EOF){
            fputc(c,f_to);
            c=fgetc(f_from);
        }
        fclose(f_to);
        fclose(f_from);
    }else{
        printf("fnc cp: Le fichier n'exite pas\n");
        return;
    }
}

//retourne le chemin d’un fichier à partir de son hash
char * hashToPath ( char * hash ) {
    char * dir = malloc (( strlen ( hash ) +1) * sizeof ( char ) ) ;
    dir [0] = hash [0];
    dir [1] = hash [1];
    dir [2] = '/' ;
    int i;
    for ( i = 3; i <= strlen ( hash ) ; i ++) {
        dir [i] = hash [i -1];
    }
    dir[i]= '\0' ;
    return dir ;
}





//enregistre un instantané du fichier donné en entrée
void blobFile(char* file) {
    // Calcule le hash du fichier donné en entrée
    char* hash=sha256file(file);
    char * ch2 = strdup ( hash ) ;
    ch2 [2] = '\0' ;
    if (! file_exists ( ch2 ) ) {
    char buff [100];
    sprintf ( buff , " mkdir %s " , ch2 ) ;
    system ( buff ) ;
    }
    char * ch = hashToPath ( hash ) ;
    cp ( ch , file ) ;
}