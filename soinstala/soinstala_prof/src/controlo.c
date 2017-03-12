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

struct controlo *Controlo;          // controlo da abertura do SOinstala

void cond_criar(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE UMA CONDIÇÃO PARTILHÁVEL ENTRE PROCESSOS
    //
    so_cond_criar(cond, mutex);
    //==============================================
}

void controlo_criar()
{
    //==============================================
    // CRIAÇÃO DE ZONA DE MEMÓRIA PARTILHADA
    //
    // Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
    so_controlo_criar();
    //==============================================

    //==============================================
    // CRIAÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SOinstala
    //
    // Criar e iniciar variaveis de condicao e respetivos mutexes do Controlo
    so_controlo_criar_abertura();
    //==============================================

    //==============================================
    // CRIAÇÃO DE MECANISMOS PARA INSTALADOR ACORDAR CLIENTE A QUE SE DESTINA O RELATORIO DE CONCLUSAO
    //
    // Criar e iniciar semáforos
    so_controlo_criar_acordar_cliente();
    //==============================================

    //==============================================
    // CRIAÇÃO DE MECANISMOS PARA RECECIONISTA ACORDAR INSTALADOR A QUE SE DESTINA O PEDIDO DA INSTALACAO
    //
    // Criar e iniciar semáforos
    so_controlo_criar_acordar_instalador();
    //==============================================

    //==============================================
    // CRIAÇÃO DE MECANISMOS PARA CLIENTE ACORDAR RECECIONISTAS A QUE SE DESTINA O PEDIDO DE SERVICO
    //
    // Criar e iniciar semáforos
    so_controlo_criar_acordar_rececionista();
    //==============================================

}

void cond_terminar(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE VARIÁVEIS DE CONDIÇÃO
    //
    so_cond_terminar(cond, mutex);
    //==============================================
}

void controlo_destruir()
{
    //==============================================
    // DESTRUIÇÃO DE MECANISMOS DE CONTROLO DE ABERTURA DO SOinstala
    //
    // utilizar a função genérica cond_terminar
    so_controlo_destruir_abertura();
    //==============================================

    //==============================================
    // DESTRUIR SEMÁFOROS E RESPETIVOS NOMES
    //
    // utilizar a função genérica prodcons_terminar
    so_controlo_destruir_acordar_cliente();
    so_controlo_destruir_acordar_instalador();
    so_controlo_destruir_acordar_rececionista();
    //==============================================

    //==============================================
    // DESTRUIR ZONA DE MEMÓRIA
    //
    // utilizar a função genérica memoria_terminar
    so_controlo_destruir();
    //==============================================
}

//***************************************************
// ABERTURA DO SOinstala
//
void controlo_abrir_soinstala()
{
    //==============================================
    // ABRIR SOinstala
    //
    so_controlo_abrir_soinstala();
    //==============================================
}

void controlo_fechar_soinstala()
{
    //==============================================
    // FECHAR SOinstala
    //
    so_controlo_fechar_soinstala();
    //==============================================
}

//***************************************************
// CONTROLO ENTRE CLIENTES/RECECIONISTAS/INSTALADORES
//
void controlo_cliente_submete_pedido(int id)
{
    //==============================================
    // SINALIZAR RECECIONISTA DA SUBMISSÃO DE PEDIDO DE SERVIÇO
    //
    so_controlo_cliente_submete_pedido(id);
    //==============================================
}
//******************************************
int controlo_rececionista_aguarda_pedido(int id)
{
    //==============================================
    // RECECIONISTA AGUARDA SUBMISSÃO DE PEDIDO DE SERVIÇO
    //
    // verificar também se SOinstala está aberto
    return so_controlo_rececionista_aguarda_pedido(id);
    //==============================================
}
//******************************************
void controlo_rececionista_submete_pedido(int id)
{
    //==============================================
    // SINALIZAR INSTALADOR DA SUBMISSÃO DE PEDIDO DE INSTALAÇÃO
    //
    so_controlo_rececionista_submete_pedido(id);
    //==============================================
}
//******************************************
int controlo_instalador_aguarda_pedido(int id)
{
    //==============================================
    // INSTALADOR AGUARDA SUBMISSÃO DE PEDIDO DE INSTALAÇÃO
    //
    // verificar também se SOinstala está aberto
    return so_controlo_instalador_aguarda_pedido(id);
    //==============================================
}
//******************************************
void controlo_instalador_submete_relatorio(int id)
{
    //==============================================
    // SINALIZAR INSTALADOR DA SUBMISSÃO DE RELATÓRIO DE CONCLUSÃO
    //
    so_controlo_instalador_submete_relatorio(id);
    //==============================================
}
//******************************************
void controlo_cliente_aguarda_relatorio(int id)
{
    //==============================================
    // CLIENTE AGUARDA SUBMISSÃO DE RELATÓRIO DE CONCLUSÃO
    //
    so_controlo_cliente_aguarda_relatorio(id);
    //==============================================
}
