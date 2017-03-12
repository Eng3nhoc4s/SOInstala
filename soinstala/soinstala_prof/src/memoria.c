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
#include "so.h"
#include "memoria.h"
#include "prodcons.h"
#include "controlo.h"
#include "escalonador.h"
#include "ficheiro.h"

struct relatorio_c BConclusao;  // buffer instalador-cliente
struct pedido_i    BInstalacao; // buffer rececionista-instalador
struct pedido_s    BServico; 	// buffer cliente-rececionista

//==============================================
// Declarar o acesso a eventuais variáveis ou estruturas externas
// ex: extern struct ...

//==============================================

//******************************************
// FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
//
// parâmetros de entrada: 
// name - nome para a zona de memória partilhada 
// size - dimensão da memória partilhada 
//  
// retorno: 
// - endereço para a memória partilhada 
// 
void * memoria_criar(char * name, int size)
{
    //==============================================
    // CRIAR UMA ZONA DE MEMORIA PARTILHADA
    //
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);
    return so_memoria_criar(name_uid, size);
    //==============================================
}

//******************************************
// FUNÇÃO DE CRIAÇÃO DE ZONA DE MEMÓRIA PARA ARMAZENAR OS STOCKS
//
// altera:
// - Config.stock 
// 
// usa:
// - a função genérica memoria_criar(char *,int)
// - informação da variável Config 
//
void memoria_criar_stock()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O STOCK DE CADO SERVICO
    //
    so_memoria_criar_stock();
    //==============================================
}

//******************************************
// FUNÇÃO DE CRIAÇÃO DE ZONAS DE MEMÓRIA PARA OS BUFFERS E ESTRUTURAS DE APOIO
//
// altera:
// - BConclusao, BInstalacao e BServico
//
// usa:
// - a função genérica memoria_criar(char *,int)
// - informação da variável Config 
//
void memoria_criar_buffers()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS E RESPETIVAS ESTRUTURAS DE APOIO
    //
    so_memoria_criar_buffers();
    //==============================================
}

//******************************************
// FUNÇÃO DE CRIAÇÃO DE ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
//
// altera:
// - Escalonamento.ptr
//
// usa:
// - a função genérica memoria_criar(char *,int)
// - informação da variável Config 
//
void memoria_criar_escalonador()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
    //
    so_memoria_criar_escalonador();
    //==============================================
}

//******************************************
// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
//
// parâmetros de entrada: 
// name - nome para a zona de memória partilhada 
// ptr  - apontador para a zona de memória partilhada
// size - dimensão da memória partilhada 
//  
void memoria_terminar(char * name, void * ptr, int size)
{
    //==============================================
    // DESTRUIR MEMÓRIA PARTILHADA
    //
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);

    so_memoria_terminar(name_uid, ptr, size);
    //==============================================
}

//******************************************
// FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
//
// usa:
// - a função genérica memoria_terminar(char *,void *,int)
//
void memoria_destruir()
{
    //==============================================
    // DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
    //
    so_memoria_destruir();
    //==============================================
}



//******************************************
// FUNÇÃO USADA PELO CLIENTE PARA ESCREVER NO BUFFER
//
// parâmetros de entrada: 
// id - identificador do cliente 
// pServico - com os identificadores do serviço e do cliente 
// 
// funcionamento:
// - preenche alguns campos do buffer com o conteúdo de pServicos e a hora_servico.tv_sec e hora_servico.tv_nsec
//
// altera: 
// - BServico 
// 
// obs: 
// - id igual a pServico.id 
//
void memoria_pedido_s_escreve (int id, struct servico *pServico)
{
    prodcons_pedido_s_produzir_inicio();

    //==============================================
    // ESCREVER PEDIDO DE SERVIÇO NO BUFFER PEDIDO DE SERVIÇOS
    //
    so_memoria_pedido_s_escreve (id, pServico);
    //==============================================
    
    prodcons_pedido_s_produzir_fim();
    
    // informar rececionista de pedido de servico
    controlo_cliente_submete_pedido(id);

    // registar hora do pedido de servico
    tempo_registar(&BServico.buffer[BServico.ptr->in].hora_servico);
    
    // log
    ficheiro_escrever_log_ficheiro(1,id);
}

//******************************************
// FUNÇÃO USADA PELO RECECIONISTA PARA LER DO BUFFER
//
// parâmetro de entrada: 
// id - identificador do rececionista 
//
// parâmetro de saída:
// pServico - com os identificadores do serviço e do cliente 
// 
// funcionamento:
// - preenche alguns campos de pServicos com o conteúdo do buffer, o seu id e verifica se existe stock para realizar o serviço atualizando o campo "disponivel"
//
// retorno: 
// 0 - não existe stock
// 1 - existe stock
//
int memoria_pedido_s_le (int id, struct servico *pServico)
{
    // testar se existem clientes e se o SOinstala esta aberto
    if(controlo_rececionista_aguarda_pedido(id) == 0)
        return 0;

    prodcons_pedido_s_consumir_inicio();
    
    //==============================================
    // LER PEDIDO DE SERVIÇO DO BUFFER PEDIDO DE SERVIÇOS
    //
    so_memoria_pedido_s_le (id, pServico);
    //==============================================

    // testar se existe stock do servico pedido pelo cliente
    if(prodcons_atualizar_stock(pServico->id) == 0) {
        pServico->disponivel = 0;
	prodcons_pedido_s_consumir_fim();
        return 2;
    } else
        pServico->disponivel = 1;
    
    prodcons_pedido_s_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(2,id);
    
    return 1;
}

