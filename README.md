# Game Of Life
## PCPC homework

**Studente:**      Domenico Liguori, 0522500580

**Homework:**      Conway’s 'Game of Life' Game

**Istanze AWS:**   m4.xlarge

### Come...
**Compilare:**    `mpicc gameOfLife.c -o gameOfLife`

**Eseguire:**      `mpirun -np {Numero di VCPU} ./gameOfLife {Righe matrice} {Colonne matrice} {Numero di generazioni}`

`{Numero di VCPU} <= {Righe matrice}`


### Descrizione soluzione
Nella soluzione proposta, prima di inziaire con l'avvio del gioco vero e proprio, tutti i processi inizializzano le strutture dati necessarie, come `world`, per memorizzare la matrice iniziale e `newWorld` che conterrà lo stato finale della matrice, oltre che a diverse strutture per la computazione locale e l'invio di dati ai processi adiacenti. Tutti i processi recuperano le informazioni relative alla dimensione del problema, al numero di processi allocati, calcolano le dimensioni locali del problema che dovranno essere computate e le dimensioni dei dati da comunicare ai processi adiancenti.

La suddivisione della matrice contenente lo stato delle cellule, è stata fatta per righe; questa scelta non è stata casuale, ma adottata per favorire la distrubizione del carico da parte del Master e la comunicazione tra pocessi, i quali leggono i dati da inviare da posizioni contigue di memoria.

Terminata la parte di inizializzazione, il processo Master genera lo stato iniziale del gioco tramite la funzione `generateFirst()`, questi crea una matrice di dimensione N x M contenente 0 (cellula morta) ed 1 (cellula viva) assegnati in modo pseudo-casuale, i quali sono stati precedentente letti dagli argomenti passati tramite linea di comando dall'utente. 

Subito dopo, viene eseguito un `while` che si ferma quando tutte le generazioni sono state computate; qui viene verificato se ci troviamo nella prima iterazione, in tal caso viene fatta una `MPI_Scatterv` che distribuisce in modo equo il carico, nel caso sia possibile, altrimenti i processi con `rank < row % numprocs` avranno una riga in più da computare rispetto gli altri. La parte locale di informazioni da computare per ogni processo, sarà contenuta in `localWorld`. Subito dopo, i processi adiacenti comunicano le proprie righe estreme. Gli invii sono fatti tramite `ISend` e ricevuti tramite `IReceive`: questo tipo di comunicazione non-bloccante è stata scelta per far in modo che, tra la comunicazione e la ricezione, i processi possono già iniziare ad aggiornare il proprio stato di `world` locale in base a ciò che è contenuto in `localWorld`, e completare l'aggiornamento con le righe mancanti quando queste saranno disponibili. Lo stato locale di `world` è quindi aggiornato, viene chiamata la funzione `updatePopulation()` che controlla in base alle cellule adiacenti se la cellula in questione sopravvive, muore per overpopulation o underpopulation, oppure resuscita. Il nuovo stato locale delle cellule sarà contenuto per ogni processo nella matrice `locaNewWorld`, viene quindi fatto uno swap di riferimento tra `localNewWorld` e `localWorld`, incrementato il contatore delle generazioni e si ripete il ciclo.

Quando il ciclo delle generazioni è completo, viene fatta una `MPI_Gatherv` che prende tutti i `localNewWorld` e li aggrega dentro `newWorld`, qui sarà contenuto lo stato finale del gioco.

### Benchmarks
* Righe 1000, Colonne 1000, Generazioni = 100

    INSTANCEs | VCPUs  |  AVG Time   
    --------- | ------ |------------ 
    1         |1       |  3.480594
    1	      | 4      |  1.489598	
    2	      | 8      |  0.832074	 	
    3	      | 12     |  0.619511
    4	      | 16     |  0.542398	 	
    5	      | 20     |  0.496264	
    6	      | 24     | 0.493969	
    7	      | 28     |  0.501541 
    8         | 32     |  0.520974

