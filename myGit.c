#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "hash.h"
#include "lcc.h"
#include "git.h"
#include "wf.h"
#include "commit.h"
#include "gestCommit.h"

int main ( int argc , char * argv []) {

    printf("Bienvenue dans MyGit !\n");
    printf("Que souhaitez-vous faire ?\n");
    printf("1. Initialiser le référentiel\n");
    printf("2. Lister les références\n");
    printf("3. Créer une référence\n");
    printf("4. Supprimer une référence\n");
    printf("5. Ajouter des fichiers à la zone de préparation\n");
    printf("6. Vider la zone de préparation\n");
    printf("7. Afficher la liste des fichiers dans la zone de préparation\n");
    printf("8. Effectuer un commit\n");
    printf("9. Quitter\n");

    int choice;
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            initRefs();
            break;
        case 2:
            printf("REFS:\n");
            if(file_exists(".refs")){
                List* L = listdir(".refs");
                for(Cell* ptr = *L; ptr != NULL; ptr = ptr->next) {
                    if(ptr->data[0] == '.')
                        continue;
                    char* content = getRef(ptr->data);
                    printf("- %s\t%s\n", ptr->data, content);
                }
            }
            break;
        case 3:
            if(argc < 4) {
                printf("Usage : mygit create-ref <nom_ref> <hash>\n");
            } else {
                createUpdateRef(argv[2], argv[3]);
            }
            break;
        case 4:
            if(argc < 3) {
                printf("Usage : mygit delete-ref <nom_ref>\n");
            } else {
                deleteRef(argv[2]);
            }
            break;
        case 5:
            if(argc < 3) {
                printf("Usage : mygit add <fichier>\n");
            } else {
                for(int i = 2; i < argc; i++) {
                    myGitAdd(argv[i]);
                }
            }
            break;
        case 6:
            system("rm .add");
            break;
        case 7:
            printf("Zone de préparation :\n");
            if(file_exists(".add")) {
                WorkTree* wt = ftwt(".add");
                printf("%s\n", wtts(wt));
            }
            break;
        case 8:
            if(argc < 3) {
                printf("Usage : mygit commit <nom> -m <message>\n");
            } else {
                if(strcmp(argv[3], "-m") == 0) {
                    myGitCommit(argv[2], argv[4]);
                } else {
                    myGitCommit(argv[2], NULL);
                }
            }
            break;
        case 9:
            printf("Au revoir !\n");
            break;
        default:
            printf("Choix invalide.\n");
            break;
    }

    return 0;
/*
    if (strcmp(argv[1],"init") == 0) {
        initRefs ();
    }

    if ( strcmp ( argv [1] , "refs-list" ) == 0) {
        printf ( "REFS:\n");
        if(file_exists(".refs")){
            List * L = listdir (".refs");
            for ( Cell * ptr = * L ; ptr != NULL ; ptr = ptr -> next ) {
                if ( ptr ->data[0] == '.' )
                    continue ;
            char * content = getRef (ptr->data) ;
        printf ( "− %s \t %s \n" , ptr ->data , content ) ;
            }
        }
    }

    if (strcmp(argv[1],"create-ref")==0){
        createUpdateRef ( argv [2] , argv [3]) ;
    }
    if (strcmp(argv [1],"delete-ref") == 0) {
        deleteRef(argv[2]);
    }
    if ( strcmp ( argv [1] , "add") == 0) {
        for ( int i = 2; i < argc ; i ++) {
            myGitAdd(argv[i]);
        }
    }
    if (strcmp(argv [1],"clear-add") == 0) {
        system ( "rm .add") ;
    }
    if ( strcmp ( argv [2] , "add-list") ==0) {
        printf ( "Zone de préparation :\n");
        if (file_exists ( ".add" )) {
            WorkTree * wt = ftwt(".add");
            printf ( "%s\n" , wtts(wt));
        }
    }
    if (strcmp(argv[1],"commit") == 0) {
        if (strcmp (argv [3], "-m" ) == 0) {
            myGitCommit (argv [2],argv[4]);
        } else {
            myGitCommit (argv [2],NULL);
        }
    }
    return 0;
*/
}