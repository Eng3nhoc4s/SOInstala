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

//==============================================
// Declarar o acesso a eventuais variáveis ou estruturas externas
// ex: extern struct ...

//==============================================

/* main_cliente recebe como parâmetro o nº de clientes a criar */
void main_cliente(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
	// para cada processo criado:
    // - chamar cliente_executar
	// - terminar processo (exit) passando o valor devolvido por cliente_executar
    // o processo criador deve:
	// - guardar pid de cada filho no vetor Ind.pid_clientes[n], com n=0,1,...
    so_main_cliente(quant);
    //==============================================
}

/* main_rececionista recebe como parâmetro o nº de rececionistas a criar */
void main_rececionista(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
	// para cada processo criado:
    // - chamar rececionista_executar
	// - terminar processo (exit) passando o valor devolvido por rececionista_executar
    // o processo criador deve:
    // - guardar pid de cada filho no vetor Ind.pid_rececionistas[n], com n=0,1,...
    so_main_rececionista(quant);
    //==============================================
}

/* main_instalador recebe como parâmetro o nº de instaladores a criar */
void main_instalador(int quant)
{
    //==============================================
    // CRIAR PROCESSOS
    //
	// para cada processo criado:
    // - chamar instalador_executar
	// - terminar processo (exit) passando o valor devolvido por instalador_executar
    // o processo criador deve:
    // - guardar pid de cada filho no vetor Ind.pid_instaladores[n], com n=0,1,...
    so_main_instalador(quant);
    //==============================================
}

int main(int argc, char* argv[])
{
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
    intervalo = so_main_args(argc, argv, &ficEntrada, &ficSaida, &ficLog);
    //==============================================

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
    // esperar pela terminacao de cada cliente (waitpid)
	// se processo terminou normalmente (WIFEXITED) e valor devolvido não é Config.SERVICOS:
	// - incrementar o indicador Ind.servicos_obtidos_pelos_clientes[n], n=0,1,...
    so_main_wait_clientes();
    //==============================================

    // Desarmar alarme
    tempo_terminar(intervalo);

    printf("*** Fechar SOinstala\n\n");
    controlo_fechar_soinstala();

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS RECECIONISTAS E ATUALIZAR INDICADORES
    //
    // esperar pela terminacao de cada rececionista (wait ou waitpid)
	// se processo terminou normalmente (WIFEXITED) e valor devolvido não é Config.SERVICOS:
	// - incrementar o indicador Ind.clientes_atendidos_pelos_rececionistas[n], n=0,1,...
    so_main_wait_rececionistas();
    //==============================================

    //==============================================
    // ESPERAR PELA TERMINAÇÃO DOS INSTALADORES E ATUALIZAR INDICADORES
    //
    // esperar pela terminacao de cada instalador (wait ou waitpid)
	// se processo terminou normalmente (WIFEXITED) e valor devolvido não é Config.SERVICOS:
	// - incrementar o indicador Ind.clientes_atendidos_pelos_instaladores[n], n=0,1,...
    so_main_wait_instaladores();
    //==============================================

    printf("*** Indicadores\n");
    so_escreve_indicadores();

    // destruir zonas de memória e semáforos
    ficheiro_destruir();
    controlo_destruir();
    prodcons_destruir();
    memoria_destruir();

    return 0;
}
