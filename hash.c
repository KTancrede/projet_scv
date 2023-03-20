#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
//calcule le hash du contenu du premier fichier et l’écrit dans le deuxième fichier
int hashFile(char* source, char* dest){
    
    static char template[1024];
    snprintf(template,1024,"/tmp/%sXXXXXX",dest);

    //On recupere le nom de fichier
    char* fname=((char*)malloc(1024*sizeof(char)));
    fname[0] = '\0';
    strcpy(fname,template);

    //On cree le fichier et on gére les erreurs
    int fd =mkstemp(fname);
    if(fd<0){
        printf("Erreur de création du fichier dans tmp\n");
        return -1;
    }
    //printf("fname hashfile: %s\n",fname);


    char* s=((char*)malloc(1024*sizeof(char)));
    s[0]='\0';
    snprintf(s,1024,"cat %s| sha256sum  > %s", source, fname);
    system(s);
    printf("%s %s\n",dest,fname);

    dest = (char*) malloc(strlen(fname) + 1);
    strcpy(dest,fname);

    free(fname);
    free(s);
    return 0;
}
//renvoie une chaı̂ne de caractères contenant le hash du fichier donné en paramètre.
char * sha256file(char* file){
    //On crée le fichier temporaire et on l'initialise avec les variables le pattern XXXXXX qu'on a choisit
    char * temp_file=((char*)malloc(1024*sizeof(char)));
    temp_file[0]='\0';
    strcat(temp_file,"/tmp/XXXXXX");
    //On utilise mkstemp et on gére les erreurs 
    int fd =mkstemp(temp_file);
    if(fd<0){
        printf("Erreur de création du fichier dans tmp\n");
        return NULL;
    }

    //On crée après la commande
    char commande[1024];
    snprintf(commande, sizeof(commande), "sha256sum %s > %s", file, temp_file);
    system(commande);
    //free(commande);

    
    //Maintenant on essaye de récuperer les informations contenues sur le fichier que l'on vient de créer
    char* hash=((char*)malloc(1024*sizeof(char)));
    FILE *f=fopen(temp_file,"r");

    if(f==NULL){
        printf("Le fichier hash ne s'est pas correctement ouvert\n");
        return NULL;
    }
    
    fread(hash,sizeof(char),1024,f);
    
    fclose(f);

    char* hashH=((char*)malloc(1024*sizeof(char)));
    int i=0;
    while(hash[i]!=' '){
        hashH[i]=hash[i];
        i++;
    }
    hashH[i]='\0';
    free(temp_file);
    return hashH;
}
