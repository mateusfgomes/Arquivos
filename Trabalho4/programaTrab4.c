
/*
   Disciplina: SCC0215 - Organizacao de Arquivos - Profa. Dra. Cristina D. A. Ciferri
   Quarto Trabaho Pratico

   Nome: Guilherme Targon Marques Barcellos
   Num. USP: 10724181

   Nome: Mateus Ferreira Gomes
   Num. USP: 10734773	
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Struct "DADOS" utilizada para armazenar os dados de um registro de dados.
typedef struct Dados{
	int id;
	double salario;
	char telefone[15];
	int tamanhoNome;
	char nome[150];
	int tamanhoCargo;
	char cargo[150];
	int tamanhoRegistro;
}Dados;

//Struct "REGISTROCABECALHO" utilizada para armazenar os campos do cabecalho.
typedef struct registroCabecalho{
	char desCampo1[40];
	char desCampo2[40];
	char desCampo3[40];
	char desCampo4[40];
	char desCampo5[40];
}registroCabecalho;

//Struct "DADOSBUSCA" utilizado para armazenar variaveis a serem buscadas.
typedef struct dadosBusca{
	int idBuscado;
	double salarioBuscado;
	char telefoneBuscado[14];
	char nomeBuscado[150];
	char cargoBuscado[150];
}dadosBusca;


typedef struct registroIndice{
	char chaveBusca[120];
	long long int byteOffset;
}registroIndice;

//Struct "NO" que sera utilizada na lista encadeada. Contem byteoffser e tamaho registro(cado no eh um registro removido).
typedef struct no NO;
struct no{
	long long int byteoffset;
	int tamReg;
	NO* prox;
};

//Struct "LISTA", armazena quantidade de registros removidos + ponterio inicio da lista e fim.
typedef struct LISTA{
	int quantidade;
	NO* inicio;
	NO* fim;
}LISTA;


//Cria e retorna uma lista vazia.
LISTA* cria_lista(){
	LISTA* l = (LISTA*) malloc(sizeof(LISTA));
	if(l != NULL){
		l->quantidade = 0;
		l->inicio = NULL;
		l->fim = NULL;
		return l;
	}
	return NULL;
}

//Cria e retorna um "NO" de acordo com parametro recebido.
NO* cria_no(long long int byteoff, int tam){
	NO* n = (NO*) malloc(sizeof(NO));
	if(n != NULL){
		n->byteoffset = byteoff;
		n->tamReg = tam;
		n->prox = NULL;
		return n;
	}
	return NULL;
}

//Esta funcao adiciona um no de forma ordenada na lista. Mesmo tamanho entra antes.
void adiciona_ordenado_lista(LISTA* l, NO* no){
	if(l->quantidade == 0){
		l->inicio = no;
		l->fim = no;
		l->quantidade++;
		return;
	}
	if(no->tamReg > l->fim->tamReg){
		l->fim->prox = no;
		l->fim = no;
		l->quantidade++;
		return;
	}
	if(no->tamReg <= l->inicio->tamReg){
		no->prox = l->inicio;
		l->inicio = no;
		l->quantidade++;
		return;
	}
	NO* aux = l->inicio;
	while(1){
		
		if(aux->prox->tamReg >= no->tamReg){
			no->prox = aux->prox;
			aux->prox = no;
			l->quantidade++;
			return;
		}
		aux = aux->prox;
	}
}

//Esta funcao adiciona um no de forma ordenada na lista. Mesmo tamanho entra depois.
void adiciona_ordenado_lista_final(LISTA* l, NO* no){
	if(l->quantidade == 0){
		l->inicio = no;
		l->fim = no;
		l->quantidade++;
		return;
	}
	if(no->tamReg >= l->fim->tamReg){
		l->fim->prox = no;
		l->fim = no;
		l->quantidade++;
		return;
	}
	if(no->tamReg < l->inicio->tamReg){
		no->prox = l->inicio;
		l->inicio = no;
		l->quantidade++;
		return;
	}
	NO* aux = l->inicio;
	while(1){
		if(aux->prox->tamReg > no->tamReg){
			no->prox = aux->prox;
			aux->prox = no;
			l->quantidade++;
			return;
		}
		aux = aux->prox;
	}
}

//Funcao para recuperar a lista de removidos que esta contida no arquivo binario(caso exista uma).
void recupera_lista_em_disco(FILE* arqB, LISTA* list){

	int t = 0; //Tamanho do registro
	long long int BO = 0; //Byteoffset do registro
	
	fseek(arqB, 1, SEEK_SET);
	fread(&BO,sizeof(long long int), 1, arqB);

	while(BO != -1){
		fseek(arqB, (BO+1), SEEK_SET);
		fread(&t, sizeof(int), 1, arqB);
		NO* aux = cria_no(BO, t);	
		adiciona_ordenado_lista_final(list, aux);
		fread(&BO,sizeof(long long int), 1, arqB);
	}
	
}

//Funcao que libera memoria(NÓS + LISTA).
void free_lista(LISTA* list){

	if(list->quantidade == 0){
		free(list);
		return;
	}
	NO* aux = list->inicio;
	while(aux != NULL){
		list->inicio = aux->prox;
		free(aux);
		aux = list->inicio;
	}
	free(list);
}

//Dado um byteoffset, remove da lista o nó que contem este byteoofset.
void remove_no_lista(LISTA* list, long long int BO){
	NO* aux;
	NO* aux2;
	if(list->quantidade == 0){
		return;
	}
	if(list->inicio->byteoffset == BO ){
		aux = list->inicio;
		list->inicio = list->inicio->prox;
		free(aux);
		list->quantidade--;
		return;
	}
	if(list->quantidade == 1){
		return;
	}
	aux = list->inicio;
	while(aux->prox != NULL){
		if(aux->prox->byteoffset == BO){
			if(aux->prox == list->fim){
				list->fim = aux;
			}
			aux2 = aux->prox;
			aux->prox = aux2->prox;
			free(aux2);
			list->quantidade--;
			break;
		}
		aux = aux->prox;
	}
	
}

//Realiza a busca binaria, procurando por uma chave do tipo STRING. Caso encontre, retorna posicao no
//vetor. Caso contrário, retorna -1.
int busca_binaria(registroIndice *reg, int l, int r, char *chave) { 
    if (r >= l) { 
        int m = l + (r - l) / 2; 
        if (strcmp(reg[m].chaveBusca,chave) == 0) return m; 
		if (strcmp(reg[m].chaveBusca,chave) > 0) return busca_binaria(reg, l, m - 1, chave); 
        return busca_binaria(reg, m + 1, r, chave); 
    } 
    return -1; 
} 

int string_compare(const void *A, const void *B) {
      registroIndice *pA, *pB;
      pA = (registroIndice *) A;
      pB = (registroIndice *) B;
      if(strcmp(pB->chaveBusca, pA->chaveBusca) == 0){
      	return (int) pB->byteOffset - pA->byteOffset;
      }
      return strcmp(pB->chaveBusca, pA->chaveBusca);
}

int int_compare(const void *A, const void *B) {
      Dados *pA, *pB;
      pA = (Dados *) A;
      pB = (Dados *) B;
      return pB->id - pA->id;
}

void MS_sort(void *vector, unsigned long n, size_t memsize, int (*fcmp)(const void *, const void *)) {
	unsigned long middle, rN, j, k;
	void *aux, *r;

	if(n < 2) {
		return;
	}

	middle = (n / 2);
	r = vector + middle * memsize;
	rN = n - middle;
	MS_sort(vector, middle, memsize, fcmp);
	MS_sort(r, rN, memsize, fcmp);

	aux = (void *) malloc(memsize * n);
	j = k = 0;
	while(j + k < n) {
		if(k >= rN || (j < middle && fcmp(vector + memsize * j, r + memsize * k) >= 0)) {
			memcpy(aux + memsize * (j + k), vector + memsize * j, memsize);
			j++;
		} else {
			memcpy(aux + memsize * (j + k), r + memsize * k, memsize);
			k++;
		}
	}
	memcpy(vector, aux, memsize * n);
	free(aux);
}

void binarioNaTela1(FILE *ponteiroArquivoBinario) {

	/* Escolha essa função se você ainda tem o ponteiro de arquivo 'FILE *' aberto.
	*  Lembrando que você tem que ter aberto ele no fopen para leitura também pra funcionar (exemplo: rb, rb+, wb+, ...) */

	unsigned char *mb;
	unsigned long i;
	size_t fl;
	fseek(ponteiroArquivoBinario, 0, SEEK_END);
	fl = ftell(ponteiroArquivoBinario);
	fseek(ponteiroArquivoBinario, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, ponteiroArquivoBinario);
	for(i = 0; i < fl; i += sizeof(unsigned char)) {
		printf("%02X ", mb[i]);
		if((i + 1) % 16 == 0)	printf("\n");
	}
	free(mb);
}

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}

