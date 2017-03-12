//cd ..; make; cd bin; reset; ./soinstala ../testes/in/cenario1

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

extern struct configuracao Config;
extern struct escalonamento Escalonamento;
extern struct indicadores Ind;

struct relatorio_c BConclusao;  // buffer instalador-cliente
struct pedido_i    BInstalacao; // buffer rececionista-instalador
struct pedido_s    BServico; 	// buffer cliente-rececionista

//******************************************
// CRIAR ZONAS DE MEMORIA
//
void * memoria_criar(char * name, int size)
{

    //printf("name -> %s/ size -> %d\n", name, size);

    //==============================================
    // FUNÇÃO GENÉRICA DE CRIAÇÃO DE MEMÓRIA PARTILHADA
    //
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"/%s_%d", name, uid);
    //return so_memoria_criar(name_uid, size);
    //==============================================

	int *ptr; 
	int ret; 
	int fd = shm_open(name_uid, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR); 
	
	if(fd == -1){ 
		perror(name_uid); 	
		exit(1); 
	} 
	 
	ret = ftruncate(fd,size); 
	
	if (ret == -1){ 
		perror(name_uid); 
		exit(2); 
	} 
	
	ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
	
	if (ptr == MAP_FAILED){ 
		perror("shm-mmap"); 
		exit(3); 
	}

	return (void *) ptr; 
}

void memoria_criar_stock()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O STOCK DE CADO SERVICO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    //so_memoria_criar_stock();
    //==============================================
	Config.stock = memoria_criar("shm_stock",sizeof(int) * Config.SERVICOS); 
}
void memoria_criar_buffers()
{
    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS BUFFERS: PEDIDOS DE SERVIÇO, PEDIDOS DE INSTALAÇÃO e RELATÓRIOS DE CONCLUSÃO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    //so_memoria_criar_buffers();
    //==============================================
	
	BServico.buffer = memoria_criar("memserv", sizeof(struct servico) * Config.SERVICOS);
	BServico.ptr = memoria_criar("ptrserv", sizeof(int) * 2);
	BInstalacao.buffer = memoria_criar("memins", sizeof(struct servico) * Config.SERVICOS);
	BInstalacao.ptr = memoria_criar("ptrins", sizeof(int));
	BConclusao.buffer = memoria_criar("memconc", sizeof(struct servico) * Config.SERVICOS);
	BConclusao.ptr = memoria_criar("ptrconc", sizeof(int));
}
void memoria_criar_escalonador()
{
    //==============================================
    // CRIAR ZONA DE MEMÓRIA PARA O MAPA DE ESCALONAMENTO
    //
    // utilizar a função genérica memoria_criar(char *,int)
    //so_memoria_criar_escalonador();
    //==============================================
    Escalonamento.ptr = memoria_criar("shm_escalonador",sizeof(int) * Config.SERVICOS); 
}

void memoria_terminar(char * name, void * ptr, int size)
{
    //==============================================
    // FUNÇÃO GENÉRICA DE DESTRUIÇÃO DE MEMÓRIA PARTILHADA
    //so_memoria_terminar(name_uid, ptr, size);
    //==============================================
    int uid = getuid();
    char name_uid[strlen(name)+10];
    sprintf(name_uid,"/%s_%d", name, uid);	

    int ret;
	ret = munmap(ptr, size); 
		if(ret == -1){ 
			perror(name_uid); 
			exit(4); 
		} 

	ret = shm_unlink(name_uid); 
		if(ret == -1){ 
			perror(name_uid); 
			exit(5); 
		} 
}

//******************************************
// MEMORIA_DESTRUIR
//
void memoria_destruir()
{
    //==============================================
    // DESTRUIR MAPEAMENTO E NOME DE PÁGINAS DE MEMÓRIA
    //
    // utilizar a função genérica memoria_terminar(char *,void *,int)

    //so_memoria_destruir(); //Sabeer onde estao as zonas de memoria armazenadas
    //==============================================
    memoria_terminar("shm_stock", Config.stock,sizeof(int) * Config.SERVICOS);
    memoria_terminar("shm_escalonador", Escalonamento.ptr, sizeof(int) * Config.SERVICOS);
    memoria_terminar("memserv", BServico.buffer,sizeof(struct servico) * Config.SERVICOS);
    memoria_terminar("memins", BInstalacao.buffer,sizeof(struct servico) * Config.SERVICOS);
    memoria_terminar("memconc", BConclusao.buffer,sizeof(struct servico) * Config.SERVICOS);
    memoria_terminar("ptrserv", BServico.ptr,sizeof(int) * 2);
    memoria_terminar("ptrins",BInstalacao.ptr,sizeof(int) * 2);
    memoria_terminar("ptrconc", BConclusao.ptr,sizeof(int) * 2);

    free(Ind.stock_inicial);
    free(Ind.pid_clientes);
    free(Ind.pid_rececionistas);
    free(Ind.pid_instaladores);
    free(Ind.clientes_atendidos_pelos_rececionistas);
    free(Ind.clientes_atendidos_pelos_instaladores);
    free(Ind.servicos_obtidos_pelos_clientes);
    memoria_terminar("serv_inst",Ind.servicos_realizados_pelos_instaladores,sizeof(int) * Config.CLIENTES);
}



