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
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "memoria.h"
#include "escalonador.h"
#include "ficheiro.h"

struct ficheiros Ficheiros; // informação sobre nomes e handles de ficheiros

extern struct configuracao Config; // configuração da execução do SOinstala

void ficheiro_iniciar(char *fic_entrada, char *fic_saida, char *fic_log)
{
    //==============================================
    // GUARDAR NOMES DOS FICHEIROS NA ESTRUTURA Ficheiros
    //
	// utilizar a função strdup
    so_ficheiro_iniciar(fic_entrada,fic_saida,fic_log);
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE ENTRADA
    //
    so_ficheiro_abrir_ficheiro_entrada();
    //==============================================

    // parse do ficheiro de configuração
    // esta funcao carrega os campos da estrutura Config (char *)
    // com os dados do ficheiro de entrada
    if (ini_parse_file(Ficheiros.h_entrada, handler, &Config) < 0) {
        printf("Erro a carregar o ficheiro de configuracao!'\n");
        exit(1);
    }

    // agora e' preciso inicializar os restantes campos da estrutura Config

    //==============================================
    // CONTAR SERVICOS
    //
    so_ficheiro_contar_servicos();
    //==============================================

    // iniciar memoria para o vetor com o stock por servico e semaforo
    memoria_criar_stock();
    prodcons_criar_stock();

    //==============================================
    // LER STOCK DE CADO SERVICO
    //
    so_ficheiro_ler_stock();
    //==============================================

    //==============================================
    // CONTAR CLIENTES
    //
	// usar strtok
	//
    so_ficheiro_contar_clientes();
    //==============================================

    //==============================================
    // CONTAR RECECIONISTAS
    //
	// usar strtok
	//
    so_ficheiro_contar_rececionistas();
    //==============================================

    //==============================================
    // CONTAR INSTALADORES
    //
	// usar strtok
	//
    so_ficheiro_contar_instaladores();
    //==============================================

	
    so_ficheiro_ler_especialidades();

	
    //==============================================
    // LER CAPACIDADES DOS BUFFERS
    //
    so_ficheiro_ler_capacidades();
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE SAIDA (se foi especificado)
    //
    so_ficheiro_abrir_ficheiro_saida();
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE LOG (se foi especificado)
    //
    so_ficheiro_abrir_ficheiro_log();
    //==============================================
}

void ficheiro_destruir()
{
    //==============================================
    // DESTRUIR ZONAS DE MEMÓRIA RESERVADAS DINAMICAMENTE
    //
	// estas zonas foram criadas com strdup por so_ficheiro_iniciar
    so_ficheiro_destruir();
    //==============================================
}

void ficheiro_escrever_log_ficheiro(int etapa, int id)
{
    double t_diff;
    
    if( Ficheiros.h_log != NULL ) {

        prodcons_buffers_inicio();

		// guardar timestamp
		t_diff = tempo_ate_agora();

		// guardar dados no ficheiro de log
		so_ficheiro_escrever_log_ficheiro(etapa,id,t_diff);

        prodcons_buffers_fim();
    }
}

void ficheiro_escrever_linha(char * linha)
{
    // escrever uma linha no ficheiro de saida
    so_ficheiro_escrever_linha(linha);
}

int stricmp (const char *s1, const char *s2)
{
   if (s1 == NULL) return s2 == NULL ? 0 : -(*s2);
   if (s2 == NULL) return *s1;

   char c1, c2;
   while ((c1 = tolower (*s1)) == (c2 = tolower (*s2)))
   {
     if (*s1 == '\0') break;
     ++s1; ++s2;
   }

   return c1 - c2;
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    struct configuracao* pconfig = (struct configuracao*)user;

    #define MATCH(s, n) stricmp(section, s) == 0 && stricmp(name, n) == 0
    if (MATCH("servicos", "stock")) {
        pconfig->lista_servicos = strdup(value);
    } else if (MATCH("clientes", "servico")) {
        pconfig->lista_clientes = strdup(value);	
    } else if (MATCH("rececionistas", "lista")) {
        pconfig->lista_rececionistas = strdup(value);
    } else if (MATCH("instaladores", "especialidades")) {
        pconfig->lista_instaladores = strdup(value);
    } else if (MATCH("buffers", "capacidade")) {
        pconfig->lista_buffers = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