//Funcao que escreve apenas o registro de cabecalho no arquio binario.
void escreve_cab_binario(registroCabecalho cab, FILE* arqBinario){
	// Variaveis do cabecalho
	char numUm = '1';
	char numZero = '0';
	long long int topoLista = -1;
	char tagCampo1 = 'i';
	char arroba = '@'; 
	char tagCampo2 = 's';
	char tagCampo3 = 't';
	char tagCampo4 = 'n';
	char tagCampo5 = 'c';
	
	//Comeca a gravacao do cabecalho.
	fwrite(&numZero,sizeof(char),1,arqBinario); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.
	fwrite(&topoLista,sizeof(long long int),1,arqBinario);
	fwrite(&tagCampo1,sizeof(char),1,arqBinario);
	int tamCab = strlen(cab.desCampo1) + 1; //+1 do barra zero.
	fwrite(&cab.desCampo1,sizeof(char),tamCab,arqBinario); //Campo 1 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo2,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo2) + 1;
	fwrite(&cab.desCampo2,sizeof(char),tamCab,arqBinario); //Campo 2 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo3,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo3) + 1;
	fwrite(&cab.desCampo3,sizeof(char),tamCab,arqBinario); //Campo 3 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo4,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo4) + 1;
	fwrite(&cab.desCampo4,sizeof(char),tamCab,arqBinario); //CAmpo 4 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo5,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo5) + 1;
	fwrite(&cab.desCampo5,sizeof(char),tamCab,arqBinario); //Campo 5 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}

	
	for(int i = 0; i<31786; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	//Fim da gravacao do cabecalho.
}

//Funcao que escreve os regisotr em um arquivo binario. Recebe os registros, quantidade de regisotros e o cabecalho.
void escreve_binario(Dados* lido, int quantidade, registroCabecalho cab, FILE* arqBinario){



	// Variaveis do cabecalho
	char numUm = '1';
	char numZero = '0';
	long long int topoLista = -1;
	char tagCampo1 = 'i';
	char arroba = '@'; 
	char tagCampo2 = 's';
	char tagCampo3 = 't';
	char tagCampo4 = 'n';
	char tagCampo5 = 'c';
	
	//Comeca a gravacao do cabecalho.
	fwrite(&numZero,sizeof(char),1,arqBinario); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.
	fwrite(&topoLista,sizeof(long long int),1,arqBinario);
	fwrite(&tagCampo1,sizeof(char),1,arqBinario);
	int tamCab = strlen(cab.desCampo1) + 1; //+1 do barra zero.
	fwrite(&cab.desCampo1,sizeof(char),tamCab,arqBinario); //Campo 1 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo2,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo2) + 1;
	fwrite(&cab.desCampo2,sizeof(char),tamCab,arqBinario); //Campo 2 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo3,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo3) + 1;
	fwrite(&cab.desCampo3,sizeof(char),tamCab,arqBinario); //Campo 3 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo4,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo4) + 1;
	fwrite(&cab.desCampo4,sizeof(char),tamCab,arqBinario); //CAmpo 4 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	fwrite(&tagCampo5,sizeof(char),1,arqBinario);
	tamCab = strlen(cab.desCampo5) + 1;
	fwrite(&cab.desCampo5,sizeof(char),tamCab,arqBinario); //Campo 5 do cabecalho.
	tamCab = 40 - tamCab;
	for(int i = 0; i<tamCab; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}

	
	for(int i = 0; i<31786; i++){
		fwrite(&arroba,sizeof(char),1,arqBinario);
	}
	//Fim da gravacao do cabecalho.

	

	char removido = '-';
	long long int encadeamento = -1;
	char delimitadorN = 'n';
	char delimitadorC = 'c';
	char barraZero = '\0';

	

	int qntBytesEscritos = 0; //COntrole da pagina de disco atual.
	int faltantePagina = 0; //Quantidade de arrobas a serem adicionaods final de uma pagina de disco.

	

	for(int i = 0; i< quantidade; i++){
		
		if(lido[i].tamanhoNome == 2){ //Entra aqui se nao tiver nome
			lido[i].tamanhoRegistro = lido[i].tamanhoRegistro - 6;  //Necessario remover os bytes --> 4 bytes tamanhoNome + 1 byte tagNome + 1 byte barrazero
		}
		if(lido[i].tamanhoCargo == 2){ //Entra aqui se nao tiver cargo.
			lido[i].tamanhoRegistro = lido[i].tamanhoRegistro - 6; //Necessario remover os bytes --> 4 bytes tamanhoCargo + 1 byte tagCargo + 1 byte barrazero
		}
		//Caso nao seja o ultimo registro E caso nao seja possivel gravar dois registro(pois segundo iria exceder pagina de disco)
		if(i != (quantidade-1) && (qntBytesEscritos + lido[i].tamanhoRegistro + lido[i+1].tamanhoRegistro + 10) > 32000){
			qntBytesEscritos = qntBytesEscritos + lido[i].tamanhoRegistro + 5;
			faltantePagina = 32000 - qntBytesEscritos; //Quantidade arrobas para preencer a pag de disco.
			lido[i].tamanhoRegistro = lido[i].tamanhoRegistro + faltantePagina; //Adiciona os arrobas no tamanho do ultimo registro da pagina de disco
			fwrite(&removido,sizeof(char),1,arqBinario); //Removido(1 byte = '-')
			fwrite(&lido[i].tamanhoRegistro,sizeof(int),1,arqBinario);	//Indicador tamanho: tamRegistro
			fwrite(&encadeamento,sizeof(long long int),1,arqBinario);//Encadeamento lista
			fwrite(&lido[i].id,sizeof(int),1,arqBinario);   //id
			fwrite(&lido[i].salario,sizeof(double),1,arqBinario);	//salario
			if(lido[i].telefone[0] == '@' ){
				fwrite(&barraZero,sizeof(char),1,arqBinario);
				for(int l = 0; l<13; l++){
					fwrite(&arroba,sizeof(char),1,arqBinario);
				}
			}
			else{
				fwrite(&lido[i].telefone,sizeof(char),14,arqBinario); //telefone
			}
			if(lido[i].tamanhoNome > 0){
				fwrite(&lido[i].tamanhoNome,sizeof(int),1,arqBinario);	 //tamanho nome
				fwrite(&delimitadorN,sizeof(char),1,arqBinario); //TagCampo4 = 'n'
				lido[i].tamanhoNome--;
				fwrite(&lido[i].nome,sizeof(char),lido[i].tamanhoNome,arqBinario); //nome
			}
			if(lido[i].tamanhoCargo > 0){
				fwrite(&lido[i].tamanhoCargo,sizeof(int),1,arqBinario);	//tamanho cargo
				fwrite(&delimitadorC,sizeof(char),1,arqBinario); //TagCampo5 = 'c'
				lido[i].tamanhoCargo--;
				fwrite(&lido[i].cargo,sizeof(char),lido[i].tamanhoCargo,arqBinario); //cargo
			}
			for(int k = 0; k<faltantePagina; k++){ //Preenche com arroba
				fwrite(&arroba,sizeof(char),1,arqBinario);
			}
			qntBytesEscritos = 0; //Zera variavel de controle da pagina de disco.
			faltantePagina = 0; //Zera faltante pagina.
			
		}
		//Caso gravacao dos dois proximos nao exceda a pag de disco OU ultimo registro.
		else{
			qntBytesEscritos = qntBytesEscritos + lido[i].tamanhoRegistro + 5;
			fwrite(&removido,sizeof(char),1,arqBinario); //Removido(1 byte = '-')
			fwrite(&lido[i].tamanhoRegistro,sizeof(int),1,arqBinario);	//Indicador tamanho: tamRegistro
			fwrite(&encadeamento,sizeof(long long int),1,arqBinario);//Encadeamento lista
			fwrite(&lido[i].id,sizeof(int),1,arqBinario);   //id
			fwrite(&lido[i].salario,sizeof(double),1,arqBinario);	//salario
			if(lido[i].telefone[0] == '@' ){
				fwrite(&barraZero,sizeof(char),1,arqBinario);
				for(int z = 0; z<13; z++){
					fwrite(&arroba,sizeof(char),1,arqBinario);
				}
			}
			else{
				fwrite(&lido[i].telefone,sizeof(char),14,arqBinario); //telefone
			}
			if(lido[i].tamanhoNome > 0){
				fwrite(&lido[i].tamanhoNome,sizeof(int),1,arqBinario);	 //tamanho nome
				fwrite(&delimitadorN,sizeof(char),1,arqBinario); //TagCampo4 = 'n'
				lido[i].tamanhoNome--;
				fwrite(&lido[i].nome,sizeof(char),lido[i].tamanhoNome,arqBinario); //nome
			}
			if(lido[i].tamanhoCargo > 0){
				fwrite(&lido[i].tamanhoCargo,sizeof(int),1,arqBinario);	//tamanho cargo
				fwrite(&delimitadorC,sizeof(char),1,arqBinario); //TagCampo5 = 'c'
				lido[i].tamanhoCargo--;
				fwrite(&lido[i].cargo,sizeof(char),lido[i].tamanhoCargo,arqBinario); //cargo
			}

			
		}
	}

	

	rewind(arqBinario); //Volta inicio arquivo.
	fwrite(&numUm,sizeof(char),1,arqBinario); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	//fclose(arqBinario); //Fechando arquivo.
	free(lido); //Liberando memoria alocado para os registros.
	//printf("arquivoTrab1.bin");
	
}

//Funcao que realiza leitura do arquivo csv. No final, chama uma funcao para escrever em um arquivo binario, passando os dados lidos aqui.
void leitura_csv(){

	char s[30];
	scanf("%s", s);
	FILE* arq;
	arq = fopen(s, "r");

	if(arq == NULL){ //Caso aconteca algum erro na abertura do arquivo.
		printf("Falha no carregamento do arquivo.\n");
		return;
	}
	
	Dados *leituraDados;
	
	int numeroLinhas = 0;      //Total de linhas = Total de "Pessoas" no arquivo csv.
	char caracter;
	while(fscanf(arq, "%c", &caracter) !=EOF){
		if(caracter == '\n')numeroLinhas++; //Cada \n eh uma quebra de linha, ou seja, um registro de dados.
	}

	numeroLinhas--;  //Desconsiderando a primeira linha do arquivo read_csv("idServidor,salario,telefone,nome,cargo").
	
	leituraDados = (Dados*) malloc (numeroLinhas * sizeof(Dados));

	rewind(arq);  //Voltando para inicio do arquivo.

	
	//Realiza a leitura do cabecalho e armazena em "cab".
	registroCabecalho cab;
	fscanf(arq, "%[^,]", cab.desCampo1);
	fscanf(arq, "%*c");
	fscanf(arq, "%[^,]", cab.desCampo2);
	fscanf(arq, "%*c");
	fscanf(arq, "%[^,]", cab.desCampo3);
	fscanf(arq, "%*c");
	fscanf(arq, "%[^,]", cab.desCampo4);
	fscanf(arq, "%*c");
	fscanf(arq, "%[^\r\n]", cab.desCampo5);
	fscanf(arq, "%*c");
	

	//Comeca a leitura dos dados campo a campo.
	for(int i = 0; i<numeroLinhas; i++ ) {

		 
		strcpy(leituraDados[i].nome, "\0");
		strcpy(leituraDados[i].cargo, "\0");
		strcpy(leituraDados[i].telefone, "@@@@@@@@@@@@@@");

		fscanf(arq, "%d", &leituraDados[i].id);
		fscanf(arq, "%*c");
		fscanf(arq, "%lf", &leituraDados[i].salario);
		fscanf(arq, "%*c");
		fscanf(arq, "%[^,]", leituraDados[i].telefone);
		fscanf(arq, "%*c");
		fscanf(arq, "%[^,]", leituraDados[i].nome);
		fscanf(arq, "%*c");
		fscanf(arq, "%[^\r\n]", leituraDados[i].cargo);
		fscanf(arq, "%*c");

		leituraDados[i].tamanhoNome = ((strlen(leituraDados[i].nome)) + 2); //+1 pois strlen nao conta '\0' e + 1 para tag do campo.
		leituraDados[i].tamanhoCargo = ((strlen(leituraDados[i].cargo)) + 2); //+1 pois strlen nao conta '\0' e + 1 para tag do campo.
		leituraDados[i].tamanhoRegistro = (42 + leituraDados[i].tamanhoNome + leituraDados[i].tamanhoCargo);  //Alem do tamanho nome/campo, contabilizando outras variaveis(descrito na especificacao).
	}

		fclose(arq);  //Fechando o arquivo.

		FILE *arqBinario = NULL;

		arqBinario = fopen("arquivoTrab1.bin", "wb+");

		escreve_binario(leituraDados, numeroLinhas, cab, arqBinario);  //Chama a funcao para escrever em um arquivo binario, passando todos os dados lidos.
		fclose(arqBinario);
		printf("arquivoTrab1.bin");
}

//Funcao que le arquivo binario e printa na tela os registros.
void leitura_binario(){


	char s[30];
	scanf("%s", s);
	FILE* arqB;
	arqB = fopen(s, "rb");
	if(arqB == NULL){ //Caso aconteca algum erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Conforme especificacao, verifica se o arquivo binario esta "corrompido".
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	

	rewind(arqB); //Volta comeco do arquivo.
	fseek(arqB, 32000, SEEK_SET); //Avanca 32k bytes, pulando a primeira pagina de disco que contem apenas o cabecario + arrobas.

	
	lixo = fgetc(arqB);
	if(lixo == EOF ){ //Caso nao exista nenhum registro no arquivo binario.
		printf("Registro inexistente.\n");
		return;
	}

	rewind(arqB);
	fseek(arqB, 32000, SEEK_SET);

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000; //Variavel que controla nunero total de bytes lidos(comeca com 32000 por causa da primeira pag disco)
	int auxBytes = 0; //Variavel para controle da pagina de disco atual.

	fread(&lixo, sizeof(char), 1, arqB);

	//Comeco da leitura dos dados do arquivo binario.
	while(lixo == '-' || lixo == '@' || lixo == '*'){
		if(lixo == '*'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);
			if(auxBytes == 32000)auxBytes = 0;
		}
		else if(lixo == '-'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Caso entre aqui, nao tem nome nem cargo declarados.
				if(printar.salario == -1 && printar.telefone[0] == '\0' ){
					printf("%d                        \n", printar.id);
				}
				else if(printar.salario == -1){//Salario nao declarado.
					printf("%d          %s\n", printar.id, printar.telefone);
				}
				else if(printar.telefone[0] == '\0'){ //Telefone nao declarado.
					printf("%d %.2lf               \n", printar.id, printar.salario);
				}
				else{
					printf("%d %.2lf %s\n", printar.id, printar.salario, printar.telefone);
				}

			}



			else if(auxBytes == 32000){ // Caso esteja final pagina de disco
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@'){ //Se for arroba, nao tem nome nem cargo.
					if(printar.salario == -1 && printar.telefone[0] == '\0' ){
						printf("%d                        \n", printar.id);
					}
					else if(printar.salario == -1){
						printf("%d            %s\n", printar.id, printar.telefone);
					}
					else if(printar.telefone[0] == '\0'){
						printf("%d %.2lf               \n", printar.id, printar.salario);
					}
					else{
						printf("%d %.2lf %s\n", printar.id, printar.salario, printar.telefone);
					}
				}
				else{ //Pode ter nome ou cargo ou os dois.
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					if(printar.salario == -1 && printar.telefone[0] == '\0' ){
						printf("%d                         ", printar.id);
					}
					else if(printar.salario == -1){
						printf("%d          %s ", printar.id, printar.telefone);
					}
					else if(printar.telefone[0] == '\0'){
						printf("%d %.2lf                ", printar.id, printar.salario);
					}
					else{
						printf("%d %.2lf %s ", printar.id, printar.salario, printar.telefone);
					}
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == 'n'){ //Tag campo, com == 'n', tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@'){ //Tem apenas nome.
							printf("%d %s\n",printar.tamanhoNome-2 ,printar.nome);
						}
						else{ //Tem cargo tambem.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
							printf("%d %s %d %s\n",printar.tamanhoNome-2 ,printar.nome,printar.tamanhoCargo-2 ,printar.cargo);
						}

					}
					else{ //NAo tem nome mas tem cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						printf("%d %s\n",printar.tamanhoCargo-2, printar.cargo);
					}

				}
			}



			else{ //ENtra neste else caso NAO esteja no final pagina disco E tem nome ou cargo ou os dois.
				if(printar.salario == -1 && printar.telefone[0] == '\0' ){
					printf("%d                         ", printar.id);
				}
				else if(printar.salario == -1){
					printf("%d          %s ", printar.id, printar.telefone);
				}
				else if(printar.telefone[0] == '\0'){
					printf("%d %.2lf                ", printar.id, printar.salario);
				}
				else{
					printf("%d %.2lf %s ", printar.id, printar.salario, printar.telefone);
				}
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == 'n'){ //Tem nome.
					printar.tamanhoNome = tamAuxiliar;
					fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
					if(((printar.tamanhoRegistro - 38 - tamAuxiliar) > 0)){ //tem nome e cargo.
						fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						printf("%d %s %d %s\n",printar.tamanhoNome-2 ,printar.nome,printar.tamanhoCargo-2 ,printar.cargo);

					}
					else{ //Tem apenas nome.
						printf("%d %s\n",printar.tamanhoNome-2 ,printar.nome);
					}

				}

				else{ //Tem cargo mas n tem nome.
					printar.tamanhoCargo = tamAuxiliar;
					fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					printf("%d %s\n",printar.tamanhoCargo-2, printar.cargo);
				}
			}
			
		}
		
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Quando chegar final do arquivo sai do while.
		
	}

	
	
	if(bytes % 32000 == 0){ //Caso divisao seja exata
		printf("Número de páginas de disco acessadas: %lld\n", bytes / 32000);
	}
	else{ //Caso divisao nao seja exata, nao existe "meio acesso" a uma pagina de disco.
		printf("Número de páginas de disco acessadas: %lld\n", ((bytes / 32000)+1));
	}


	fclose(arqB); //Fecha arquivo.

}

//Funcionalidade 3. Realiza busca por determinado campo, definido pela "flag". Campo a ser buscado passado como parametro.
int funcionalidade3(FILE* arqB, int flag, int idBuscado, double salarioBuscado, char* telefoneBuscado, char *nomeBuscado, char *cargoBuscado, int flagFuncionalidade14){

	rewind(arqB); //Voltando inicio drquivo.
	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina disco.
	long long int found = 0;
	char lixo;

	lixo = fgetc(arqB);
	if(lixo == EOF ){ //Verificando se existe algum registro no arquivo.
		printf("Registro inexistente.\n");
		return found;
	}
	

	rewind(arqB); //Voltando inicio do arquivo binario.
	registroCabecalho cab; //Aqui serao armazenados os campos do cabecalho.
	char teste;

	fseek(arqB, 10, SEEK_SET);
	fscanf(arqB, "%[^@]", cab.desCampo1); //Leitura campo 1
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo2); //Leitura campo 2
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo3); //Leitura campo 3
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo4); //Leitura campo 4
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo5); //Leitura campo 5

	
	rewind(arqB); //Voltando inicio arquivo.

	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina de disco.

	//Esta funcao eh uma adaptacao da funcionalidade 2. Eh feita leitura do registro, sem printar. No final, vejo se o campoBuscado esta no registro atual.

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000;
	int auxBytes = 0;

	fread(&lixo, sizeof(char), 1, arqB);
	while(lixo == '-' || lixo == '@'){
		if(lixo == '-'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
				//Nao precisa fazer nada.
			}



			else if(auxBytes == 32000){ //FInal pagina de disco.
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@'){ //NAo tem nome nem cargo.
					//NAo precisa fazer nada.
				}

				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@'){ //Tem apenas nome.
							
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}

					}
					else{ //Tem so cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
			}



			else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
				
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == 'n'){ //Tem nome.
					printar.tamanhoNome = tamAuxiliar;
					fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
					if(((printar.tamanhoRegistro - 38 - tamAuxiliar) > 0)){ //Tem nome e cargo.
						fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						

					}
					
				}

				else{ //Tem apenas cargo.
					printar.tamanhoCargo = tamAuxiliar;
					fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
				}
			}

			if(flag == 1){ //Flag == 1 siginifica busca por ID
				if(idBuscado == printar.id){ //Caso idBuscado seja igual ao id do registro atual.
					printf("%s: %d\n",cab.desCampo1, printar.id);
					if(printar.salario == -1){
						printf("%s: valor nao declarado\n", cab.desCampo2);
					}
					else{
						printf("%s: %.2lf\n",cab.desCampo2, printar.salario);
					}
					if(printar.telefone[0] == '\0'){
						printf("%s: valor nao declarado\n", cab.desCampo3);
					}
					else{
						printf("%s: %s\n",cab.desCampo3, printar.telefone);
					}
					if(printar.tamanhoNome > 2){
						printf("%s: %s\n",cab.desCampo4, printar.nome);
					}
					else{
						printf("%s: valor nao declarado\n", cab.desCampo4);
					}
					if(printar.tamanhoCargo > 2){
						printf("%s: %s\n\n",cab.desCampo5, printar.cargo);
					}
					else{
						printf("%s: valor nao declarado\n\n", cab.desCampo5);
					}

					if(bytes % 32000 == 0){
						printf("Número de páginas de disco acessadas: %lld\n", bytes / 32000);
					}
					else{
						printf("Número de páginas de disco acessadas: %lld\n", ((bytes / 32000)+1));
					}
					return 1;	//No caso do id, nao é necessario continuar a busca, pois id nao repete.	
				}
			}



			else if(flag == 2){ //Flag == 2 siginifica busca por salario.
				if(salarioBuscado == printar.salario){ //Caso salarioBuscado seja igual ao salario do registro atual.
					found++;
					printf("%s: %d\n",cab.desCampo1 ,printar.id);
					
					printf("%s: %.2lf\n",cab.desCampo2, printar.salario);
					
					if(printar.telefone[0] == '\0'){
						printf("%s: valor nao declarado\n", cab.desCampo3);
					}
					else{
						printf("%s: %s\n",cab.desCampo3, printar.telefone);
					}
					if(printar.tamanhoNome > 2){
						printf("%s: %s\n",cab.desCampo4, printar.nome);
					}
					else{
						printf("%s: valor nao declarado\n", cab.desCampo4);
					}
					if(printar.tamanhoCargo > 2){
						printf("%s: %s\n\n",cab.desCampo5, printar.cargo);
					}
					else{
						printf("%s: valor nao declarado\n\n", cab.desCampo5);
					}
				}
			}



			else if(flag == 3){ //Flag == 3 siginifica busca por telefone.
				if(strcmp(printar.telefone, telefoneBuscado) == 0){ ////Caso telefoneBuscado seja igual ao telefone do registro atual.
					found++;
					printf("%s: %d\n",cab.desCampo1 ,printar.id);
					if(printar.salario == -1){
						printf("%s: valor nao declarado\n", cab.desCampo2);
					}
					else{
						printf("%s: %.2lf\n",cab.desCampo2, printar.salario);
					}
					
					printf("%s: %s\n",cab.desCampo3, printar.telefone);
					
					if(printar.tamanhoNome > 2){
						printf("%s: %s\n",cab.desCampo4, printar.nome);
					}
					else{
						printf("%s: valor nao declarado\n", cab.desCampo4);
					}
					if(printar.tamanhoCargo > 2){
						printf("%s: %s\n\n",cab.desCampo5, printar.cargo);
					}
					else{
						printf("%s: valor nao declarado\n\n", cab.desCampo5);
					}



				}
			}


			else if(flag == 4){ //Flag == 4 siginifica busca por nome.
				
				if(strcmp(printar.nome, nomeBuscado) == 0){ //Caso nomeBuscado seja igual ao nome do registro atual.
					found++;
					printf("%s: %d\n",cab.desCampo1 ,printar.id);
					if(printar.salario == -1){
						printf("%s: valor nao declarado\n", cab.desCampo2);
					}
					else{
						printf("%s: %.2lf\n",cab.desCampo2, printar.salario);
					}
					if(printar.telefone[0] == '\0'){
						printf("%s: valor nao declarado\n", cab.desCampo3);
					}
					else{
						printf("%s: %s\n",cab.desCampo3, printar.telefone);
					}
					
					printf("%s: %s\n",cab.desCampo4, printar.nome);
					
					
					if(printar.tamanhoCargo > 2){
						printf("%s: %s\n\n",cab.desCampo5, printar.cargo);
					}
					else{
						printf("%s: valor nao declarado\n\n", cab.desCampo5);
					}
				}
			}


			else if(flag == 5){ //Flag == 5 siginifica busca por cargo.
				if(strcmp(printar.cargo, cargoBuscado) == 0){ //Caso cargoBuscado seja igual ao cargo do registro atual.
					found++;
					printf("%s: %d\n",cab.desCampo1 ,printar.id);
					if(printar.salario == -1){
						printf("%s: valor nao declarado\n", cab.desCampo2);
					}
					else{
						printf("%s: %.2lf\n",cab.desCampo2, printar.salario);
					}
					if(printar.telefone[0] == '\0'){
						printf("%s: valor nao declarado\n", cab.desCampo3);
					}
					else{
						printf("%s: %s\n",cab.desCampo3, printar.telefone);
					}
					if(printar.tamanhoNome > 2){
						printf("%s: %s\n",cab.desCampo4, printar.nome);
					}
					else{
						printf("%s: valor nao declarado\n", cab.desCampo4);
					}
					
					printf("%s: %s\n\n",cab.desCampo5, printar.cargo);
					
				}
			}

			//Zerando o registro.
			printar.tamanhoNome = 0;
			printar.salario = -1;
			printar.telefone[0] = '\0';
			printar.nome[0] = '\0';
			printar.cargo[0] = '\0';
			printar.tamanhoCargo = 0;
		}
		
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Caso chege no final arquivo, break.
		
	}


	if(found == 0 && flagFuncionalidade14 == 0){ //Caso nao tenha sido encontrado nenhum registro com o campo buscado.
		printf("Registro inexistente.\n");
		return found;
	}
	else if(found == 0){
		printf("Registro inexistente.\n");
	}
	
	if(bytes % 32000 == 0){
		printf("Número de páginas de disco acessadas: %lld\n", bytes / 32000);
		return bytes / 32000;
	}
	else{
		printf("Número de páginas de disco acessadas: %lld\n", ((bytes / 32000)+1));
		return ((bytes / 32000)+1);
	}
	
}

