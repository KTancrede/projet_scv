#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "dirent.h"
#include "hash.h"
#include "lcc.h"

//prend en paramètre une adresse et renvoie une liste contenant le noms des fichiers et répertoires qui s’y trouvent
List* listdir(char* root_dir){
    List* l=initList();
    DIR* dp= opendir(root_dir);
    struct dirent * ep ;
    if(dp!=NULL){
        while ((ep=readdir(dp))!=NULL){
            //buildCell(ep->d_name);
            insertFirst(l,buildCell(ep->d_name));
        }
    }
    return l;
}

//retourne 1 si le fichier existe dans le répertoire courant et 0 sinon 
int file_exists(char *file){
    List* dir_list = listdir(".");
    if (dir_list == NULL) {
        printf("Erreur lors de la récupération de la liste de fichiers\n");
        return 0;
    }
    if (searchList(dir_list, file) == NULL) {
        return 0;
    }
    return 1;
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
char* hashToPath(char* hash){
    char* path = (char*)malloc((strlen(hash)+4)*sizeof(char));
    if(path==NULL){
        printf("fnc  hashToPath: erreur de l'alocaiton memoire");
        return NULL; 
    }
    strncpy(path, hash, 2);
    path[2] = '\0'; 
    strcat(path, "/");
    strcat(path, hash + 2);
    path[strlen(hash)+1]='\0';
    return path;
}





//enregistre un instantané du fichier donné en entrée
void blobFile(char* file) {
    // Calcule le hash du fichier donné en entrée
    char hash[1024];
    hash[0]='\0';
    strcat(hash, sha256file(file));
    
    // Construit le chemin du répertoire où stocker le fichier
    char chemin[1024];
    chemin[0]='\0';
    strcat(chemin, hashToPath(hash));
    
    // Crée le répertoire s'il n'existe pas déjà
    char commande[1024];
    snprintf(commande, 1024 + 10, "mkdir -p %s", chemin);
    system(commande);
    
    // Copie le fichier dans le répertoire
    char dest[1024];
    dest[0] = '\0';
    strcat(dest, chemin);
    strcat(dest, "/");
    strcat(dest, file);
    cp(dest, file);
}