#include "mpi.h"              /* MPI header file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
/*
    Your Hash: 12f4a2723c08f99c4222763796f94027
    Your String: Domenico Liguori 07/09/1994
    
    1. gameoflife	Conway’s Game of Life Game
    f. m4.xlarge

    mpirun -np {numeroprocessori} ./gol_par {righe} {colonne} {generazioni}
*/

// Funzione che setta lo stato di world con lo stato locale contenuto in localWorld
static void updateWorldStatusLocal(int *world, int *localWorld, int offset, int elements){
    for(int i=0; i<elements; i++)
        world[offset+i] = localWorld[i];
}

// Genera la prima generazione in modo random
static void generateFirst(int *world, int row, int col){
    // printf("Generazione 0\n");
    for(int i = 0; i<row; i++){
        for(int j = 0; j<col; j++){
            world[j + (i*col)] = rand() % 2;
        }
    }
}

// Underpopulation
// Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// Overpopulation
// Any live cell with more than three live neighbours dies, as if by overpopulation.
static int underpopulationOrOverpopupationCheck(int current_col, int current_row, int *world, int row, int col){
    int alive;
    int top, right, bot, left;
    int aliveNeighbours = 0;

    // Si vicino sinistro
    left = (current_col>0) ? 1 : 0;
    // Si vicino destro
    right = (current_col<col-1) ? 1 : 0;
    // Si vicino top
    top = (current_row>0) ? 1 : 0;
    // Si vicino bot
    bot = (current_row<row-1) ? 1 : 0;

    // Checktop
    if(top==1){
        aliveNeighbours = (world[(current_col) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check top-right
        if(right==1)
            aliveNeighbours = (world[(current_col+1) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check top-left
        if(left==1)
            aliveNeighbours = (world[(current_col-1) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    }

    // Check bot
    if(bot==1){
        aliveNeighbours = (world[(current_col) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check bot-right
        if(right==1)
            aliveNeighbours = (world[(current_col+1) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check bot-left
        if(left==1)
            aliveNeighbours = (world[(current_col-1) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
    }

    // Check right
    if(right==1)
        aliveNeighbours = (world[(current_col+1) + ((current_row) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    // Check left
    if(left==1)
        aliveNeighbours = (world[(current_col-1) + ((current_row) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    // printf("Alive neighbours %d\n", aliveNeighbours);
    // Check underpopulation
    alive = (aliveNeighbours > 1) ? 1 : 0;

    // Check overpopulation
    alive = (aliveNeighbours > 3) ? 0 * alive : 1 * alive;

    // alive == 1 cella viva || alive == 0 cella morta
    return alive;
}

// Reproduction
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
int reproductionCheck(int current_col, int current_row, int *world, int row, int col){
    int resurrect;

    int top, right, bot, left;
    int aliveNeighbours = 0;

    // Si vicino sinistro
    left = (current_col>0) ? 1 : 0;
    // Si vicino destro
    right = (current_col<col-1) ? 1 : 0;
    // Si vicino top
    top = (current_row>0) ? 1 : 0;
    // Si vicino bot
    bot = (current_row<row-1) ? 1 : 0;

    // Checktop
    if(top==1){
        aliveNeighbours = (world[(current_col) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check top-right
        if(right==1)
            aliveNeighbours = (world[(current_col+1) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check top-left
        if(left==1)
            aliveNeighbours = (world[(current_col-1) + ((current_row-1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    }

    // Check bot
    if(bot==1){
        aliveNeighbours = (world[(current_col) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check bot-right
        if(right==1)
            aliveNeighbours = (world[(current_col+1) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
        // Check bot-left
        if(left==1)
            aliveNeighbours = (world[(current_col-1) + ((current_row+1) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;
    }

    // Check right
    if(right==1)
        aliveNeighbours = (world[(current_col+1) + ((current_row) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    // Check left
    if(left==1)
        aliveNeighbours = (world[(current_col-1) + ((current_row) * col)] == 1) ? aliveNeighbours+1 : aliveNeighbours;

    // Check resurrect
    resurrect = (aliveNeighbours == 3) ? 1 : 0;

    return resurrect;
}

// Update delle cellule
void updatePopulation(int *world, int *newWorld, int row, int col, int localRows, int start){
    // int start = my_rank * (row / numprocs); 
    // start = (my_rank < row % numprocs) ? start + my_rank : start + row % numprocs;
    for(int i = start; i < (start + localRows); i++){
        for(int j = 0; j<col; j++){
            if(world[j + (i*col)] == 1){// per le celle vive controllo solo overpop/underpop, per quelle morte reprod
                // Underpopulation
                newWorld[j + ((i-start)*col)] = underpopulationOrOverpopupationCheck(j, i, world, row, col); // underpopulation return 1 setta la casella a 1 (viva), cella sopravvissuta
            }else{
                // Reproduction
                newWorld[j + ((i-start)*col)] = reproductionCheck(j, i, world, row, col); // reporductio return 1 setta la casella ad 1 (viva), cella resuscitata
            }
        }
    }
}


int main (int argc, char *argv[])
{
	int numprocs, my_rank;
	MPI_Status status;
    MPI_Request request1, request2;
    int max_generation; // Numero di generazioni
    int row; // Numero righe
    int col; // Numero colonne
    int localRows; // Numero di righe che ogni processore "processsa"
    int localDim;
    int currentGeneration = 0; // Contatore delle generazioni
    double T_inizio,T_fine,T_max; // Variabili per conteggio tempo
    int start; // Offset utilizzato per associare elementi di world con localNewWorld

	/* Usual startup tasks */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
    /* Informazioni di esecuzione utili a tutti i processi */
    row = atoi(argv[1]); // Righe
    col = atoi(argv[2]); // Colonne
    max_generation = atoi(argv[3]); // Generazioni massime

    if(my_rank == 0)
        printf("Parallel\n");

    // Calcola quante righe per processore
    // Se è possibile dividere le righe sui processori in modo equo allora lo fa, se non può aggiunge una riga in piu' da calcolare
    // ai processi di rank minore del modulo row mod(numprocs) 
    localRows = (my_rank < (row % numprocs)) ? ((row / numprocs) + 1) : (row / numprocs);
    localDim = localRows * col;
	
    // Allocazione spazio di memoria per world
    int *world = (int*)calloc(row*col, sizeof(int));
    // Allocazione spazio di memoria per il world corrente che ogni processo vede
    int *localWorld = (int*)malloc(localDim * sizeof(int));
    // Allocazioen spazio di memoria per il newWorld, che contiene la generazione successiva
    int *newWorld;
    if(my_rank == 0){
        newWorld = (int*)malloc(row*col * sizeof(int));
    } 
    // Allocazione spazio di memoria per la parte del nuovo world che ogni processo calcola
    int *localNewWorld = (int*)malloc(localDim * sizeof(int));
    // Allocazione spazio di memoria per recvcounts che indica quanti elementi ogni processore invia
    int *recvcounts = (int*)malloc(numprocs * sizeof(int));
    // Allorazione spazio di memoria per displs che indica da dove ogni processore può iniziare a scrivere
    int *displs = (int*)malloc(numprocs * sizeof(int));
    // Ultima riga locale da inviare al processo successivo
    int *botLocalRow = (int*)malloc(col * sizeof(int));
    int *topLocalRow;
    if(my_rank > 0){
        // Prima riga locale da inviare al processo che precede
        topLocalRow = (int*)malloc(col * sizeof(int)); 
    }
    // Spazio di memoria per la riga da inviare
    int *rowBuff = (int*)malloc(col * sizeof(int));

    // Popola recvcounts e displs per tutti i processi
    for(int i=0; i<numprocs; i++){
        recvcounts[i] = (i < (row % numprocs)) ? ((row / numprocs) + 1) * col : (row / numprocs) * col;

        displs[i] = (i == 0) ? 0 : displs[i-1]+recvcounts[i-1];
    }

    // Calcolo di start
    start = my_rank * (row / numprocs); 
    start = (my_rank < row % numprocs) ? start + my_rank : start + row % numprocs;
    

    /*  Start timer */
    T_inizio=MPI_Wtime();

    // Master genera la prima generazione
	if(my_rank == 0){     
        generateFirst(world, row, col);
	}


    // Generation loop
    while(currentGeneration < max_generation){

        // Update di world su tutti i processi che non sono root
        // Invio le parti del world corrente su cui ogni processo deve lavorare
        if(currentGeneration == 0)
            MPI_Scatterv(world, recvcounts, displs, MPI_INT, localWorld, localDim, MPI_INT, 0, MPI_COMM_WORLD);

        // Ogni processo si occupa di inviare prima ed ultima riga al processo successivo e precedente; eccezione per il primo ed ultimo processo
    
        // Invia la propria top row
        if(my_rank > 0){
            memcpy(rowBuff, &localWorld[0], col * sizeof(int) );
            MPI_Isend(rowBuff, col, MPI_INT, my_rank - 1, my_rank, MPI_COMM_WORLD, &request1);
        }
        
        // Invia la propria bot row
        if(my_rank < numprocs - 1){
            memcpy(rowBuff, &localWorld[(localRows - 1) * col], col * sizeof(int));
            MPI_Isend(rowBuff, col, MPI_INT, my_rank + 1, my_rank, MPI_COMM_WORLD, &request2);
        }

        // Riceve la top row del processo successivo
        if(my_rank < numprocs - 1)
            MPI_Irecv(botLocalRow, col, MPI_INT, my_rank + 1, my_rank + 1, MPI_COMM_WORLD, &request1);

        // Riceve la bot row del processo precedente
        if(my_rank > 0)
            MPI_Irecv(topLocalRow, col, MPI_INT, my_rank - 1, my_rank - 1, MPI_COMM_WORLD, &request2);

        
        // Update dello stato di world su tutti i processi
        updateWorldStatusLocal(world, localWorld, displs[my_rank], recvcounts[my_rank]);

        if(my_rank > 0){
            MPI_Wait(&request2, &status);
            updateWorldStatusLocal(world, topLocalRow, displs[my_rank]-col , col);
        }
        if(my_rank < numprocs - 1){
            MPI_Wait(&request1, &status);
            updateWorldStatusLocal(world, botLocalRow, displs[my_rank + 1], col);
        }
        
        updatePopulation(world, localNewWorld, row, col, localRows, start); // Chiamo funzione update

        
        localWorld = localNewWorld;

        currentGeneration++; // Incremento contatore generazione  
    }

    // Lo stato finale della computazione è contenuto in newWorld
    MPI_Gatherv(localNewWorld, localDim, MPI_INT, newWorld, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    /*  End timer */
    MPI_Barrier(MPI_COMM_WORLD);
	T_fine=MPI_Wtime()-T_inizio;
    MPI_Reduce(&T_fine, &T_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Tutte le generazioni sono state computate, report tempi
    if(my_rank == 0){
        printf("Number of processors: %d\n", numprocs);
		printf("Max time overall: %f\n",T_max);
    }

    // Memory free
    free(world);
    free(localNewWorld);
    if(my_rank == 0)
        free(newWorld);
    free(botLocalRow);
    if(my_rank > 0)
        free(topLocalRow);
    free(rowBuff);
    free(displs);
    free(recvcounts);
    
    // MPI ending operation
    MPI_Finalize();
}