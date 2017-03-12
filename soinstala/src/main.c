#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "rececionista.h"
#include "instalador.h"
#include "cliente.h"
#include "memoria.h"
#include "prodcons.h"
#include "controlo.h"
#include "ficheiro.h"
#include "tempo.h"
#include "so.h"

struct indicadores Ind;     // indicadores do funcionamento do SOinstala
struct configuracao Config; // configuração da execução do SOinstala

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant){
    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar cliente_executar
    // e guardar pid do filho no vetor Ind.pid_clientes[n], com n=0,1,...
    //so_main_cliente(quant);
    //==============================================

   int i, temp;
    for(i = 0; i < quant; i++){
        temp = fork();
        
        //Se o fork não tiver sucesso
        if(temp == -1){
            perror("main_cliente: fork() - FAIL");
            exit(1);
            
        //Se o fork tiver sucesso
        }else if(temp == 0)
            exit(cliente_executar(i));
        else
            Ind.pid_clientes[i] = temp;
        
    }   


}

/* main_rececionista recebe como parâmetro o nº de rececionistas a criar */
void main_rececionista(int quant){
    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar rececionista_executar 
    // e guardar pid do filho no vetor Ind.pid_rececionistas[n], com n=0,1,...
    //so_main_rececionista(quant);
    //==============================================

	int i, temp;
    for(i = 0; i < quant; i++){
        temp = fork();
        
        //Se o fork não tiver sucesso
        if(temp == -1){
            perror("main_rececionista: fork() - FAIL");
            exit(1);
            
        //Se o fork tiver sucesso
        }else if(temp == 0)
            exit(rececionista_executar(i));
        else
            Ind.pid_rececionistas[i] = temp;
    }
}

void printUsage(){
    printf("Como usar:\nsonistala <ficheiro_configuracao> [ficheiro resultados] [-l ficheiro_log] [-t intervalo(us)]\n");
}


/* main_instalador recebe como parâmetro o nº de instaladores a criar */
void main_instalador(int quant){
    //==============================================
    // CRIAR PROCESSOS
    //
    // após a criação de cada processo, chamar instalador_executar 
    // e guardar pid do filho no vetor Ind.pid_instaladores[n], com n=0,1,...
    //so_main_instalador(quant);
    //==============================================

    int i, temp;
    for(i = 0; i < quant; i++){
        temp = fork();
        
        //Se o fork não tiver sucesso
        if(temp == -1){
            perror("main_rececionista: fork() - FAIL");
            exit(1);
            
        //Se o fork tiver sucesso
        }else if(temp == 0)
            exit(instalador_executar(i));
        else
            Ind.pid_instaladores[i] = temp;
    }
}

int main(int argc, char* argv[]){

    int n,result;
    char *ficEntrada=NULL;
    char *ficSaida=NULL;
    char *ficLog=NULL;
    long intervalo=0;

    //==============================================
    // TRATAR PARÂMETROS DE ENTRADA
    // parâmetro obrigatório: ficheiro_configuracao
    // parâmetros opcionais:
    //   ficheiro_resultados
    //   -l ficheiro_log
    //   -t intervalo(us)    // us: microsegundos
    //
    // para qualquer parâmetro desconhecido ou falta de parâmetros
    // escrever mensagem de utilização "Como usar", dar exemplo e terminar
    //intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

    //Não foram fornecidos argumentos
    if(argc < 2){
        printf("ERRO: Nenhum argumento fornecido!\n");
        printUsage();
        exit(-1);
    
    //Caso tenham sido fornecidos argumentos
    }else{

        ficEntrada = argv[1];
        n = 2;

        while(n < argc){

            if(strcmp(argv[n], "-t") == 0)
                if (n+1 < argc){
                    intervalo = atol(argv[n+1]);
                    n++;

                }else{
                    printUsage();
                    exit(-1);
                }
            else if(strcmp(argv[n], "-l") == 0)
                if (n+1 < argc){
                    ficLog = argv[n+1];
                    n++;

                }else{
                    printUsage();
                    exit(-1);                
                }
            else
                ficSaida = argv[n];
                
            n++;
        }
    }

    printf("\n---------------------------");
    printf("\n--- Oficina SOinstala ---");
    printf("\n---------------------------\n");

    // Ler dados de entrada
    ficheiro_iniciar(ficEntrada,ficSaida,ficLog);

    // criar zonas de memória e semáforos
    memoria_criar_buffers();
    prodcons_criar_buffers();
    controlo_criar();

    // Criar estruturas para indicadores e guardar stock inicial
    memoria_criar_indicadores();

    printf("\n*** Abrir SOinstala\n");
    controlo_abrir_soinstala();

    // Registar início de operação e armar alarme
    tempo_iniciar(intervalo);

    //
    // Iniciar sistema
    //

    // Criar rececionistas
    main_rececionista(Config.RECECIONISTAS);
    // Criar instalador
    main_instalador(Config.INSTALADORES);
    // Criar clientes
    main_cliente(Config.CLIENTES);


    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de clientes
    // Ind.servicos_obtidos_pelos_clientes[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_clientes();
    //==============================================

    for(n = 0; n < Config.CLIENTES; n++){
        waitpid(Ind.pid_clientes[n], &result, 0);
        
        if(WIFEXITED(result))
            Ind.servicos_obtidos_pelos_clientes[WEXITSTATUS(result)] += 1;
        else
            printf("Erro \n");
    
    }

    printf("*** Fechar SOinstala\n\n");
    controlo_fechar_soinstala();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de rececionistas
    // Ind.clientes_atendidos_pelos_rececionistas[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_rececionistas();
    //==============================================

    for(n = 0; n < Config.RECECIONISTAS; n++){
        waitpid(Ind.pid_rececionistas[n], &result, 0);
        Ind.clientes_atendidos_pelos_rececionistas[n] = WEXITSTATUS(result);
    }

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS CLIENTES E ATUALIZAR INDICADORES
    //
    // esperar e incrementar o indicador de instaladores
    // Ind.clientes_atendidos_pelos_instaladores[n], n=0,1,...
    // com o estado devolvido pela terminação do processo
    //so_main_wait_instaladores();
    //==============================================

    for(n = 0; n < Config.INSTALADORES; n++){
        waitpid(Ind.pid_instaladores[n], &result, 0);
        Ind.clientes_atendidos_pelos_instaladores[n] = WEXITSTATUS(result);
    }

    printf("*** Indicadores\n");
    so_escreve_indicadores();

    // destruir zonas de memória e semáforos
    ficheiro_destruir();
    controlo_destruir();
    prodcons_destruir();
    memoria_destruir();

    return 0;
}
