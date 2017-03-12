#ifndef SO_H_GUARD
#define SO_H_GUARD

void so_escreve_indicadores();

/* inih -- simple .INI file parser

   inih is released under the New BSD license (see LICENSE.txt). Go to the project
   home page for more info:
   http://code.google.com/p/inih/


   Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error, or -1 on
   file open error.
*/
int ini_parse(const char* filename,
              int (*handler)(void* user, const char* section,
                             const char* name, const char* value),
              void* user);

/* Nonzero to allow multi-line value parsing, in the style of Python's
   ConfigParser. If allowed, ini_parse() will call the handler with the same
   name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE 1
#endif

//***********************************
// Funções de apoio ao desenvolvimento do trabalho
//***********************************
// MAIN
//
void so_main_rececionista(int);
void so_main_instalador(int);
void so_main_cliente(int);
long so_main_args(int, char**, char **, char **, char **);
void so_main_wait_clientes();
void so_main_wait_rececionistas();
void so_main_wait_instaladores();
//***********************************
// ESCALONADOR
//
void so_escalonador_iniciar (int, int);
int  so_escalonador_obter_instalador(int, int);
//***********************************
// MEMORIA
//
void *so_memoria_criar(char *, int);
void  so_memoria_criar_stock();
void  so_memoria_criar_buffers();
void  so_memoria_criar_escalonador();
void  so_memoria_terminar(char *, void *, int);
void  so_memoria_destruir();
void  so_memoria_pedido_s_escreve (int, struct servico *);
void  so_memoria_pedido_s_le (int, struct servico *);
int   so_memoria_pedido_i_escreve (int, struct servico *, int);
void  so_memoria_pedido_i_le (int, struct servico *);
int   so_memoria_relatorio_c_escreve (int, struct servico *);
void  so_memoria_relatorio_c_le (int, struct servico *);
void  so_memoria_criar_indicadores();
void  so_memoria_iniciar_indicadores();
//***********************************
// PRODCONS
//
sem_t *so_semaforo_criar(char *, int);
void   so_prodcons_criar_stock();
void   so_prodcons_criar_buffers();
void   so_semaforo_terminar(char *, sem_t *);
void   so_prodcons_destruir();
void   so_prodcons_pedido_s_produzir_inicio();
void   so_prodcons_pedido_s_produzir_fim();
void   so_prodcons_pedido_s_consumir_inicio();
void   so_prodcons_pedido_s_consumir_fim();
void   so_prodcons_pedido_i_produzir_inicio();
void   so_prodcons_pedido_i_produzir_fim();
void   so_prodcons_pedido_i_consumir_inicio();
void   so_prodcons_pedido_i_consumir_fim();
void   so_prodcons_relatorio_c_produzir_inicio();
void   so_prodcons_relatorio_c_produzir_fim();
void   so_prodcons_relatorio_c_consumir_inicio();
void   so_prodcons_relatorio_c_consumir_fim();
void   so_prodcons_buffers_inicio();
void   so_prodcons_buffers_fim();
int    so_prodcons_atualizar_stock(int);
//***********************************
// CONTROLO
//
void so_cond_criar(pthread_cond_t *, pthread_mutex_t *);
void so_controlo_criar();
void so_controlo_criar_abertura();
void so_controlo_criar_acordar_cliente();
void so_controlo_criar_acordar_instalador();
void so_controlo_criar_acordar_rececionista();
void so_cond_terminar(pthread_cond_t *, pthread_mutex_t *);
void so_controlo_destruir();
void so_controlo_destruir_abertura();
void so_controlo_destruir_acordar_cliente();
void so_controlo_destruir_acordar_instalador();
void so_controlo_destruir_acordar_rececionista();
void so_controlo_abrir_soinstala();
void so_controlo_fechar_soinstala();
void so_controlo_cliente_submete_pedido(int);
int  so_controlo_rececionista_aguarda_pedido(int);
void so_controlo_rececionista_submete_pedido(int);
int  so_controlo_instalador_aguarda_pedido(int);
void so_controlo_instalador_submete_relatorio(int);
void so_controlo_cliente_aguarda_relatorio(int);
//***********************************
// FICHEIRO
//
void so_ficheiro_iniciar(char *,char *,char *);
void so_ficheiro_destruir();
void so_ficheiro_abrir_ficheiro_entrada();
void so_ficheiro_contar_servicos();
void so_ficheiro_ler_stock();
void so_ficheiro_contar_clientes();
void so_ficheiro_contar_rececionistas();
void so_ficheiro_contar_instaladores();
void so_ficheiro_ler_especialidades();
void so_ficheiro_ler_capacidades();
void so_ficheiro_abrir_ficheiro_saida();
void so_ficheiro_abrir_ficheiro_log();
void so_ficheiro_escrever_log_ficheiro(int,int,double);
void so_ficheiro_escrever_linha(char *);
//***********************************
// TEMPO
//
void   so_tempo_iniciar(long);
void   so_tempo_terminar(long);
void   so_tempo_armar_alarme();
void   so_tempo_escrever_log_temporizado(int);
double so_tempo_diferenca(struct timespec,struct timespec);
double so_tempo_ate_agora();
void   so_tempo_registar(struct timespec *);
void   so_tempo_instalacao();

#endif