//FUncao para buscar dado um campo e um valor de busca,
void busca_por_campo(){

	char s[30];
	scanf("%s", s);
	scanf("%*c"); //Lendo e ignorando espaco.
	char campoBuscado[40]; //Campo a ser buscado.
	scanf("%s", campoBuscado);
	scanf("%*c"); //Lendo e ignorando espaco.
	int flag = -1;
	int found = 0; //Controlar se encontrou algum registro que corresponde a busca.
	int idBuscado;
	double salarioBuscado;
	char telefoneBuscado[14];
	char nomeBuscado[100];
	char cargoBuscado[100];

	//Aqui verifica qual o campo a ser buscados, utilizando uma flag para sinalizar.
	if(strcmp(campoBuscado, "idServidor") == 0){
		scanf("%d", &idBuscado);
		flag = 1;
	}
	else if(strcmp(campoBuscado, "salarioServidor") == 0){
		scanf("%lf", &salarioBuscado);
		flag = 2;
	}
	else if(strcmp(campoBuscado, "telefoneServidor") == 0){
		scanf("%s", telefoneBuscado);
		flag = 3;
	}
	else if(strcmp(campoBuscado, "nomeServidor") == 0){
		scanf("%[^\n\r]", nomeBuscado);
		flag = 4;
	}
	else if(strcmp(campoBuscado, "cargoServidor") == 0){
		scanf("%[^\n\r]", cargoBuscado);
		flag = 5;
	}

	FILE* arqB;
	arqB = fopen(s, "rb");
	if(arqB == NULL){ //Verificanndo se houve eroo na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	

	found = funcionalidade3(arqB,flag,idBuscado,salarioBuscado,telefoneBuscado,nomeBuscado,cargoBuscado, 0);
	fclose(arqB);

}

//Dado um BYTEOFFSET de um registro a ser removido, realiza a remocao, acessando diretamente o BO.
void remocao_logica_dado_BO(FILE* arqB, LISTA* list, long long int BO){

	int tamRegistroRemovido = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	fseek(arqB, BO, SEEK_SET);
	fread(&lixo, sizeof(char), 1, arqB);
	fread(&tamRegistroRemovido,sizeof(int), 1, arqB);


	NO* aux = cria_no(BO, tamRegistroRemovido);	
	adiciona_ordenado_lista(list, aux);

	fseek(arqB, BO, SEEK_SET);
	fwrite(&removido, sizeof(char), 1, arqB);
	
	fseek(arqB, 12, SEEK_CUR);
	for(int i = 0; i < tamRegistroRemovido-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
		fwrite(&arroba,sizeof(char),1,arqB);
	}
}

//Funcao que realiza a remocao logica de um registro.
void remocao_logica(int flag, dadosBusca dados, FILE* arqB, LISTA* list){

	
	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina de disco.

	//Esta funcao eh uma adaptacao da funcionalidade 2. Eh feita leitura do registro, sem printar. No final, vejo se o campo a ser removido esta no registro atual( se estiver remove o registro logicamente)

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000;
	int auxBytes = 0;
	long long int BYTEOFFSET = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	

	fread(&lixo, sizeof(char), 1, arqB);
	while(lixo == '-' || lixo == '@' || lixo == '*'){ //Adaptacao pois agora existem registros removidos que sao sinalizados com '*'. 
		if(lixo == '*'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);
			if(auxBytes == 32000)auxBytes = 0;

		}
		else if(lixo == '-'){
			BYTEOFFSET = (long long int)(ftell(arqB)-1);
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
				//Nao precisa fazer nada.
			}



			else if(auxBytes == 32000){ //FInal pagina de disco.
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@'){ //NAo tem nome nem cargo.
					//NAo precisa fazer nada.
				}

				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@'){ //Tem apenas nome.
							
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}

					}
					else{ //Tem so cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
			}



			else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
				
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == 'n'){ //Tem nome.
					printar.tamanhoNome = tamAuxiliar;
					fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
					if(((printar.tamanhoRegistro - 38 - tamAuxiliar) > 0)){ //Tem nome e cargo.
						fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						

					}
					
				}

				else{ //Tem apenas cargo.
					printar.tamanhoCargo = tamAuxiliar;
					fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
				}
			}

			if(flag == 1){ //Flag == 1 siginifica busca por ID
				if(dados.idBuscado == printar.id){ //Caso idBuscado seja igual ao id do registro atual.
					NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
					adiciona_ordenado_lista(list, aux);
					
					fseek(arqB, BYTEOFFSET, SEEK_SET);
					fwrite(&removido, sizeof(char), 1, arqB);
					
					fseek(arqB, 12, SEEK_CUR);
					for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
						fwrite(&arroba,sizeof(char),1,arqB);
					}
					

					fseek(arqB, bytes, SEEK_SET);
							
					

					return;
				}
			}



			else if(flag == 2){ //Flag == 2 siginifica busca por salario.
				if(dados.salarioBuscado == printar.salario){ //Caso salarioBuscado seja igual ao salario do registro atual.
					NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
					adiciona_ordenado_lista(list, aux);

						

					fseek(arqB, BYTEOFFSET, SEEK_SET);
					fwrite(&removido, sizeof(char), 1, arqB);
					
					fseek(arqB, 12, SEEK_CUR);
					for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
						fwrite(&arroba,sizeof(char),1,arqB);
					}
					
					fseek(arqB, bytes, SEEK_SET);			
				}

			}



			else if(flag == 3 ){ //Flag == 3 siginifica busca por telefone. 
				if(strcmp(printar.telefone, dados.telefoneBuscado) == 0){ ////Caso telefoneBuscado seja igual ao telefone do registro atual.
					NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
					adiciona_ordenado_lista(list, aux);

					fseek(arqB, BYTEOFFSET, SEEK_SET);
					fwrite(&removido, sizeof(char), 1, arqB);
					
					fseek(arqB, 12, SEEK_CUR);
					for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
						fwrite(&arroba,sizeof(char),1,arqB);
					}
					
					fseek(arqB, bytes, SEEK_SET);			

					
				}
			}


			else if(flag == 4 ){ //Flag == 4 siginifica busca por nome. 
				
				if(strcmp(printar.nome, dados.nomeBuscado) == 0){ //Caso nomeBuscado seja igual ao nome do registro atual.
					NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
					adiciona_ordenado_lista(list, aux);

					fseek(arqB, BYTEOFFSET, SEEK_SET);
					fwrite(&removido, sizeof(char), 1, arqB);
					
					fseek(arqB, 12, SEEK_CUR);
					for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
						fwrite(&arroba,sizeof(char),1,arqB);
					}
					
					fseek(arqB, bytes, SEEK_SET);			


				}
			}


			else if(flag == 5){ //Flag == 5 siginifica busca por cargo. 
				if(strcmp(printar.cargo, dados.cargoBuscado) == 0){ //Caso cargoBuscado seja igual ao cargo do registro atual.
					NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
					adiciona_ordenado_lista(list, aux);


					fseek(arqB, BYTEOFFSET, SEEK_SET);
					fwrite(&removido, sizeof(char), 1, arqB);
					
					fseek(arqB, 12, SEEK_CUR);
					for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
						fwrite(&arroba,sizeof(char),1,arqB);
					}
					
					fseek(arqB, bytes, SEEK_SET);
					//printf("TAM REG: %d    POSICAO: %lld\n", printar.tamanhoRegistro, BYTEOFFSET);				
				}
			}

			//Zerando o registro.
			printar.tamanhoNome = 0;
			printar.salario = -1;
			printar.telefone[0] = '\0';
			printar.nome[0] = '\0';
			printar.cargo[0] = '\0';
			printar.tamanhoCargo = 0;
		}
		
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Caso chege no final arquivo, break.
		
	}

}

//Funcao utilizada para testes(printar a lista de removidos).
void printa_lista(LISTA* list){
	
	NO* aux = list->inicio;
	while(aux != NULL){
		printf("TAMANHO: %d    BYTEOFFSET: %lld\n", aux->tamReg, aux->byteoffset);
		aux = aux->prox;
	}
	printf("QUANTIDADE NA LISTA: %d\n", list->quantidade);
}

//Funcao para gravar a lista de removidos em disco. A lista inicialmente esta em RAM e entao e escrita no arquivo binario. Antes recupera lista em disco.
void insere_lista_em_disco(FILE* arqB, LISTA* list){
	
	int t = 0; //Tamanho do registro
	long long int BO = 0; //Byteoffset do registro
	long long int menosum = -1;

	//1 PARTE -> Nesta primeira parte recupera a lista que esta em disco.
	fseek(arqB, 1, SEEK_SET);
	fread(&BO,sizeof(long long int), 1, arqB);

	while(BO != -1){
		fseek(arqB, (BO+1), SEEK_SET);
		fread(&t, sizeof(int), 1, arqB);
		NO* aux = cria_no(BO, t);	
		adiciona_ordenado_lista_final(list, aux);
		fread(&BO,sizeof(long long int), 1, arqB);
	}
	//FIM PRIMEIRA PARTE
	if(list->quantidade == 0)return;

	fseek(arqB, 1, SEEK_SET);//Vai para TOPOLISTA

	NO* atual = list->inicio;
	//Comeca gravar a lista em disco
	while(atual != NULL){
		fwrite(&atual->byteoffset, sizeof(long long int), 1, arqB);
		fseek(arqB, ((atual->byteoffset) + 5), SEEK_SET);	
		atual = atual->prox;
	}
	fwrite(&menosum, sizeof(long long int), 1, arqB); // Ultimo elemento da lista recebe -1 em seu encadeamentoLista.

	free_lista(list); //Chama funcao para liberar a memoria da lista em RAM, uma vez que ja esta em disco.

}

//Funcao para gravar a lista de removidos em disco. A lista inicialmente esta em RAM e entao e escrita no arquivo binario.
void insere_lista_em_disco_direto(FILE* arqB, LISTA* list){
	
	
	long long int menosum = -1;


	fseek(arqB, 1, SEEK_SET);

	NO* atual = list->inicio;
	//Comeca gravar a lista em disco
	while(atual != NULL){
		fwrite(&atual->byteoffset, sizeof(long long int), 1, arqB);
		fseek(arqB, ((atual->byteoffset) + 5), SEEK_SET);	
		atual = atual->prox;
	}
	fwrite(&menosum, sizeof(long long int), 1, arqB); // Ultimo elemento da lista recebe -1 em seu encadeamentoLista.

	free_lista(list); //Chama funcao para liberar a memoria da lista em RAM, uma vez que ja esta em disco.

}

