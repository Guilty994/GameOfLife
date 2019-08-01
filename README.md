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
  Nella soluzione proposta, prima di inziaire con l'avvio del gioco vero e proprio, tutti i processi inizializzano le strutture dati necessarie, come `world`, per memorizzare la matrice iniziale e `newWorld` che contiene lo stato finale della matrice dopo la computazione della generazione, oltre che a diverse strutture per la computazione locale e l'invio di dati ai processi adiacenti. Tutti i processi recuperano le informazioni relative alla dimensione del problema ed al numero di processi allocati e calcolano le dimensioni locali del problema che dovranno computare essere computate e le dimensioni dei dati da comunicare ai processi adiancenti.

  La suddivisione della matrice contenente lo stato delle cellule, è stata fatta per righe; questa scelta non è stata casuale, ma adottata per favorire la distrubizione del carico da parte del Master e la comunicazione tra pocessi, i quali leggono i dati da inviare da posizioni contigue di memoria.

  Terminata la parte di inizializzazione, il processo Master genera lo stato iniziale del gioco tramite la funzione `generateFirst()`, questi crea una matrice di 0 ed 1 assegnati in modo pseudo-casuale di dimensione N x M, i quali sono stati precedentente letti dagli argomenti passati tramite linea di comando dall'utente. 

  Inizia subito dopo un ciclo che si ferma quando tutte le generazioni sono state computate; qui viene verificato se ci troviamo nella prima iterazione, in tal caso viene fatta una `MPI_Scatterv` che distribuisce in modo equo il carico, nel caso sia possibile, altrimenti i processi con `rank < row%(numprocs)` avranno una riga in più da computare rispetto gli altri. La parte locale di informazioni da coputare per ogni processo, sarà contenuta in localWorld. Subito dopo i processi comunicano le proprie righe estreme ai processi adiacenti. Gli invii sono fatti tramite `ISend` e ricevuti tramite `IReceive`; questo tipo di comunicazione non-bloccante è stata scelta per far in modo che tra la comunicazione e la ricezione i processi possono già iniziare ad aggiornare il proprio stato di `world` locale, e completare l'aggiornamento con le righe mancanti quando queste sono disponibili. Quando lo stato locale di `world` è quindi aggiornato, viene chiamata la funzione `updatePopulation()` che controlla in base alle cellule adiacenti se la cellula in questione sopravvive, muore per overpopulation o underpopulation, oppure resuscita. Il nuovo stato locale delle cellule sarà contenuto per ogni processo nella matrice locaNewWorld. Viene quindi fatto uno swap di riferimento tra `localNewWorld` e `localWorld`, incrementato il contatore delle generazioni e si ripete il ciclo.

  Quando il ciclo delle generazioni è completo, viene fatta una `MPI_Gatherv` che prende tutti i `localNewWorld` e li aggrega dentro `newWorld`, qui sarà contenuto lo stato finale del gioco.

  ### Benchmark


