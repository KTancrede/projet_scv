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

int main(int argc, char * argv[]) {

    if (argc < 2) {
        printf("Erreur : pas assez d'argument\n");
        printf("On rappelle:\n");
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
        return 0;
    }
    //printf("%s",argv[0]);
    if (strcmp(argv[1], "init") == 0) {
        initRefs();
        initBranch();
        return 1;
    }

    if (strcmp(argv[1], "refs-list") == 0) {
        printf("REFS:\n");
        if (file_exists(".refs")) {
            List * L = listdir(".refs");
            for (Cell * ptr = * L; ptr != NULL; ptr = ptr -> next) {
                if (ptr -> data[0] == '.')
                    continue;
                char * content = getRef(ptr -> data);
                printf("− %s \t %s \n", ptr -> data, content);
            }
        }
        return 2;
    }

    if (strcmp(argv[1], "create-ref") == 0) {
        createUpdateRef(argv[2], argv[3]);
        return 3;
    }
    if (strcmp(argv[1], "delete-ref") == 0) {
        deleteRef(argv[2]);
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
    if (strcmp(argv[2], "add-list") == 0) {
        printf("Zone de préparation :\n");
        if (file_exists(".add")) {
            WorkTree * wt = ftwt(".add");
            printf("%s\n", wtts(wt));
        }
        return 7;
    }
    if (strcmp(argv[1], "commit") == 0) {
        if (strcmp(argv[3], "-m") == 0) {
            myGitCommit(argv[2], argv[4]);
        } else {
            myGitCommit(argv[2], NULL);
        }
        return 8;
    }
    if (strcmp(argv[1], "get-current-branch") == 0) {
        printf("%s", getCurrentBranch());
        return 9;
    }
    if (strcmp(argv[1], "branch") == 0) {
        if (!branchExists(argv[2]))
            createBranch(argv[2]);
        else
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
    return -1;
}