//Leitura da funcionalidade "REMOCAO".
void leitura_remocao(){

	char s[30];
	scanf("%s", s); //Lendo nome arquivo binario.
	scanf("%*c"); //Lendo e ignorando espaco.
	int quantidadeRemocoes = 0;
	scanf("%d", &quantidadeRemocoes);

	FILE* arqB;
	arqB = fopen(s, "rb+");
	if(arqB == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	char numUm = '1';
	char numZero = '0';
	rewind(arqB);
	fwrite(&numZero,sizeof(char),1,arqB); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.


	char campoBuscado[40]; //Campo a ser buscado.
	int flag = -1;
	dadosBusca dados; //Onde serao armazenados os dados da remocao.
	LISTA* list = cria_lista(); //Criando lista de removidos.
	
	//Cada iteracao do for eh uma remocao.
	for(int i = 0; i<quantidadeRemocoes; i++){
		scanf("%s", campoBuscado);
		scanf("%*c"); //Lendo e ignorando espaco.
		//Aqui verifica qual o campo a ser buscados, utilizando uma flag para sinalizar.
		if(strcmp(campoBuscado, "idServidor") == 0){
			scanf("%d", &dados.idBuscado);
			flag = 1;
			remocao_logica(flag, dados, arqB, list);
			dados.idBuscado = 0;
		}
		else if(strcmp(campoBuscado, "salarioServidor") == 0){
			scanf("%lf", &dados.salarioBuscado);
			flag = 2;
			remocao_logica(flag, dados, arqB, list);
			dados.salarioBuscado = -1;
		}
		else if(strcmp(campoBuscado, "telefoneServidor") == 0){
			scan_quote_string(dados.telefoneBuscado);
			if(strcmp(dados.telefoneBuscado, "") == 0){
				dados.telefoneBuscado[0] = '\0';
			}
		
			flag = 3;
			
			remocao_logica(flag, dados, arqB, list);
			dados.telefoneBuscado[0] = '\0';
		}
		else if(strcmp(campoBuscado, "nomeServidor") == 0){
			scan_quote_string(dados.nomeBuscado);
			if(strcmp(dados.nomeBuscado, "") == 0){
				dados.nomeBuscado[0] = '\0';
			}
			
			flag = 4;
			
			remocao_logica(flag, dados, arqB, list);
			dados.nomeBuscado[0] = '\0';
		}
		else if(strcmp(campoBuscado, "cargoServidor") == 0){
			scan_quote_string(dados.cargoBuscado);
			if(strcmp(dados.cargoBuscado, "") == 0){
				dados.cargoBuscado[0] = '\0';
			}
			
			flag = 5;
			
			remocao_logica(flag, dados, arqB, list);
			dados.cargoBuscado[0] = '\0';
		}
	}

	
	insere_lista_em_disco(arqB, list); //Chama funcao para inserir a lista em dico.
	rewind(arqB);
	fwrite(&numUm,sizeof(char),1,arqB); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	binarioNaTela1(arqB);//Comparacao RUNCODES.
	fclose(arqB);	//Fecha arquivo binario.
}

//Funcao que realiza insercao de um registro no arquivo Binario.
long long int insercao_registro(FILE* arqB, LISTA* list, Dados reg){
	
	int found = 0; //Variavel para sinalizar se encontrou um registro removido que possa ser usado(tamanho comporta o novo registro).
	long long int BO = 0; 
	char nao_remov = '-';
	long long int encadeamento_lista = -1;
	char arroba = '@';
	char n = 'n';
	char c = 'c';
	char barraZero = '\0';

	NO* aux = list->inicio;
	//Aqui procuramos na lista de removidos por um registro removido que tenha tamanho suficiente para comportar o novo registro.
	while(aux != NULL){
		if(aux->tamReg >= reg.tamanhoRegistro){
			found = 1;
			BO = aux->byteoffset;
			break;
		}
		aux = aux->prox;
	}

	//Caso nao exita nenhum regisotr removido / nenhum que tenha tamanho >= ao da insercao, ela deve ser realizada no final do arquivo.
	if(found == 0){
		fseek(arqB, 0, SEEK_END); //Vai para final arquivo.
		BO = ftell(arqB);
		fwrite(&nao_remov, sizeof(char), 1, arqB);//Grava '-'.
		fwrite(&reg.tamanhoRegistro, sizeof(int), 1, arqB); //Grava tamanho registro.

		fwrite(&encadeamento_lista, sizeof(long long int), 1, arqB); //Escreve encadeamento lista = -1.
		fwrite(&reg.id, sizeof(int), 1, arqB);//Grava ID.
		fwrite(&reg.salario, sizeof(double), 1, arqB);//Grava Salario.

		if(reg.telefone[0] == '\0' ){ //Caso telefone nulo;
			fwrite(&barraZero,sizeof(char),1,arqB);
			for(int l = 0; l<13; l++){
				fwrite(&arroba,sizeof(char),1,arqB);
			}
		}
		else{ //Telefone nao nulo.
			fwrite(&reg.telefone,sizeof(char),14,arqB); //Grava Telefone
		}


		if(reg.tamanhoNome > 0){ //Caso nome nao nulo.
			
			fwrite(&reg.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
			fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
			fwrite(&reg.nome, sizeof(char), reg.tamanhoNome-1, arqB);//Grava nome.
		}
		if(reg.tamanhoCargo > 0){ //Caso cargo nao nulo.

			fwrite(&reg.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
			fwrite(&c, sizeof(char), 1, arqB); //Grava 'c'.
			fwrite(&reg.cargo, sizeof(char), reg.tamanhoCargo-1, arqB);//Grava cargo
		}

		return BO;
	}

	
	fseek(arqB, BO, SEEK_SET); //Vai para posicao da insercao.
	fwrite(&nao_remov, sizeof(char), 1, arqB); //Grava '-'.
	fseek(arqB, 4, SEEK_CUR); // Pula tam REgistro(nao necessario alterar).
	fwrite(&encadeamento_lista, sizeof(long long int), 1, arqB); //Escreve encadeamento lista = -1.
	fwrite(&reg.id, sizeof(int), 1, arqB); //Grava ID.
	fwrite(&reg.salario, sizeof(double), 1, arqB); //Grava Salario.

	if(reg.telefone[0] == '\0' ){ // Caso telefone seja NULO.
		fwrite(&barraZero,sizeof(char),1,arqB);
		for(int l = 0; l<13; l++){
			fwrite(&arroba,sizeof(char),1,arqB);
		}
	}
	else{ //Caso telefone nao seja nulo.
		fwrite(&reg.telefone,sizeof(char),14,arqB); //Grava telefone.
	}


	if(reg.tamanhoNome > 0){ // Caso nome nao seja nulo.
		fwrite(&reg.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
		fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
		fwrite(&reg.nome, sizeof(char), reg.tamanhoNome-1, arqB);//Grava nome.
	}
	if(reg.tamanhoCargo > 0){ //Caso cargo nao seja nulo.
		fwrite(&reg.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
		fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
		fwrite(&reg.cargo, sizeof(char), reg.tamanhoCargo-1, arqB); //Grava cargo
	}

	remove_no_lista(list, BO); //Remove da lista o registro removido que sera usado na insercao.
	
	return BO;

}

//Leitura da funcionalidae 5("Insercao registro").
void leitura_insercao(){
	
	char s[30];
	scanf("%s", s); //Le nome arquivo Binario.
	scanf("%*c"); //Lendo e ignorando espaco.
	int quantidadeInsercoes = 0;
	scanf("%d", &quantidadeInsercoes);

	FILE* arqB;
	arqB = fopen(s, "rb+");
	if(arqB == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	
	char numUm = '1';
	char numZero = '0';
	rewind(arqB);
	fwrite(&numZero,sizeof(char),1,arqB); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.

	char stringSalario[10];
	Dados registro_inserir; //Registro a ser inserido.
	LISTA* list = cria_lista(); // Cria uma lista de removidos.
	recupera_lista_em_disco(arqB, list); //Recupera a lista de removidos que esta no arquivo.
	long long int lixoBO = 0;
	
	//Cada iteracao do for eh uma insercao.
	for(int i = 0; i<quantidadeInsercoes; i++){
		scanf("%d", &registro_inserir.id);
		scanf("%*c");
		scan_quote_string(stringSalario);
		if(stringSalario[0] == '\0'){
			registro_inserir.salario = -1;
		}
		else{
			registro_inserir.salario = atof(stringSalario);
		}
		scan_quote_string(registro_inserir.telefone);
		scan_quote_string(registro_inserir.nome);
		scan_quote_string(registro_inserir.cargo);

		if(registro_inserir.nome[0] == '\0'){ //Caso nome seja NULO.
			registro_inserir.tamanhoNome = -4; //-4 -> int tamanhoNome(para tirar do tamanhoRegistro que contabiliza esse int).
		}
		else{
			registro_inserir.tamanhoNome = strlen(registro_inserir.nome) + 2; //Barra zero + tag.
		}


		if(registro_inserir.cargo[0] == '\0'){ //-4 -> int tamanhoCargo(para tirar do tamanhoRegistro que contabiliza esse int).
			registro_inserir.tamanhoCargo = -4;
		}
		else{
			registro_inserir.tamanhoCargo = strlen(registro_inserir.cargo) + 2; //Barra zero + tag.
		}
		registro_inserir.tamanhoRegistro = 42 + registro_inserir.tamanhoNome + registro_inserir.tamanhoCargo;
		lixo = insercao_registro(arqB, list, registro_inserir);
		strcpy(registro_inserir.telefone, "");
		strcpy(registro_inserir.nome, "");
		strcpy(registro_inserir.cargo, "");
		registro_inserir.tamanhoRegistro = 0;
		registro_inserir.tamanhoNome = 0;
		registro_inserir.tamanhoCargo = 0;
	}

	insere_lista_em_disco_direto(arqB, list); //Insere a lista em disco.
	rewind(arqB);
	fwrite(&numUm,sizeof(char),1,arqB); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	binarioNaTela1(arqB);//Comparacao RUNCODES.
	fclose(arqB);//Fecha arquivo binario.

}

//Funcao que realiza atualizacao do registro. Faz a busca do registro a ser atualizado, caso encontre, verifica se sera necessario mover o registro(devido ao tamanho novo) ou
//se eh possivel mante-lo no mesmo lugar. Caso seja necessario move-lo, adiciona o lugar ocupado na lista de removidos.
void atualizacao_registro(FILE* arqB, LISTA* list, dadosBusca dados, int flag, Dados atualiza, int flagAtualiza){

	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina de disco.

	//Esta funcao eh uma adaptacao da funcionalidade 2/4. Eh feita leitura do registro, sem printar. No final, vejo se o campo a ser atualizado esta no registro atual.

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000;
	int auxBytes = 0;
	long long int BYTEOFFSET = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	char barraZero = '\0';
	char n = 'n';
	char c = 'c';
	

	fread(&lixo, sizeof(char), 1, arqB);
	while(lixo == '-' || lixo == '@' || lixo == '*'){ //Adaptacao pois agora existem registros removidos que sao sinalizados com '*'. 
		if(lixo == '*'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);
			if(auxBytes == 32000)auxBytes = 0;

		}
		else if(lixo == '-'){
			memset(printar.nome, '\0', 150);
			memset(printar.cargo, '\0', 150);
			memset(printar.telefone, '\0', 14);

			BYTEOFFSET = (long long int)(ftell(arqB)-1);
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
				//Nao precisa fazer nada.
			}



			else if(auxBytes == 32000){ //FInal pagina de disco.
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@' || lixo == '-' || lixo == '*'){ //NAo tem nome nem cargo.
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				}

				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@' || lixo == '-' || lixo == '*'){ //Tem apenas nome.
							fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}

					}
					else{ //Tem so cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
			}



			else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == '@' || lixo == '-' || lixo == '*'){ //NAo tem nome nem cargo.
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				}
				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@' || lixo == '-' || lixo == '*'){ //Tem apenas nome.
							fseek(arqB, -1, SEEK_CUR);
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}
						
					}

					else{ //Tem apenas cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}
				}	
			}
			
			//Neste ponto registro foi lido e entao verificamos se é o registro desejado.

			if(flag == 1){ //Flag == 1 siginifica busca por ID
				if(dados.idBuscado == printar.id){ //Caso idBuscado seja igual ao id do registro atual.

					if(flagAtualiza == 1){ //Atualizar o ID
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						fwrite(&atualiza.id, sizeof(int), 1, arqB);
					}
					else if(flagAtualiza == 2){ //Atualizar salario
						fseek(arqB, BYTEOFFSET+17, SEEK_SET);
						fwrite(&atualiza.salario, sizeof(double), 1, arqB);
					}
					else if(flagAtualiza == 3){ //Atualizar o telefone
						fseek(arqB, BYTEOFFSET+25, SEEK_SET);
						if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
							fwrite(&barraZero,sizeof(char),1,arqB);
							for(int l = 0; l<13; l++){
								fwrite(&arroba,sizeof(char),1,arqB);
							}
						}
						else{ //Caso telefone nao seja nulo.
							fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
						}
					}
					else if(flagAtualiza == 4){ //Atualiza nome
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.cargo, printar.cargo);
						if(strcmp(atualiza.cargo, "") == 0){
							atualiza.tamanhoCargo = -4; 
						}
						else{
							atualiza.tamanhoCargo = strlen(atualiza.cargo) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					else if(flagAtualiza == 5){ //Atualiza o cargo

						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.nome, printar.nome);
						if(strcmp(atualiza.nome, "") == 0){
							atualiza.tamanhoNome = -4;
						}
						else{
							atualiza.tamanhoNome = strlen(atualiza.nome) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}
					}

			
					return;
				}
			}



			else if(flag == 2){ //Flag == 2 siginifica busca por salario.
				if(dados.salarioBuscado == printar.salario){ //Caso salarioBuscado seja igual ao salario do registro atual.
					if(flagAtualiza == 1){ //Atualizar o ID
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						fwrite(&atualiza.id, sizeof(int), 1, arqB);
					}
					else if(flagAtualiza == 2){ //Atualizar salario
						fseek(arqB, BYTEOFFSET+17, SEEK_SET);
						fwrite(&atualiza.salario, sizeof(double), 1, arqB);
					}
					else if(flagAtualiza == 3){ //Atualizar o telefone
						fseek(arqB, BYTEOFFSET+25, SEEK_SET);
						if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
							fwrite(&barraZero,sizeof(char),1,arqB);
							for(int l = 0; l<13; l++){
								fwrite(&arroba,sizeof(char),1,arqB);
							}
						}
						else{ //Caso telefone nao seja nulo.
							fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
						}
					}
					else if(flagAtualiza == 4){ //Atualiza nome
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.cargo, printar.cargo);
						if(strcmp(atualiza.cargo, "") == 0){
							atualiza.tamanhoCargo = -4;
						}
						else{
							atualiza.tamanhoCargo = strlen(atualiza.cargo) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					else if(flagAtualiza == 5){ //Atualiza o cargo
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.nome, printar.nome);
						if(strcmp(atualiza.nome, "") == 0){
							atualiza.tamanhoNome = -4;
						}
						else{
							atualiza.tamanhoNome = strlen(atualiza.nome) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}
					}
					
					fseek(arqB, bytes, SEEK_SET);			
				}

			}



			else if(flag == 3 ){ //Flag == 3 siginifica busca por telefone. 
				if(strcmp(printar.telefone, dados.telefoneBuscado) == 0){ ////Caso telefoneBuscado seja igual ao telefone do registro atual.
					if(flagAtualiza == 1){ //Atualizar o ID
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						fwrite(&atualiza.id, sizeof(int), 1, arqB);
					}
					else if(flagAtualiza == 2){ //Atualizar salario
						fseek(arqB, BYTEOFFSET+17, SEEK_SET);
						fwrite(&atualiza.salario, sizeof(double), 1, arqB);
					}
					else if(flagAtualiza == 3){ //Atualizar o telefone
						fseek(arqB, BYTEOFFSET+25, SEEK_SET);
						if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
							fwrite(&barraZero,sizeof(char),1,arqB);
							for(int l = 0; l<13; l++){
								fwrite(&arroba,sizeof(char),1,arqB);
							}
						}
						else{ //Caso telefone nao seja nulo.
							fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
						}
					}
					else if(flagAtualiza == 4){ //Atualiza nome
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.cargo, printar.cargo);
						if(strcmp(atualiza.cargo, "") == 0){
							atualiza.tamanhoCargo = -4;
						}
						else{
							atualiza.tamanhoCargo = strlen(atualiza.cargo) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					else if(flagAtualiza == 5){ //Atualiza o cargo
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.nome, printar.nome);
						if(strcmp(atualiza.nome, "") == 0){
							atualiza.tamanhoNome = -4;
						}
						else{
							atualiza.tamanhoNome = strlen(atualiza.nome) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}
					}
					
					fseek(arqB, bytes, SEEK_SET);			

					
				}
			}


			else if(flag == 4 ){ //Flag == 4 siginifica busca por nome. 
				
				if(strcmp(printar.nome, dados.nomeBuscado) == 0){ //Caso nomeBuscado seja igual ao nome do registro atual.
					
					

					if(flagAtualiza == 1){ //Atualizar o ID
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						fwrite(&atualiza.id, sizeof(int), 1, arqB);
					}
					else if(flagAtualiza == 2){ //Atualizar salario
						fseek(arqB, BYTEOFFSET+17, SEEK_SET);
						fwrite(&atualiza.salario, sizeof(double), 1, arqB);
					}
					else if(flagAtualiza == 3){ //Atualizar o telefone
						fseek(arqB, BYTEOFFSET+25, SEEK_SET);
						if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
							fwrite(&barraZero,sizeof(char),1,arqB);
							for(int l = 0; l<13; l++){
								fwrite(&arroba,sizeof(char),1,arqB);
							}
						}
						else{ //Caso telefone nao seja nulo.
							fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
						}
					}
					else if(flagAtualiza == 4){ //Atualiza nome
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.cargo, printar.cargo);
						if(strcmp(atualiza.cargo, "") == 0){
							atualiza.tamanhoCargo = -4;
						}
						else{
							atualiza.tamanhoCargo = strlen(atualiza.cargo) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					else if(flagAtualiza == 5){ //Atualiza o cargo
						
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.nome, printar.nome);
						if(strcmp(atualiza.nome, "") == 0){
							atualiza.tamanhoNome = -4;
						}
						else{
							atualiza.tamanhoNome = strlen(atualiza.nome) + 2;

						}

						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
			
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					
					fseek(arqB, bytes, SEEK_SET);



				}
			}


			else if(flag == 5){ //Flag == 5 siginifica busca por cargo.

				if(strcmp(printar.cargo, dados.cargoBuscado) == 0){ //Caso cargoBuscado seja igual ao cargo do registro atual.
					if(flagAtualiza == 1){ //Atualizar o ID
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						fwrite(&atualiza.id, sizeof(int), 1, arqB);
					}
					else if(flagAtualiza == 2){ //Atualizar salario
						fseek(arqB, BYTEOFFSET+17, SEEK_SET);
						fwrite(&atualiza.salario, sizeof(double), 1, arqB);
					}
					else if(flagAtualiza == 3){ //Atualizar o telefone
						fseek(arqB, BYTEOFFSET+25, SEEK_SET);
						if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
							fwrite(&barraZero,sizeof(char),1,arqB);
							for(int l = 0; l<13; l++){
								fwrite(&arroba,sizeof(char),1,arqB);
							}
						}
						else{ //Caso telefone nao seja nulo.
							fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
						}
					}
					else if(flagAtualiza == 4){ //Atualiza nome
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.cargo, printar.cargo);
						if(strcmp(atualiza.cargo, "") == 0){
							atualiza.tamanhoCargo = -4;
						}
						else{
							atualiza.tamanhoCargo = strlen(atualiza.cargo) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}

						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}

					}
					else if(flagAtualiza == 5){ //Atualiza o cargo
						atualiza.id = printar.id;
						atualiza.salario = printar.salario;
						strcpy(atualiza.telefone, printar.telefone);
						strcpy(atualiza.nome, printar.nome);
						if(strcmp(atualiza.nome, "") == 0){
							atualiza.tamanhoNome = -4;
						}
						else{
							atualiza.tamanhoNome = strlen(atualiza.nome) + 2;
						}
						atualiza.tamanhoRegistro = 42 + atualiza.tamanhoNome + atualiza.tamanhoCargo;
						fseek(arqB, BYTEOFFSET+13, SEEK_SET);
						for(int i = 0; i < printar.tamanhoRegistro-8; i++){ //Retirando o encadeamento lista pois nao sera sobreescrito
							fwrite(&arroba,sizeof(char),1,arqB);
						}
						if(atualiza.tamanhoRegistro <= printar.tamanhoRegistro){ //Entao continua no msm lugar
							fseek(arqB, BYTEOFFSET+13, SEEK_SET);
							fwrite(&atualiza.id, sizeof(int), 1, arqB);
							fwrite(&atualiza.salario, sizeof(double), 1, arqB);
							if(atualiza.telefone[0] == '\0' ){ // Caso telefone seja NULO.
								fwrite(&barraZero,sizeof(char),1,arqB);
								for(int l = 0; l<13; l++){
									fwrite(&arroba,sizeof(char),1,arqB);
								}
							}
							else{ //Caso telefone nao seja nulo.
								fwrite(&atualiza.telefone,sizeof(char),14,arqB); //Grava telefone.
							}
							if(atualiza.tamanhoNome > 0){
								fwrite(&atualiza.tamanhoNome, sizeof(int), 1, arqB);//Grava tamanhoNome.
								fwrite(&n, sizeof(char), 1, arqB);//Grava 'n'.
								fwrite(&atualiza.nome, sizeof(char), atualiza.tamanhoNome-1, arqB);//Grava nome.
							}
							if(atualiza.tamanhoCargo > 0){
								fwrite(&atualiza.tamanhoCargo, sizeof(int), 1, arqB);//Grava tamanhoCargo.
								fwrite(&c, sizeof(char), 1, arqB);//Grava 'c'.
								fwrite(&atualiza.cargo, sizeof(char), atualiza.tamanhoCargo-1, arqB); //Grava cargo
							}

						}
						else{//Aqui este entra p/ removidos e adiciona o reg em outro lugar
							NO* aux = cria_no(BYTEOFFSET, printar.tamanhoRegistro);	
							adiciona_ordenado_lista(list, aux);
					
							fseek(arqB, BYTEOFFSET, SEEK_SET);
							fwrite(&removido, sizeof(char), 1, arqB);
							insercao_registro(arqB,list, atualiza);

						}
					}
					
					fseek(arqB, bytes, SEEK_SET);				
				}
			}

			
		}
		
		fseek(arqB, bytes, SEEK_SET);
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Caso chege no final arquivo, break.
		
	}

}

