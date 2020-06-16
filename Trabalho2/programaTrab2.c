/*
Mateus Ferreira Gomes

Trabalho de Organização de Arquivos

Programa que trabalha com 6 funcoes
  Escreve um csv em bin
  Printa um binario na tela
  Faz busca em arquivo binario
  Remove registros existentes no arquivo binario
  Insere novos registros no arquivo binario
  Atualiza registros ja existentes

Instituto de Ciencias Matematicas e de Computacao - Universidade de Sao Paulo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "escrever.h"

typedef struct noremovido_ NO;
typedef struct nodados_ NOE;

int tam_ultimo_reg; //Globais para guardar o tamanho do registro e o numero de bytes que faltam pra completar a pagina
int bytes_restantes; //de disco

/*
Estrutura do Registro de Dados
*/
typedef struct Dados{
  double salario; 
  long long int encadeamentolista;
  char telefone[14];
  char nome[151];
  char cargo[151];
  int id;
  int tamanhonome;
  int tamanhocargo;
  int tamanhoregistro;
  char removido;
  char campo4;
  char campo5;
  int tipo;
}Dados;

/*
Estrutura de um registro de cabecalho
*/
typedef struct encontrar{

  int id;
  double salario;
  char telefone[14];
  char nome[151];
  char cargo[151];
  int tipo;

}Encontrar;

typedef struct removido{

  long long int encad;
  int tamanho_reg;

}Removido;

struct nodados_{

  Dados *d;
  NOE *prox;

};

struct noremovido_{

  Removido *r;
  NO *prox;

};

typedef struct list_{
  NO *inicio;
  NO *fim;
  int tamanho;
}LIST;

typedef struct liste_{
  NOE *inicio;
  NOE *fim;
  int tamanho;
}LIST_E;



/*Função empty_list:
 Verifica se uma lista está vazia;
@Parâmetros:
-Um ponteiro para lista;
@Retorno:
-Se a lista estiver vazia, retorna 1. Caso contrário, retorna 0;*/
int empty_list(LIST *L){
	if(L != NULL && L->inicio == NULL) return 1;
 	return 0;
}

/**
 * Funcao que imprime uma lista que contem o tipo de dados de um registro removido
 * @Parametros: Lista a ser impressa
**/
void imprime_lista(LIST *l){

  NO *aux;

  aux = l->inicio;

  while(aux != NULL){
    printf("tam_reg: %d    ", aux->r->tamanho_reg);
    printf("encad: %lld\n", aux->r->encad);
    aux = aux->prox;
  }
  printf("\n");

}

/**
 * Funcao que imprime uma lista com o tipo de dados de registros para insercao
 * @Parametros: Lista a ser impressa
**/
void imprime_lista_e(LIST_E *l){

  NOE *aux;

  aux = l->inicio;

  while(aux != NULL){
    printf("id: %d\n", aux->d->id);
    printf("salario: %lf\n", aux->d->salario);
    printf("nome: %s\n", aux->d->nome);
    printf("cargo: %s\n", aux->d->cargo);
    printf("tam_reg: %d    ", aux->d->tamanhoregistro);
    printf("encad: %lld\n", aux->d->encadeamentolista);
    aux = aux->prox;
  }
  printf("\n");

}

/**
 * Funcao que cria um lista do tipo de dados a serem removidos
 * @Retorno: Lista criada
**/
LIST *create_list(void){
	LIST *L; 
	L = (LIST *) malloc(sizeof(LIST)); 
	if(L != NULL){
		L->inicio = NULL; 
    L->fim = NULL;
		L->tamanho = 0; 
	}	
	return(L);	
}

/**
 * Funcao que cria um lista do tipo de dados a serem inseridos
 * @Retorno: Lista criada
**/
LIST_E *create_list_e(void){
	LIST_E *L; 
	L = (LIST_E *) malloc(sizeof(LIST_E)); 
	if(L != NULL){
		L->inicio = NULL; 
    L->fim = NULL;
		L->tamanho = 0; 
	}	
	return(L);	
}

/**
 * Funcao que insere dados no final de uma lista cujo tipo de dados sao removidos
 * @Parametros: A lista e o no a ser inserido
**/
int list_insertion_final(LIST *l, Removido *R){
  
  NO *new = (NO *) malloc(sizeof(NO));
  new->r = R;
  new->prox = NULL;

  if(l != NULL){
    if(l->tamanho == 0){
      l->inicio = new;
      l->fim = new;
      l->tamanho++;
      return 1;
    }
    l->fim->prox = new;
    l->fim = l->fim->prox;
    l->tamanho++;
    return 1;
  }

  return 0;

}

/**
 * Funcao que insere dados no final de uma lista cujo tipo de dados sao inseridos
 * @Parametros: A lista e o no a ser inserido
**/
int list_insertion_final_e(LIST_E *l, Dados *R){
  
  NOE *new = (NOE *) malloc(sizeof(NOE));
  new->d = R;
  new->prox = NULL;

  if(l != NULL){
    if(l->tamanho == 0){
      l->inicio = new;
      l->fim = new;
      l->tamanho++;
      return 1;
    }
    l->fim->prox = new;
    l->fim = l->fim->prox;
    l->tamanho++;
    return 1;
  }

  return 0;

}

/**
 * Funcao que insere dados ordenadamente numa lista cujo tipo de dados sao removidos
 * @Parametros: A lista e o no a ser inserido
**/
int list_insertion(LIST *L, Removido *R){

  int posicao = 0;

 	if(L == NULL || R == NULL) return 0;
	NO *new = (NO *) malloc(sizeof(NO)), *search = NULL, *previous = NULL;		/*
	search -> nó posicionado onde deveria estar o new;
	previous -> nó anterior à search		
	new -> novo nó a ser inserido entre o previous e o search; */
	if(new != NULL){
		new->r = R;
		new->prox = NULL;
		if(empty_list(L)){ /*LISTA VAZIA*/
			L->inicio = new;
      L->fim = new;  
		}else{
			search = L->inicio;
			while((search != NULL) && (R->tamanho_reg > search->r->tamanho_reg)){
				previous = search;
				search = search->prox; /*previous recebe o nó anterior de search*/
        posicao++;

			}
			if(search == L->inicio){ /*CASO PARTICULAR: inserir como primeiro elemento*/
				new->prox = search;
				L->inicio = new;
        posicao = 0;
			}
      else if(search == NULL){	//CASO PARTICULAR: inserir como último elemento	
				previous->prox = new;
        L->fim = new;
        posicao = L->tamanho;
      }
			else{
				new->prox = search;
				previous->prox = new;
			}
		}
		L->tamanho++;
    return posicao;
  }
	return -1;
}

/**
 * Funcao que insere dados ordenadamente numa lista cujo tipo de dados sao inseridos
 * @Parametros: A lista e o no a ser inserido
**/
int list_insertion_e(LIST_E *L, Dados *d){ 

 	if(L == NULL || d == NULL) return 0;
	NOE *new = (NOE *) malloc(sizeof(NOE)), *search = NULL, *previous = NULL;		/*
	search -> nó posicionado onde deveria estar o new;
	previous -> nó anterior à search		
	new -> novo nó a ser inserido entre o previous e o search; */
	if(new != NULL){
		new->d = d;
		new->prox = NULL;
		if(L->inicio == NULL){ /*LISTA VAZIA*/
			L->inicio = new;  
      L->fim = new;
		}else{
			search = L->inicio;
      //printf("tam_reg %d", d->id);
			while((search != NULL) && (d->tamanhoregistro > search->d->tamanhoregistro)){
				previous = search;
				search = search->prox; /*previous recebe o nó anterior de search*/
			}
			if(search == L->inicio){ /*CASO PARTICULAR: inserir como primeiro elemento*/
				new->prox = search;
				L->inicio = new;
			}
      else if(search == NULL){	//CASO PARTICULAR: inserir como último elemento	
				previous->prox = new;
        L->fim = new;
      }
			else{
				new->prox = search;
				previous->prox = new;
			}
		}
		L->tamanho++;
		return 1;
	}
	return 0;
}

/*
Funcao auxiliar usada para resetar variaveis
e ler dados
*/
void reset_var(Dados* pessoa, int p){

  memset(pessoa[p].telefone, '@', 14);


  strcpy(pessoa[p].nome, "\0");
  strcpy(pessoa[p].cargo, "\0");


}

/*
Funcao para contar o numero de linhas
do arquivo csv a ser lido
*/
int contar_linhas(FILE* fp){

  char aux;
  int count = 0;

  while(fscanf(fp, "%c", &aux) != EOF){
    if(aux == '\n')
      count++;
  }

  return count;
}

/*
Funcao para ler do csv o registro de cabecalho e
escrever o registro de cabecalho
*/
void reg_cabecalho(FILE* bin, int n_linhas, FILE* fp){




  char abre = '0';
  long long int topoLista = -1;
  char tagCampo1 = 'i';
  char desCampo1[40];
  char tagCampo2 = 's';
  char desCampo2[40];
  char tagCampo3 = 't';
  char desCampo3[40];
  char tagCampo4 =  'n';
  char desCampo4[40];
  char tagCampo5 = 'c';
  char desCampo5[40];
  char especial = '@';

  fscanf(fp, "%[^,]", desCampo1);
  fscanf(fp, "%*c");
  fscanf(fp, "%[^,]", desCampo2);
  fscanf(fp, "%*c");
  fscanf(fp, "%[^,]", desCampo3);
  fscanf(fp, "%*c");
  fscanf(fp, "%[^,]", desCampo4);
  fscanf(fp, "%*c");
  fscanf(fp, "%[^\r\n]", desCampo5);

  fwrite(&abre, sizeof(char), 1, bin);
  fwrite(&topoLista, sizeof(long long int), 1, bin);
  fwrite(&tagCampo1, sizeof(char), 1, bin);
  fwrite(&desCampo1, sizeof(char), strlen(desCampo1) + 1, bin);
  for(int i = 0; i < (40 - strlen(desCampo1) - 1); i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo2, sizeof(char), 1, bin);
  fwrite(&desCampo2, sizeof(char), (strlen(desCampo2) + 1), bin);
  for(int i = 0; i < (40 - strlen(desCampo2) - 1); i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo3, sizeof(char), 1, bin);
  fwrite(&desCampo3, sizeof(char), (strlen(desCampo3) + 1), bin);
  for(int i = 0; i < (40 - strlen(desCampo3) - 1); i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo4, sizeof(char), 1, bin);
  fwrite(&desCampo4, sizeof(char), (strlen(desCampo4) + 1), bin);
  for(int i = 0; i < (40 - strlen(desCampo4) - 1); i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo5, sizeof(char), 1, bin);
  fwrite(&desCampo5, sizeof(char), (strlen(desCampo5) + 1), bin);

  for(int i = 0; i < (40 - strlen(desCampo5) - 1); i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }

  for(int i = 0; i < 31786; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }

}

