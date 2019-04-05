#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dados{
  double salario;
  int id;
  int tamanhonome;
  int tamanhocargo;
  int tamanhoregistro;
  char* telefone;
  char* nome;
  char* cargo;
}Dados;

void limpar_pessoa(Dados** pessoa, int n_linhas){

  for(int i = 0; i < n_linhas; i++){
    free(pessoa[i]->telefone);
    //free(pessoa[i]->nome);
    //free(pessoa[i]->cargo);
    free(pessoa[i]);
  }
  free(pessoa);


}


void reset_var(Dados** pessoa, int p){

  pessoa[p]->telefone[0] = '\0';
  for (int i = 1; i < 14; i++) {
    pessoa[p]->telefone[i] = '@';
  }

  strcpy(pessoa[p]->nome, "\0");
  strcpy(pessoa[p]->cargo, "\0");


}

int contar_linhas(FILE* fp){

  char aux;
  int count = 0;

  while(fscanf(fp, "%c", &aux) != EOF){
    if(aux == '\n')
      count++;
  }

  return count;
}

Dados** ler_arquivo(FILE* fp, int n_linhas){

  Dados** pessoa = (Dados**) malloc(n_linhas*(sizeof(Dados*)));
  for (int i = 0; i < n_linhas; i++) {
    pessoa[i] = (Dados*) malloc(sizeof(Dados));
    pessoa[i]->telefone = (char*) malloc(14*sizeof(char));
    pessoa[i]->nome = (char*) malloc(50*sizeof(char));
    pessoa[i]->cargo = (char*) malloc(100*sizeof(char));
  }
  char c;

  /*Ignorando a primeira linha*/
  while(1){
    fscanf(fp, "%c", &c);
    if(c == '\n') break;
  }

  for(int j = 0; j < n_linhas; j++){

    reset_var(pessoa, j);

    fscanf(fp, "%d", &pessoa[j]->id);
    fscanf(fp, "%*c");
    fscanf(fp, "%lf", &pessoa[j]->salario);
    fscanf(fp, "%*c");
    fscanf(fp, "%[^,]", pessoa[j]->telefone);
    fscanf(fp, "%*c");
    fscanf(fp, "%[^,]", pessoa[j]->nome);
    fscanf(fp, "%*c");
    fscanf(fp, "%[^\r\n]", pessoa[j]->cargo);
    fscanf(fp, "%*c");

    // printf("%ld\n", strlen(pessoa[j]->nome));
    // printf("%ld\n", strlen(pessoa[j]->cargo));

		// printf("%d\n", pessoa[j]->id);
		// printf("%s\n", pessoa[j]->telefone);
		// printf("%.1lf\n", pessoa[j]->salario);
		// printf("%s\n", pessoa[j]->cargo);
		// printf("%s\n", pessoa[j]->nome);
    // printf("\n\n\n");

  }

  return pessoa;

}

void escrever_binario(Dados** vet, FILE* bin, int n_linhas){


  for(int i = 0; i < n_linhas; i++){
    vet[i]->tamanhonome = strlen(vet[i]->nome);
    vet[i]->tamanhocargo = strlen(vet[i]->cargo);
    vet[i]->tamanhoregistro = 49 + vet[i]->tamanhonome + vet[i]->tamanhocargo;
    fwrite(&vet[i]->tamanhoregistro, sizeof(int), 1, bin);
    fwrite(&vet[i]->id, sizeof(int), 1, bin);
    fwrite(&vet[i]->salario, sizeof(double), 1, bin);
    fwrite(&vet[i]->telefone, sizeof(char), 14, bin);
    fwrite(&vet[i]->tamanhonome, sizeof(int), 1, bin);
    fwrite(&vet[i]->nome, sizeof(char), strlen(vet[i]->nome), bin);
    fwrite(&vet[i]->tamanhocargo, sizeof(int), 1, bin);
    fwrite(&vet[i]->cargo, sizeof(char), strlen(vet[i]->cargo), bin);
  }

}

int main(void){

  //char s[50];
  //scanf("%s", s);
  FILE* fp;
  Dados** pessoa;
  FILE* bin;
  int n_linhas;

  fp = fopen("arquivo.csv", "r");
  bin = fopen("arquivo.bin", "wb+");
  n_linhas = contar_linhas(fp);
  n_linhas--;

  rewind(fp);

  pessoa = ler_arquivo(fp, n_linhas);
  escrever_binario(pessoa, bin, n_linhas);


  limpar_pessoa(pessoa, n_linhas);
  fclose(fp);
  fclose(bin);

  return 0;
}