//Funcao para leitura da funcionalidade 6("Atualizacao de registros")
void leitura_atualizacao(){

	char s[30];
	scanf("%s", s); //Le nome arquivo Binario.
	scanf("%*c"); //Lendo e ignorando espaco.
	int quantidadeAtualizacoes = 0;
	scanf("%d", &quantidadeAtualizacoes);

	FILE* arqB;
	arqB = fopen(s, "rb+");
	if(arqB == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	
	char numUm = '1'; 
	char numZero = '0';
	rewind(arqB);
	fwrite(&numZero,sizeof(char),1,arqB); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.

	dadosBusca dadosBuscado;
	char campoBuscado[40];
	char campoAtualiza[40];
	int flag = -1; //Sinaliza qual campo esta sendo buscado.
	int flagAtualiza = -1; //Sinaliza qual dos campos vai ser atualizado.
	char stringSalario[10];
	Dados registro_atualiza; //Registro a ser atualizado.
	LISTA* list = cria_lista(); // Cria uma lista de removidos.
	recupera_lista_em_disco(arqB, list); //Recupera a lista de removidos que esta no arquivo.



	for(int i = 0; i<quantidadeAtualizacoes; i++){
		scanf("%s", campoBuscado);
		scanf("%*c"); //Lendo e ignorando espaco.
		//Aqui verifica qual o campo a ser buscados, utilizando uma flag para sinalizar.
		if(strcmp(campoBuscado, "idServidor") == 0){
			scanf("%d", &dadosBuscado.idBuscado);
			flag = 1; //Busca pelo id.
		}
		else if(strcmp(campoBuscado, "salarioServidor") == 0){
			scan_quote_string(stringSalario);
			if(stringSalario[0] == '\0'){
				dadosBuscado.salarioBuscado = -1;
			}
			else{
				dadosBuscado.salarioBuscado = atof(stringSalario);
			}
			flag = 2; //Busca pelo salario.
		}
		else if(strcmp(campoBuscado, "telefoneServidor") == 0){
			scan_quote_string(dadosBuscado.telefoneBuscado);
			if(strcmp(dadosBuscado.telefoneBuscado, "") == 0){
				dadosBuscado.telefoneBuscado[0] = '\0';
			}
			flag = 3; //Busca pelo telefone.
		}
		else if(strcmp(campoBuscado, "nomeServidor") == 0){
			scan_quote_string(dadosBuscado.nomeBuscado);
			if(strcmp(dadosBuscado.nomeBuscado, "") == 0){
				dadosBuscado.nomeBuscado[0] = '\0';
			}
			flag = 4;//Busca pelo nome.
		}
		else if(strcmp(campoBuscado, "cargoServidor") == 0){
			scan_quote_string(dadosBuscado.cargoBuscado);
			if(strcmp(dadosBuscado.cargoBuscado, "") == 0){
				dadosBuscado.cargoBuscado[0] = '\0';
			}	
			flag = 5; //Busca pelo cargo.
		}

		//Os ifs de cima fizeram a leitura da "primeira parte da linha", isto é, o registro a ser atualizado. Agora, necessario ler qual campo 
		//a ser atualizado e seu respectivo valor.

		scanf("%s", campoAtualiza); //Leitura qual campo sera atualizado

		if(strcmp(campoAtualiza, "idServidor") == 0){
			scanf("%d", &registro_atualiza.id);
			flagAtualiza = 1; //Atualizacao do ID.
		}
		else if(strcmp(campoAtualiza, "salarioServidor") == 0){
			scan_quote_string(stringSalario);
			if(stringSalario[0] == '\0'){
				registro_atualiza.salario = -1;
			}
			else{
				registro_atualiza.salario = atof(stringSalario);
			}
			flagAtualiza = 2; //Atualizacao SALARIO.
		}
		else if(strcmp(campoAtualiza, "telefoneServidor") == 0){
			scan_quote_string(registro_atualiza.telefone);
			if(strcmp(registro_atualiza.telefone, "") == 0){
				registro_atualiza.telefone[0] = '\0';
			}
			flagAtualiza = 3;//Atualizacao TELEFONE.
		}
		else if(strcmp(campoAtualiza, "nomeServidor") == 0){
			scan_quote_string(registro_atualiza.nome);
			if(strcmp(registro_atualiza.nome, "") == 0){
				registro_atualiza.nome[0] = '\0';
				registro_atualiza.tamanhoNome = -4; //Tirando os 4 bytes do tamNome(contabilizado na conta do tamRegistro).
			}
			else{
				registro_atualiza.tamanhoNome = strlen(registro_atualiza.nome) + 2; //Adicionando '\0' e tagNome.
			}
			flagAtualiza = 4; //Atualizacao NOME.
		}
		else if(strcmp(campoAtualiza, "cargoServidor") == 0){
			scan_quote_string(registro_atualiza.cargo);
			if(strcmp(registro_atualiza.cargo, "") == 0){
				registro_atualiza.cargo[0] = '\0';
				registro_atualiza.tamanhoCargo = -4; //Tirando os 4 bytes do tamCargo(contabilizado na conta do tamRegistro).
			}
			else{
				registro_atualiza.tamanhoCargo = strlen(registro_atualiza.cargo) + 2; //Adicionando '\0' e tagCargo.
			}	
			flagAtualiza = 5; //Atualizacao CARGO.
		}
		
		atualizacao_registro(arqB, list, dadosBuscado, flag, registro_atualiza, flagAtualiza); //Chama funcao para atualizar o(s) registro(s).

		//Zerando registro.
		dadosBuscado.idBuscado = 0;
		dadosBuscado.salarioBuscado = -1;
		dadosBuscado.telefoneBuscado[0] = '\0';
		dadosBuscado.nomeBuscado[0] = '\0';
		dadosBuscado.cargoBuscado[0] = '\0';
	}

	
	insere_lista_em_disco_direto(arqB, list); //Insere a lista em disco.
	rewind(arqB);//Volta inicio arquivo.
	fwrite(&numUm,sizeof(char),1,arqB); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	binarioNaTela1(arqB);//Comparacao RUNCODES.
	fclose(arqB);//Fecha arquivo binario.
}

//Funcao que realiza leitura apenas do registro de cabecalho, retornando-o.
registroCabecalho leitura_cab(FILE* arqB){

	rewind(arqB); //Voltando inicio do arquivo binario.
	registroCabecalho cab; //Aqui serao armazenados os campos do cabecalho.
	char teste;

	fseek(arqB, 10, SEEK_SET);
	fscanf(arqB, "%[^@]", cab.desCampo1); //Leitura campo 1
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo2); //Leitura campo 2
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo3); //Leitura campo 3
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo4); //Leitura campo 4
	fscanf(arqB, "%c", &teste);
	while(teste == '@'){
		fscanf(arqB, "%c", &teste);
	}
	fscanf(arqB, "%[^@]", cab.desCampo5); //Leitura campo 5

	return cab;
}

//Funcao realiza leitura de todo o arquivo binario, passando todos os registros nao removidos para um vetor de registros.
//No final, chama mergesort para ordenar este vetor a partir do ID. Retorna quantidade de registros no vetor.
int leitura_binario_add_vetor(FILE* arqB, Dados* leituraDados){

	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina de disco.

	//Esta funcao eh uma adaptacao da funcionalidade 2/4/6. Eh feita leitura do registro, sem printar.

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000;
	int auxBytes = 0;
	long long int BYTEOFFSET = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	char barraZero = '\0';
	char n = 'n';
	char c = 'c';

	int qntdArrobas = 0;
	
	int quantidade = 0;

	fread(&lixo, sizeof(char), 1, arqB);
	while(lixo == '-' || lixo == '@' || lixo == '*'){ //Adaptacao pois agora existem registros removidos que sao sinalizados com '*'. 
		if(lixo == '*'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);
			if(auxBytes == 32000)auxBytes = 0;

		}
		else if(lixo == '-'){
			memset(printar.nome, '\0', 150);
			memset(printar.cargo, '\0', 150);
			memset(printar.telefone, '\0', 14);

			BYTEOFFSET = (long long int)(ftell(arqB)-1);
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
				//Nao precisa fazer nada.
			}



			else if(auxBytes == 32000){ //FInal pagina de disco.
				
				qntdArrobas = printar.tamanhoRegistro;
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@'){ //NAo tem nome nem cargo.
					qntdArrobas -= 34;
				}

				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					qntdArrobas = printar.tamanhoRegistro - 34;
					fread(&lixo, sizeof(char), 1, arqB);
					
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						qntdArrobas -= printar.tamanhoNome;
						qntdArrobas -= 4;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@'){ //Tem apenas nome.
							fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							qntdArrobas -= printar.tamanhoCargo;
							qntdArrobas -= 4;
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}

					}
					else{ //Tem so cargo.
						printar.tamanhoCargo = tamAuxiliar;
						qntdArrobas -= printar.tamanhoCargo;
						qntdArrobas -= 4;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
				printar.tamanhoRegistro -= qntdArrobas;
				
				qntdArrobas = 0;
			}



			else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == '@'){ //NAo tem nome nem cargo.
					qntdArrobas = 1;
					fread(&lixo, sizeof(char), 1, arqB);
					while(lixo == '@'){
						qntdArrobas++;
						fread(&lixo, sizeof(char), 1, arqB);
					}
					printar.tamanhoRegistro -= qntdArrobas;
					qntdArrobas = 0;
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				}
				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo != 'n' && lixo != 'c'){ //Nao tem nome nem cargo
						fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
					}
					else if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						
						fread(&tamAuxiliar,sizeof(int), 1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo != 'c'){
							fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
						}
						else{ //Tem nome e cargo.
							printar.tamanhoCargo = tamAuxiliar;
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}
						
					}

					else{ //Tem apenas cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}
				}	
			}
			
			
			//Aqui passa os dados para o vetor.

			strcpy(leituraDados[quantidade].nome, "\0");
			strcpy(leituraDados[quantidade].cargo, "\0");
			strcpy(leituraDados[quantidade].telefone, "@@@@@@@@@@@@@@");
			

			leituraDados[quantidade].id = printar.id;
			leituraDados[quantidade].salario = printar.salario;
			strcpy(leituraDados[quantidade].telefone, printar.telefone);
			leituraDados[quantidade].tamanhoNome = printar.tamanhoNome;
			strcpy(leituraDados[quantidade].nome, printar.nome);
			leituraDados[quantidade].tamanhoCargo = printar.tamanhoCargo;
			strcpy(leituraDados[quantidade].cargo, printar.cargo);
			leituraDados[quantidade].tamanhoRegistro = printar.tamanhoRegistro;

			quantidade++;


			//Zerando o registro.
			printar.tamanhoNome = 0;
			printar.salario = -1;
			printar.telefone[0] = '\0';
			printar.nome[0] = '\0';
			printar.cargo[0] = '\0';
			printar.tamanhoCargo = 0;
			printar.tamanhoRegistro = 0;


		}
		
		fseek(arqB, bytes, SEEK_SET);
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Caso chege no final arquivo, break.
		
	}

	MS_sort(leituraDados, quantidade, sizeof(Dados), int_compare);
	return quantidade;

}


//Funcionalidade 7. Realiza ordenacao dos registros a partir do ID.
void ordenacao_interna(){

	char nomeArquivoEntrada[30];
	scanf("%s", nomeArquivoEntrada); //Lendo nome arquivo binario de entrada.

	char nomeArquivoSaida[30];
	scanf("%s", nomeArquivoSaida); //Lendo nome arquivo binario saida.

	FILE* arqB;
	arqB = fopen(nomeArquivoEntrada, "rb+");
	if(arqB == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqB);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqB);
		return;
	}
	
	char numUm = '1';
	char numZero = '0';
	rewind(arqB);
	fwrite(&numZero,sizeof(char),1,arqB); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.


	registroCabecalho cab = leitura_cab(arqB);
	Dados *leituraDados;
	leituraDados = (Dados*) malloc (6000 * sizeof(Dados));
	int quantidade = 0; //Quantidade de registros nao removidos presente no arquivo de entrada.
	quantidade = leitura_binario_add_vetor(arqB, leituraDados);

	FILE* arqFinal = NULL;
	arqFinal = fopen(nomeArquivoSaida, "wb+");

	escreve_binario(leituraDados, quantidade, cab, arqFinal);
	


	//Chegando final da funcionalidade 7 fecha o arq e usa binarionatela.


	rewind(arqB);
	fwrite(&numUm,sizeof(char),1,arqB); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	binarioNaTela1(arqFinal);//Comparacao RUNCODES.
	fclose(arqB);//Fecha arquivo binario.
	fclose(arqFinal);

}

//Le apenas um registro dado um arquivo binario. Caso leitura seja feita com sucesso, retorna 1. Caso tenha chego
//no final do arquivo, retorna 0.
int leitura_um_registro(FILE* arqB, Dados *printar){

	
	int tamAuxiliar = 0;
	long long int bytes = ftell(arqB);
	long long int auxBytes = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	char barraZero = '\0';
	char n = 'n';
	char c = 'c';

	int qntdArrobas = 0;

	//Zerando o registro.

	strcpy(printar->nome, "\0");
	strcpy(printar->cargo, "\0");
	strcpy(printar->telefone, "@@@@@@@@@@@@@@");
	printar->tamanhoNome = 0;
	printar->salario = -1;
	printar->tamanhoCargo = 0;
	printar->tamanhoRegistro = 0;
	
	
	fread(&lixo, sizeof(char), 1, arqB);
	if(feof(arqB) != 0)return 0; //Final do arquivo, retorna 0.
	while(lixo == '*'){
		fread(&printar->tamanhoRegistro,sizeof(int), 1, arqB);
		bytes = bytes + printar->tamanhoRegistro + 5;
		fseek(arqB, bytes, SEEK_SET);
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)return 0; //Final do arquivo, retorna 0.

	}

	if(lixo == '-'){
		memset(printar->nome, '\0', 150);
		memset(printar->cargo, '\0', 150);
		memset(printar->telefone, '\0', 14);

	
		fread(&printar->tamanhoRegistro,sizeof(int), 1, arqB);
		bytes = bytes + printar->tamanhoRegistro + 5;
		auxBytes = bytes % 32000; //Verificar se esta final da pagina de disco.
		fseek(arqB, 8, SEEK_CUR);	
		fread(&printar->id,sizeof(int), 1, arqB);
		fread(&printar->salario,sizeof(double), 1, arqB);
		fread(&printar->telefone,sizeof(char), 14, arqB);
		printar->telefone[14] = '\0';
		if(printar->tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
			//Nao precisa fazer nada.
		}



		else if(auxBytes == 0){ //FInal pagina de disco.
				
			qntdArrobas = printar->tamanhoRegistro;
			fread(&lixo, sizeof(char), 1, arqB);

			if(lixo == '@'){ //NAo tem nome nem cargo.
				qntdArrobas -= 34;
			}

			else{
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				qntdArrobas = printar->tamanhoRegistro - 34;
				fread(&lixo, sizeof(char), 1, arqB);
				
				if(lixo == 'n'){ //Tem nome.
					printar->tamanhoNome = tamAuxiliar;
					qntdArrobas -= printar->tamanhoNome;
					qntdArrobas -= 4;
					fread(&printar->nome,sizeof(char), printar->tamanhoNome-1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == '@'){ //Tem apenas nome.
						fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					}
					else{ //Tem nome e cargo.
						fseek(arqB, -1, SEEK_CUR);
						fread(&printar->tamanhoCargo,sizeof(int), 1, arqB);
						qntdArrobas -= printar->tamanhoCargo;
						qntdArrobas -= 4;
						fread(&lixo, sizeof(char), 1, arqB);
						fread(&printar->cargo,sizeof(char), printar->tamanhoCargo-1, arqB);
					}

				}
				else{ //Tem so cargo.
					printar->tamanhoCargo = tamAuxiliar;
					qntdArrobas -= printar->tamanhoCargo;
					qntdArrobas -= 4;
					fread(&printar->cargo,sizeof(char), printar->tamanhoCargo-1, arqB);
				}

			}
			printar->tamanhoRegistro -= qntdArrobas;
				
		}



		else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
			fread(&lixo, sizeof(char), 1, arqB);
			if(lixo == '@'){ //NAo tem nome nem cargo.
				qntdArrobas = 1;
				fread(&lixo, sizeof(char), 1, arqB);
				while(lixo == '@'){
					qntdArrobas++;
					fread(&lixo, sizeof(char), 1, arqB);
				}
				printar->tamanhoRegistro -= qntdArrobas;
				qntdArrobas = 0;
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
			}
			else{
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo != 'n' && lixo != 'c'){ //Nao tem nome nem cargo
					fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
				}
				else if(lixo == 'n'){ //Tem nome.
					printar->tamanhoNome = tamAuxiliar;
					fread(&printar->nome,sizeof(char), printar->tamanhoNome-1, arqB);
						
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo != 'c'){
						fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
					}
					else{ //Tem nome e cargo.
						printar->tamanhoCargo = tamAuxiliar;
						fread(&printar->cargo,sizeof(char), printar->tamanhoCargo-1, arqB);
					}
						
				}

				else{ //Tem apenas cargo.
					printar->tamanhoCargo = tamAuxiliar;
					fread(&printar->cargo,sizeof(char), printar->tamanhoCargo-1, arqB);
				}
			}	
		}
	
	}
		
	fseek(arqB, bytes, SEEK_SET); //Da um fseek para deixar o ponteiro onde comeca o proximo registro(ou final do arquivo).
	return 1; //Leitura realizada com sucesso.
		
}

