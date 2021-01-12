#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define SRAND_VALUE 1985
#define N 2048
#define GENS 2000

int** Inicia_Jogo(){
    int **Cels;
    Cels = (int**) malloc(sizeof(int*)*N);
    int i;
    
    for(i = 0;i < N;i++){
            Cels[i] = (int*) malloc(sizeof(int)*N);
    }
    return Cels;
}

void Inicia_Celulas(int **Cels){
    int i,j;

    srand(SRAND_VALUE);

    for(i = 0; i < N; i++) { 
        for(j = 0; j < N; j++) {
            Cels[i][j] = rand() % 2;
        }
    }
}

int getNeighbors(int** grid, int i, int j){
    int qtdvivo = 0;
    int iplus = (i+1)%N;
    int isub;
    int jplus = (j+1)%N;
    int jsub;

    if(i > 0)
        isub = i - 1;
    else 
        isub = N - 1;
    if(j > 0)
        jsub = j - 1;
    else 
        jsub = N - 1;

    if(grid[i][jplus] == 1)
        qtdvivo++;
    
    if(grid[i][jsub] == 1)
        qtdvivo++;
    
    if(grid[iplus][j] == 1)
        qtdvivo++;
    
    if(grid[isub][j] == 1)
        qtdvivo++;

    if(grid[isub][jsub] == 1)
        qtdvivo++;
    
    if(grid[isub][jplus] == 1)
        qtdvivo++;

    if(grid[iplus][jplus] == 1)
        qtdvivo++;

    if(grid[iplus][jsub] == 1)
        qtdvivo++;
    
    return qtdvivo;

}

void Imprime_Tabuleiro(int **Cels){
    int i,j;
    for(i = 0; i < N; i++) { 
        for(j = 0; j < N; j++) {
            printf("%d ",Cels[i][j]);
        }
        putchar('\n');
    }

}

int** Jogo_da_Vida(int **Cels,int **Cels2){
    int i,j;
    int vizinhanca = 0,qtd = 0;
    long res;
    struct timeval tstart,tend;

#pragma omp parallel private(i,j) 
    {
#pragma omp for
        for(i = 0; i < N; i++) {
            for(j = 0; j < N; j++) {                
                vizinhanca = getNeighbors(Cels,i,j);
                if(Cels[i][j]){
                    if(vizinhanca < 2)
                        Cels2[i][j] = 0;  
                    else if(vizinhanca >= 4)
                        Cels2[i][j] = 0;
                    else
                        Cels2[i][j] = 1;
     
                }else
                    if(vizinhanca == 3)
                        Cels2[i][j] = 1;
            }
        }
    }
    gettimeofday(&tstart,NULL);
    for(i = 0; i < N; i++) {
            for(j = 0; j < N; j++) {
                if(Cels[i][j])
                    qtd++;
       }
    }
    gettimeofday (&tend,NULL);
    putchar('\n');
    res = (1000000*tend.tv_sec + tend.tv_usec) - (1000000*tstart.tv_sec + tstart.tv_usec);
    //printf("tempo:%ld\n",res);
    printf("%d\n",qtd);
    return Cels2;
}

int** Copia_Grid(int **m1,int **m2){
    int i,j;
    for(i = 0; i < N; i++) { 
        for(j = 0; j < N; j++) {
            m1[i][j] = m2[i][j];
        }
        
    }
    
    return m1;
}

void Libera_Memoria(int **l){
    int i;
    for(i = 0;i < N;i++)
        free(l[i]);

    free(l);
}

int main(){ 
    int i,j;
    int **grid,**newgrid;

    grid = Inicia_Jogo();
    newgrid = Inicia_Jogo();

    Inicia_Celulas(grid);
    for(i = 0;i < GENS;i++){
        printf("Geracao %d: ",i);
        newgrid = Jogo_da_Vida(grid,newgrid);
        grid = Copia_Grid(grid,newgrid);

    }
    
    Libera_Memoria(grid);
    Libera_Memoria(newgrid);

    return 0;
}