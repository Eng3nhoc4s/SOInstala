
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

//grupo so046
//-----------

struct ficheiros Ficheiros; // informação sobre nomes e handles de ficheiros

extern struct configuracao Config; // configuração da execução do SOinstala

void ficheiro_iniciar(char *fic_entrada, char *fic_saida, char *fic_log)
{
    //==============================================
    // GUARDAR NOMES DOS FICHEIROS NA ESTRUTURA Ficheiros
    //
	// utilizar a função strdup

	Ficheiros.entrada = strdup(fic_entrada);
	
	if(fic_saida == NULL)
		perror("ficheiro de saida nao especificado");
	else
		Ficheiros.saida = strdup(fic_saida);
	

	if(fic_log == NULL)
		perror("ficheiro log nao especificado");
	else
		Ficheiros.log = strdup(fic_log);
	
	
    //so_ficheiro_iniciar(fic_entrada,fic_saida,fic_log);
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE ENTRADA
    //

	Ficheiros.h_entrada = fopen(Ficheiros.entrada, "r");
	
    //so_ficheiro_abrir_ficheiro_entrada();
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
	// usar a função strtok para subdividir a string Config.lista_servicos
	// e contar o nº de elementos
	//

	int cservicos;
	
	char* dup = strdup(Config.lista_servicos);
	char* p = strtok(Config.lista_servicos, " ");

	if(p == NULL){
		cservicos = 0;
	}
	
	while( p != NULL){
		p = strtok(NULL, " ");
		cservicos ++;	

	}

	Config.SERVICOS = cservicos;
	
    //so_ficheiro_contar_servicos();
    //==============================================

    // iniciar memoria para o vetor com o stock por servico e semaforo
    memoria_criar_stock();
    prodcons_criar_stock();

    //==============================================
    // LER STOCK DE CADO SERVICO
    //

	int i;
	
	char* st = strtok(dup, " ");
	Config.stock[0] = atoi(st);
	
	for(i = 1; i < Config.SERVICOS; i++){
		
		st = strtok(NULL, " ");	
		Config.stock[i] = atoi(st);		
	}

    //so_ficheiro_ler_stock();
    //==============================================

    //==============================================
    // CONTAR CLIENTES
    //
	// usar strtok para subdividir a string Config.lista_clientes
	// e contar o nº de elementos
	//

	int cclientes;

	char* p2 = strtok(Config.lista_clientes, " ");

	if(p2 == NULL){
		cclientes = 0;
	}
	
	while( p2 != NULL){
		p2 = strtok(NULL, " ");
		cclientes ++;	

	}

	Config.CLIENTES = cclientes;
    
	//so_ficheiro_contar_clientes();
    //==============================================

    //==============================================
    // CONTAR RECECIONISTAS
    //
	// usar strtok para subdividir a string Config.lista_rececionistas
	// e contar o nº de elementos
	//

	int crecep;

	char* p3 = strtok(Config.lista_rececionistas, " ");

	if(p3 == NULL){
		crecep = 0;
	}
	
	while( p3 != NULL){
		p3 = strtok(NULL, " ");
		crecep ++;	

	}

	Config.RECECIONISTAS = crecep;
    //so_ficheiro_contar_rececionistas();
    //==============================================

    //==============================================
    // CONTAR INSTALADORES
    //
	// usar strtok para subdividir a string Config.lista_instaladores
	// e contar o nº de elementos
	//

	//espaco em branco -> especialidade do instalador
	int cinst;

	char* p4 = strtok(Config.lista_instaladores, ",");

	if(p4 == NULL){
		cinst = 0;
	}
	
	while( p4 != NULL){
		p4 = strtok(NULL, ",");
		cinst ++;	

	}

	Config.INSTALADORES = cinst;
    //so_ficheiro_contar_instaladores();
    //==============================================

	
    so_ficheiro_ler_especialidades();

	
    //==============================================
    // LER CAPACIDADES DOS BUFFERS
    //
	// usar strtok para subdividir a string Config.lista_buffers
	// e depois usar atoi sobre cada elemento para definir a capacidade de:
	// - Config.BUFFER_SERVICO, Config.BUFFER_INSTALACAO, Config.BUFFER_CONCLUSAO
	//
		
	char* dup2 = strdup(Config.lista_buffers);
	char* buf;
		
	for(i = 0; i < 3; i++){
		buf = strtok_r(dup2, " ", &dup2);
		Config.lista_buffers[i] = atoi(buf);
	}
	
	Config.BUFFER_SERVICO = Config.lista_buffers[0];
	Config.BUFFER_INSTALACAO = Config.lista_buffers[1];
	Config.BUFFER_CONCLUSAO = Config.lista_buffers[2];


    //so_ficheiro_ler_capacidades();
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE SAIDA (se foi especificado)
    //
	
	if(fic_saida != NULL)
		Ficheiros.h_saida = fopen(Ficheiros.saida, "w");
    
	//so_ficheiro_abrir_ficheiro_saida();
    //==============================================

    //==============================================
    // ABRIR FICHEIRO DE LOG (se foi especificado)
    //

	if(fic_log != NULL)
		Ficheiros.h_log = fopen(Ficheiros.log, "w");
    
	//so_ficheiro_abrir_ficheiro_log();
    //==============================================
}

void ficheiro_destruir()
{
    //==============================================
    // DESTRUIR ZONAS DE MEMÓRIA RESERVADAS DINAMICAMENTE
    //
	// estas zonas foram criadas com strdup por so_ficheiro_iniciar
	
	fclose(Ficheiros.h_entrada);

	if(Ficheiros.saida != NULL)
		fclose(Ficheiros.h_saida);

	if(Ficheiros.log != NULL)
		fclose(Ficheiros.h_log);

    //so_ficheiro_destruir();
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