//Escreve apenas um unico registro no arquivo binario passado como parametro. 
void escreve_um_registro(FILE* arqBinario, Dados reg, long long int *ultimoRegistro){

	char naoRemovido = '-';
	long long int encadeamento = -1;
	char delimitadorN = 'n';
	char delimitadorC = 'c';
	char barraZero = '\0';
	char arroba = '@';
	

	int bytesPagDisco = ftell(arqBinario) % 32000; //Controle da pagina de disco atual.
	int faltantePagina = 0; //Quantidade de arrobas a serem adicionaods final de uma pagina de disco.
	long long int posAtual; 
	
	int auxTamReg = 0;
	
		
	
	
	if(bytesPagDisco + 5 + reg.tamanhoRegistro > 32000){ //Excederia a pagina de disco, portanto precisa preecnher com '@' e atualizar tamanho do registro anterior.
		faltantePagina = 32000 - bytesPagDisco; //Quantidade arrobas para preencer a pag de disco.
		
		for(int k = 0; k<faltantePagina; k++){ //Preenche com arroba
			fwrite(&arroba,sizeof(char),1,arqBinario);
		}
		posAtual = ftell(arqBinario); //Posicao atual, para poder retornar aqui(onde deve ser feita a insercao).
		fseek(arqBinario, *ultimoRegistro, SEEK_SET); //Fseek para o tamReg do ultimo registro, para atualiza-lo com os '@' usados para preencher a pag de disco.
		fread(&auxTamReg, sizeof(int), 1,arqBinario); //Le valor tamReg gravado.
		auxTamReg += faltantePagina; //Atualiza valor tamReg.
		fseek(arqBinario, -4, SEEK_CUR); 
		fwrite(&auxTamReg, sizeof(int), 1, arqBinario); //Grava valor atualizado.
		fseek(arqBinario, posAtual, SEEK_SET); //VOlta para posicao da insercao.
	}
	
	
	fwrite(&naoRemovido,sizeof(char),1,arqBinario); //Removido(1 byte = '-')
	*ultimoRegistro = ftell(arqBinario); //Atualiza posicao do ultimo registro colocado em disco.
	fwrite(&reg.tamanhoRegistro,sizeof(int),1,arqBinario);	//Indicador tamanho: tamRegistro
	fwrite(&encadeamento,sizeof(long long int),1,arqBinario);//Encadeamento lista
	fwrite(&reg.id,sizeof(int),1,arqBinario);   //id
	fwrite(&reg.salario,sizeof(double),1,arqBinario);	//salario
	if(reg.telefone[0] == '@' ){
		fwrite(&barraZero,sizeof(char),1,arqBinario);
		for(int z = 0; z<13; z++){
			fwrite(&arroba,sizeof(char),1,arqBinario);
		}
	}
	else{
		fwrite(&reg.telefone,sizeof(char),14,arqBinario); //telefone
	}
	if(reg.tamanhoNome > 0){
		fwrite(&reg.tamanhoNome,sizeof(int),1,arqBinario);	 //tamanho nome
		fwrite(&delimitadorN,sizeof(char),1,arqBinario); //TagCampo4 = 'n'
		reg.tamanhoNome--;
		fwrite(&reg.nome,sizeof(char),reg.tamanhoNome,arqBinario); //nome
	}
	if(reg.tamanhoCargo > 0){
		fwrite(&reg.tamanhoCargo,sizeof(int),1,arqBinario);	//tamanho cargo
		fwrite(&delimitadorC,sizeof(char),1,arqBinario); //TagCampo5 = 'c'
		reg.tamanhoCargo--;
		fwrite(&reg.cargo,sizeof(char),reg.tamanhoCargo,arqBinario); //cargo
	}


}

//FUncionalidade 8, dados dois arquivos binarios, realiza o "merging"(uniao).Os registros de ambos os arquivos sao colocados em um terceiro arquivo, ordenado pelo ID.
void merging(){

	char nomePrimeiroArquivo[30];
	scanf("%s", nomePrimeiroArquivo); //Lendo nome do primeiro arquivo de entrada.

	char nomeSegundoArquivo[30];
	scanf("%s", nomeSegundoArquivo); //Lendo nome do segundo arquivo de entrada.

	char nomeArquivoSaida[30];
	scanf("%s", nomeArquivoSaida); //Lendo nome arquivo binario saida.



	FILE* primeiroArq;
	FILE* segundoArq;
	primeiroArq = fopen(nomePrimeiroArquivo, "rb+"); //Abrindo primeiro arquivo.
	segundoArq = fopen(nomeSegundoArquivo, "rb+"); //Abrindo segundo arquivo.
	if(primeiroArq == NULL || segundoArq == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, primeiroArq);
	if(lixo == '0'){ //Caso 1 arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(primeiroArq);
		fclose(segundoArq);
		return;
	}

	fread(&lixo, sizeof(char), 1, segundoArq);
	if(lixo == '0'){ //Caso 2 arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(primeiroArq);
		fclose(segundoArq);
		return;
	}
	
	char numUm = '1';
	char numZero = '0';

	rewind(primeiroArq);
	fwrite(&numZero,sizeof(char),1,primeiroArq); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.
	rewind(segundoArq);
	fwrite(&numZero,sizeof(char),1,segundoArq);

	//Inicio da funcionalidade.

	registroCabecalho cab = leitura_cab(primeiroArq); //REgistro de cabecalho.
	FILE* arqFinal;
	arqFinal = fopen(nomeArquivoSaida, "w+b"); //Abrindo arquivo final(destino).
	escreve_cab_binario(cab, arqFinal);

	fseek(primeiroArq, 32000, SEEK_SET); //Pulando primeira pagina de disco primeiro arquivo.
	fseek(segundoArq, 32000, SEEK_SET); //Pulando primeira pagina de disco segundo arquivo.

	int fimPrimeiro = 1; //Quando == 0 terminou primeiro arquivo.
	int fimSegundo = 1; //Quando ==0 terminou primeiro arquivo.
	long long int ultimo = 0;
	Dados registro1;
	Dados registro2;

	fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
	fimSegundo = leitura_um_registro(segundoArq, &registro2);
	
	while(fimPrimeiro == 1 && fimSegundo == 1){ //Enquanto nao chega final de nenhum arquivo.
		if(registro1.id < registro2.id){ //Escreve registro do arquivo 1 e le outro.
			escreve_um_registro(arqFinal, registro1, &ultimo);
			fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
		}
		else if(registro2.id < registro1.id){ //EScreve registro do arquivo 2 e le outro.
			escreve_um_registro(arqFinal, registro2, &ultimo);
			fimSegundo = leitura_um_registro(segundoArq, &registro2);
		}
		else{ //ID's iguais, escreve reg1 e le ambos.
			escreve_um_registro(arqFinal, registro1, &ultimo);
			fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
			fimSegundo = leitura_um_registro(segundoArq, &registro2);
		}
	}
	
	if(fimPrimeiro == 0 && fimSegundo == 0){
		//Nesse caso ambos terminaram ao mesmo tempo e portanto nao restam registros.
	}
	else if(fimPrimeiro == 0){ //Acabou o primeiro, necessario ainda escrevers os restantes do arquivo 2.
		escreve_um_registro(arqFinal, registro2, &ultimo);
		fimSegundo = leitura_um_registro(segundoArq, &registro2);
		while(fimSegundo == 1){
			escreve_um_registro(arqFinal, registro2, &ultimo);
			fimSegundo = leitura_um_registro(segundoArq, &registro2);
		}
	}
	else if(fimSegundo == 0){ //Acabou o segundo, necessario ainda escrever os restantes do arquivo 1.
		escreve_um_registro(arqFinal, registro1, &ultimo);
		fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
		while(fimPrimeiro == 1){
			escreve_um_registro(arqFinal, registro1, &ultimo);
			fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
		}
	}
	
	//Chegando fim da funcionalidade 8, agora necessario fechar os arquivos + binario na tela.
	rewind(primeiroArq);
	fwrite(&numUm,sizeof(char),1,primeiroArq); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(segundoArq);
	fwrite(&numUm,sizeof(char),1,segundoArq); 
	rewind(arqFinal);
	fwrite(&numUm,sizeof(char),1,arqFinal);
	binarioNaTela1(arqFinal);//Comparacao RUNCODES.
	fclose(primeiroArq);
	fclose(segundoArq);
	fclose(arqFinal);
}

//Funcionalidade 9, dado 2 arquivos realiza o "matching", isto eh, sao colocados no arquivo final apenas os registro que estao
//em ambos arquivos de entrada. Operacao de "interseccao".
void matching(){

	char nomePrimeiroArquivo[30];
	scanf("%s", nomePrimeiroArquivo); //Lendo nome do primeiro arquivo de entrada.

	char nomeSegundoArquivo[30];
	scanf("%s", nomeSegundoArquivo); //Lendo nome do segundo arquivo de entrada.

	char nomeArquivoSaida[30];
	scanf("%s", nomeArquivoSaida); //Lendo nome arquivo binario saida.



	FILE* primeiroArq;
	FILE* segundoArq;
	primeiroArq = fopen(nomePrimeiroArquivo, "rb+"); //Abrindo primeiro arquivo.
	segundoArq = fopen(nomeSegundoArquivo, "rb+"); //Abrindo segundo arquivo.
	if(primeiroArq == NULL || segundoArq == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, primeiroArq);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(primeiroArq);
		fclose(segundoArq);
		return;
	}

	fread(&lixo, sizeof(char), 1, segundoArq);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(primeiroArq);
		fclose(segundoArq);
		return;
	}
	
	char numUm = '1';
	char numZero = '0';

	rewind(primeiroArq);
	fwrite(&numZero,sizeof(char),1,primeiroArq); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.
	rewind(segundoArq);
	fwrite(&numZero,sizeof(char),1,segundoArq);

	//Inicio da funcionalidade.

	registroCabecalho cab = leitura_cab(primeiroArq);
	FILE* arqFinal;
	arqFinal = fopen(nomeArquivoSaida, "w+b"); //Abrindo arquivo final(destino).
	escreve_cab_binario(cab, arqFinal);

	fseek(primeiroArq, 32000, SEEK_SET); //Pulando primeira pagina de disco primeiro arquivo.
	fseek(segundoArq, 32000, SEEK_SET); //Pulando primeira pagina de disco segundo arquivo.

	int fimPrimeiro = 1; //Quando == 0 terminou primeiro arquivo.
	int fimSegundo = 1; //Quando ==0 terminou primeiro arquivo.
	long long int ultimo = 0;
	Dados registro1;
	Dados registro2;

	fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
	fimSegundo = leitura_um_registro(segundoArq, &registro2);
	
	while(fimPrimeiro == 1 && fimSegundo == 1){ //Enquanto nao chega final de nenhum arquivo.
		if(registro1.id < registro2.id){ //Escreve registro do arquivo 1 e le outro.
			fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
		}
		else if(registro2.id < registro1.id){ //EScreve registro do arquivo 2 e le outro.
			fimSegundo = leitura_um_registro(segundoArq, &registro2);
		}
		else{ //ID's iguais, escreve reg1 e le ambos.
			escreve_um_registro(arqFinal, registro1, &ultimo);
			fimPrimeiro = leitura_um_registro(primeiroArq, &registro1);
			fimSegundo = leitura_um_registro(segundoArq, &registro2);
		}
	}
	
	
	
	//Chegando fim da funcionalidade 8, agora necessario fechar os arquivos + binario na tela.
	rewind(primeiroArq);
	fwrite(&numUm,sizeof(char),1,primeiroArq); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(segundoArq);
	fwrite(&numUm,sizeof(char),1,segundoArq); 
	rewind(arqFinal);
	fwrite(&numUm,sizeof(char),1,arqFinal);
	binarioNaTela1(arqFinal);//Comparacao RUNCODES.
	fclose(primeiroArq);
	fclose(segundoArq);
	fclose(arqFinal);
}

//Grava o cabecalho do arquivo de INDICE.
void grava_cabecalho_indice(FILE* arq){

	char numZero = '0';
	int zero = 0;
	char arroba = '@';


	fwrite(&numZero,sizeof(char),1,arq); //Quando comeca a gravar, status == 0. Caso chegue ao final com sucesso, valor eh alterado pra 1.
	fwrite(&zero, sizeof(int), 1, arq);

	for(int i = 0; i<31995; i++){ //Preenche com arrobas ate final da pag de disco.
		fwrite(&arroba,sizeof(char),1,arq);
	}
}

//Escreve no arquivo de INDICE.
void escreve_registro_indice(FILE* arq, registroIndice *registrosInserir, int quantidade){
	
	int tam = 0;
	char arroba = '@';
	for(int i = 0; i< quantidade; i++){
		if(strcmp(registrosInserir[i].chaveBusca, "REMOVIDO") != 0){
			tam = strlen(registrosInserir[i].chaveBusca);
			fwrite(&registrosInserir[i].chaveBusca, sizeof(char), tam+1, arq); //+1 por causa do '\0'.
			for(int j = 0; j< 119 - tam; j++){
				fwrite(&arroba,sizeof(char),1,arq);
			}
			fwrite(&registrosInserir[i].byteOffset, sizeof(long long int), 1, arq);
		}
	}
}