/*
Funcao que le arquivos csv e escreve no binario os 
varios registros de dados
*/
Dados* ler_arquivo(FILE* fp, int n_linhas, FILE* bin){

  Dados* pessoa = (Dados*) malloc(n_linhas*(sizeof(Dados)));
  reg_cabecalho(bin, n_linhas, fp);

  for(int j = 0; j < n_linhas; j++){

    char aux[14];
    reset_var(pessoa, j);

    fscanf(fp, "%d", &pessoa[j].id);
    fscanf(fp, "%*c");
    fscanf(fp, "%lf", &pessoa[j].salario);
    fscanf(fp, "%*c");
    strcpy(aux, "\0@@@@@@@@@@@@@");
    fscanf(fp, "%[^,]", aux);
    strcpy(pessoa[j].telefone, aux);
    fscanf(fp, "%*c");
    fscanf(fp, "%[^,]", pessoa[j].nome);
    fscanf(fp, "%*c");
    fscanf(fp, "%[^\r\n]", pessoa[j].cargo);
    fscanf(fp, "%*c");

  }

  return pessoa;

}

/*
Funcao que preenche o restante desejado com o caracter '@' 
no arquivo binario
*/
void preenche_arroba(int restante, FILE* bin, int* contador){

  char especial = '@';

  for(int i = 0; i < restante; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }

  *contador = 0;

}
/*
Funcao que escreve o arquivo binario contando o numero de paginas
de discos utilizadas para escrever, preechendo o restante necessario com
'@'s e coloca os registros de dados
*/
void escrever_binario(Dados* vet, FILE* bin, int n_linhas){

  int count_pag = 0;

  for(int i = 0; i < n_linhas; i++){

    vet[i].campo5 = 'c';
    vet[i].campo4 = 'n';
    vet[i].removido = '-';
    vet[i].encadeamentolista = -1;
    vet[i].tamanhonome = ((strlen(vet[i].nome)) + 2);
    vet[i].tamanhocargo = ((strlen(vet[i].cargo)) + 2);
    vet[i].tamanhoregistro = (42 + vet[i].tamanhonome + vet[i].tamanhocargo);
    if(vet[i].tamanhonome == 2){
      vet[i].tamanhoregistro -= 6;
    }
    if(vet[i].tamanhocargo == 2){
      vet[i].tamanhoregistro -= 6;
    }
  }

  for(int i = 0; i < n_linhas; i++){

    if(i != n_linhas - 1){
      if(count_pag + vet[i].tamanhoregistro + vet[i+1].tamanhoregistro + 10 > 32000){
        count_pag += vet[i].tamanhoregistro + 5;
        vet[i].tamanhoregistro += (32000 - count_pag);
        fwrite(&(vet[i].removido), sizeof(char), 1, bin);
        fwrite(&(vet[i].tamanhoregistro), sizeof(int), 1, bin);
        fwrite(&(vet[i].encadeamentolista), sizeof(long long int), 1, bin);
        fwrite(&(vet[i].id), sizeof(int), 1, bin);
        fwrite(&(vet[i].salario), sizeof(double), 1, bin);
        fwrite(&(vet[i].telefone),sizeof(char),14,bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].tamanhonome), sizeof(int), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].campo4), sizeof(char), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].nome), sizeof(char), vet[i].tamanhonome - 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].tamanhocargo), sizeof(int), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].campo5), sizeof(char), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].cargo), sizeof(char), vet[i].tamanhocargo - 1, bin);
        preenche_arroba(32000 - count_pag, bin, &count_pag);
      }
      else{

        count_pag += vet[i].tamanhoregistro + 5;
        fwrite(&(vet[i].removido), sizeof(char), 1, bin);
        fwrite(&(vet[i].tamanhoregistro), sizeof(int), 1, bin);
        fwrite(&(vet[i].encadeamentolista), sizeof(long long int), 1, bin);
        fwrite(&(vet[i].id), sizeof(int), 1, bin);
        fwrite(&(vet[i].salario), sizeof(double), 1, bin);
        fwrite(&(vet[i].telefone),sizeof(char),14,bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].tamanhonome), sizeof(int), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].campo4), sizeof(char), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].nome), sizeof(char), vet[i].tamanhonome - 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].tamanhocargo), sizeof(int), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].campo5), sizeof(char), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].cargo), sizeof(char), vet[i].tamanhocargo - 1, bin);
      }
    }
    else{
        fwrite(&(vet[i].removido), sizeof(char), 1, bin);
        fwrite(&(vet[i].tamanhoregistro), sizeof(int), 1, bin);
        fwrite(&(vet[i].encadeamentolista), sizeof(long long int), 1, bin);
        fwrite(&(vet[i].id), sizeof(int), 1, bin);
        fwrite(&(vet[i].salario), sizeof(double), 1, bin);
        fwrite(&(vet[i].telefone),sizeof(char),14,bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].tamanhonome), sizeof(int), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].campo4), sizeof(char), 1, bin);
        if(vet[i].tamanhonome > 2) fwrite(&(vet[i].nome), sizeof(char), vet[i].tamanhonome - 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].tamanhocargo), sizeof(int), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].campo5), sizeof(char), 1, bin);
        if(vet[i].tamanhocargo > 2) fwrite(&(vet[i].cargo), sizeof(char), vet[i].tamanhocargo - 1, bin);
    }    
   
  }

}

/**
 * Funcao que cria uma estrutura que esta no no' do tipo de dados removido
 * @Retorno: A estrutura criada
**/
Removido* cria_removido(){

  Removido *r = malloc(sizeof(Removido));
  r->encad = 0;
  r->tamanho_reg = 0;
  return r;

}

/**
 * Funcao que pega uma lista de removidos que ja foi feita no arquivo binario
 * @Parametros: A lista que se deseja inserir os nos
 *              O arquivo
**/
void get_lista(FILE* bin, LIST *l){

    long long int aux;
    int tam_reg;


    rewind(bin);
    fseek(bin, 1, SEEK_CUR);
    fread(&aux, sizeof(long long int), 1, bin);
    fseek(bin, aux, SEEK_SET);

    while(aux != -1){
      fseek(bin, 1, SEEK_CUR);
      fread(&tam_reg, sizeof(int), 1, bin);
      Removido *r = cria_removido();
      r->encad = aux;
      r->tamanho_reg = tam_reg;
      list_insertion_final(l, r);
      fread(&aux, sizeof(long long int), 1, bin);
      fseek(bin, aux, SEEK_SET);
    }
}

/**
 * Funcao que escreve a lista encadeada de removidos no arquivo binario
 * @Parametros: Arquivo a escrever
 *              Lista de removidos a ser escrita
**/
void escreve_arquivo(FILE* bin, LIST *l){

  fseek(bin, 1, SEEK_SET);
  fwrite(&(l->inicio->r->encad), sizeof(long long int), 1, bin);

  NO *aux = l->inicio;

  while(aux->prox != NULL){
    fseek(bin, aux->r->encad, SEEK_SET);
    fseek(bin, 5, SEEK_CUR);
    fwrite(&(aux->prox->r->encad), sizeof(long long int), 1, bin);
    aux = aux->prox;
  }
  
  long long int menosum = -1;
  fseek(bin, aux->r->encad, SEEK_SET);
  fseek(bin, 5, SEEK_CUR);
  fwrite(&menosum, sizeof(long long int), 1, bin);
  
}

/**
 * Funcao que apaga uma lista de removidos
 * @Parametros: Lista a ser apagada
**/
void apaga_lista(LIST *l){

  NO* aux = l->inicio;
  NO* apaga;

  while(aux != NULL){
    apaga = aux;
    aux = aux->prox;
    free(apaga);
  }

  free(l);
}

/**
 * Funcao que cria um no o qua sera utilizado para insercao
 * @Parametros: No que sera usado para insercao
 * @Retorno: Dados a serem inseridos
**/
Dados *cria_no_encontrar(Encontrar *e){

  Dados *d = (Dados*) malloc(sizeof(Dados));
  memset(d->telefone, '@', 14);
  strcpy(d->nome, "\0");
  strcpy(d->cargo, "\0");
  
  d->id = e->id;
  strcpy(d->cargo, e->cargo);
  strcpy(d->nome, e->nome);
  strcpy(d->telefone, e->telefone);
  d->salario = e->salario;
  d->removido = '-';
  d->campo5 = 'c';
  d->campo4 = 'n';
  d->removido = '-';
  d->encadeamentolista = -1;
  d->tamanhonome = ((strlen(d->nome)) + 2);
  d->tamanhocargo = ((strlen(d->cargo)) + 2);
  d->tamanhoregistro = (42 + d->tamanhonome + d->tamanhocargo);
  if(d->tamanhonome == 2){
    d->tamanhoregistro -= 6;
  }
  if(d->tamanhocargo == 2){
    d->tamanhoregistro -= 6;
  }
  d->tipo = e->tipo;

  //printf("%s--\n\n");

  return d;

}

/**
 * Funcao que cria um no que sera colocado na lista de removidos
 * @Retorno: No criado
**/
NO *cria_no(){

  NO *n = (NO *) malloc(sizeof(NO));

  if(n != NULL){
    return n;
  }

  return NULL;

}

/**
 * Funcao que refaz o encadeamento lista de uma determinada posicao da lista no arquivo:
 * Ex:
 * Temos:
 * 1->2->3->4->5
 * 
 * No arquivo precisamos que os nos apontem os encadeamentos lista nessa mesma ordem, porem, se inserimos um novo
 * na lista de removidos, precisaremos que este trecho o qual o novo foi inserido seja alterado, portanto, utilizamos esta 
 * funcao para mexer os "ponteiros" nos encadeamentos lista do arquivo binario
 * @Parametros: Arquivo binario
 *              Posicao do novo no que foi removido na LISTA ENCADEADA
 *              Lista Encadeada de removidos
**/
void refaz_encad(FILE *bin, int posicao, LIST *removidos){

  long long int menos_um = -1;


  if(posicao == 0){
      fseek(bin, 1, SEEK_SET);
      long long int topoListaAtual;
      fread(&topoListaAtual, sizeof(long long int), 1, bin);
      fseek(bin, 1, SEEK_SET);
      fwrite(&(removidos->inicio->r->encad), sizeof(long long int), 1, bin);
      fseek(bin, removidos->inicio->r->encad, SEEK_SET);
      fseek(bin, 5, SEEK_CUR);
      fwrite(&topoListaAtual, sizeof(long long int), 1, bin);
      /*NO *aux2 = removidos->inicio;
      removidos->inicio = removidos->inicio->prox;
      free(aux2);
      removidos->tamanho--;*/
    }
    else if(posicao == removidos->tamanho - 1){

      fseek(bin, removidos->fim->r->encad, SEEK_SET);
      fseek(bin, 5, SEEK_CUR);
      fwrite(&menos_um, sizeof(long long int), 1, bin);
      NO *aux3 = removidos->inicio;
      while(aux3->prox != removidos->fim){
        aux3 = aux3->prox;
      }
      fseek(bin, aux3->r->encad, SEEK_SET);
      fseek(bin, 5, SEEK_CUR);
      fwrite(&removidos->fim->r->encad, sizeof(long long int), 1, bin);
    }
    else{
      NO *aux = removidos->inicio;
      NO *anterior;
      int count = 0;
      while(count < posicao){
        anterior = aux;
        aux = aux->prox;
        count++;
      }
      fseek(bin, anterior->r->encad, SEEK_SET);
      fseek(bin, 5, SEEK_CUR);
      fwrite(&aux->r->encad, sizeof(long long int), 1, bin);
      fseek(bin, aux->r->encad, SEEK_SET);
      fseek(bin, 5, SEEK_CUR);
      fwrite(&aux->prox->r->encad, sizeof(long long int), 1, bin);
      /*removidos->tamanho--;
      free(aux);
      */
    }

}

