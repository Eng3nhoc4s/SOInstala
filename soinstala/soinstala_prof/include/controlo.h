#ifndef CONTROLO_H_GUARD
#define CONTROLO_H_GUARD

// estrutura de condições para controlar o encerramento do SOinstala
struct controlo {
    pthread_mutex_t rececionistas_mutex;
    pthread_cond_t  rececionistas_cond;
    pthread_mutex_t instaladores_mutex;
    pthread_cond_t  instaladores_cond;
    // semaforo para o cliente informar os rececionistas de uma encomenda
    sem_t *rececionista_pedido;
    // semaforos para o rececionista informar o instalador de um pedido
    sem_t **instalador_pedido;
    // semaforos para o instalador informar o cliente de um relatorio
    sem_t **cliente_relatorio;
    int aberto;  // 0: se SOinstala fechado, 1: se SOinstala aberto
};


void controlo_criar();
void controlo_destruir();
void controlo_lock();
void controlo_unlock();
void controlo_aguarda_pedido();
void controlo_acorda_instaladores();
void controlo_abrir_soinstala();
void controlo_fechar_soinstala();

void controlo_cliente_submete_pedido(int);
int  controlo_rececionista_aguarda_pedido(int);
void controlo_rececionista_submete_pedido(int);
int  controlo_instalador_aguarda_pedido(int);
void controlo_instalador_submete_relatorio(int);
void controlo_cliente_aguarda_relatorio(int);

#endif
