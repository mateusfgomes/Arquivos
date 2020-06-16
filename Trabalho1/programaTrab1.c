/*
Mateus Ferreira Gomes

Trabalho de Organização de Arquivos

Programa que trabalha com 3 funcoes
  Escreve um csv em bin
  Printa um binario na tela
  Faz busca em arquivo binario

Instituto de Ciencias Matematicas e de Computacao - Universidade de Sao Paulo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Estrutura do Registro de Dados
*/
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

/*
Estrutura de um registro de cabecalho
*/
typedef struct encontrar{

  int id;
  double salario;
  char telefone[14];
  char nome[100];
  char cargo[100];

}Encontrar;

/*
Funcao que libera os dados da memoria
que sao alocados
*/
void limpar_pessoa(Dados* pessoa, int n_linhas){

  for(int i = 0; i < n_linhas; i++){
    free(pessoa[i].telefone);
    free(pessoa[i].nome);
    free(pessoa[i].cargo);
  }
  free(pessoa);


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

/*
Funcao utilizada para printar a funcao 2 do registro, printando o arquivo binario
tanto pelo registro de cabecalho quanto por todos os registros de dados.
Ao fim, printa o numero de paginas de disco escritas na tela
*/
void printa_busca(FILE* bin, Encontrar* e, int tipo, char descampo1[40], char descampo2[40], char descampo3[40], char descampo4[40], char descampo5[40]){


  Dados* d = (Dados*) malloc(sizeof(Dados));
  char c;
  int reg_tam;
  long long int ignora;
  int id;
  double salario;
  char telefone[14];
  char nome[50];
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
      fread(&ignora, sizeof(long long int), 1, bin); 
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
  long long int ignora;
  int id;
  double salario;
  char telefone[14];
  char nome[50];
  int indicador_nome;
  char tagCampo4;
  int indicador_cargo;
  char tagCampo5;
  char cargo[100];
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
    if(c == '-'){ 
      fread(&reg_tam, sizeof(int), 1, bin);
      count += reg_tam + 5;
      fread(&ignora, sizeof(long long int), 1, bin); //ignora o encadeamento lista
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

/*
Funcao que le o cabecalho do arquivo binario e faz a busca pelo elemento que deseja-se
buscar dado pela funcao 3 deste trabalho
*/
void le_cabecalho(Encontrar* e, FILE* bin, int tipo){

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
  rewind(bin);
}


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
        return 0;
      }
      rewind(bin);
      if(strcmp(compara, "idServidor") == 0){
        int tipo = 0;
        scanf("%d", &e->id);
        le_cabecalho(e, bin, tipo);
      }
      else if(strcmp(compara, "salarioServidor") == 0){
        int tipo = 1;
        scanf("%lf", &e->salario);
        le_cabecalho(e, bin, 1);
      }
      else if(strcmp(compara, "telefoneServidor") == 0){
        int tipo = 2;
        scanf("%s", e->telefone);
        le_cabecalho(e, bin, 2);
      }
      else if(strcmp(compara, "nomeServidor") == 0){
        int tipo = 3;
        scanf("%[^\n\r]", e->nome);
        le_cabecalho(e, bin, 3);
      }
      else if(strcmp(compara, "cargoServidor") == 0){
        int tipo = 4;
        scanf("%[^\n\r]", e->cargo);
        le_cabecalho(e, bin, 4);
      }			
      else {
        scanf("%*s");
		  	printf("Registro inexistente.\n");
	  	}

      rewind(bin);
      fwrite(&um, sizeof(char), 1, bin);
      fclose(bin);
      break;
  }
  

  return 0;
}