//Funcao le todos os registros e cria para todos aqueles que CONTEM nome um registro de INDICE.
//No final, ordena pelo nome(chaveBusca). Caso tenha empate, ByteOffset usado como desempate.
int le_binario_add_vetor_indice(FILE* arqB, registroIndice* leituraDados){

	fseek(arqB, 32000, SEEK_SET); //Pulando primeira pagina de disco.

	//Esta funcao eh uma adaptacao da funcionalidade 2/4/6. Eh feita leitura do registro, sem printar.

	Dados printar;
	int tamAuxiliar = 0;
	long long int bytes = 32000;
	int auxBytes = 0;
	long long int BYTEOFFSET = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	char barraZero = '\0';
	char n = 'n';
	char c = 'c';

	int qntdArrobas = 0;
	
	int quantidade = 0;

	fread(&lixo, sizeof(char), 1, arqB);
	while(lixo == '-' || lixo == '@' || lixo == '*'){ //Adaptacao pois agora existem registros removidos que sao sinalizados com '*'. 
		if(lixo == '*'){
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);
			if(auxBytes == 32000)auxBytes = 0;

		}
		else if(lixo == '-'){
			memset(printar.nome, '\0', 150);
			memset(printar.cargo, '\0', 150);
			memset(printar.telefone, '\0', 14);

			BYTEOFFSET = (long long int)(ftell(arqB)-1);
			fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
			bytes = bytes + printar.tamanhoRegistro + 5;
			auxBytes = auxBytes + printar.tamanhoRegistro + 5;
			fseek(arqB, 8, SEEK_CUR);	
			fread(&printar.id,sizeof(int), 1, arqB);
			fread(&printar.salario,sizeof(double), 1, arqB);
			fread(&printar.telefone,sizeof(char), 14, arqB);
			printar.telefone[14] = '\0';
			if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
				//Nao precisa fazer nada.
			}



			else if(auxBytes == 32000){ //FInal pagina de disco.
				
				qntdArrobas = printar.tamanhoRegistro;
				fread(&lixo, sizeof(char), 1, arqB);
				auxBytes = 0;
				if(lixo == '@'){ //NAo tem nome nem cargo.
					qntdArrobas -= 34;
				}

				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					qntdArrobas = printar.tamanhoRegistro - 34;
					fread(&lixo, sizeof(char), 1, arqB);
					
					if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						qntdArrobas -= printar.tamanhoNome;
						qntdArrobas -= 4;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo == '@'){ //Tem apenas nome.
							fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
						}
						else{ //Tem nome e cargo.
							fseek(arqB, -1, SEEK_CUR);
							fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
							qntdArrobas -= printar.tamanhoCargo;
							qntdArrobas -= 4;
							fread(&lixo, sizeof(char), 1, arqB);
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}

					}
					else{ //Tem so cargo.
						printar.tamanhoCargo = tamAuxiliar;
						qntdArrobas -= printar.tamanhoCargo;
						qntdArrobas -= 4;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
				printar.tamanhoRegistro -= qntdArrobas;
				
				qntdArrobas = 0;
			}



			else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo == '@'){ //NAo tem nome nem cargo.
					qntdArrobas = 1;
					fread(&lixo, sizeof(char), 1, arqB);
					while(lixo == '@'){
						qntdArrobas++;
						fread(&lixo, sizeof(char), 1, arqB);
					}
					printar.tamanhoRegistro -= qntdArrobas;
					qntdArrobas = 0;
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				}
				else{
					fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo != 'n' && lixo != 'c'){ //Nao tem nome nem cargo
						fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
					}
					else if(lixo == 'n'){ //Tem nome.
						printar.tamanhoNome = tamAuxiliar;
						fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						
						fread(&tamAuxiliar,sizeof(int), 1, arqB);
						fread(&lixo, sizeof(char), 1, arqB);
						if(lixo != 'c'){
							fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
						}
						else{ //Tem nome e cargo.
							printar.tamanhoCargo = tamAuxiliar;
							fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
						}
						
					}

					else{ //Tem apenas cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}
				}	
			}
			
			
			//Aqui passa os dados para o vetor.

			if(printar.tamanhoNome > 0){ //Tem nome e portanto sera adicionado no registroIndice.

				strcpy(leituraDados[quantidade].chaveBusca, "\0");
				strcpy(leituraDados[quantidade].chaveBusca, printar.nome);
				leituraDados[quantidade].byteOffset = BYTEOFFSET;
				quantidade++;
			}
			

			//Zerando o registro.
			printar.tamanhoNome = 0;
			printar.salario = -1;
			printar.telefone[0] = '\0';
			printar.nome[0] = '\0';
			printar.cargo[0] = '\0';
			printar.tamanhoCargo = 0;
			printar.tamanhoRegistro = 0;


		}
		
		fseek(arqB, bytes, SEEK_SET);
		fread(&lixo, sizeof(char), 1, arqB);
		if(feof(arqB) != 0)break; //Caso chege no final arquivo, break.
		
	}

	MS_sort(leituraDados, quantidade, sizeof(registroIndice), string_compare); 
	return quantidade; 

}

//A partir de um arquivo com os registros de dados, cria um arquivo com INDICES, utilizando 
//como chave NomeServidor(chave secundaria).
void cria_indice_secundario_fortemente_ligado(){

	char nomeArquivoEntrada[30];
	scanf("%s", nomeArquivoEntrada); //Lendo nome do arquivo de entrada.

	char nomeArquivoIndice[30];
	scanf("%s", nomeArquivoIndice); //Lendo nome do arquivo de indice.


	FILE* arqEntrada;
	arqEntrada = fopen(nomeArquivoEntrada, "rb+");
	if(arqEntrada == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqEntrada);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqEntrada);
		return;
	}
	
	char numUm = '1';
	char numZero = '0';
	rewind(arqEntrada);
	fwrite(&numZero,sizeof(char),1,arqEntrada);

	FILE* arqIndice;
	arqIndice = fopen(nomeArquivoIndice, "w+b");

	//Inicio da Funcionalidade

	grava_cabecalho_indice(arqIndice);
	registroIndice *registrosLidos;
	registrosLidos = (registroIndice*) malloc (6000 * sizeof(registroIndice));

	int quantidade = 0;
	quantidade = le_binario_add_vetor_indice(arqEntrada, registrosLidos);
	escreve_registro_indice(arqIndice, registrosLidos, quantidade);
	free(registrosLidos);
	fseek(arqIndice,1, SEEK_SET);
	fwrite(&quantidade, sizeof(int), 1, arqIndice);

	//Chegando final da funcionalidade 10 fecha os arquivos e usa binarionatela.


	rewind(arqEntrada);
	fwrite(&numUm,sizeof(char),1,arqEntrada); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(arqIndice);
	fwrite(&numUm,sizeof(char),1,arqIndice);
	binarioNaTela1(arqIndice);//Comparacao RUNCODES.
	fclose(arqEntrada);//Fecha arquivo binario.
	fclose(arqIndice);
}

//Dado o arquivo de indices e quantidade de registros nele presente, passa tudo para RAM.
void recupera_registros_indice(FILE* arq, registroIndice *reg, int qtd){

	fseek(arq, 32000, SEEK_SET);

	for(int i = 0; i<qtd; i++){
		fread(&reg[i].chaveBusca, sizeof(char), 120, arq);
		fread(&reg[i].byteOffset, sizeof(long long int), 1, arq);
	}
}

//Dado um ByteOffset que é o inicio de um registro, vai ate a posicao e faz a leitura do registro e 
//entao printa.
void le_registro_e_printa(FILE* arqB, long long int BO, registroCabecalho cab){

	fseek(arqB, BO, SEEK_SET);

	int tamAuxiliar = 0;
	long long int bytes = ftell(arqB);
	long long int auxBytes = 0;
	char lixo;
	char removido = '*';
	char arroba = '@';
	char barraZero = '\0';
	char n = 'n';
	char c = 'c';

	int qntdArrobas = 0;

	//Zerando o registro.

	Dados printar;
	strcpy(printar.nome, "\0");
	strcpy(printar.cargo, "\0");
	strcpy(printar.telefone, "@@@@@@@@@@@@@@");
	printar.tamanhoNome = 0;
	printar.salario = -1;
	printar.tamanhoCargo = 0;
	printar.tamanhoRegistro = 0;
	
	
	fread(&lixo, sizeof(char), 1, arqB);
	if(feof(arqB) != 0)return; //Final do arquivo.
	
	if(lixo == '-'){
		memset(printar.nome, '\0', 150);
		memset(printar.cargo, '\0', 150);
		memset(printar.telefone, '\0', 14);

	
		fread(&printar.tamanhoRegistro,sizeof(int), 1, arqB);
		bytes = bytes + printar.tamanhoRegistro + 5;
		auxBytes = bytes % 32000; //Verificar se esta final da pagina de disco.
		fseek(arqB, 8, SEEK_CUR);	
		fread(&printar.id,sizeof(int), 1, arqB);
		fread(&printar.salario,sizeof(double), 1, arqB);
		fread(&printar.telefone,sizeof(char), 14, arqB);
		printar.telefone[14] = '\0';
		if(printar.tamanhoRegistro - 34 == 0){ //Nao tem nome nem cargo.
			//Nao precisa fazer nada.
		}



		else if(auxBytes == 0){ //FInal pagina de disco.
				
			qntdArrobas = printar.tamanhoRegistro;
			fread(&lixo, sizeof(char), 1, arqB);

			if(lixo == '@'){ //NAo tem nome nem cargo.
				qntdArrobas -= 34;
			}

			else{
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				qntdArrobas = printar.tamanhoRegistro - 34;
				fread(&lixo, sizeof(char), 1, arqB);
				
				if(lixo == 'n'){ //Tem nome.
					printar.tamanhoNome = tamAuxiliar;
					qntdArrobas -= printar.tamanhoNome;
					qntdArrobas -= 4;
					fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo == '@'){ //Tem apenas nome.
						fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
					}
					else{ //Tem nome e cargo.
						fseek(arqB, -1, SEEK_CUR);
						fread(&printar.tamanhoCargo,sizeof(int), 1, arqB);
						qntdArrobas -= printar.tamanhoCargo;
						qntdArrobas -= 4;
						fread(&lixo, sizeof(char), 1, arqB);
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}

				}
				else{ //Tem so cargo.
					printar.tamanhoCargo = tamAuxiliar;
					qntdArrobas -= printar.tamanhoCargo;
					qntdArrobas -= 4;
					fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
				}

			}
			printar.tamanhoRegistro -= qntdArrobas;
				
		}



		else{ //Nao esta final pagina de disco e tem pelo menos nome ou cargo(ou os dois).
			fread(&lixo, sizeof(char), 1, arqB);
			if(lixo == '@'){ //NAo tem nome nem cargo.
				qntdArrobas = 1;
				fread(&lixo, sizeof(char), 1, arqB);
				while(lixo == '@'){
					qntdArrobas++;
					fread(&lixo, sizeof(char), 1, arqB);
				}
				printar.tamanhoRegistro -= qntdArrobas;
				qntdArrobas = 0;
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
			}
			else{
				fseek(arqB, -1, SEEK_CUR); //**volta 1 byte **
				fread(&tamAuxiliar,sizeof(int), 1, arqB);
				fread(&lixo, sizeof(char), 1, arqB);
				if(lixo != 'n' && lixo != 'c'){ //Nao tem nome nem cargo
					fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
				}
				else if(lixo == 'n'){ //Tem nome.
					printar.tamanhoNome = tamAuxiliar;
					fread(&printar.nome,sizeof(char), printar.tamanhoNome-1, arqB);
						
					fread(&tamAuxiliar,sizeof(int), 1, arqB);
					fread(&lixo, sizeof(char), 1, arqB);
					if(lixo != 'c'){
						fseek(arqB, -5, SEEK_CUR); //**volta 5 bytes **
					}
					else{ //Tem nome e cargo.
						printar.tamanhoCargo = tamAuxiliar;
						fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
					}
						
				}

				else{ //Tem apenas cargo.
					printar.tamanhoCargo = tamAuxiliar;
					fread(&printar.cargo,sizeof(char), printar.tamanhoCargo-1, arqB);
				}
			}	
		}
	
	}
		
	printf("%s: %d\n", cab.desCampo1, printar.id); //ID.
	if(printar.salario == -1){ //Salario NULO.
		printf("%s: valor nao declarado\n", cab.desCampo2);
	}
	else{
		printf("%s: %.2lf\n", cab.desCampo2, printar.salario); //Salario.
	}
	if(printar.telefone[0] == '\0'){
		printf("%s: valor nao declarado\n", cab.desCampo3);//Telefone NULO.
	}
	else{
		printf("%s: %s\n", cab.desCampo3, printar.telefone); //Telefone.
	}
	if(printar.tamanhoNome == 0){
		printf("%s: valor nao declarado\n", cab.desCampo4); //Nome NULO.
	}
	else{
		printf("%s: %s\n", cab.desCampo4, printar.nome);//Nome.
	}
	if(printar.tamanhoCargo == 0){
		printf("%s: valor nao declarado\n", cab.desCampo5);//Cargo NULO.
	}
	else{
		printf("%s: %s\n", cab.desCampo5, printar.cargo); //Cargo.
	}
	printf("\n");
}

//Funcionalidade 11, busca no arquivo de indices por uma chave de busca(nomeServidor) passa pelo stdin.
//Caso encontre pelo menos uma correspondencia, da um fseek para o local do registro e printa seus dados.
void funcionalidade11(){

	char nomeArquivoDados[30];
	scanf("%s", nomeArquivoDados); //Lendo nome do arquivo de dados.

	char nomeArquivoIndice[30];
	scanf("%s", nomeArquivoIndice); //Lendo nome do arquivo de indice.

	char campoBusca[30];
	scanf("%s", campoBusca);
	scanf("%*c");

	char chaveDeBusca[120];
	scanf("%[^\n\r]", chaveDeBusca); //Lendo chave a ser buscada(NomeServidor).


	FILE* arqDados;
	arqDados = fopen(nomeArquivoDados, "rb+");
	if(arqDados == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqDados);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}

	FILE* arqIndice;
	arqIndice = fopen(nomeArquivoIndice, "rb+");
	if(arqIndice == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}
	fread(&lixo, sizeof(char), 1, arqIndice);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		fclose(arqIndice);
		return;
	}

	char numUm = '1';
	char numZero = '0';
	rewind(arqDados);
	fwrite(&numZero,sizeof(char),1,arqDados);
	rewind(arqIndice);
	fwrite(&numZero,sizeof(char),1,arqIndice);

	//Comeco da funcionalidade.

	
	int quantidadeRegistrosIndice = 0; //Quantidade de registros presentes no arquivo de indice.
	fread(&quantidadeRegistrosIndice, sizeof(int), 1, arqIndice); //Lendo do cabecalho.
	registroIndice *registrosNoArqIndice;
	registrosNoArqIndice = (registroIndice*) malloc (quantidadeRegistrosIndice * sizeof(registroIndice));
	recupera_registros_indice(arqIndice, registrosNoArqIndice, quantidadeRegistrosIndice); //REcupera todos os dados do indice.
	
	int registros_encontrados = 0; //Quantidade de registros que correspondem a busca.

	registroCabecalho cab = leitura_cab(arqDados); //Leitura do registro cabecalho do arquivo de dados.

	int pos = busca_binaria(registrosNoArqIndice, 0, quantidadeRegistrosIndice-1, chaveDeBusca); //Busca binaria.

	if(pos == -1){ //SE a busca binaria retorna -1 entoa nao foi encontrado nenhum registro.
		registros_encontrados = 0;
	}
	else{ //CAso contrario existe pelo menos um registro. While retorna ate a primeira aparicao do nome, e depois vai ate a ultima aparicao, printando.
		while(pos != 0 && strcmp(registrosNoArqIndice[pos-1].chaveBusca, chaveDeBusca) == 0){
			pos--;
		}
		le_registro_e_printa(arqDados, registrosNoArqIndice[pos].byteOffset, cab); //Chama funcao que le o registro e printa.
		registros_encontrados++; //Registro encontrado e printado.
		while(pos != quantidadeRegistrosIndice &&strcmp(registrosNoArqIndice[pos+1].chaveBusca, chaveDeBusca) == 0){
			pos++;
			le_registro_e_printa(arqDados, registrosNoArqIndice[pos].byteOffset, cab); //Chama funcao que le o registro e printa.
			registros_encontrados++; //Registro encontrado e printado.
		}
	}
	
	long int pagsDisco = 0;

	if(registros_encontrados == 0){ //Caso nenhum registro tenha sido encontrado.
		printf("Registro inexistente.\n");
	}
	else{
		fseek(arqIndice, 0, SEEK_END);
		pagsDisco = ftell(arqIndice);
		if(pagsDisco % 32000 == 0){
			pagsDisco = pagsDisco / 32000;
		}
		else{
				pagsDisco = (pagsDisco / 32000) + 1;
		}
		printf("Número de páginas de disco para carregar o arquivo de índice: %ld\n", pagsDisco);
		printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", registros_encontrados);
	}


	//Chegando final da funcionalidade 11 fecha os arquivos.


	rewind(arqDados);
	fwrite(&numUm,sizeof(char),1,arqDados); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(arqIndice);
	fwrite(&numUm,sizeof(char),1,arqIndice);
	fclose(arqDados);//Fecha arquivo de dados.
	fclose(arqIndice); //Fecha arquivo indice.
}