//******************************************
// MEMORIA_PEDIDO_S_ESCREVE
//
void memoria_pedido_s_escreve (int id, struct servico *pServico)
{
    prodcons_pedido_s_produzir_inicio();
	
    //==============================================
    // ESCREVER PEDIDO DE SERVIÇO NO BUFFER PEDIDO DE SERVIÇOS
    //
    //so_memoria_pedido_s_escreve (id, pServico);
    //==============================================
  
    pServico->cliente = id;   
    BServico.buffer[BServico.ptr->in] = *pServico;
    
    int num = Config.BUFFER_SERVICO - BServico.ptr->in + 1;
    BServico.ptr->in = ((BServico.ptr->in) + 1) % num;
    
    prodcons_pedido_s_produzir_fim();
    
    // informar rececionista de pedido de servico
    controlo_cliente_submete_pedido(id);

    // registar hora do pedido de servico
    tempo_registar(&BServico.buffer[BServico.ptr->in].hora_servico);
    
    // log
    ficheiro_escrever_log_ficheiro(1,id);
}
//******************************************
// MEMORIA_PEDIDO_S_LE
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
    //so_memoria_pedido_s_le (id, pServico);
    //==============================================

    pServico->rececionista = id;   
    *pServico = BServico.buffer[BServico.ptr->out];
    
    int num = Config.BUFFER_SERVICO - BServico.ptr->out + 1;
    BServico.ptr->out = ((BServico.ptr->out) + 1) % num;

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
// MEMORIA_PEDIDO_I_ESCREVE
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
    //pos = so_memoria_pedido_i_escreve (id, pServico, instalador);
    //==============================================

	pServico->instalador = instalador;	

	for(pos = 0; pos < Config.BUFFER_INSTALACAO; pos++){
		
		if(BInstalacao.ptr[pos] == 0){
			
			BInstalacao.buffer[pos] = *pServico; 
			BInstalacao.ptr[pos] = 1;			
			break;
		}
	}	

    prodcons_pedido_i_produzir_fim();

    // informar instalador respetivo de pedido de instalacao
    controlo_rececionista_submete_pedido(instalador);

    // registar hora pedido (instalacao)
    tempo_registar(&BConclusao.buffer[pos].hora_instalacao);
    
    // log
    ficheiro_escrever_log_ficheiro(3,id);
}
//******************************************
// MEMORIA_PEDIDO_LE
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
    //so_memoria_pedido_i_le (id, pServico);
    //==============================================

	int i;	

	for(i = 0; i < Config.BUFFER_INSTALACAO; i++){
		
		if(BInstalacao.ptr[i] == 1){

			if(BInstalacao.buffer[i].instalador == id){

				*pServico = BInstalacao.buffer[i]; 
				BInstalacao.ptr[i] = 0;			
				break;
			}
		}
	}	
    prodcons_pedido_i_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(4,id);
    
    return 1;
}



//******************************************
// MEMORIA_RELATORIO_C_ESCREVE
//
void memoria_relatorio_c_escreve (int id, struct servico *pServico)
{
    int pos;

    prodcons_relatorio_c_produzir_inicio();

    //==============================================
    // ESCREVER RELATORIO DE CONCLUSAO NO BUFFER DE RELATORIOS DE CONCLUSAO
    //
    //pos = so_memoria_relatorio_c_escreve (id, pServico);
    //==============================================

	for(pos = 0; pos < Config.BUFFER_CONCLUSAO; pos++){
		
		if(BConclusao.ptr[pos] == 0){
			
			BConclusao.buffer[pos] = *pServico; 
			BConclusao.ptr[pos] = 1;			
			break;
		}
	}	


    prodcons_relatorio_c_produzir_fim();
    
    // informar cliente de que o relatorio de conclusao esta pronto
    controlo_instalador_submete_relatorio(pServico->cliente);

    // registar hora pronta (relatorio)
    tempo_registar(&BConclusao.buffer[pos].hora_conclusao);
    
    // log
    ficheiro_escrever_log_ficheiro(5,id);
}
//******************************************
// MEMORIA_RELATORIO_C_LE
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
    //so_memoria_relatorio_c_le (id, pServico);
    //==============================================

	int i;	

	for(i = 0; i < Config.BUFFER_CONCLUSAO; i++){
		
		if(BConclusao.ptr[i] == 1){

				*pServico = BConclusao.buffer[i]; 
				BConclusao.ptr[i] = 0;			
				break;
			}
	}	

    prodcons_relatorio_c_consumir_fim();

    // log
    ficheiro_escrever_log_ficheiro(6,id);
}



//******************************************
// MEMORIA_CRIAR_INDICADORES
//
void memoria_criar_indicadores()
{
   
	Ind.stock_inicial = malloc(sizeof(int) * Config.SERVICOS);
	Ind.pid_clientes = malloc(sizeof(int) * Config.CLIENTES);
	Ind.pid_rececionistas = malloc(sizeof(int) * Config.RECECIONISTAS);
	Ind.pid_instaladores = malloc(sizeof(int) * Config.INSTALADORES);
	Ind.clientes_atendidos_pelos_rececionistas = calloc(sizeof(int), Config.CLIENTES);
	Ind.clientes_atendidos_pelos_instaladores = calloc(sizeof(int), Config.CLIENTES);
	Ind.servicos_realizados_pelos_instaladores = memoria_criar("serv_inst", sizeof(int) * Config.CLIENTES);
	Ind.servicos_obtidos_pelos_clientes = calloc(sizeof(int), Config.CLIENTES);
   

    //==============================================
    // CRIAR ZONAS DE MEMÓRIA PARA OS INDICADORES
    //
    // criação dinâmica de memória
    // para cada campo da estrutura indicadores
    //so_memoria_criar_indicadores();
    // iniciar indicadores relevantes:
    // - Ind.stock_inicial
    // - Ind.clientes_atendidos_pelos_rececionistas
    // - Ind.clientes_atendidos_pelos_instaladores
    // - Ind.servicos_obtidos_pelos_clientes
    //so_memoria_iniciar_indicadores();
    //==============================================

    int i;
	int * aux = Config.stock;

	for(i= 0; i<Config.SERVICOS; i++)
		Ind.stock_inicial[i] = aux[i];
}

