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
#include "merge.h"

int main(int argc, char * argv[]) {

    if (argc < 2) {
        printf("Erreur : pas assez d'argument\n");
        printf("Les commandes sont les suivantes:\n");
        printf("./myGit init : initialise le répertoire de références et de la branche courrante\n");
        printf("./myGit list-refs : affiche toutes les références existantes\n");
        printf("./myGit create-ref <name> <hash> : crée la référence <name> qui pointe vers le commit correspondant au hash donné\n");
        printf("./myGit delete-ref <name> : supprime la référence name\n");
        printf("./myGit add <elem> [<elem2> ...] : ajoute un ou plusieurs fichiers/répertoires à la zone de préparation (prochain commit)\n");
        printf("./myGit clear-add : vide la zone de préparation\n");
        printf("./myGit list-add : affiche le contenu de la zone de préparation\n");
        printf("./myGit commit <branch name> [-m <message>] : effectue un commit sur une branche, avec ou sans message descriptif\n");
        printf("./myGit get-current-branch : affiche le nom de la branche courante\n");
        printf("./myGit branch <branch-name> : crée une branche qui s’appelle <branch-name> si elle n’existe pas déjà\n");
        printf("./myGit branch-print <branch-name> : affiche le hash de tous les commits de la branche, accompagné de leur message descriptif éventuel\n");
        printf("./myGit checkout-branch <branch-name> : réalise un déplacement sur la branche <branch-name>\n");
        printf("./myGit checkout-commit <pattern> : réalise un déplacement sur le commit qui commence par <pattern>\n");
        printf("./myGit merge <branch> <message>: \n");
        return 0;
    }
    //printf("%s",argv[0]);
    if (strcmp(argv[1], "init") == 0) {
        initRefs();
        initBranch();
        return 1;
    }

    if (strcmp(argv[1], "list-refs") == 0) {
        printf("REFS:\n");
        if (file_exists(".refs")) {
            //printf("Le fichier existe\n");
            List * L = listdir(".refs");
            //printf("list L: %s\n",ltos(L));
            for (Cell * ptr = * L; ptr != NULL; ptr = ptr -> next) {
                if (ptr -> data[0] == '.')
                    continue;
                char * content = getRef(ptr -> data);
                printf("- %s \t %s \n", ptr -> data, content);
            }
        }
        return 2;
    }

    if (strcmp(argv[1], "create-ref") == 0) {
        createUpdateRef(argv[2], argv[3]);
        printf("La ref '%s' a bien été crée\n",argv[2]);
        return 3;
    }
    if (strcmp(argv[1], "delete-ref") == 0) {
        deleteRef(argv[2]);
        printf("La ref '%s' a bien été supprimé\n",argv[2]);
        return 4;
    }
    if (strcmp(argv[1], "add") == 0) {
        for (int i = 2; i < argc; i++) {
            myGitAdd(argv[i]);
        }
        return 5;
    }
    if (strcmp(argv[1], "clear-add") == 0) {
        system("rm .add");
        return 6;
    }
    if (strcmp(argv[1], "list-add") == 0) {
        printf("Zone de préparation :\n");
        if (file_exists(".add")) {
            WorkTree * wt = ftwt(".add");
            printf("%s\n", wtts(wt));
        }
        else
            printf("Il n'y a aucun fichier dans la zone de préparation\n");
        return 7;
    }
    if (strcmp(argv[1], "commit") == 0) {
        if (argc >3) {
            if (strcmp(argv[3], "-m") == 0) {
                myGitCommit(argv[2], argv[4]);
            }
        }else {
            myGitCommit(argv[2], NULL);
        }
        return 8;
    }
    if (strcmp(argv[1], "get-current-branch") == 0) {
        printf("%s\n", getCurrentBranch());
        return 9;
    }
    if (strcmp(argv[1], "branch") == 0) {
        if (!branchExists(argv[2])){
            createBranch(argv[2]);
        }else
            printf("The branch already exist");
        return 10;
    }
    if (strcmp(argv[1], "branch-print") == 0) {
        if (!branchExists(argv[2]))
            printf("The branch does not exist");
        else
            printBranch(argv[2]);
        return 11;
    }
    if (strcmp(argv[1], "checkout-branch") == 0) {
        if (!branchExists(argv[2]))
            printf("The branch does not exist");
        else
            myGitCheckoutBranch(argv[2]);
        return 12;
    }
    if (strcmp(argv[1], "checkout-commit") == 0) {
        myGitCheckoutCommit(argv[2]);
        return 13;
    }

    if (strcmp(argv[1], "merge") == 0) {
        if (argc != 4) {
            printf("Usage: ./myGit merge <branch> <message>\n");
            return 0;
        }


        char* targetBranch = argv[2];
        char* message = argv[3];
        char* currentBranch = getCurrentBranch();

        List* conflicts = merge(targetBranch, message);

        if (conflicts == NULL) {
            printf("La fusion s'est bien passee.\n");
        } else {
            printf("Il y a des collisions entre les branches. Choisissez l'une des options suivantes:\n");
            printf("1. Garder les fichiers de la branche courante et creer un commit de suppression pour la branche <branch>, avant de faire appel a la fonction merge.\n");
            printf("2. Garder les fichiers de la branche <branch> et creer un commit de suppression pour la branche courante, avant de faire appel a la fonction merge.\n");
            printf("3. Choisir manuellement, conflit par conflit, la branche sur laquelle vous souhaitez garder le fichier/repertoire qui est en conflit, avant de faire appel a la fonction merge.\n");

            int choix;
            scanf("%d", &choix);

            if (choix == 1) {
                createDeletionCommit(targetBranch, conflicts, message);
                merge(targetBranch, message);
            } else if (choix == 2) {
                createDeletionCommit(currentBranch, conflicts, message);
                merge(targetBranch, message);
            } else if (choix == 3) {
                List* conflictsCurrent = initList();
                List* conflictsTarget = initList();

                Cell* currentCell = (*conflicts)->next;
                while (currentCell != NULL) {
                    printf("Pour le conflit %s, entrez 1 pour garder la branche courante ou 2 pour garder la branche <branch>: ", currentCell->data);
                    int conflictChoice;
                    scanf("%d", &conflictChoice);

                    if (conflictChoice == 1) {
                        insertFirst(conflictsCurrent, buildCell(currentCell->data));
                    } else if (conflictChoice == 2) {
                        insertFirst(conflictsTarget, buildCell(currentCell->data));
                    }

                    currentCell = currentCell->next;
                }

                createDeletionCommit(currentBranch, conflictsCurrent, message);
                createDeletionCommit(targetBranch, conflictsTarget, message);
                merge(targetBranch, message);

                freeList(conflictsCurrent);
                freeList(conflictsTarget);
            } else {
                printf("Choix invalide. Fusion interrompue.\n");
            }

            freeList(conflicts);
        }

        free(currentBranch);

        return 14;
    }

    return -1;
}
