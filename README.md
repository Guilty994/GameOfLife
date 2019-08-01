# Game Of Life
 ## PCPC homework

  **Studente:**      Domenico Liguori, 0522500580

  **Homework:**      Conway’s 'Game of Life' Game

  **Istanze AWS:**   m4.xlarge

  ### Come...
   **Compilare:**    `mpicc gameOfLife.c -o gameOfLife`

   **Eseguire:**      `mpirun -np {Numero di VCPU} ./gameOfLife {Righe matrice} {Colonne matrice} {Numero di generazioni}`
   
   `{Numero di VCPU} <= {Righe matrice}`

  
  ### Descrzione soluzione
  Nella soluzione proposta, prima di inziaire con l'avvio del gioco vero e proprio, tutti i processi inizializzano le strutture dati necessarie, come `world`, per memorizzare la matrice iniziale e `newWorld` che conterrà lo stato finale della matrice, oltre che a diverse strutture per la computazione locale e l'invio di dati ai processi adiacenti. Tutti i processi recuperano le informazioni relative alla dimensione del problema, al numero di processi allocati, calcolano le dimensioni locali del problema che dovranno essere computate e le dimensioni dei dati da comunicare ai processi adiancenti.

  La suddivisione della matrice contenente lo stato delle cellule, è stata fatta per righe; questa scelta non è stata casuale, ma adottata per favorire la distrubizione del carico da parte del Master e la comunicazione tra pocessi, i quali leggono i dati da inviare da posizioni contigue di memoria.

  Terminata la parte di inizializzazione, il processo Master genera lo stato iniziale del gioco tramite la funzione `generateFirst()`, questi crea una matrice di dimensione N x M contenente 0 (cellula morta) ed 1 (cellula viva) assegnati in modo pseudo-casuale, i quali sono stati precedentente letti dagli argomenti passati tramite linea di comando dall'utente. 

  Subito dopo, viene eseguito un `while` che si ferma quando tutte le generazioni sono state computate; qui viene verificato se ci troviamo nella prima iterazione, in tal caso viene fatta una `MPI_Scatterv` che distribuisce in modo equo il carico, nel caso sia possibile, altrimenti i processi con `rank < row % numprocs` avranno una riga in più da computare rispetto gli altri. La parte locale di informazioni da computare per ogni processo, sarà contenuta in `localWorld`. Subito dopo, i processi adiacenti comunicano le proprie righe estreme. Gli invii sono fatti tramite `ISend` e ricevuti tramite `IReceive`: questo tipo di comunicazione non-bloccante è stata scelta per far in modo che, tra la comunicazione e la ricezione, i processi possono già iniziare ad aggiornare il proprio stato di `world` locale in base a ciò che è contenuto in `localWorld`, e completare l'aggiornamento con le righe mancanti quando queste saranno disponibili. Lo stato locale di `world` è quindi aggiornato, viene chiamata la funzione `updatePopulation()` che controlla in base alle cellule adiacenti se la cellula in questione sopravvive, muore per overpopulation o underpopulation, oppure resuscita. Il nuovo stato locale delle cellule sarà contenuto per ogni processo nella matrice `locaNewWorld`, viene quindi fatto uno swap di riferimento tra `localNewWorld` e `localWorld`, incrementato il contatore delle generazioni e si ripete il ciclo.

  Quando il ciclo delle generazioni è completo, viene fatta una `MPI_Gatherv` che prende tutti i `localNewWorld` e li aggrega dentro `newWorld`, qui sarà contenuto lo stato finale del gioco.

  ### Benchmark
  Row | Col | Gen 
  --- | --- |----
  7500|7500 |100

  INSTANCEs | VCPUs  |  AVG Time   |  SpeedUp	
  --------- | ------ |------------ |-------------
  1	        | 4    	 |  83.975919	 |  2.346936507	
  2	        | 8    	 |  44.227987	 |  4.456141086	
  3	        | 12     |  30.609167	 |  6.438794953	
  4	        | 16     |  23.848921	 |  8.2639441	
  5	        | 20     |  20.004864	 |  9.851911515	
  6	        | 24     |  17.329431	 |  11.37291524	
  7	        | 28     |  15.349215	 |  12.84014525	
  8         | 32     |  13.862413	 |  14.21730474	
  ## Strong scalability
  ![Strong scalability](https://github.com/Guilty994/GameOfLife/blob/master/strong.png)

  ## Weak scalability
  ![Weak scalability](https://github.com/Guilty994/GameOfLife/blob/master/weak.png)