//Funcionalidade 12, realiza remocao logica de um registro do arquivo de DAdos. Alem disso, caso o registro tenha o campo
//"nomeServidor" nao nulo, remove do arquivo de indice.
void funcionalidade12(){

	char nomeArquivoDados[30];
	scanf("%s", nomeArquivoDados); //Lendo nome do arquivo de dados.

	char nomeArquivoIndice[30];
	scanf("%s", nomeArquivoIndice); //Lendo nome do arquivo de indice.

	int numRemocoes; //Quantidade de remocoes.
	scanf("%d", &numRemocoes);


	FILE* arqDados;
	arqDados = fopen(nomeArquivoDados, "rb+");
	if(arqDados == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqDados);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}

	FILE* arqIndice;
	arqIndice = fopen(nomeArquivoIndice, "rb+");
	if(arqIndice == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}
	fread(&lixo, sizeof(char), 1, arqIndice);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		fclose(arqIndice);
		return;
	}

	char numUm = '1';
	char numZero = '0';
	rewind(arqDados);
	fwrite(&numZero,sizeof(char),1,arqDados);
	rewind(arqIndice);
	fwrite(&numZero,sizeof(char),1,arqIndice);







	//Comeco da funcionalidade.


	char campoBuscado[40]; //Campo a ser buscado.
	dadosBusca dados;
	LISTA* list = cria_lista(); //Criando lista de removidos.
	recupera_lista_em_disco(arqDados, list); //Recupera lista de removidos.

	int quantidadeRegistrosIndice = 0; //Quantidade de registros presentes no arquivo de indice.
	fread(&quantidadeRegistrosIndice, sizeof(int), 1, arqIndice); //Lendo do cabecalho.
	registroIndice *registrosNoArqIndice; 
	registrosNoArqIndice = (registroIndice*) malloc (quantidadeRegistrosIndice * sizeof(registroIndice));
	recupera_registros_indice(arqIndice, registrosNoArqIndice, quantidadeRegistrosIndice);

	int quantidadeRemovidos = 0;

	int pos =0;

	for(int i = 0; i< numRemocoes; i++){
		scanf("%s", campoBuscado);
		scanf("%*c"); //Lendo e ignorando espaco.


		scan_quote_string(dados.nomeBuscado);
		if(strcmp(dados.nomeBuscado, "") == 0){
			dados.nomeBuscado[0] = '\0';
		}
		pos = busca_binaria(registrosNoArqIndice, 0, quantidadeRegistrosIndice-1, dados.nomeBuscado); //Busca binario.
		if(pos != -1){ //SE a busca binaria retorna -1 entoa nao foi encontrado nenhum registro.
			while(pos != 0 && strcmp(registrosNoArqIndice[pos-1].chaveBusca, dados.nomeBuscado) == 0){ //VOltando ate a primeira aparicao(podem existir repeticoes).
			pos--;
			}
			remocao_logica_dado_BO(arqDados, list, registrosNoArqIndice[pos].byteOffset);//Remove
			strcpy(registrosNoArqIndice[pos].chaveBusca, "REMOVIDO"); //Marca como removido para que nao seja reescrito no arquivo indice.
			quantidadeRemovidos++; //Registro removido.

			while(pos != quantidadeRegistrosIndice &&strcmp(registrosNoArqIndice[pos+1].chaveBusca, dados.nomeBuscado) == 0){ //VAi ate a ultima aparicao.
				pos++;
				remocao_logica_dado_BO(arqDados, list, registrosNoArqIndice[pos].byteOffset);//Remove.
				strcpy(registrosNoArqIndice[pos].chaveBusca, "REMOVIDO"); //Marca como removido para que nao seja reescrito no arquivo indice.
				quantidadeRemovidos++; //Registro removido.
			}
		}
	}


	fclose(arqIndice); //Fecha o arquivo de indice antigo.
	arqIndice = fopen(nomeArquivoIndice, "wb+"); //Abre novo arquivo para escrita do indice(mesmo nome).
	grava_cabecalho_indice(arqIndice);
	escreve_registro_indice(arqIndice, registrosNoArqIndice, quantidadeRegistrosIndice);

	int aux = quantidadeRegistrosIndice - quantidadeRemovidos;
	fseek(arqIndice,1, SEEK_SET);
	fwrite(&aux, sizeof(int), 1, arqIndice);


	//Chegando final da funcionalidade 12 fecha os arquivos e usa binarionatela.




	free(registrosNoArqIndice);
	insere_lista_em_disco(arqDados, list); //Chama funcao para inserir a lista em dico.
	rewind(arqDados);
	fwrite(&numUm,sizeof(char),1,arqDados); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(arqIndice);
	fwrite(&numUm,sizeof(char),1,arqIndice);
	binarioNaTela1(arqIndice);//Comparacao RUNCODES.
	fclose(arqDados);//Fecha arquivo binario.
	fclose(arqIndice);
}

//Funcionalidade 13, realiza a insercao de registros no arquivo de Dados. Adicionalmente, caso o registro inserido tenha
//o campo "nomeServidor" nao nulo, é necessario adiciona-lo no arquivo de indices.
void funcionalidade13(){

	char nomeArquivoDados[30];
	scanf("%s", nomeArquivoDados); //Lendo nome do arquivo de dados.

	char nomeArquivoIndice[30];
	scanf("%s", nomeArquivoIndice); //Lendo nome do arquivo de indice.

	int numInsercoes; //Quantidade de remocoes.
	scanf("%d", &numInsercoes);


	FILE* arqDados;
	arqDados = fopen(nomeArquivoDados, "rb+");
	if(arqDados == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqDados);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}

	FILE* arqIndice;
	arqIndice = fopen(nomeArquivoIndice, "rb+");
	if(arqIndice == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}
	fread(&lixo, sizeof(char), 1, arqIndice);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		fclose(arqIndice);
		return;
	}

	char numUm = '1';
	char numZero = '0';
	rewind(arqDados);
	fwrite(&numZero,sizeof(char),1,arqDados);
	rewind(arqIndice);
	fwrite(&numZero,sizeof(char),1,arqIndice);

	//Comeco da funcionalidade.




	LISTA* list = cria_lista(); //Criando lista de removidos.
	recupera_lista_em_disco(arqDados, list);

	int quantidadeRegistrosIndice = 0; //Quantidade de registros presentes no arquivo de indice.
	fread(&quantidadeRegistrosIndice, sizeof(int), 1, arqIndice); //Lendo do cabecalho.
	registroIndice *registrosNoArqIndice;
	int TOTAL = quantidadeRegistrosIndice + numInsercoes; //Total de registros.
	registrosNoArqIndice = (registroIndice*) malloc (TOTAL * sizeof(registroIndice));
	recupera_registros_indice(arqIndice, registrosNoArqIndice, quantidadeRegistrosIndice);

	int quantidadeInseridosIndice = 0;
	char stringSalario[10];
	Dados registro_inserir; //Registro a ser inserido.
	long long int BO = 0;

	for(int i = 0; i<numInsercoes; i++){
		scanf("%d", &registro_inserir.id);
		scanf("%*c");
		scan_quote_string(stringSalario);
		if(stringSalario[0] == '\0'){
			registro_inserir.salario = -1;
		}
		else{
			registro_inserir.salario = atof(stringSalario);
		}
		scan_quote_string(registro_inserir.telefone);
		scan_quote_string(registro_inserir.nome);
		scan_quote_string(registro_inserir.cargo);

		if(registro_inserir.nome[0] == '\0'){ //Caso nome seja NULO.
			registro_inserir.tamanhoNome = -4; //-4 -> int tamanhoNome(para tirar do tamanhoRegistro que contabiliza esse int).
		}
		else{
			registro_inserir.tamanhoNome = strlen(registro_inserir.nome) + 2; //Barra zero + tag.
		}


		if(registro_inserir.cargo[0] == '\0'){ //-4 -> int tamanhoCargo(para tirar do tamanhoRegistro que contabiliza esse int).
			registro_inserir.tamanhoCargo = -4;
		}
		else{
			registro_inserir.tamanhoCargo = strlen(registro_inserir.cargo) + 2; //Barra zero + tag.
		}
		registro_inserir.tamanhoRegistro = 42 + registro_inserir.tamanhoNome + registro_inserir.tamanhoCargo;
		BO = insercao_registro(arqDados, list, registro_inserir); //Insere.
		//Ao chegar aqui registro ja foi inserido no arq de Dados. Basta verificar se deve ser colocado no arq indice.
		if(registro_inserir.tamanhoNome > 0){ //Tem nome e portanto entra no indice.
			strcpy(registrosNoArqIndice[quantidadeRegistrosIndice + i].chaveBusca, registro_inserir.nome);
			registrosNoArqIndice[quantidadeRegistrosIndice + i].byteOffset = BO;
			quantidadeInseridosIndice++;
		}
		else{
			strcpy(registrosNoArqIndice[quantidadeRegistrosIndice+i].chaveBusca, "REMOVIDO");//Caso nao tenha nome marca como removido para que nao seja passado p o arquivo de indice.
		}

		strcpy(registro_inserir.telefone, "");
		strcpy(registro_inserir.nome, "");
		strcpy(registro_inserir.cargo, "");
		registro_inserir.tamanhoRegistro = 0;
		registro_inserir.tamanhoNome = 0;
		registro_inserir.tamanhoCargo = 0;
	}

	MS_sort(registrosNoArqIndice, TOTAL, sizeof(registroIndice), string_compare); //Ordena os registros indice.
	fclose(arqIndice);
	arqIndice = fopen(nomeArquivoIndice, "wb+"); //ABre para escrever novamente arquivo de indice.
	grava_cabecalho_indice(arqIndice);
	escreve_registro_indice(arqIndice, registrosNoArqIndice, TOTAL);

	int aux = quantidadeRegistrosIndice + quantidadeInseridosIndice;
	fseek(arqIndice,1, SEEK_SET);
	fwrite(&aux, sizeof(int), 1, arqIndice);
	//Chegando final da funcionalidade 13 fecha os arquivos e usa binarionatela.




	free(registrosNoArqIndice);
	insere_lista_em_disco(arqDados, list); //Chama funcao para inserir a lista em dico.
	rewind(arqDados);
	fwrite(&numUm,sizeof(char),1,arqDados); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(arqIndice);
	fwrite(&numUm,sizeof(char),1,arqIndice);
	binarioNaTela1(arqIndice);//Comparacao RUNCODES.
	fclose(arqDados);//Fecha arquivo binario.
	fclose(arqIndice);
}

//Funcionalidade 14, realiza comparacao de eficiencia entre a funcionalidade 11 e a 3, isto e, da busca pelo nome utilizando
//ou nao um indice secundario fortemente ligado como auxilio.
void funcionalidade14(){
	char nomeArquivoDados[30];
	scanf("%s", nomeArquivoDados); //Lendo nome do arquivo de dados.

	char nomeArquivoIndice[30];
	scanf("%s", nomeArquivoIndice); //Lendo nome do arquivo de indice.

	char campoBusca[30];
	scanf("%s", campoBusca);
	scanf("%*c");

	char chaveDeBusca[120];
	scanf("%[^\n\r]", chaveDeBusca); //Lendo chave a ser buscada(NomeServidor).


	FILE* arqDados;
	arqDados = fopen(nomeArquivoDados, "rb+");
	if(arqDados == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	char lixo;
	fread(&lixo, sizeof(char), 1, arqDados);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}

	FILE* arqIndice;
	arqIndice = fopen(nomeArquivoIndice, "rb+");
	if(arqIndice == NULL){ //Verificanndo se houve erro na abertura.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		return;
	}
	fread(&lixo, sizeof(char), 1, arqIndice);
	if(lixo == '0'){ //Caso arquivo esteja corrompido.
		printf("Falha no processamento do arquivo.\n");
		fclose(arqDados);
		fclose(arqIndice);
		return;
	}

	char numUm = '1';
	char numZero = '0';
	rewind(arqDados);
	fwrite(&numZero,sizeof(char),1,arqDados);
	rewind(arqIndice);
	fwrite(&numZero,sizeof(char),1,arqIndice);


	//Agora executar a funcionalidade 3.
	

	long long int pagDiscoFunc3 = 0; //Numero de paginas de disco acessadas

	printf("*** Realizando a busca sem o auxílio de índice\n");
	pagDiscoFunc3 = funcionalidade3(arqDados,4,0,0,"telefoneBuscado",chaveDeBusca,"cargoBuscado", 1); //FUncionalidade 3, sem auxilio do indice.

	
	rewind(arqDados);
	//Fim da execuacao da funcionalidade 3


	//Inicio execucao funcionalidade 11(COM auxilio indice)

	printf("*** Realizando a busca com o auxílio de um índice secundário fortemente ligado\n");
	int quantidadeRegistrosIndice = 0; //Quantidade de registros presentes no arquivo de indice.
	fread(&quantidadeRegistrosIndice, sizeof(int), 1, arqIndice); //Lendo do cabecalho.
	registroIndice *registrosNoArqIndice;
	registrosNoArqIndice = (registroIndice*) malloc (quantidadeRegistrosIndice * sizeof(registroIndice));
	recupera_registros_indice(arqIndice, registrosNoArqIndice, quantidadeRegistrosIndice); //REcupera todos os dados do indice.
	
	int registros_encontrados = 0; //Quantidade de registros que correspondem a busca.
	int vetor_paginas_disco[100]; //Sera utilizado para sinalizar acessos a uma pagina de disco especifica. Cada posicao e uma pagina de disco. Pos 0 eh o cabecario.
	int auxConta = 0;
	vetor_paginas_disco[0] = 1; 
	for(int k = 1; k<100; k++){
		vetor_paginas_disco[k] = 0; //Inicialmente nenhuma pagina de disco foi acessado(exceto cabecalho q eh posicao 0).
	}

	registroCabecalho cab = leitura_cab(arqDados); //Leitura do registro cabecalho do arquivo de dados.

	int pos = busca_binaria(registrosNoArqIndice, 0, quantidadeRegistrosIndice-1, chaveDeBusca); //Busca binaria.

	if(pos == -1){ //SE a busca binaria retorna -1 entoa nao foi encontrado nenhum registro.
		registros_encontrados = 0;
	}
	else{ //CAso contrario existe pelo menos um registro. While retorna ate a primeira aparicao do nome, e depois vai ate a ultima aparicao, printando.
		while(pos != 0 && strcmp(registrosNoArqIndice[pos-1].chaveBusca, chaveDeBusca) == 0){
			pos--;
		}
		le_registro_e_printa(arqDados, registrosNoArqIndice[pos].byteOffset, cab); //Chama funcao que le o registro e printa.
		if(registrosNoArqIndice[pos].byteOffset % 32000 == 0){
			auxConta = (int)(registrosNoArqIndice[pos].byteOffset / 32000) +1;
			vetor_paginas_disco[auxConta] = 1;
		}
		else{
			auxConta = (int)(registrosNoArqIndice[pos].byteOffset / 32000);
			vetor_paginas_disco[auxConta] = 1;
		}
		registros_encontrados++; //Registro encontrado e printado.
		while(pos != quantidadeRegistrosIndice &&strcmp(registrosNoArqIndice[pos+1].chaveBusca, chaveDeBusca) == 0){
			pos++;
			le_registro_e_printa(arqDados, registrosNoArqIndice[pos].byteOffset, cab); //Chama funcao que le o registro e printa.
			if(registrosNoArqIndice[pos].byteOffset % 32000 == 0){
				auxConta = (int)(registrosNoArqIndice[pos].byteOffset / 32000) +1;
				vetor_paginas_disco[auxConta] = 1;
			}
			else{
				auxConta = (int)(registrosNoArqIndice[pos].byteOffset / 32000);
				vetor_paginas_disco[auxConta] = 1;
			}
			registros_encontrados++; //Registro encontrado e printado.
		}
	}
	
	long int pagsDisco = 0;
	auxConta = 0;
	for(int l = 0; l < 100; l++){
		if(vetor_paginas_disco[l] == 1){ //Uma pagina de disco acessada, incrementa auxCOnta.
			auxConta++;
		}
	}

	if(registros_encontrados == 0){ //Caso nenhum registro tenha sido encontrado.
		printf("Registro inexistente.\n");
		fseek(arqIndice, 0, SEEK_END);
		pagsDisco = ftell(arqIndice);
		if(pagsDisco % 32000 == 0){
			pagsDisco = pagsDisco / 32000;
		}
		else{
				pagsDisco = (pagsDisco / 32000) + 1;
		}
		printf("Número de páginas de disco para carregar o arquivo de índice: %ld\n", pagsDisco);
		printf("Número de páginas de disco para acessar o arquivo de dados: 1\n"); //Neste caso apenas o cabecalho.
	}
	else{
		fseek(arqIndice, 0, SEEK_END);
		pagsDisco = ftell(arqIndice);
		if(pagsDisco % 32000 == 0){
			pagsDisco = pagsDisco / 32000;
		}
		else{
				pagsDisco = (pagsDisco / 32000) + 1;
		}
		printf("Número de páginas de disco para carregar o arquivo de índice: %ld\n", pagsDisco);
		printf("Número de páginas de disco para acessar o arquivo de dados: %d\n", auxConta); //AuxCOnta nesse momento armazena qntd paginas disco sacessadas.
	}


	//Fim execucao funcionalidade 11

	printf("\nA diferença no número de páginas de disco acessadas: %lld\n", pagDiscoFunc3 - auxConta);

	rewind(arqDados);
	fwrite(&numUm,sizeof(char),1,arqDados); //Se chegou ate aqui, arquivo nao esta corrompido e por isso mudar para valor 1.
	rewind(arqIndice);
	fwrite(&numUm,sizeof(char),1,arqIndice);
	fclose(arqDados);//Fecha arquivo de dados.
	fclose(arqIndice); //Fecha arquivo indice.
}

//Funcao "menu", faz a leitura da opcao desejada e faz a chamada da funcao utilizando switch case.
void menu(){
	int opcao;
	scanf("%d", &opcao);
	switch(opcao){
		case 1: leitura_csv();
		break;
		case 2: leitura_binario();
		break;
		case 3: busca_por_campo();
		break;
		case 4: leitura_remocao();
		break;
		case 5: leitura_insercao();
		break;
		case 6: leitura_atualizacao();
		break;
		case 7: ordenacao_interna();
		break;
		case 8: merging();
		break;
		case 9: matching();
		break;
		case 10: cria_indice_secundario_fortemente_ligado();
		break;
		case 11: funcionalidade11();
		break;
		case 12: funcionalidade12();
		break;
		case 13: funcionalidade13();
		break;
		case 14: funcionalidade14();
		break;
	}
}


int main(){
	menu();
	return 0; 
}