/**
 * Funcao que insere um novo registro no arquivo binario, insere na melhor posicao, ou seja, se há removidos, insere no que
 * melhor cabe o registro, caso nao haja, insere no fim do arquivo
 * @Parametros: Arquivo binario
 *              Lista de registros removidos
 *              Lista de novos registros a serem inseridos
**/
void insere_registro(FILE *bin, LIST *removidos, LIST_E *inserir){
  
  char c, asterisco = '*', telefone[15], nome[5000], tagCampo4, tagCampo5, cargo[5000], test, lastpage;
  int zero = 0, id, reg_tam, indicador_nome, indicador_cargo;
  long long int encad_lista, menos_um = -1;
  double salario;

  NO *removido;
  NO *anterior;
  NOE *encontra;

  encontra = inserir->inicio;
  removido = removidos->inicio;

  //Se a lista nao acabou
  while(encontra != NULL){
    if(encontra->d->tamanhoregistro > bytes_restantes){ //Ve se nao ta no fim da pagina
      fseek(bin, -tam_ultimo_reg, SEEK_END);
      fseek(bin, -4, SEEK_CUR);
      tam_ultimo_reg += bytes_restantes;
      fwrite(&tam_ultimo_reg, sizeof(int), 1, bin);
      fseek(bin, 0, SEEK_END);
      preenche_arroba(bytes_restantes, bin, &zero);
    }
    if(removido == NULL){ //Se nao tem removidos pra por no lugar deles, poe no final
      fseek(bin, 0, SEEK_END);
      fwrite(&(encontra->d->removido), sizeof(char), 1, bin);
      fwrite(&(encontra->d->tamanhoregistro), sizeof(int), 1, bin);
      fwrite(&(encontra->d->encadeamentolista), sizeof(long long int), 1, bin);
      fwrite(&(encontra->d->id), sizeof(int), 1, bin);
      fwrite(&(encontra->d->salario), sizeof(double), 1, bin);
      fwrite(&(encontra->d->telefone),sizeof(char),14,bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->tamanhonome), sizeof(int), 1, bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->campo4), sizeof(char), 1, bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->nome), sizeof(char), encontra->d->tamanhonome - 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->tamanhocargo), sizeof(int), 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->campo5), sizeof(char), 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->cargo), sizeof(char), encontra->d->tamanhocargo - 1, bin);
      tam_ultimo_reg = encontra->d->tamanhoregistro;
      encontra = encontra->prox;
    }
    else{ //Se tiver, poe no lugar deles
      int contador = 0;
      removido = removidos->inicio;
      while(removido != NULL && removido->r->tamanho_reg < encontra->d->tamanhoregistro){
        anterior = removido;
        removido = removido->prox;
        contador++;
      }
      if(removido == NULL){
        fseek(bin, 0, SEEK_END);
        fwrite(&(encontra->d->removido), sizeof(char), 1, bin);
        fwrite(&(encontra->d->tamanhoregistro), sizeof(int), 1, bin);
        fwrite(&(encontra->d->encadeamentolista), sizeof(long long int), 1, bin);
        fwrite(&(encontra->d->id), sizeof(int), 1, bin);
        fwrite(&(encontra->d->salario), sizeof(double), 1, bin);
        fwrite(&(encontra->d->telefone),sizeof(char),14,bin);
        if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->tamanhonome), sizeof(int), 1, bin);
        if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->campo4), sizeof(char), 1, bin);
        if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->nome), sizeof(char), encontra->d->tamanhonome - 1, bin);
        if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->tamanhocargo), sizeof(int), 1, bin);
        if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->campo5), sizeof(char), 1, bin);
        if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->cargo), sizeof(char), encontra->d->tamanhocargo - 1, bin);
        tam_ultimo_reg = encontra->d->tamanhoregistro;
        encontra = encontra->prox;   
        continue;
      }


      fseek(bin, removido->r->encad, SEEK_SET);
      fwrite(&(encontra->d->removido), sizeof(char), 1, bin);
      fwrite(&(removido->r->tamanho_reg), sizeof(int), 1, bin);
      fwrite(&(encontra->d->encadeamentolista), sizeof(long long int), 1, bin);
      fwrite(&(encontra->d->id), sizeof(int), 1, bin);
      fwrite(&(encontra->d->salario), sizeof(double), 1, bin);
      fwrite(&(encontra->d->telefone),sizeof(char),14,bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->tamanhonome), sizeof(int), 1, bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->campo4), sizeof(char), 1, bin);
      if(encontra->d->tamanhonome > 2) fwrite(&(encontra->d->nome), sizeof(char), encontra->d->tamanhonome - 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->tamanhocargo), sizeof(int), 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->campo5), sizeof(char), 1, bin);
      if(encontra->d->tamanhocargo > 2) fwrite(&(encontra->d->cargo), sizeof(char), encontra->d->tamanhocargo - 1, bin);
            
      if(removido == removidos->inicio){
        fseek(bin, 1, SEEK_SET);
        if(removido->prox != NULL){ 
          fwrite(&(removido->prox->r->encad), sizeof(long long int), 1, bin);
        }
        else{
          fwrite(&(menos_um), sizeof(long long int), 1, bin);
        }
        removidos->inicio = removido->prox;
        free(removido);
      }
      else if(removido == removidos->fim){
        fseek(bin, anterior->r->encad, SEEK_SET);
        fseek(bin, 1, SEEK_CUR);
        fseek(bin, sizeof(int), SEEK_CUR);
        fwrite(&(menos_um), sizeof(long long int), 1, bin);
        anterior->prox = NULL;
        free(removido);
      }
      else{
        fseek(bin, anterior->r->encad, SEEK_SET);
        fseek(bin, 1, SEEK_CUR);
        fseek(bin, sizeof(int), SEEK_CUR);
        fwrite(&(removido->prox->r->encad), sizeof(long long int), 1, bin);
        anterior->prox = removido->prox;
        free(removido);
      }
      encontra = encontra->prox;
      removidos->tamanho--;

    }
  }
}

/**
 * Funcao que busca um determinado registro no arquivo
 * @Retorno: Sua posicao no arquivo (ftell)
**/
int busca_registro(FILE *bin, Encontrar *e, int tipo){

  char c, asterisco = '*', telefone[15], nome[5000], tagCampo4, tagCampo5, cargo[5000], test, lastpage;
  int zero = 0, id, reg_tam, indicador_nome, indicador_cargo;
  long long int encad_lista, menos_um = -1;
  double salario;

  fseek(bin, 32000, SEEK_SET);
  while(fread(&c, sizeof(char), 1, bin)){
    if(c == '-' || c == '*'){
      int save = (int) ftell(bin) - 1;
      memset(telefone, '\0', 15);
      memset(cargo, '\0', 5000);
      memset(nome, '\0', 5000); 
      fread(&reg_tam, sizeof(int), 1, bin);
      if(c == '*'){
        if(reg_tam)        
          fseek(bin, reg_tam, SEEK_CUR);
        continue;
      }
      fread(&encad_lista, sizeof(long long int), 1, bin); //encad_lista o encadeamento lista
      fread(&id, sizeof(int), 1, bin);
      if(id == e->id && tipo == 0)
        return save;

      fread(&salario, sizeof(double), 1, bin);
      if(salario == e->salario && tipo == 1)
        return save;

      fread(telefone, sizeof(char), 14, bin);
      if(strcmp(telefone, e->telefone) == 0 && tipo == 2){
        return save;
      }  

      fread(&lastpage, sizeof(char), 1, bin);
      if(lastpage == '@'){
        continue;
      }
      else{
        fseek(bin, -1, SEEK_CUR);
      }

      if(reg_tam - 34 == 0){
        continue;
      }
      else{
        fseek(bin, 4, SEEK_CUR);
        fread(&test, sizeof(char), 1, bin);
        if(test == 'n'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_nome, sizeof(int), 1, bin);
          fread(&tagCampo4, sizeof(char), 1, bin);
          fread(&nome, sizeof(char), indicador_nome-1, bin);
          if(strcmp(nome, e->nome) == 0 && tipo == 3)
            return save;
          fseek(bin, 4, SEEK_CUR);
          fread(&test, sizeof(char), 1, bin);
          if(test == 'c'){
            fseek(bin, -5, SEEK_CUR);
            fread(&indicador_cargo, sizeof(int), 1, bin);
            fread(&tagCampo5, sizeof(char), 1, bin);
            fread(&cargo, sizeof(char), indicador_cargo-1, bin);
            if(strcmp(cargo, e->cargo) == 0 && tipo == 4)
              return save;
          }
          else{
            fseek(bin, -5, SEEK_CUR);
          }
        }
        else if(test == 'c'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_cargo, sizeof(int), 1, bin);
          fread(&tagCampo5, sizeof(char), 1, bin);
          fread(&cargo, sizeof(char), indicador_cargo - 1, bin);
          if(strcmp(cargo, e->cargo) == 0 && tipo == 4)
            return save;
        }
      }
    }
  }

}

