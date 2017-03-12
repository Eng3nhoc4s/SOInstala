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
#include "controlo.h"
#include "prodcons.h"

struct prodcons ProdCons;

//==============================================
// Declarar o acesso a eventuais variáveis ou estruturas externas
// ex: extern struct ...

//==============================================

//******************************************
// FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
//
// parâmetros de entrada: 
// name - nome para o semáforo
// value - valor inicial
//  
// retorno: 
// - apontador para o semáforo
// 
sem_t * semaforo_criar(char * name, int value)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE UM SEMÁFORO
    //
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"%s_%d", name, uid);

    return so_semaforo_criar(name_uid, value);
    //==============================================
}
void prodcons_criar_stock()
{
    //==============================================
    // CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
    //
    // utilizar a função genérica semaforo_criar para criar o semáforo:
    // - mutex
    so_prodcons_criar_stock();
    //==============================================
}
void prodcons_criar_buffers()
{
    //==============================================
    // CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
    //
    // utilizar a função genérica semaforo_criar para criar os semáforos:
    // - empty, full e mutex (para cada buffer)
    so_prodcons_criar_buffers();
    //==============================================
}
void semaforo_terminar(char * name, void * ptr)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE UM SEMÁFORO E RESPETIVO NOME
    //
    int uid ;
    char name_uid[strlen(name)+10];
    uid=getuid();
    sprintf(name_uid,"%s_%d", name, uid);
    so_semaforo_terminar(name_uid, ptr);
    //==============================================
}

void prodcons_destruir()
{
    //==============================================
    // DESTRUIR TODOS OS SEMÁFOROS
    //
    // utilizar a função genérica semaforo_terminar
    so_prodcons_destruir();
    //==============================================
}


//******************************************
void prodcons_pedido_s_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //
    so_prodcons_pedido_s_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //
    so_prodcons_pedido_i_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //
    so_prodcons_relatorio_c_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_buffers_inicio()
{
    //==============================================
    // INICIAR ACESSO EXCLUSIVO AOS 3 BUFFERS EM SIMULTÂNEO
    //
    so_prodcons_buffers_inicio();
    //==============================================
}
//******************************************
void prodcons_buffers_fim()
{
    //==============================================
    // TERMINAR ACESSO EXCLUSIVO AOS 3 BUFFERS EM SIMULTÂNEO
    //
    so_prodcons_buffers_fim();
    //==============================================
}
//******************************************
// FUNÇÃO DE ATUALIZAÇÃO DO STOCK
//
// parâmetros de entrada: 
// servico - id do serviço a atualizar
//  
// retorno: 
// 0 - não existe stock
// 1 - existe stock
// 
int prodcons_atualizar_stock(int servico)
{
    //==============================================
    // OBTER MUTEX DO STOCK E ATUALIZAR STOCK
    //
    // se stock de servico>0 então reduzi-lo de uma unidade
    return so_prodcons_atualizar_stock(servico);
    //==============================================
}
