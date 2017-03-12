#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

// estrutura que representa uma servico nas tres etapas: pedido do servico, pedido da instalacao e relatorio de conclusao
struct servico {
    int id;  		            	// identificador do tipo de servico pedido
    int disponivel;                 // stock: 0 - indisponivel, 1 - disponivel
    int cliente;     	            // id do cliente que solicitou o servico
    int rececionista;    	        // id do rececionista que atendeu o cliente
    int instalador;  	            // id do instalador que efetuou o servico
    struct timespec hora_servico; 	// hora a que o servico foi pedido ao rececionista (registada pelo cliente)
    struct timespec hora_instalacao;// hora a que a instalacao foi pedida ao instalador (registada pelo rececionista)
    struct timespec hora_conclusao; // hora a que o relatorio foi produzido pelo instalador (registada pelo instalador)
};

// estrutura com valores dos parâmetros do ficheiro de configuracao
struct configuracao {
    char* lista_servicos;      	// linha com stock de cada servico
    char* lista_clientes;    	// linha com servico pretendido por cada cliente
    char* lista_rececionistas;  // linha com os rececionistas (nomes nao interessam)
    char* lista_instaladores; 	// linha com as especialidades por instalador separadas por virgula
    char* lista_buffers;     	// linha com capacidade dos tres buffers
    int   SERVICOS;            	// nº de servicos disponiveis
    int   CLIENTES;          	// nº de clientes
    int   RECECIONISTAS;        // nº de rececionistas
    int   INSTALADORES;       	// nº de instaladores
    int   BUFFER_SERVICO; 		// capacidade do buffer de pedidos de servico
    int   BUFFER_INSTALACAO;    // capacidade do buffer de pedidos de instalacao
    int   BUFFER_CONCLUSAO;     // capacidade do buffer de relatorios de conclusao
    int*  stock;             	// vetor com capacidade por tipo de servico
};

// estrutura de registo de indicadores de funcionamento do SOinstala
struct indicadores {
    int *stock_inicial;
    int *pid_clientes;
    int *pid_rececionistas;
    int *pid_instaladores;
    int *clientes_atendidos_pelos_rececionistas;// vetor de rececionistas
    int *clientes_atendidos_pelos_instaladores;	// vetor de instaladores
    int *servicos_realizados_pelos_instaladores;// vetor de instaladores
    int *servicos_obtidos_pelos_clientes; 		// vetor de servicos
};


#endif
