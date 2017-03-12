#ifndef MEMORIA_H_GUARD
#define MEMORIA_H_GUARD

// estrutura que contêm os ponteiros para um buffer circular
struct ponteiros {
    int in;  // ponteiro de escrita
    int out; // ponteiro de leitura
};

// estrutura onde são guardadas os pedidos de servicos do cliente para o rececionista
struct pedido_s {
    struct servico *buffer;   // ponteiro para a lista de pedidos de servico
    struct ponteiros *ptr;  // ponteiro para a estrutura de índices de escrita e leitura
};

// estrutura onde são guardados os pedidos de instalacao do rececionista para o instalador
struct pedido_i {
    struct servico *buffer; // ponteiro para a lista de pedidos de instalacao
    int *ptr;             // ponteiro para a lista de inteiros que indicam
                          // se a posição respetiva está livre ou ocupada
};

// estrutura onde o instalador disponibiliza os relatorios de conclusao
struct relatorio_c {
    struct servico *buffer; // ponteiro para a lista de relatorios de instalacao
    int *ptr;             // ponteiro para a lista de inteiros que indicam
                          // se a posição respetiva está livre ou ocupada
};

void * memoria_criar(char*,int);
void memoria_criar_stock();
void memoria_criar_buffers();
void memoria_criar_escalonador();
void memoria_destruir();
void memoria_pedido_s_escreve (int,struct servico *);
int  memoria_pedido_s_le (int,struct servico *);
void memoria_pedido_i_escreve (int,struct servico *);
int  memoria_pedido_i_le (int,struct servico *);
void memoria_relatorio_c_escreve (int,struct servico *);
void memoria_relatorio_c_le (int,struct servico *);
void memoria_criar_indicadores();

#endif