/**
 * Funcao que remove um registro do arquivo binario (funcionalidade 4) e coloca numa lista de registros removidos
 * @Parametros: Arquivo binario
 *              Estrutura (registro) a ser removida
 *              Tipo da remocao, ou seja, 0 para remocao por id, 1 para salario, 2 para telefone...
 *              Lista de removidos para inserir o novo registro removido
 * @Retorno: Posicao na lista de removidos do novo registro removido
**/
int remove_registro(FILE* bin, Encontrar* e, int tipo, LIST *l){

  Dados* d = (Dados*) malloc(sizeof(Dados));
  char asterisco = '*', c, telefone[15], nome[151], cargo[151], test, lastpage, teste, tagCampo4, tagCampo5;
  int zero = 0, reg_tam, id, indicador_cargo, indicador_nome, posicao = -1;
  long long int encad_lista, count = 32000;
  double salario;

  fseek(bin, 32000, SEEK_SET);

  while(fread(&c, sizeof(char), 1, bin)){
    if(c == '-'){ 
      memset(telefone, '\0', 15);
      memset(cargo, '\0', 151);
      memset(nome, '\0', 151);
      fread(&reg_tam, sizeof(int), 1, bin);
      fread(&encad_lista, sizeof(long long int), 1, bin); 
      fread(&id, sizeof(int), 1, bin);
      if(tipo == 0){
        if(id == e->id){
          fseek(bin, -17, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          return posicao;
        }
        fseek(bin, reg_tam - 12, SEEK_CUR);
        continue;
      }
      fread(&salario, sizeof(double), 1, bin);
      if(tipo == 1){
        if(salario == e->salario){
          fseek(bin, -25, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          continue;
        }
      }
      fread(telefone, sizeof(char), 14, bin);
      if(tipo == 2){
        if(strcmp(telefone, e->telefone) == 0){
          fseek(bin, -39, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fread(&teste, sizeof(char), 1, bin);
          fseek(bin, -1, SEEK_CUR);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          continue;
        }
      }
      fread(&lastpage, sizeof(char), 1, bin);
      if(lastpage == '@'){
        continue;
      }
      else{
        fseek(bin, -1, SEEK_CUR);
      }
      if(reg_tam - 34 == 0){
        if(strcmp(e->telefone, telefone) == 0 && tipo == 2){
          fseek(bin, -39, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          continue;
        }
        if(strcmp(e->nome, nome) == 0 && tipo == 3){
          fseek(bin, -39, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          continue;
        }
        if(strcmp(e->cargo, cargo) == 0 && tipo == 4){
          fseek(bin, -39, SEEK_CUR);
          Removido *r = cria_removido();
          r->encad = (long long int) ftell(bin);
          r->tamanho_reg = reg_tam;
          posicao = list_insertion(l, r);
          fwrite(&asterisco, sizeof(char), 1, bin);
          fseek(bin, 12, SEEK_CUR);
          preenche_arroba(reg_tam - 8, bin, &zero);
          continue;
        }
        continue;
      }
      else{
        fseek(bin, 4, SEEK_CUR);
        fread(&test, sizeof(char), 1, bin);
        if(test == 'n'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_nome, sizeof(int), 1, bin);
          fread(&tagCampo4, sizeof(char), 1, bin);
          fread(&nome, sizeof(char), indicador_nome-1, bin);
          if(tipo == 3){
            if(strcmp(nome, e->nome) == 0){
              fseek(bin, -(43 + indicador_nome), SEEK_CUR);
              Removido *r = cria_removido();
              r->encad = (long long int) ftell(bin);
              r->tamanho_reg = reg_tam;
              posicao = list_insertion(l, r);
              fwrite(&asterisco, sizeof(char), 1, bin);
              fseek(bin, 12, SEEK_CUR);
              preenche_arroba(reg_tam - 8, bin, &zero);
              continue;
            }
          }
          fseek(bin, 4, SEEK_CUR);
          fread(&test, sizeof(char), 1, bin);
          if(test == 'c'){
            fseek(bin, -5, SEEK_CUR);
            fread(&indicador_cargo, sizeof(int), 1, bin);
            fread(&tagCampo5, sizeof(char), 1, bin);
            fread(&cargo, sizeof(char), indicador_cargo-1, bin);
            if(tipo == 4){
              if(strcmp(cargo, e->cargo) == 0){
                fseek(bin, -(47 + indicador_nome + indicador_cargo), SEEK_CUR);
                Removido *r = cria_removido();
                r->encad = (long long int) ftell(bin);
                r->tamanho_reg = reg_tam;
                posicao = list_insertion(l, r);
                fwrite(&asterisco, sizeof(char), 1, bin);
                fseek(bin, 12, SEEK_CUR);
                preenche_arroba(reg_tam - 8, bin, &zero);
                continue;
              }
            }
          }
          else{
            fseek(bin, -5, SEEK_CUR);
          }
        }
        else if(test == 'c'){
          if(strcmp(e->nome, nome) == 0 && tipo == 3){
            fseek(bin, -44, SEEK_CUR);
            Removido *r = cria_removido();
            r->encad = (long long int) ftell(bin);
            r->tamanho_reg = reg_tam;
            posicao = list_insertion(l, r);
            fwrite(&asterisco, sizeof(char), 1, bin);
            fseek(bin, 12, SEEK_CUR);
            preenche_arroba(reg_tam - 8, bin, &zero);
            continue;
          }
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_cargo, sizeof(int), 1, bin);
          fread(&tagCampo5, sizeof(char), 1, bin);
          fread(&cargo, sizeof(char), indicador_cargo - 1, bin);
          if(strcmp(cargo, e->cargo) == 0){
            fseek(bin, -(43 + indicador_cargo), SEEK_CUR);
            Removido *r = cria_removido();
            r->encad = (long long int) ftell(bin);
            r->tamanho_reg = reg_tam;
            posicao = list_insertion(l, r);
            fwrite(&asterisco, sizeof(char), 1, bin);
            fseek(bin, 12, SEEK_CUR);
            preenche_arroba(reg_tam - 8, bin, &zero);
            continue;
          }
        }
      }
    }
  }

  return posicao;

}

/*
Funcao utilizada para printar a funcao 2 do registro, printando o arquivo binario
tanto pelo registro de cabecalho quanto por todos os registros de dados.
Ao fim, printa o numero de paginas de disco escritas na tela
*/
void printa_busca(FILE* bin, Encontrar* e, int tipo, char descampo1[40], char descampo2[40], char descampo3[40], char descampo4[40], char descampo5[40]){


  Dados* d = (Dados*) malloc(sizeof(Dados));
  char c;
  int reg_tam;
  long long int encad_lista;
  int id;
  double salario;
  char telefone[15];
  char nome[100];
  int indicador_nome;
  char tagCampo4;
  int indicador_cargo;
  char tagCampo5;
  char cargo[100];
  char test;
  long long int count = 32000;
  char lastpage;
  int flag_achou_id = 0;
  int flag_achou_salario = 0;
  int flag_tem_cargo = 0;
  int flag_printa_salario = 0;
  int flag_achou_telefone = 0;
  int flag_printa_telefone = 0;
  int flag_achou_nome = 0;
  int flag_printa_nome = 0;
  int flag_achou_cargo = 0;
  int flag_printa_cargo = 0;
  int flag_printa_dps = 0;

  fseek(bin, 32000, SEEK_SET);
  
  while(fread(&c, sizeof(char), 1, bin)){
    if(c == '-'){ 
      fread(&reg_tam, sizeof(int), 1, bin);
      count += reg_tam + 5;
      fread(&encad_lista, sizeof(long long int), 1, bin); 
      fread(&id, sizeof(int), 1, bin);
      if(tipo == 0){
        if(id == e->id){
          printf("%s: %d\n", descampo1, id);
          flag_achou_id = 1;
        }
      }
      fread(&salario, sizeof(double), 1, bin);
      if(tipo == 1){
        if(salario == e->salario){
          printf("%s: %d\n", descampo1, id);
          printf("%s: %.2lf\n", descampo2, salario);
          flag_achou_salario = 1;
          flag_printa_salario = 1;
        }
      }     
      if(flag_achou_id){
        if(salario == -1) printf("%s: valor nao declarado\n", descampo2);
        else printf("%s: %.2lf\n", descampo2, salario);
      }
      fread(telefone, sizeof(char), 14, bin);
      if(tipo == 2){
        if(strcmp(telefone, e->telefone) == 0){
          printf("%s: %d\n", descampo1, id);
          if(salario == -1) printf("%s: valor nao declarado\n", descampo2);
          else printf("%s: %.2lf\n", descampo2, salario);
          printf("%s: %s\n", descampo3, telefone);
          flag_achou_telefone = 1;
          flag_printa_telefone = 1;
        }
      }
      if(flag_achou_id || flag_achou_salario){
        if(telefone[4] != '@') 
          printf("%s: %.14s\n", descampo3, telefone);
        else{
          printf("%s: valor nao declarado\n", descampo3);
        }
      }

      fread(&lastpage, sizeof(char), 1, bin);
      if(lastpage == '@'){
        continue;
      }
      else{
        fseek(bin, -1, SEEK_CUR);
      }

      if(reg_tam - 34 == 0 && ((flag_achou_id) || (flag_achou_salario) || (flag_achou_telefone))){
        flag_printa_dps = 1;
      }
      else if(reg_tam - 34 == 0){
        continue;
      }
      else{
        fseek(bin, 4, SEEK_CUR);
        fread(&test, sizeof(char), 1, bin);
        if(test == 'n'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_nome, sizeof(int), 1, bin);
          fread(&tagCampo4, sizeof(char), 1, bin);
          fread(&nome, sizeof(char), indicador_nome-1, bin);
          if(tipo == 3){
            if(strcmp(nome, e->nome) == 0){
              printf("%s: %d\n", descampo1, id);
              if(salario == -1) printf("%s: valor nao declarado\n", descampo2);
              else printf("%s: %.2lf\n", descampo2, salario);
              if(telefone[4] != '@') 
                printf("%s: %.14s\n", descampo3, telefone);
              else{
                printf("%s: valor nao declarado\n", descampo3);
              }
              printf("%s: %s\n", descampo4, nome);
              flag_achou_nome = 1;
              flag_printa_nome = 1;
            }
          }
          if(flag_achou_id || flag_achou_salario || flag_achou_telefone){
            printf("%s: %s\n", descampo4, nome);
          }
          fseek(bin, 4, SEEK_CUR);
          fread(&test, sizeof(char), 1, bin);
          if(test == 'c'){
            flag_tem_cargo = 1;
            fseek(bin, -5, SEEK_CUR);
            fread(&indicador_cargo, sizeof(int), 1, bin);
            fread(&tagCampo5, sizeof(char), 1, bin);
            fread(&cargo, sizeof(char), indicador_cargo-1, bin);
            if(tipo == 4){
              if(strcmp(cargo, e->cargo) == 0){
                printf("%s: %d\n", descampo1, id);
                if(salario == -1) printf("%s: valor nao declarado\n", descampo2);
                else printf("%s: %.2lf\n", descampo2, salario);
                if(telefone[4] != '@') 
                  printf("%s: %.14s\n", descampo3, telefone);
                else{
                  printf("%s: valor nao declarado\n", descampo3);
                }
                printf("%s: %s\n", descampo4, nome);
                flag_achou_cargo = 1;
                flag_printa_cargo = 1;
              }
            }
            if(flag_achou_id || flag_achou_salario || flag_achou_telefone || flag_achou_nome || flag_achou_cargo){
              printf("%s: %s\n", descampo5, cargo);
              printf("\n");
              flag_achou_salario = 0;
              flag_achou_telefone = 0;
              flag_achou_nome = 0;
              flag_achou_cargo = 0;
            }
          }
          else{
            fseek(bin, -5, SEEK_CUR);
            if(flag_achou_id){  
              printf("%s: valor nao declarado\n", descampo5);
              printf("\n");
              printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
              break;
            }
            if(flag_achou_salario || flag_achou_telefone || flag_achou_nome){
              printf("%s: valor nao declarado\n", descampo5);
              printf("\n");
              flag_achou_salario = 0;
              flag_achou_telefone = 0;
              flag_achou_nome = 0;
            }
          }
        }
        else if(test == 'c'){
          flag_tem_cargo = 1;
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_cargo, sizeof(int), 1, bin);
          fread(&tagCampo5, sizeof(char), 1, bin);
          fread(&cargo, sizeof(char), indicador_cargo - 1, bin);
          if(flag_achou_salario || flag_achou_telefone || flag_achou_nome){
            printf("%s: valor nao declarado\n", descampo4);
            printf("%s: %s\n", descampo5, cargo);
            printf("\n");
            flag_achou_salario = 0;
            flag_achou_telefone = 0;
            flag_achou_nome = 0;
            flag_achou_cargo = 0;
          }
          if(flag_achou_id){
            printf("%s: valor nao declarado\n", descampo4);
            printf("%s: %s\n", descampo5, cargo);
            printf("\n");
            printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
            break;
          }
        }
        else if((flag_tem_cargo == 0)){
          printf("%s: valor nao declarado\n", descampo5);
          printf("\n");
          printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
          break;
        }
        if(flag_achou_id){
          printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
          break;
        }
      }
    }
    if(flag_printa_dps){
      printf("%s: valor nao declarado\n", descampo4);
      printf("%s: valor nao declarado\n", descampo5);
      printf("\n");
      if(flag_achou_id){
         printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
         break;
      }
      
    }
  }
  if((tipo == 0 && !flag_achou_id) || (tipo == 1 && !flag_printa_salario) || (tipo == 2 && !flag_printa_telefone) || (tipo == 3 && !flag_printa_nome) || (tipo == 4 && !flag_printa_cargo)) printf("Registro inexistente.\n");
  if(flag_printa_salario || flag_printa_telefone || flag_printa_nome || flag_printa_cargo) printf("Número de páginas de disco acessadas: %lld\n", (count/32000)+1);
  
}

/*
Funcao utilizada para printar a funcao 3, printando no arquivo binario
usando o registro de cabecalho e os registros de dados, faz-se a busca.
Ao fim, printa o numero de paginas de disco lidas para fazer a busca ate' o que foi buscado.
A busca pode ser feita por qualquer um dos campos uteis do registro de cabecalho
*/
void printa_binario(FILE* bin){

  Dados* d = (Dados*) malloc(sizeof(Dados));
  char c;
  int reg_tam;
  long long int encad_lista;
  int id;
  double salario;
  char telefone[15];
  char nome[151];
  int indicador_nome;
  char tagCampo4;
  int indicador_cargo;
  char tagCampo5;
  char cargo[151];
  char test, c_test;
  long long int count = 32000;
  char lastpage;
  char c_status;
  long long int topoLista;
  char c_tcampo1, c_tcampo2, c_tcampo3, c_tcampo4, c_tcampo5, h[10];
  char descampo1[40], descampo2[40], descampo3[40], descampo4[40], descampo5[40];


  fread(&c_status, sizeof(char), 1, bin);
  if(c_status == '0'){
    fread(&topoLista, sizeof(long long int), 1, bin);
    fread(&c_tcampo1, sizeof(char), 1, bin);
    fread(&c_test, sizeof(char), 1, bin);
    int i = 0;
    while(c_test != '@'){
      descampo1[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo2 = c_test;
    fseek(bin, 1, SEEK_CUR);
    i = 0;
    while(c_test != '@'){
      descampo2[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo3 = c_test;
    fseek(bin, 1, SEEK_CUR);
    i = 0;
    while(c_test != '@'){
      descampo3[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo4 = c_test;
    fseek(bin, 1, SEEK_CUR);
    i = 0;
    while(c_test != '@'){
      descampo4[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }    
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo5 = c_test;
    fseek(bin, 1, SEEK_CUR);
    i = 0;
    while(c_test != '@'){
      descampo5[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
  }

  rewind(bin);
  fseek(bin, 32000, SEEK_SET);
  
  while(fread(&c, sizeof(char), 1, bin)){
    if(c == '-' || c == '*'){
      memset(telefone, '\0', 15);
      memset(cargo, '\0', 151);
      memset(nome, '\0', 151); 
      fread(&reg_tam, sizeof(int), 1, bin);
      count += reg_tam + 5;
      if(c == '*'){
        fseek(bin, reg_tam, SEEK_CUR);
        continue;
      }
      fread(&encad_lista, sizeof(long long int), 1, bin); //encad_lista o encadeamento lista
      fread(&id, sizeof(int), 1, bin);
      printf("%d ", id);
      fread(&salario, sizeof(double), 1, bin);
      if(salario == -1) printf("         ");
      else printf("%.2lf ", salario);
      fread(telefone, sizeof(char), 14, bin);
      if(telefone[4] != '@') 
        printf("%.14s ", telefone);
      else printf("               ");
      
      fread(&lastpage, sizeof(char), 1, bin);
      if(lastpage == '@'){
        printf("\n");
        continue;
      }
      else{
        fseek(bin, -1, SEEK_CUR);
      }

      if(reg_tam - 34 == 0){
        printf("\n");
        continue;
      }
      else{
        fseek(bin, 4, SEEK_CUR);
        fread(&test, sizeof(char), 1, bin);
        if(test == 'n'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_nome, sizeof(int), 1, bin);
          printf("%d ", indicador_nome - 2);
          fread(&tagCampo4, sizeof(char), 1, bin);
          fread(&nome, sizeof(char), indicador_nome-1, bin);
          printf("%s ", nome);
          fseek(bin, 4, SEEK_CUR);
          fread(&test, sizeof(char), 1, bin);
          if(test == 'c'){
            fseek(bin, -5, SEEK_CUR);
            fread(&indicador_cargo, sizeof(int), 1, bin);
            printf("%d ", indicador_cargo - 2);
            fread(&tagCampo5, sizeof(char), 1, bin);
            fread(&cargo, sizeof(char), indicador_cargo-1, bin);
            printf("%s\n", cargo);
          }
          else{
            printf("\n");
            fseek(bin, -5, SEEK_CUR);
          }
        }
        else if(test == 'c'){
          fseek(bin, -5, SEEK_CUR);
          fread(&indicador_cargo, sizeof(int), 1, bin);
          if(indicador_cargo != 2) printf("%d ", indicador_cargo - 2);
          fread(&tagCampo5, sizeof(char), 1, bin);
          fread(&cargo, sizeof(char), indicador_cargo - 1, bin);
          printf("%s\n", cargo);
        }
      }
    }
  }
  if(count % 32000 == 0)
    printf("Número de páginas de disco acessadas: %lld\n", count/32000); 
  else{
    printf("Número de páginas de disco acessadas: %lld\n", 1 + (count/32000)); 
  }
}

/**
 * Funcao que opera como auxiliar para escolher se a funcionalidade a ser executada será a 3 ou a 4
 * @Parametros: A descricao dos campos do registro de cabeçalho(func 3)
 *              O tipo de dado a ser removido(func 4)
 *              A lista de removidos (func 4)
 *              A funcionalidade
 *              O arquivo binario
**/
void executa_funcionalidade(char* descampo1, char* descampo2, char* descampo3, char* descampo4, char* descampo5, Encontrar *e, FILE *bin, int tipo, int funcionalidade, LIST* l){
  
  if(funcionalidade == 3){
    if(tipo == 0){
      printa_busca(bin, e, 0, descampo1, descampo2, descampo3, descampo4, descampo5);
    }
    else if(tipo == 1){
      printa_busca(bin, e, 1, descampo1, descampo2, descampo3, descampo4, descampo5);
    }
    else if(tipo == 2){
      printa_busca(bin, e, 2, descampo1, descampo2, descampo3, descampo4, descampo5);
    }
    else if(tipo == 3){
      printa_busca(bin, e, 3, descampo1, descampo2, descampo3, descampo4, descampo5);
    }
    else if(tipo == 4){
      printa_busca(bin, e, 4, descampo1, descampo2, descampo3, descampo4, descampo5);
    }
  }
  else{
    if(tipo == 0){
      remove_registro(bin, e, tipo, l);
    }
    else if(tipo == 1){
      remove_registro(bin, e, tipo, l);
    }
    else if(tipo == 2){
      remove_registro(bin, e, tipo, l);
    }
    else if(tipo == 3){
      remove_registro(bin, e, tipo, l);
    }
    else if(tipo == 4){
      remove_registro(bin, e, tipo, l);
    }
    
  }

}

/*
Funcao que le o cabecalho do arquivo binario e faz a busca pelo elemento que deseja-se
buscar dado pela funcao 3 deste trabalho
*/
void le_cabecalho(Encontrar* e, FILE* bin, int tipo, int funcionalidade, LIST *l){

  char c_status, c_test;
  char c_tcampo1, c_tcampo2, c_tcampo3, c_tcampo4, c_tcampo5;
  char descampo1[40], descampo2[40], descampo3[40], descampo4[40], descampo5[40];
  long long int topoLista;

  fread(&c_status, sizeof(char), 1, bin);
  if(c_status == '1'){
    fread(&topoLista, sizeof(long long int), 1, bin);
    fread(&c_tcampo1, sizeof(char), 1, bin);
    fread(&c_test, sizeof(char), 1, bin);
    int i = 0;
    while(c_test != '@'){
      descampo1[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo2 = c_test;
    i = 0;
    fread(&c_test, sizeof(char), 1, bin);
    while(c_test != '@'){
      descampo2[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo3 = c_test;
    fread(&c_test, sizeof(char), 1, bin);
    i = 0;
    while(c_test != '@'){
      descampo3[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo4 = c_test;
    fread(&c_test, sizeof(char), 1, bin);
    i = 0;
    while(c_test != '@'){
      descampo4[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }    
    while(c_test == '@'){
      fread(&c_test, sizeof(char), 1, bin);
    }
    c_tcampo5 = c_test;
    fread(&c_test, sizeof(char), 1, bin);
    i = 0;
    while(c_test != '@'){
      descampo5[i] = c_test;
      i++;
      fread(&c_test, sizeof(char), 1, bin);      
    }
  }
  else{
    printf("Falha no processamento do arquivo.\n");
  } 

  executa_funcionalidade(descampo1, descampo2, descampo3, descampo4, descampo5, e, bin, tipo, funcionalidade, l);

  rewind(bin);
}

/**
 * Funcao para ler a entrada que e' dada pelo usuario e montar uma lista de nos a serem atualizados
 * e nos a serem inseridos utilizada na funcionalidade 6
 * @Parametros: Arquivo binario
 *              Lista de nos a serem buscados
 *              Lista de nos a serem atualizados
**/
void compara_leitura(FILE *bin, LIST_E *encontrar, LIST_E *inserir){

  char compara[50];
  char compara2[50];

  memset(compara, '\0', 50);
  memset(compara2, '\0', 50);

  scanf("%s", compara);
  Encontrar* e = (Encontrar*) malloc(sizeof(Encontrar));
  Encontrar* alterar = (Encontrar*) malloc(sizeof(Encontrar));
  scanf("%*c");

  

  if(strcmp(compara, "idServidor") == 0){
    int tipo = 0;
    scanf("%d", &e->id);
    e->tipo = 0;
    scanf("%*c");
    scanf("%s", compara2);
    if(strcmp(compara2, "idServidor") == 0){
      alterar->tipo = 0;
      scanf("%d", &alterar->id);
    }
    else if(strcmp(compara2, "salarioServidor") == 0){
      alterar->tipo = 1;
      char teste[40];
      memset(teste, '\0', 40);
      scan_quote_string(teste);
      if(strcmp(teste, "\0") == 0){
        alterar->salario = -1;
      }
      else{
        alterar->salario = atof(teste);
      }
    }
    else if(strcmp(compara2, "telefoneServidor") == 0){
      alterar->tipo = 2;      
      scan_quote_string(alterar->telefone);
    }
    else if(strcmp(compara2, "nomeServidor") == 0){
      alterar->tipo = 3;
      scan_quote_string(alterar->nome);
    }
    else if(strcmp(compara2, "cargoServidor") == 0){
      alterar->tipo = 4;
      scan_quote_string(alterar->cargo);
    }
  }
  else if(strcmp(compara, "salarioServidor") == 0){
    e->tipo = 1;
    char teste[40];
    memset(teste, '\0', 40);
    scan_quote_string(teste);
    if(strcmp(teste, "\0") == 0){
      e->salario = -1;
    }
    else{
      e->salario = atof(teste);
    }

    scanf("%*c");  
    scanf("%s", compara2);

    if(strcmp(compara2, "idServidor") == 0){
      alterar->tipo = 0;
      scanf("%d", &alterar->id);
    }
    else if(strcmp(compara2, "salarioServidor") == 0){
      alterar->tipo = 1;
      char teste[40];
      memset(teste, '\0', 40);
      scan_quote_string(teste);
      if(strcmp(teste, "\0") == 0){
        alterar->salario = -1;
      }
      else{
        alterar->salario = atof(teste);
      }
    }
    else if(strcmp(compara2, "telefoneServidor") == 0){
      alterar->tipo = 2;
      scan_quote_string(alterar->telefone);
    }
    else if(strcmp(compara2, "nomeServidor") == 0){
      alterar->tipo = 3;
      scan_quote_string(alterar->nome);
    }
    else if(strcmp(compara2, "cargoServidor") == 0){
      alterar->tipo = 4;
      scan_quote_string(alterar->cargo);
    }
    
  }
  else if(strcmp(compara, "telefoneServidor") == 0){
    e->tipo = 2;
    scan_quote_string(e->telefone);
    scanf("%*c");  
    scanf("%s", compara2);

    if(strcmp(compara2, "idServidor") == 0){
      alterar->tipo = 0;
      scanf("%d", &alterar->id);
    }
    else if(strcmp(compara2, "salarioServidor") == 0){
      alterar->tipo = 1;
      char teste[40];
      memset(teste, '\0', 40);
      scan_quote_string(teste);
      if(strcmp(teste, "\0") == 0){
        alterar->salario = -1;
      }
      else{
        alterar->salario = atof(teste);
      }
    }
    else if(strcmp(compara2, "telefoneServidor") == 0){
      alterar->tipo = 2;
      scan_quote_string(alterar->telefone);
    }
    else if(strcmp(compara2, "nomeServidor") == 0){
      alterar->tipo = 3;
      scan_quote_string(alterar->nome);
    }
    else if(strcmp(compara2, "cargoServidor") == 0){
      alterar->tipo = 4;
      scan_quote_string(alterar->cargo);
    }
  }
  else if(strcmp(compara, "nomeServidor") == 0){
    e->tipo = 3;
    scan_quote_string(e->nome);

    scanf("%*c");  
    scanf("%s", compara2);

    if(strcmp(compara2, "idServidor") == 0){
      alterar->tipo = 0;
      scanf("%d", &alterar->id);
    }
    else if(strcmp(compara2, "salarioServidor") == 0){
      alterar->tipo = 1;
      char teste[40];
      memset(teste, '\0', 40);
      scan_quote_string(teste);
      if(strcmp(teste, "\0") == 0){
        alterar->salario = -1;
      }
      else{
        alterar->salario = atof(teste);
      }
    }
    else if(strcmp(compara2, "telefoneServidor") == 0){
      alterar->tipo = 2;
      scan_quote_string(alterar->telefone);
    }
    else if(strcmp(compara2, "nomeServidor") == 0){
      alterar->tipo = 3;
      scan_quote_string(alterar->nome);
    }
    else if(strcmp(compara2, "cargoServidor") == 0){
      alterar->tipo = 4;
      scan_quote_string(alterar->cargo);
    }
  }
  else if(strcmp(compara, "cargoServidor") == 0){
    e->tipo = 4;
    scan_quote_string(e->cargo);

    scanf("%*c");  
    scanf("%s", compara2);

    if(strcmp(compara2, "idServidor") == 0){
      alterar->tipo = 0;
      scanf("%d", &alterar->id);
    }
    else if(strcmp(compara2, "salarioServidor") == 0){
      alterar->tipo = 1;
      char teste[40];
      memset(teste, '\0', 40);
      scan_quote_string(teste);
      if(strcmp(teste, "\0") == 0){
        alterar->salario = -1;
      }
      else{
        alterar->salario = atof(teste);
      }
    }
    else if(strcmp(compara2, "telefoneServidor") == 0){
      alterar->tipo = 2;
      scan_quote_string(alterar->telefone);
    }
    else if(strcmp(compara2, "nomeServidor") == 0){
      alterar->tipo = 3;
      scan_quote_string(alterar->nome);
    }
    else if(strcmp(compara2, "cargoServidor") == 0){
      alterar->tipo = 4;
      scan_quote_string(alterar->cargo);
    }
  }			
  else {
    scanf("%*s");
    printf("Registro inexistente.\n");
  }

  Dados *d = cria_no_encontrar(e);
  Dados *a = cria_no_encontrar(alterar);

  list_insertion_final_e(inserir, a);
  list_insertion_final_e(encontrar, d);
  
  /*
  printf("id: %d\n", e->id);
  printf("nome: %s\n", e->nome);
  printf("cargo: %s\n", e->cargo);
  printf("telefone: %s\n", e->telefone);

  printf("id: %d\n", alterar->id);
  printf("nome: %s\n", alterar->nome);
  printf("cargo: %s\n", alterar->cargo);
  printf("telefone: %s\n", alterar->telefone);

  printf("---------------------\n");
  */
  rewind(bin);



}

/**
 * Funcao que remove um no da lista de registros removidos
 * @Parametros: A posicao desse no na lista
 *              A Lista
**/
void remove_lista(int posicao, LIST *l){

  NO *aux = l->inicio;
  NO *anterior;
  int count = 0;

  if(posicao == 0){
    NO *rem_ini = l->inicio;
    l->inicio = l->inicio->prox;
    free(rem_ini);
    l->tamanho--;
    return;
  }
  while(count < posicao){
    anterior = aux;
    aux = aux->prox;
    count++;
  }
  anterior->prox = aux->prox;
  free(aux);
  l->tamanho--;
  return;

}

/**
 * Funcao que remove um no da lista de registro a serem inseridos
 * @Parametros: A posicao do no a ser removido da lista
 *              A lista
**/
void remove_lista_e(int posicao, LIST_E *l){

  NOE *aux = l->inicio;
  NOE *anterior;
  int count = 0;

  if(posicao == 0){
    NOE *rem_ini = l->inicio;
    l->inicio = l->inicio->prox;
    free(rem_ini);
    l->tamanho--;
    return;
  }
  while(count < posicao){
    anterior = aux;
    aux = aux->prox;
    count++;
  }
  anterior->prox = aux->prox;
  free(aux);
  l->tamanho--;
  return;

}

/**
 * Funcao que atualiza registros de dados, dado o campo pelo qual se busca seguido pelo campo a ser atualizado
 * se nao houver registro removido que possa se inserir o registro atualizado, insere no final do arquivo
 * @Parametros: Arquivo binario
 *              Lista de registros pelos quais a informacao sera buscada
 *              Lista de registros com a informacao que sera usada para atualizar a atual
 *              Lista de registros removidos para inserir no lugar deles se necessario
**/
void atualiza_registros(FILE *bin, LIST_E *buscar, LIST_E *atualizar, LIST *removidos){

  int posicao, tamanho_registro, encadeamentolista, count, id, tamanho_cargo, save1;
  double salario;
  LIST_E *inserir = create_list_e();
  NOE *aux_buscar = buscar->inicio;
  NOE *aux_atualizar = atualizar->inicio;
  Encontrar *e = (Encontrar *) malloc(sizeof(Encontrar));
  fseek(bin, 0, SEEK_END);
  int n_paginas, zero, menos_um = -1;
  char cargo[151], teste, telefone[14], nome[151], traco = '-', tagnome = 'n', tagcargo = 'c';

  save1 = ftell(bin);

  while(aux_buscar != NULL){ //Enquanto nao acabar a lista de registros a serem buscados
    fseek(bin, 0, SEEK_END); // Conta o numero de bytes da pagina pra ver se vai ter que por arrobas e mudar de pagina
    n_paginas = ftell(bin)%32000;
    bytes_restantes = 32000 - n_paginas;
    fseek(bin, save1, SEEK_SET);
    Encontrar *insere = (Encontrar *) malloc(sizeof(Encontrar)); //Registro a ser inserido
    memset(insere->nome, '\0', 151); //INICIALIZANDO
    memset(insere->cargo, '\0', 151);
    memset(insere->telefone, '\0', 14);
    memset(cargo, '\0', 151);
    memset(nome, '\0', 151);
    memset(e->cargo, '\0', 151);
    memset(e->nome, '\0', 151);
    memset(e->telefone, '\0', 14);
    e->id = aux_buscar->d->id;
    strcpy(e->cargo, aux_buscar->d->cargo);
    strcpy(e->telefone, aux_buscar->d->telefone);
    strcpy(e->nome, aux_buscar->d->nome);
    e->tipo = aux_buscar->d->tipo;
    e->salario = aux_buscar->d->salario;

    posicao = busca_registro(bin, e, aux_buscar->d->tipo); //Pegando a posicao do registro no arquivo

    fseek(bin, posicao, SEEK_SET); //Indo pra posicao do registro

    if(aux_atualizar->d->tipo == 0){ //Se tiver que atualizar ID
      fseek(bin, posicao, SEEK_SET); //Vai pra posicao do registro
      fseek(bin, 13, SEEK_CUR); //Vai ate o ID
      fwrite(&aux_atualizar->d->id, sizeof(int), 1, bin); //Escreve o novo ID
    }
    else if(aux_atualizar->d->tipo == 1){ //Se tiver que atualizar salario
      fseek(bin, posicao, SEEK_SET); //IDEM ID
      fseek(bin, 17, SEEK_CUR);
      fwrite(&aux_atualizar->d->salario, sizeof(double), 1, bin);
    }  
    else if(aux_atualizar->d->tipo == 2){ //Se tiver que atualizar telefone
      if(aux_atualizar->d->telefone[0] == '\0') //IDEM ID
        memset(aux_atualizar->d->telefone, '@', 14);
      fseek(bin, posicao, SEEK_SET);
      fseek(bin, 25, SEEK_CUR);
      fwrite(&aux_atualizar->d->telefone, sizeof(char), 14, bin);
    }
    else if(aux_atualizar->d->tipo == 3){ //Se tiver que atualizar Nome
      fseek(bin, posicao, SEEK_SET); //Vai pra posicao do registro
      fseek(bin, 43, SEEK_CUR); //Vai ate o nome pulando o tamanho do nome
      fread(&teste, sizeof(char), 1, bin); //Testa se ja tem nome
      if(teste == 'n'){ //Se tiver
        fseek(bin, -5, SEEK_CUR); //Volta
        int tamanho_nome; 
        fread(&tamanho_nome, sizeof(int), 1, bin); //Le o tamanho do nome
        fseek(bin, 1, SEEK_CUR); //Pula a tag
        if(tamanho_nome >= strlen(aux_atualizar->d->nome)+2){ // Se o nome a ser inserido couber
          int save = (int) ftell(bin); //Salva a posicao a inserir
          fseek(bin, tamanho_nome+4, SEEK_CUR); //Pula pro cargo
          char verifica_cargo;
          fread(&verifica_cargo, sizeof(char), 1, bin); //Ve se tem cargo
          if(verifica_cargo == 'c'){ //Se tiver
            fseek(bin, -5, SEEK_CUR); //Volta e le o tamanho
            int tamanho_cargo;
            fread(&tamanho_cargo, sizeof(int), 1, bin);
            fseek(bin, 1, SEEK_CUR); // Pula a tag
            fread(&cargo, sizeof(char), tamanho_cargo-1, bin); //Le o cargo
            fseek(bin, -(tamanho_cargo + 5 + (tamanho_nome - strlen(aux_atualizar->d->nome) - 1)), SEEK_CUR); //Shifta o cargo pra tras
            fwrite(&cargo, sizeof(char), tamanho_cargo, bin); // Escreve o cargo shiftado
            preenche_arroba((tamanho_nome - strlen(aux_atualizar->d->nome)), bin, &zero); //Preenche o restante com arroba
            fseek(bin, save, SEEK_SET); //Volta pra onde tinha salvado
            fwrite(&aux_atualizar->d->nome, strlen(aux_atualizar->d->nome), 1, bin); //Escreve o nome
          }
          else{ //Se nao tiver cargo
            fseek(bin, save, SEEK_SET); //Volta pra onde vai inserir
            fwrite(&aux_atualizar->d->nome, strlen(aux_atualizar->d->nome), 1, bin); //Escreve o nome
            preenche_arroba(tamanho_nome - strlen(aux_atualizar->d->nome), bin, &zero); //Preenche o restante do nome com arroba
          }
        }
        else{ //Se nao couber
          fseek(bin, posicao + 1, SEEK_SET); //Relê o registro guardando-o porque vai ter que por no fim 
          fread(&tamanho_registro, sizeof(int), 1, bin);
          fread(&encadeamentolista, sizeof(long long int), 1, bin);
          fread(&id, sizeof(int), 1, bin);
          fread(&salario, sizeof(double), 1, bin);
          fread(telefone, sizeof(char), 14, bin);
          fseek(bin, tamanho_nome+4, SEEK_CUR);
          char verifica_cargo;
          fread(&verifica_cargo, sizeof(char), 1, bin);
          if(verifica_cargo == 'c'){ //Se tiver cargo, le
            fseek(bin, -5, SEEK_CUR);
            fread(&tamanho_cargo, sizeof(int), 1, bin);
            fseek(bin, 1, SEEK_CUR);
            fread(&cargo, sizeof(char), tamanho_cargo-1, bin);
          }
          fseek(bin, posicao, SEEK_SET); //Vai pro inicio do registro
          int posicao_lista = remove_registro(bin, e, e->tipo, removidos); //Remove ele
          aux_atualizar->d->tamanhonome = strlen(aux_atualizar->d->nome) + 2;
          tamanho_registro -= tamanho_nome; //ATUALIZA O TAMANHO DO REGISTRO
          tamanho_registro += aux_atualizar->d->tamanhonome;
          refaz_encad(bin, posicao_lista, removidos); //Refaz o encadeamento com o novo removido
          insere->id = id; //Prepara pra insercao
          insere->salario = salario;
          strcpy(insere->telefone, telefone);
          strcpy(insere->nome, aux_atualizar->d->nome);
          strcpy(insere->cargo, cargo);
          Dados *d = cria_no_encontrar(insere);
          list_insertion_final_e(inserir, d);
          insere_registro(bin, removidos, inserir); //INSERE NO FIM
          remove_lista_e(0, inserir); //TIRA DA LISTA DE SEREM INSERIDOS
        }   
      }
      else{ //Se nao tiver nome
        fseek(bin, posicao + 1, SEEK_SET); //Rele o registro salvando, pq tem que por no fim
        fread(&tamanho_registro, sizeof(int), 1, bin);
        fread(&encadeamentolista, sizeof(long long int), 1, bin);
        fread(&id, sizeof(int), 1, bin);
        fread(&salario, sizeof(double), 1, bin);
        fread(telefone, sizeof(char), 14, bin);
        if(tamanho_registro == 34){ //Se nao tiver nome nem cargo
          int posicao_lista = remove_registro(bin, e, e->tipo, removidos); // REMOVE
          refaz_encad(bin, posicao_lista, removidos); //REFAZ O ENCADEAMENTO
          insere->id = id;
          insere->salario = salario;
          strcpy(insere->telefone, telefone);
          strcpy(insere->nome, aux_atualizar->d->nome);
          strcpy(insere->cargo, cargo);
          Dados *d = cria_no_encontrar(insere);
          list_insertion_final_e(inserir, d);
          insere_registro(bin, removidos, inserir); //Insere no final
          remove_lista_e(0, inserir);
          aux_atualizar = aux_atualizar->prox;
          aux_buscar = aux_buscar->prox;
          continue; //FIM (proximo)
        } 
        fread(&tamanho_cargo, sizeof(int), 1, bin); //Se tiver cargo
        fseek(bin, 1, SEEK_CUR);
        fread(&cargo, sizeof(char), tamanho_cargo-1, bin); //Le o cargo
        int posicao_lista = remove_registro(bin, e, e->tipo, removidos); //Remove o registro
        refaz_encad(bin, posicao_lista, removidos);
        insere->id = id;
        insere->salario = salario;
        strcpy(insere->telefone, telefone);
        strcpy(insere->nome, aux_atualizar->d->nome);
        strcpy(insere->cargo, cargo);
        Dados *d = cria_no_encontrar(insere);
        list_insertion_final_e(inserir, d);
        insere_registro(bin, removidos, inserir); //Poe no fim do arquivo
        remove_lista_e(0, inserir);
      }
    } 
    else if(aux_atualizar->d->tipo == 4){ //Se tiver que atualizar cargo
      fseek(bin, posicao, SEEK_SET);
      fseek(bin, 43, SEEK_CUR);
      fread(&teste, sizeof(char), 1, bin); //Ve se o atual ja tem cargo
      if(teste == 'c'){ //Se tiver
        fseek(bin, -5, SEEK_CUR); //Volta
        int tam_cargo;  
        fread(&tam_cargo, sizeof(int), 1, bin); //Ve o tamanho do cargo
        fseek(bin, 1, SEEK_CUR); //Pula a tag
        if(tam_cargo >= (strlen(aux_atualizar->d->cargo) + 2)){ //Se couber o novo cargo
          fwrite(&aux_atualizar->d->cargo, sizeof(char), aux_atualizar->d->tamanhocargo, bin); //Escreve
          preenche_arroba(tam_cargo - (strlen(aux_atualizar->d->cargo) + 2), bin, &zero); //Completa com arroba se precisar
        }
        else{ //Se nao couber
          fseek(bin, posicao + 1, SEEK_SET); //Re-le o registro
          fread(&tamanho_registro, sizeof(int), 1, bin);
          fread(&encadeamentolista, sizeof(long long int), 1, bin);
          fread(&id, sizeof(int), 1, bin);
          fread(&salario, sizeof(double), 1, bin);
          fread(telefone, sizeof(char), 14, bin);
          aux_atualizar->d->tamanhocargo = strlen(aux_atualizar->d->cargo) + 2;
          tamanho_registro -= tam_cargo; //ATUALIZA O TAMANHO DO REGISTRO COM O NOVO CARGO
          tamanho_registro += aux_atualizar->d->tamanhocargo;
          int posicao_lista = remove_registro(bin, e, e->tipo, removidos); //Remove 
          refaz_encad(bin, posicao_lista, removidos); //Refaz o encadeamento
          insere->id = id;
          insere->salario = salario;
          strcpy(insere->telefone, telefone);
          strcpy(insere->nome, "\0");
          strcpy(insere->cargo, aux_atualizar->d->cargo);
          Dados *d = cria_no_encontrar(insere);
          list_insertion_final_e(inserir, d);
          insere_registro(bin, removidos, inserir); //Poe no fim do arquivo
          remove_lista_e(0, inserir);
        }
      } //Se nao tiver cargo ou tiver nome e cargo, tem que inserir no final
      else{
        int tamanho_nome2;
        fseek(bin, posicao + 1, SEEK_SET); //Rele o registro
        fread(&tamanho_registro, sizeof(int), 1, bin);
        fread(&encadeamentolista, sizeof(long long int), 1, bin);
        fread(&id, sizeof(int), 1, bin);
        fread(&salario, sizeof(double), 1, bin);
        fread(telefone, sizeof(char), 14, bin);
        if(tamanho_registro == 34){ //Se nao tiver nome nem cargo
          int posicao_lista = remove_registro(bin, e, e->tipo, removidos);
          refaz_encad(bin, posicao_lista, removidos);
          insere->id = id;
          insere->salario = salario;
          strcpy(insere->telefone, telefone);
          strcpy(insere->nome, "\0");
          strcpy(insere->cargo, aux_atualizar->d->cargo);
          Dados *d = cria_no_encontrar(insere);
          list_insertion_final_e(inserir, d);
          insere_registro(bin, removidos, inserir);
          remove_lista_e(0, inserir);
          aux_atualizar = aux_atualizar->prox;
          aux_buscar = aux_buscar->prox;
          continue;
        }
        fread(&tamanho_nome2, sizeof(int), 1, bin); //Se tiver nome
        fseek(bin, 1, SEEK_CUR);
        fread(&nome, sizeof(char), tamanho_nome2-2, bin); //Le o nome
        fseek(bin, 4, SEEK_CUR);
        char test;
        fread(&test, sizeof(char), 1, bin);
        if(test == 'c'){ //Se tiver nome e cargo
          fseek(bin, -5, SEEK_CUR); //Volta
          int tam_cargo;
          fread(&tam_cargo, sizeof(int), 1, bin); //Le o tamanho
          fseek(bin, 1, SEEK_CUR); //Pula a tag
          if(tam_cargo >= (strlen(aux_atualizar->d->cargo) + 2)){ //Se couber
            fwrite(&aux_atualizar->d->cargo, sizeof(char), aux_atualizar->d->tamanhocargo, bin); //Escreve
            preenche_arroba(tam_cargo - (strlen(aux_atualizar->d->cargo) + 2), bin, &zero);
          }
          else{//Se nao couber
            fseek(bin, posicao + 1, SEEK_SET); //Rele o registro
            fread(&tamanho_registro, sizeof(int), 1, bin);
            fread(&encadeamentolista, sizeof(long long int), 1, bin);
            fread(&id, sizeof(int), 1, bin);
            fread(&salario, sizeof(double), 1, bin);
            fread(telefone, sizeof(char), 14, bin);
            aux_atualizar->d->tamanhocargo = strlen(aux_atualizar->d->cargo) + 2;
            tamanho_registro -= tam_cargo;
            tamanho_registro += aux_atualizar->d->tamanhocargo; //Atualiza o tamanho do registro
            int posicao_lista = remove_registro(bin, e, e->tipo, removidos); //Remove
            refaz_encad(bin, posicao_lista, removidos); //Refaz o encadeamento
            insere->id = id;
            insere->salario = salario;
            strcpy(insere->telefone, telefone);
            strcpy(insere->nome, "\0");
            strcpy(insere->cargo, aux_atualizar->d->cargo);
            Dados *d = cria_no_encontrar(insere);
            list_insertion_final_e(inserir, d);
            insere_registro(bin, removidos, inserir); //Insere no final
            remove_lista_e(0, inserir);
          }
        }
        else{//Se tiver so nome
          fseek(bin, -1, SEEK_CUR);
          int posicao_lista = remove_registro(bin, e, e->tipo, removidos); //Remove o registro
          refaz_encad(bin, posicao_lista, removidos); //Refaz o encadeamento
          insere->id = id;
          insere->salario = salario;
          strcpy(insere->telefone, telefone);
          strcpy(insere->nome, nome);
          strcpy(insere->cargo, aux_atualizar->d->cargo);
          Dados *d = cria_no_encontrar(insere);
          list_insertion_final_e(inserir, d);
          insere_registro(bin, removidos, inserir); //Insere no final
          remove_lista_e(0, inserir); //Remove da lista de serem inseridos
        }
      }
    }
    aux_atualizar = aux_atualizar->prox; //Vai pro proximo a ser atualizado
    aux_buscar = aux_buscar->prox;
  }
  free(e);
}

/**
 * Main
**/
int main(void){

  FILE* fp;
  Dados* pessoa;
  FILE* bin;
  int n_linhas;
  char um = '1';
  int opt;
  char nome1[100];
  scanf("%d", &opt);
  char status;
  char compara[50];
  char compara2[50];
  int n_remocoes;
  int n_insercoes;
  int n_atualizacoes;
  Removido *ja_removidos;
  bytes_restantes = 999999999;
  tam_ultimo_reg = 0;

  /*
  Switch case, no qual a opcao desejada (1, 2 ou 3 e' escolhida)
  */
  switch (opt){
    case 1:
      scanf("%s", nome1);
      fp = fopen(nome1, "r");
      if(fp == NULL){ 
        printf("Falha no carregamento do arquivo.\n"); 
        return 0;
      }
      bin = fopen("arquivoTrab1.bin", "wb+"); 
      if(bin == NULL){ 
        printf("Falha no carregamento do arquivo.\n"); 
        return 0;
      }
	  fread(&status, sizeof(char), 1, bin);
      if(status == '0'){
        printf("Falha no processamento do arquivo.\n"); 
        fclose(bin);
		return 0;
      }
	  rewind(fp);
      n_linhas = contar_linhas(fp);
      n_linhas--;
      rewind(fp);
      pessoa = ler_arquivo(fp, n_linhas, bin);
      escrever_binario(pessoa, bin, n_linhas);
      rewind(bin);
      fwrite(&um, sizeof(char), 1, bin);
      fclose(bin);
      printf("arquivoTrab1.bin");
      fclose(fp);
      break;
  
    case 2:
      scanf("%s", nome1);
      bin = fopen(nome1, "rb");
      if(bin == NULL){ 
        printf("Falha no processamento do arquivo.\n"); 
        return 0;
      }
      fread(&status, sizeof(char), 1, bin);
      if(status == '0'){
        printf("Falha no processamento do arquivo.\n"); 
        fclose(bin);
		return 0;
      }
      rewind(bin);
      fseek(bin, 32000, SEEK_SET);
      char c = fgetc(bin);
      if(c == EOF){
        printf("Registro inexistente.\n"); 
        return 0;
      }
      rewind(bin);
      printa_binario(bin);
      rewind(bin);
      fwrite(&um, sizeof(char), 1, bin);
      fclose(bin);
      break;

    case 3:
      scanf("%s", nome1);
      scanf("%s", compara);
      Encontrar* e = (Encontrar*) malloc(sizeof(Encontrar));
      scanf("%*c");
      bin = fopen(nome1, "rb");
      if(bin == NULL){ 
        printf("Falha no processamento do arquivo.\n"); 
        return 0;
      } 
      fread(&status, sizeof(char), 1, bin);
      if(status == '0'){
        printf("Falha no processamento do arquivo.\n"); 
		fclose(bin);
        return 0;
      }
      rewind(bin);
      if(strcmp(compara, "idServidor") == 0){
        int tipo = 0;
        scanf("%d", &e->id);
        le_cabecalho(e, bin, tipo, 3, NULL);
      }
      else if(strcmp(compara, "salarioServidor") == 0){
        int tipo = 1;
        scanf("%lf", &e->salario);
        le_cabecalho(e, bin, 1, 3, NULL);
      }
      else if(strcmp(compara, "telefoneServidor") == 0){
        int tipo = 2;
        scanf("%s", e->telefone);
        le_cabecalho(e, bin, 2, 3, NULL);
      }
      else if(strcmp(compara, "nomeServidor") == 0){
        int tipo = 3;
        scanf("%[^\n\r]", e->nome);
        le_cabecalho(e, bin, 3, 3, NULL);
      }
      else if(strcmp(compara, "cargoServidor") == 0){
        int tipo = 4;
        scanf("%[^\n\r]", e->cargo);
        le_cabecalho(e, bin, 4, 3, NULL);
      }			
      else {
        scanf("%*s");
		  	printf("Registro inexistente.\n");
	  	}

      rewind(bin);
      fwrite(&um, sizeof(char), 1, bin);
      fclose(bin);
      break;

      case 4:
        scanf("%s", nome1);
        scanf("%d", &n_remocoes);
        LIST *l = create_list();
        bin = fopen(nome1, "rb+");
        if(bin == NULL){ 
          printf("Falha no processamento do arquivo.\n"); 
          return 0;
        } 
        fread(&status, sizeof(char), 1, bin);
        if(status == '0'){
          printf("Falha no processamento do arquivo.\n"); 
          fclose(bin);
		  return 0;
        }
        rewind(bin);
        get_lista(bin, l);
        rewind(bin);
        for(int i = 0; i < n_remocoes; i++){
          scanf("%s", compara);
          Encontrar* e = (Encontrar*) malloc(sizeof(Encontrar));
          scanf("%*c");
          if(strcmp(compara, "idServidor") == 0){
            int tipo = 0;
            scanf("%d", &e->id);
            le_cabecalho(e, bin, tipo, 4, l);
          }
          else if(strcmp(compara, "salarioServidor") == 0){
            int tipo = 1;
            char teste[40];
            memset(teste, '\0', 40);
            scan_quote_string(teste);
            if(strcmp(teste, "\0") == 0){
              e->salario = -1;
            }
            else{
              e->salario = atof(teste);
            }
            le_cabecalho(e, bin, 1, 4, l);
          }
          else if(strcmp(compara, "telefoneServidor") == 0){
            int tipo = 2;
            scan_quote_string(e->telefone);
            le_cabecalho(e, bin, 2, 4, l);
          }
          else if(strcmp(compara, "nomeServidor") == 0){
            int tipo = 3;
            scan_quote_string(e->nome);
            le_cabecalho(e, bin, 3, 4, l);
          }
          else if(strcmp(compara, "cargoServidor") == 0){
            int tipo = 4;
            scan_quote_string(e->cargo);
            le_cabecalho(e, bin, 4, 4, l);
          }			
          else {
            scanf("%*s");
            printf("Registro inexistente.\n");
          }
          rewind(bin);
        }
        escreve_arquivo(bin, l);
        binarioNaTela1(bin);
        rewind(bin);
        fwrite(&um, sizeof(char), 1, bin);
		fclose(bin);
        apaga_lista(l);
      break;

      case 5:
        scanf("%s", nome1);
        scanf("%d", &n_insercoes);

        bin = fopen(nome1, "rb+");
        if(bin == NULL){ 
          printf("Falha no processamento do arquivo.\n"); 
          return 0;
        } 
        fread(&status, sizeof(char), 1, bin);
        if(status == '0'){
          printf("Falha no processamento do arquivo.\n"); 
          fclose(bin);
		  return 0;
        }

        LIST_E *h = create_list_e();
  
        for(int i = 0; i < n_insercoes; i++){
          Encontrar* e = (Encontrar*) malloc(sizeof(Encontrar));
          
          scanf("%d", &e->id);
          
          char teste[40];
          memset(teste, '\0', 40);
          scan_quote_string(teste);
          if(strcmp(teste, "\0") == 0){
            e->salario = -1;
          }
          else{
            e->salario = atof(teste);
          }

          scan_quote_string(e->telefone);
          scan_quote_string(e->nome);
          scan_quote_string(e->cargo);

          Dados *d;
          d = cria_no_encontrar(e);
          list_insertion_final_e(h, d);
        }

        LIST *a = create_list();

        get_lista(bin, a);
        insere_registro(bin, a, h);
        binarioNaTela1(bin);
		rewind(bin);
		fwrite(&um, sizeof(char), 1, bin);
		fclose(bin);
      break;

      case 6:
        scanf("%s", nome1);
        scanf("%d", &n_atualizacoes);

        LIST_E *encontrar = create_list_e();
        LIST_E *inserir = create_list_e();

        bin = fopen(nome1, "rb+");
        if(bin == NULL){ 
          printf("Falha no processamento do arquivo.\n"); 
          return 0;
        } 
        fread(&status, sizeof(char), 1, bin);
        if(status == '0'){
          printf("Falha no processamento do arquivo.\n"); 
          fclose(bin);
		  return 0;
        }
        for(int i = 0; i < n_atualizacoes; i++){
          compara_leitura(bin, encontrar, inserir);
        }
        LIST *removidos = create_list();
        get_lista(bin, removidos);
        atualiza_registros(bin, encontrar, inserir, removidos);
        binarioNaTela1(bin);
		rewind(bin);
		fwrite(&um, sizeof(char), 1, bin);
		fclose(bin);
      break;

  }
  return 0;
}