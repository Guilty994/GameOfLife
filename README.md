# Game Of Life
 ## PCPC homework

  **Studente:**      Domenico Liguori, 0522500580

  **Homework:**      Conway’s 'Game of Life' Game

  **Istanze AWS:**   m4.xlarge

  ### Come...
   **Compilare:**    `mpicc gameOfLife.c -o gameOfLife`

   **Eseguire:**      `mpirun -np {Numero di VCPU} ./gameOfLife {Righe matrice} {Colonne matrice} {Numero di generazioni}`
   
   {Numero di VCPU} <= {Righe matrice}

  
  ### Descrzione soluzione
    Nella soluzione proposta, il processo Master genera lo stato iniziale del gioco tramite la funzione generateFirst(), questi crea una matrice di 0 ed 1 assegnati in modo pseudo-casuale di dimensione N x M, due parametri inviati da riga di comando dall'utente.


    Nella soluzione proposta, 
    
    La suddivisione della matrice contenente lo stato delle cellule, è stata fatta per righe; questa scelta non è stata casuale, ma adottata per favorire la distrubizione del carico da parte del master, il quale legge i dati da inviare da posizioni contigue di memoria.

     
  ### Benchmark


