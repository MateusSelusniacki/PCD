#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define SRAND_VALUE 1985
#define N 2048
#define MSGTAG 1
#define GENS 2000


int* Inicia_Linha(int* linha,int rank,int tam){
    int i;

    srand(SRAND_VALUE);
    for(i = 0;i < tam;i++){
        /*Pega um numero aleatorio e divide por um numero primo antes de pegar o resto da divisão 
        por 2 para garantir que vai gerar uma matriz completamente aleatoria.
        Caso contrario todas as linhas da matriz seriam iguais.*/
        linha[i] = (rand()/((rank+1)*3))%2;

    }

    return linha;
}

int *Calcula_Vizinhos(int *grid,int *linha_ant,int *linha_post,int *newgrid,int rank){
    int i,cont;

    for(i = 0;i < N;i++){
        cont = 0;
         if(i != 0){
            if(grid[(i-1)])
                cont++;

            if(linha_post[(i-1)])
                cont++;

            if(linha_ant[(i-1)])
                cont++;
        }else{
            if(grid[(N-1)])
                cont++;

            if(linha_post[(N-1)])
                cont++;

            if(linha_ant[(N-1)])
                cont++;

        }
            
        if(linha_post[i])
            cont++;

        if(linha_ant[i])
            cont++;

        if(linha_ant[(i+1)%N])
            cont++;

        if(grid[(i+1)%N])
            cont++;

        if(linha_post[(i+1)%N])
            cont++;

        if(grid[i]){
            if(cont < 2)
                newgrid[i] = 0;
            else if(cont >= 4)
                newgrid[i] = 0;
            else  
                newgrid[i] = 1;
        }else if(cont == 3)
            newgrid[i] = 1;
        else
            newgrid[i] = 0;

    }

    return newgrid;

}

int *Copia_Grid(int *vet1,int *vet2,int inicio,int fim){
    int i;

    for(i = inicio;i < fim;i++){
        vet2[i-inicio] = vet1[i];
    }

    return vet2;
}

int main(int argc,char** argv){
    int i,j,k,l; 
    int *grid,*newgrid,*aux;
    int rank,size;
    int prox,ant,nvezes = 1;
    int SubVetor_vant[N],SubVetor_vprox[N];
    MPI_Status status[4];
    MPI_Request request[4];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int soma = 0;
    int nvivos;

    prox = (rank+1)%size;
    if(rank != 0)
        ant = rank - 1;
    else{
        ant = size-1;
    }
        
    if(size < N){
        nvezes = N/size;
        if(rank < N - nvezes*size){
            nvezes++;
        }
    }
    
    grid = (int*) malloc(nvezes * sizeof(int) * N);
    newgrid = (int*) malloc(sizeof(int) * N);
    aux = (int*) malloc(sizeof(int) * N);

    grid = Inicia_Linha(grid,(rank+1),N*nvezes);
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank < N){
        for(l = 0;l < GENS;l++){
            soma = 0;
            for(k = 0;k < nvezes;k++){
                if(N%size == 0){
                    MPI_Irecv(SubVetor_vant,N,MPI_INT,ant,MSGTAG,MPI_COMM_WORLD,&request[0]);
                    MPI_Irecv(SubVetor_vprox,N,MPI_INT,prox,MSGTAG,MPI_COMM_WORLD,&request[1]);
                    MPI_Isend(&grid[k*N],N,MPI_INT,prox,MSGTAG,MPI_COMM_WORLD,&request[2]);
                    if(ant == 0 || rank == 0)
                        MPI_Isend(&grid[((k+1)%nvezes)*N],N,MPI_INT,ant,MSGTAG,MPI_COMM_WORLD,&request[3]);
                    else
                        MPI_Isend(&grid[k*N],N,MPI_INT,ant,MSGTAG,MPI_COMM_WORLD,&request[3]);
                }else{
                    if(k == nvezes - 1 && rank == 0)
                        MPI_Irecv(SubVetor_vprox,N,MPI_INT,(N%size)-1,MSGTAG,MPI_COMM_WORLD,&request[0]);
                    else
                        MPI_Irecv(SubVetor_vant,N,MPI_INT,ant ,MSGTAG,MPI_COMM_WORLD,&request[0]);
                    if(rank == N%size - 1 && k == nvezes - 1){
                        MPI_Irecv(SubVetor_vant,N,MPI_INT,0,MSGTAG,MPI_COMM_WORLD,&request[1]);
                        MPI_Isend(&grid[((k+1)%nvezes)*N],N,MPI_INT,0,MSGTAG,MPI_COMM_WORLD,&request[2]);
                    }else{
                        MPI_Irecv(SubVetor_vprox,N,MPI_INT,prox,MSGTAG,MPI_COMM_WORLD,&request[1]);
                        MPI_Isend(&grid[k*N],N,MPI_INT,prox,MSGTAG,MPI_COMM_WORLD,&request[2]);
                    }
                    
                    if(N%size != 0 && k == nvezes - 1 && rank == 0)
                        MPI_Isend(&grid[(nvezes-k-1)*N],N,MPI_INT,(N%size)-1,MSGTAG,MPI_COMM_WORLD,&request[3]);
                    else
                        MPI_Isend(&grid[(nvezes-k-1)*N],N,MPI_INT,ant,MSGTAG,MPI_COMM_WORLD,&request[3]);
                    
                }
                
                MPI_Waitall(4,request,status);
                
                aux = Copia_Grid(grid,aux,k*N,(k*N)+N);
                newgrid = Calcula_Vizinhos(aux,SubVetor_vant,SubVetor_vprox,newgrid,rank);
                for(i = 0; i < N;i++){
                    grid[k*N + i] = newgrid[i];
                    if(newgrid[i] == 1)
                        soma++;
                }
                
            }
        MPI_Reduce(&soma,&nvivos,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
        if(rank == 0)
            printf("geracao %d numero de vivos: %d\n",l+1,nvivos);
        soma = 0;
        }
    }
    MPI_Finalize();

    return 0;
}