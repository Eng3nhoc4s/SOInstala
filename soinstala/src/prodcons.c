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


//==============================================
// SO trabalho2
// grupo so046
//==============================================

struct prodcons ProdCons;
extern struct configuracao Config;

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
	
	return sem_open(name_uid, O_CREAT|O_TRUNC, 0xFFFFFFFF, value);
    //return so_semaforo_criar(name_uid, value);
    //==============================================
}
void prodcons_criar_stock()
{
    //==============================================
    // CRIAR MUTEX PARA CONTROLAR O ACESSO AO STOCK
    //
    // utilizar a função genérica semaforo_criar para criar o semáforo:
    // - mutex
	ProdCons.stock_mutex = semaforo_criar("stock_mutex", 1);
    //so_prodcons_criar_stock();
    //==============================================
}
void prodcons_criar_buffers()
{
    //==============================================
    // CRIAR SEMAFOROS PARA CONTROLAR O ACESSO AOS 3 BUFFERS
    //
    // utilizar a função genérica semaforo_criar para criar os semáforos:
    // - empty, full e mutex (para cada buffer)

	ProdCons.relatorios_c_full = semaforo_criar("relatorios_c_full", 0);
	ProdCons.relatorios_c_empty = semaforo_criar("relatorios_c_empty", Config.BUFFER_CONCLUSAO);
	ProdCons.relatorios_c_mutex = semaforo_criar("relatorios_c_mutex", 1);

	ProdCons.pedido_i_full = semaforo_criar("pedido_i_full", 0);
	ProdCons.pedido_i_empty = semaforo_criar("pedido_i_empty", Config.BUFFER_INSTALACAO);
	ProdCons.pedido_i_mutex = semaforo_criar("pedido_i_mutex", 1);

	ProdCons.pedido_s_full = semaforo_criar("pedido_s_full", 0);
	ProdCons.pedido_s_empty = semaforo_criar("pedido_s_empty", Config.BUFFER_SERVICO);
	ProdCons.pedido_s_mutex = semaforo_criar("pedido_s_mutex", 1);

    //so_prodcons_criar_buffers();
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


	sem_close(ptr);
	sem_unlink(name_uid);

    //so_semaforo_terminar(name_uid, ptr);
    //==============================================
}

void prodcons_destruir()
{
    //==============================================
    // DESTRUIR TODOS OS SEMÁFOROS
    //
    // utilizar a função genérica semaforo_terminar

	semaforo_terminar("stock_mutex", ProdCons.stock_mutex);

	semaforo_terminar("relatorios_c_full", ProdCons.relatorios_c_full);
	semaforo_terminar("relatorios_c_empty", ProdCons.relatorios_c_empty);
	semaforo_terminar("relatorios_c_mutex", ProdCons.relatorios_c_mutex);

	semaforo_terminar("pedido_i_full", ProdCons.pedido_i_full);
	semaforo_terminar("pedido_i_empty", ProdCons.pedido_i_empty);
	semaforo_terminar("pedido_i_mutex", ProdCons.pedido_i_mutex);

	semaforo_terminar("pedido_s_full", ProdCons.pedido_s_full);
	semaforo_terminar("pedido_s_empty", ProdCons.pedido_s_empty);
	semaforo_terminar("pedido_s_mutex", ProdCons.pedido_s_mutex);

    //so_prodcons_destruir();
    //==============================================
}


