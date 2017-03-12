#ifndef PRODCONS_H_GUARD
#define PRODCONS_H_GUARD

struct prodcons {
    // semáforos de controlo do acesso ao buffer entre instaladores e clientes
    sem_t *relatorios_c_full, *relatorios_c_empty, *relatorios_c_mutex;
    // semáforos de controlo do acesso ao buffer entre rececionistas e instaladores
    sem_t *pedido_i_full, *pedido_i_empty, *pedido_i_mutex;
    // semáforos de controlo do acesso ao buffer entre clientes e rececionistas
    sem_t *pedido_s_full, *pedido_s_empty, *pedido_s_mutex;
    // semaforo para exclusao mutua no acesso ao stock
    sem_t *stock_mutex;
};

sem_t * semaforo_criar(char*,int);
void prodcons_criar_acordar();
void prodcons_criar_stock();
void prodcons_criar_buffers();
void prodcons_criar_clientes();
void prodcons_terminar(char*,void*);
void prodcons_destruir();
void prodcons_pedido_s_produzir_inicio();
void prodcons_pedido_s_produzir_fim();
void prodcons_pedido_s_consumir_inicio();
void prodcons_pedido_s_consumir_fim();
void prodcons_pedido_i_produzir_inicio();
void prodcons_pedido_i_produzir_fim();
void prodcons_pedido_i_consumir_inicio();
void prodcons_pedido_i_consumir_fim();
void prodcons_relatorio_c_produzir_inicio();
void prodcons_relatorio_c_produzir_fim();
void prodcons_relatorio_c_consumir_inicio();
void prodcons_relatorio_c_consumir_fim();
void prodcons_buffers_inicio();
void prodcons_buffers_fim();
int  prodcons_atualizar_stock(int);

#endif
