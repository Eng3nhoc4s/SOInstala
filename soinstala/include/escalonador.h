#ifndef ESCALONADOR_H_GUARD
#define ESCALONADOR_H_GUARD

// estrutura onde se encontram quais os instaladores que podem fazer cada servico (1 ou 0)
struct escalonamento {
  int *ptr; // ponteiro para matriz [SERVICOS,INSTALADORES]
};

void escalonador_iniciar (int,int);
int  escalonador_obter_instalador(int,int);

#endif
