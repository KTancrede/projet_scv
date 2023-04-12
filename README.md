# PROJET Structures de données

*`./myGit init `: initialise le répertoire de références et de la branche courrante   
*`./myGit list-refs` : affiche toutes les références existantes  
*`./myGit create-ref <name> <hash>` : crée la référence `<name>` qui pointe vers le commit correspondant au `<hash>` donné  
*`./myGit delete-ref <name>` : supprime la référence name  
*`./myGit add <elem> [<elem2> ...]` : ajoute un ou plusieurs fichiers/répertoires à la zone de préparation (prochain commit)  
*`./myGit clear-add` : vide la zone de préparation  
*`./myGit list-add` : affiche le contenu de la zone de préparation  
*`./myGit commit <branch name> [-m <message>]` : effectue un commit sur une branche, avec   ou sans message descriptif  
*`./myGit get-current-branch` : affiche le nom de la branche courante  
*`./myGit branch <branch-name>` : crée une branche qui s’appelle `<branch-name>` si elle   n’existe pas déjà  
*`./myGit branch-print <branch-name>` : affiche le hash de tous les commits de la branche,   accompagné de leur message descriptif éventuel  
*`./myGit checkout-branch <branch-name>` : réalise un déplacement sur la branche   `<branch-name>`  
*`./myGit checkout-commit <pattern>` : réalise un déplacement sur le commit qui commence par   `<pattern>`  