//******************************************
// FUNÇÃO USADA PELO RECECIONISTA PARA ESCREVER NO BUFFER
//
// parâmetros de entrada: 
// id - identificador do rececionista 
// pServico - com os identificadores do serviço e do cliente, hora_servico.tv_sec e hora_servico.tv_nsec 
// 
// funcionamento:
// - preenche alguns campos do buffer com o conteúdo de pServicos, o seu identificador e o identificador do instalador (indicado pelo escalonador) e a hora_instalacao.tv_sec e hora_instalacao.tv_nsec
//
// altera: 
// - BInstalacao 
//
void memoria_pedido_i_escreve (int id, struct servico *pServico)
{
    int pos,instalador;
    
    prodcons_pedido_i_produzir_inicio();
    
    // decidir a que instalador se destina
    instalador = escalonador_obter_instalador(id, pServico->id);

    //==============================================
    // ESCREVER PEDIDO NO BUFFER DE PEDIDOS DE INSTALACAO
    //
    pos = so_memoria_pedido_i_escreve (id, pServico, instalador);
    //==============================================

    prodcons_pedido_i_produzir_fim();

    // informar instalador respetivo de pedido de instalacao
    controlo_rececionista_submete_pedido(instalador);

    // registar hora pedido (instalacao)
    tempo_registar(&BConclusao.buffer[pos].hora_instalacao);
    
    // log
    ficheiro_escrever_log_ficheiro(3,id);
}

//******************************************
// FUNÇÃO USADA PELO INSTALADOR PARA LER DO BUFFER
//
// parâmetro de entrada: 
// id - identificador do instalador 
//
// parâmetro de saída:
// pServico - com os identificadores do serviço, do cliente e do rececionista, e hora_servico.tv_sec, hora_servico.tv_nsec, hora_instalacao.tv_sec, hora_instalacao.tv_nsec 
// 
// funcionamento:
// - preenche alguns campos de pServicos com o conteúdo do buffer
//
// retorno: 
// 0 - já não existem pedidos e o SOinstala encerrou
// 1 - ainda podem existir pedidos
//
int memoria_pedido_i_le (int id, struct servico *pServico)
{
    // testar se existem pedidos e se o SOinstala esta aberto
    if(controlo_instalador_aguarda_pedido(id) == 0)
        return 0;
    
    prodcons_pedido_i_consumir_inicio();
    
    //==============================================
    // LER PEDIDO DO BUFFER DE PEDIDOS DE INSTALACAO
    //
    so_memoria_pedido_i_le (id, pServico);
    //==============================================

    prodcons_pedido_i_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(4,id);
    
    return 1;
}

//******************************************
// FUNÇÃO USADA PELO INSTALADOR PARA ESCREVER NO BUFFER
//
// parâmetros de entrada: 
// id - identificador do instalador 
// pServico - com os identificadores do serviço, do cliente, do rececionista e do instalador, hora_servico.tv_sec e hora_servico.tv_nsec, hora_instalacao.tv_sec, hora_instalacao.tv_nsec 
// 
// funcionamento:
// - preenche os campos do buffer com o conteúdo de pServicos e a hora_conclusao.tv_sec e hora_conclusao.tv_nsec
//
// altera: 
// - BConclusao
//
void memoria_relatorio_c_escreve (int id, struct servico *pServico)
{
    int pos;

    prodcons_relatorio_c_produzir_inicio();

    //==============================================
    // ESCREVER RELATORIO DE CONCLUSAO NO BUFFER DE RELATORIOS DE CONCLUSAO
    //
    pos = so_memoria_relatorio_c_escreve (id, pServico);
    //==============================================

    prodcons_relatorio_c_produzir_fim();
    
    // informar cliente de que o relatorio de conclusao esta pronto
    controlo_instalador_submete_relatorio(pServico->cliente);

    // registar hora pronta (relatorio)
    tempo_registar(&BConclusao.buffer[pos].hora_conclusao);
    
    // log
    ficheiro_escrever_log_ficheiro(5,id);
}

//******************************************
// FUNÇÃO USADA PELO CLIENTE PARA LER DO BUFFER
//
// parâmetro de entrada: 
// id - identificador do cliente 
//
// parâmetro de saída:
// pServico - com os identificadores do serviço, do cliente, do rececionista e do instalador, e hora_servico.tv_sec, hora_servico.tv_nsec, hora_instalacao.tv_sec, hora_instalacao.tv_nsec, hora_conclusao.tv_sec e hora_conclusao.tv_nsec
// 
// funcionamento:
// - preenche alguns campos de pServicos com o conteúdo do buffer
//
void memoria_relatorio_c_le (int id, struct servico *pServico)
{
    int n;
    
    // aguardar relatorio
    controlo_cliente_aguarda_relatorio(pServico->cliente);
    
    prodcons_relatorio_c_consumir_inicio();
    
    //==============================================
    // LER RELATORIO DO BUFFER DE RELATORIOS DE CONCLUSAO
    //
    so_memoria_relatorio_c_le (id, pServico);
    //==============================================

    prodcons_relatorio_c_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(6,id);
}



//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memoria_criar_indicadores()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
    //
    // criação dinâmica de memória
    // para cada campo da estrutura indicadores
    // por exemplo o Ind.pid_clientes tem de ter espaço para todos os pids dos clientes 
    so_memoria_criar_indicadores();
    // iniciar indicadores relevantes, i.e. colocá-los a zero:
    // - Ind.stock_inicial
    // - Ind.clientes_atendidos_pelos_rececionistas
    // - Ind.clientes_atendidos_pelos_instaladores
    // - Ind.servicos_obtidos_pelos_clientes
    so_memoria_iniciar_indicadores();
    //==============================================
}

