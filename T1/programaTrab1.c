#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dados{
  double salario;
  long long int encadeamentolista;
  char telefone[14];
  char nome[100];
  char cargo[100];
  int id;
  int tamanhonome;
  int tamanhocargo;
  int tamanhoregistro;
  char removido;
  char campo4;
  char campo5;
}Dados;

void limpar_pessoa(Dados* pessoa, int n_linhas){

  for(int i = 0; i < n_linhas; i++){
    free(pessoa[i].telefone);
    free(pessoa[i].nome);
    free(pessoa[i].cargo);
  }
  free(pessoa);


}


void reset_var(Dados* pessoa, int p){

  memset(pessoa[p].telefone, '@', 14);


  strcpy(pessoa[p].nome, "\0");
  strcpy(pessoa[p].cargo, "\0");


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

Dados* ler_arquivo(FILE* fp, int n_linhas){

  Dados* pessoa = (Dados*) malloc(n_linhas*(sizeof(Dados)));
  /*for (int i = 0; i < n_linhas; i++) {
    pessoa[i].telefone = (char*) malloc(14*sizeof(char));
    pessoa[i].nome = (char*) malloc(50*sizeof(char));
    pessoa[i].cargo = (char*) malloc(100*sizeof(char));
  }*/
  char c;

  /*Ignorando a primeira linha*/
  while(1){
    fscanf(fp, "%c", &c);
    if(c == '\n') break;
  }

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

void reg_cabecalho(FILE* bin, int n_linhas){

  char abre = '0';
  long long int topoLista = -1;
  char tagCampo1 = 'i';
  char desCampo1[40] = "numero de identificacao do servidor";
  char tagCampo2 = 's';
  char desCampo2[40] = "salario do servidor";
  char tagCampo3 = 't';
  char desCampo3[40] = "telefone celular do servidor";
  char tagCampo4 =  'n';
  char desCampo4[40] = "nome do servidor";
  char tagCampo5 = 'c';
  char desCampo5[40] = "cargo do servidor";
  char especial = '@';

  fwrite(&abre, sizeof(char), 1, bin);
  fwrite(&topoLista, sizeof(long long int), 1, bin);
  fwrite(&tagCampo1, sizeof(char), 1, bin);
  fwrite(&desCampo1, sizeof(char), 36, bin);
  for(int i = 0; i < 4; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo2, sizeof(char), 1, bin);
  fwrite(&desCampo2, sizeof(char), 20, bin);
  for(int i = 0; i < 20; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo3, sizeof(char), 1, bin);
  fwrite(&desCampo3, sizeof(char), 29, bin);
  for(int i = 0; i < 11; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo4, sizeof(char), 1, bin);
  fwrite(&desCampo4, sizeof(char), 17, bin);
  for(int i = 0; i < 23; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
  fwrite(&tagCampo5, sizeof(char), 1, bin);
  fwrite(&desCampo5, sizeof(char), 18, bin);

  for(int i = 0; i < 31808; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }
}

void preenche_arroba(int restante, FILE* bin, int* contador){

  char especial = '@';

  for(int i = 0; i < restante; i++){
    fwrite(&especial, sizeof(char), 1, bin);
  }

  *contador = 0;

}

void escrever_binario(Dados* vet, FILE* bin, int n_linhas){

  reg_cabecalho(bin, n_linhas);
  int count_pag = 0;

  for(int i = 0; i < n_linhas; i++){

    vet[i].campo5 = 'c';
    vet[i].campo4 = 'n';
    vet[i].removido = '-';
    vet[i].encadeamentolista = -1;
    vet[i].tamanhonome = ((strlen(vet[i].nome)) + 1);
    vet[i].tamanhocargo = ((strlen(vet[i].cargo)) + 1);
    vet[i].tamanhoregistro = (49 + vet[i].tamanhonome + vet[i].tamanhocargo);
    if(vet[i].tamanhonome == 1){
      vet[i].tamanhoregistro -= 6;
    }
    if(vet[i].tamanhocargo == 1){
      vet[i].tamanhoregistro -= 6;
    }
    if(count_pag + vet[i].tamanhoregistro > 32000)
      preenche_arroba(32000 - count_pag, bin, &count_pag);

    count_pag += vet[i].tamanhoregistro;
    fwrite(&(vet[i].removido), sizeof(char), 1, bin);
    fwrite(&(vet[i].tamanhoregistro), sizeof(int), 1, bin);
    fwrite(&(vet[i].encadeamentolista), sizeof(long long int), 1, bin);
    fwrite(&(vet[i].id), sizeof(int), 1, bin);
    //if(vet[i].salario == 0) vet[i].salario = -1;
    fwrite(&(vet[i].salario), sizeof(double), 1, bin);
    fwrite(&(vet[i].telefone),sizeof(char),14,bin);
    if(vet[i].tamanhonome > 1) fwrite(&(vet[i].tamanhonome), sizeof(int), 1, bin);
    if(vet[i].tamanhonome > 1) fwrite(&(vet[i].campo4), sizeof(char), 1, bin);
    if(vet[i].tamanhonome > 1) fwrite(&(vet[i].nome), sizeof(char), vet[i].tamanhonome, bin);
    if(vet[i].tamanhocargo > 1) fwrite(&(vet[i].tamanhocargo), sizeof(int), 1, bin);
    if(vet[i].tamanhocargo > 1) fwrite(&(vet[i].campo5), sizeof(char), 1, bin);
    if(vet[i].tamanhocargo > 1) fwrite(&(vet[i].cargo), sizeof(char), vet[i].tamanhocargo, bin);


    // printf("%d\n", vet[i].id);
		// printf("%s\n", vet[i].telefone);
		// printf("%.1lf\n", vet[i].salario);
		// printf("%s\n", vet[i].cargo);
		// printf("%s\n", vet[i].nome);
    // printf("\n\n\n\n");
  }

  preenche_arroba(32000-count_pag, bin, &count_pag);
}


int main(void){

  //char s[50];
  //scanf("%s", s);
  FILE* fp;
  Dados* pessoa;
  FILE* bin;
  int n_linhas;
  char um = '1';

  fp = fopen("arquivo.csv", "r");
  bin = fopen("arquivo.bin", "wb+");
  n_linhas = contar_linhas(fp);
  n_linhas--;

  rewind(fp);

  pessoa = ler_arquivo(fp, n_linhas);
  escrever_binario(pessoa, bin, n_linhas);


  //limpar_pessoa(pessoa, n_linhas);
  rewind(bin);
  fwrite(&um, sizeof(char), 1, bin);
  fclose(fp);
  fclose(bin);

  return 0;
}