* Righe 2500, Colonne 2500, Generazioni = 100

    INSTANCEs | VCPUs  |  AVG Time   
    --------- | ------ |------------ 
    1         |1       |21.935433
    1	      | 4      |  9.390863	
    2	      | 8      |  4.918352 	
    3	      | 12     | 3.476017	
    4	      | 16     | 2.749868 	
    5	      | 20     |  2.351745
    6	      | 24     |  2.044515	
    7	      | 28     |  1.886699 
    8         | 32     | 1.734644

* Righe 5000, Colonne 5000, Generazioni = 100

    INSTANCEs | VCPUs  |  AVG Time   
    --------- | ------ |------------ 
    1         | 1      |  87.538921
    1	      | 4      |  37.304942	
    2	      | 8      |  19.569110	 	
    3	      | 12     |  13.603843	
    4	      | 16     |  11.034255	 	
    5	      | 20     |  9.020618	
    6	      | 24     |  7.948478	
    7	      | 28     |  6.905193 
    8         | 32     |  6.253685

* Righe 7500, Colonne 7500, Generazioni = 100

    INSTANCEs | VCPUs  |  AVG Time   
    --------- | ------ |------------ 
    1         |1       |197.086150
    1	      | 4      |  83.975919	
    2	      | 8      |  44.227987	 	
    3	      | 12     |  30.609167	
    4	      | 16     |  23.848921	 	
    5	      | 20     |  20.004864	
    6	      | 24     |  17.329431	
    7	      | 28     |  15.349215	 
    8         | 32     |  13.862413	

#### Strong scalability
![Strong scalability](https://github.com/Guilty994/GameOfLife/blob/master/imgs/strong.png)

#### Risultati
Analizzando i risultati della strong scalability, abbiamo i valori più alti sui 4 VCPU, intorno al 60%, tuttavia richiede dei tempi ancora troppo alti per l'algoritmo. Lo sweet spoot possiamo trovarlo tra i 4 e gli 8 VCPU in cui abbiamo un efficieza che va dal 60% al 25%, in questo intervallo infatti pur avendo un livello di efficienza non troppo alto, abbiamo un tempo di risoluzione del programma ragionevole, sopratutto per dimensioni del problema molto grandi.
Il fatto che abbiamo dei risultati alquanto deludenti dalla strong scalability è dovuta sicuramente al fatto che il problema è cpu-bound ma anche all'overhead dell'allocare nuovi processi.

#### Weak scalability
* Righe: 1000, Colonne: 1000, Generazioni: 100
![Weak scalability 1000](https://github.com/Guilty994/GameOfLife/blob/master/imgs/weak1000.png)
* Righe: 2500, Colonne: 2500, Generazioni: 100
![Weak scalability 1000](https://github.com/Guilty994/GameOfLife/blob/master/imgs/weak2500.png)
* Righe: 5000, Colonne: 5000, Generazioni: 100
![Weak scalability 5000](https://github.com/Guilty994/GameOfLife/blob/master/imgs/weak5000.png)
* Righe: 7500, Colonne: 7500, Generazioni: 100
![Weak scalability 7500](https://github.com/Guilty994/GameOfLife/blob/master/imgs/weak7500.png)

#### Risultati
Analizzando i risultati della weak scalability, possiamo osservare che per workload bassi (Righe: 1000, Colonne: 1000, Generazioni: 100), abbiamo uno speedup che tende a crescere fino alle 20/24 unità di elaborazione per poi iniziare a descrescere, infatti possiamo osservare che tra 24 e 32 VCPU abbiamo un aumento del tempo di esecuzione di ben 30 ms. La situazione cambia quando il workload cresce, infatti possiamo osservare una crescita dello speedup quasi costante al crescere del numero di VCPU.
Possiamo giustificare questi risultati concludendo che il problema richiede molte risorse del sistema quando abbiamo un grosso workload, inoltre possiamo giustificare il decremento di prestazioni, nel caso di workload basso, a causa del grosso overhead richiesto dall'allocazione di più processi rispetto al carico del problema.