//******************************************
void prodcons_pedido_s_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //

	if (sem_wait(ProdCons.pedido_s_empty)==-1) {
		perror("sem wait -> empty");
		exit(-1);
	}
	if(sem_wait(ProdCons.pedido_s_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}

    //so_prodcons_pedido_s_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE SERVIÇO
    //


	if(sem_post(ProdCons.pedido_s_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.pedido_s_full)==-1){
		perror("sem post -> full");
		exit(-1); 
	}	



    //so_prodcons_pedido_s_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //

	if (sem_wait(ProdCons.pedido_s_full)==-1) {
		perror("sem wait -> full");
		exit(-1);
	}
	if(sem_wait(ProdCons.pedido_s_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}

    //so_prodcons_pedido_s_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_s_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE SERVIÇO
    //


	if(sem_post(ProdCons.pedido_s_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.pedido_s_empty)==-1){
		perror("sem post -> empty");
		exit(-1); 
	}	


    //so_prodcons_pedido_s_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //

	if (sem_wait(ProdCons.pedido_i_empty)==-1) {
		perror("sem wait -> empty");
		exit(-1);
	}
	if(sem_wait(ProdCons.pedido_i_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}

    //so_prodcons_pedido_i_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER PEDIDO DE INSTALACAO
    //


	if(sem_post(ProdCons.pedido_i_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.pedido_i_full)==-1){
		perror("sem post -> full");
		exit(-1); 
	}

    //so_prodcons_pedido_i_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //

	if (sem_wait(ProdCons.pedido_i_full)==-1) {
		perror("sem wait -> full");
		exit(-1);
	}
	if(sem_wait(ProdCons.pedido_i_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}

    //so_prodcons_pedido_i_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_pedido_i_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER PEDIDO DE INSTALACAO
    //

	if(sem_post(ProdCons.pedido_i_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.pedido_i_empty)==-1){
		perror("sem post -> empty");
		exit(-1); 
	}	


    //so_prodcons_pedido_i_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //

	if (sem_wait(ProdCons.relatorios_c_empty)==-1) {
		perror("sem wait -> empty");
		exit(-1);
	}
	if(sem_wait(ProdCons.relatorios_c_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}


    //so_prodcons_relatorio_c_produzir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_produzir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE PRODUTOR AO BUFFER RELATORIO DE CONCLUSAO
    //


	if(sem_post(ProdCons.relatorios_c_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.relatorios_c_full)==-1){
		perror("sem post -> full");
		exit(-1); 
	}

    //so_prodcons_relatorio_c_produzir_fim();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_inicio()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //

	if (sem_wait(ProdCons.relatorios_c_full)==-1) {
		perror("sem wait -> full");
		exit(-1);
	}
	if(sem_wait(ProdCons.relatorios_c_mutex)==-1){
		perror("sem wait -> mutex");
		exit(-1);
	}

    //so_prodcons_relatorio_c_consumir_inicio();
    //==============================================
}
//******************************************
void prodcons_relatorio_c_consumir_fim()
{
    //==============================================
    // CONTROLAR ACESSO DE CONSUMIDOR AO BUFFER RELATORIO DE CONCLUSAO
    //
	
	if(sem_post(ProdCons.relatorios_c_mutex)==-1){
		perror("sem post -> mutex");
		exit(-1);
	}
	if (sem_post(ProdCons.relatorios_c_empty)==-1){
		perror("sem post -> empty");
		exit(-1); 
	}

    //so_prodcons_relatorio_c_consumir_fim();
    //==============================================
}
//******************************************
void prodcons_buffers_inicio()
{
    //==============================================
    // CONTROLAR ACESSO AOS 3 BUFFERS
    //

	if (sem_wait(ProdCons.pedido_s_mutex)==-1) {
		perror("sem wait -> pedido_s");
		exit(-1);
	}

	if (sem_wait(ProdCons.pedido_i_mutex)==-1) {
		perror("sem wait -> pedido_i");
		exit(-1);
	}

	if (sem_wait(ProdCons.relatorios_c_mutex)==-1) {
		perror("sem wait -> relatorios_c");
		exit(-1);
	}
    //so_prodcons_buffers_inicio();
    //==============================================
}
//******************************************
void prodcons_buffers_fim()
{
    //==============================================
    // CONTROLAR ACESSO AOS 3 BUFFERS
    //

	if(sem_post(ProdCons.pedido_s_mutex)==-1){
		perror("sem post -> pedido_s");
		exit(-1);
	}

	if (sem_post(ProdCons.pedido_i_mutex)==-1) {
		perror("sem post -> pedido_i");
		exit(-1);
	}

	if (sem_post(ProdCons.relatorios_c_mutex)==-1) {
		perror("sem post -> relatorios_c");
		exit(-1);
	}

    //so_prodcons_buffers_fim();
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

	sem_wait(ProdCons.stock_mutex);

	if(Config.stock[servico]>0){
		Config.stock[servico]--;
		sem_post(ProdCons.stock_mutex);	
		return 1;
	}else{
		sem_post(ProdCons.stock_mutex);
		return 0;
	}	
		
    //==============================================
}
