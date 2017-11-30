/*********************************************************************************************
* EDA 2017/2 - ESTRUTURAS DE DADOS E ALGORITMOS (Prof. Fernando W. Cruz)
* Projeto  - Arvores e funcoes de hash
* Verifica corretude de palavras de um arquivo-texto segundo um dicionario carregado em RAM.
 *********************************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdbool.h>

/* Tamanho maximo de uma palavra do dicionario */
#define TAM_MAX 45
/* dicionario default */
#define NOME_DICIONARIO "dicioPadrao"

/* retornos desse programa */
#define SUCESSO                 0
#define NUM_ARGS_INCORRETO      1
#define ARQDICIO_NAOCARREGADO   2
#define ARQTEXTO_ERROABERTURA   3
#define ARQTEXTO_ERROLEITURA    4
#define ERRO_DICIO_NAOCARREGADO 5

#define TAM_MAX 45

typedef struct arv {
    char info[TAM_MAX];
    struct arv *esq;
    struct arv *dir;
}arv;

arv *a = NULL;
int qtd_nos = 0;

arv * rot_dir(arv *p){
    arv *q, *temp;
    q = p->esq;
    temp = q->dir;
    q->dir = p;
    p->esq = temp;
    p = q;

    return p;
}

arv * rot_esq(arv *p){
    arv *q, *temp;
    q = p->dir;
    temp = q->esq;
    q->esq = p;
    p->dir = temp;
    p = q;

    return p;
}

arv * rot_esq_dir(arv *p){
    p = rot_esq(p->esq);
    p = rot_dir(p);

    return p;
}

arv * rot_dir_esq(arv *p){
    p = rot_dir(p->dir);
    p = rot_esq(p);

    return p;
}

arv * novo_no(char *palavra){
    arv *novo = (arv*)malloc(sizeof(arv));

    strcpy(novo->info, palavra);
    novo->esq = NULL;
    novo->dir = NULL;

    return novo;
}

int calcula_nivel(arv *r) {
   if (r == NULL)
      return -1; // altura da árvore vazia
   else {
      int he = calcula_nivel(r->esq);
      int hd = calcula_nivel(r->dir);
      if (he < hd)
        return hd + 1;
      else
        return he + 1;
   }
}

int fat_bal(arv *p){
    int fat;

    if(p){
        fat = ((calcula_nivel(p->dir) + 1) - (calcula_nivel(p->esq) + 1));
        return fat;
    }

    return -1;
}

arv * balanceia_arv(arv *p){
    arv *temp;

    if(p){
        if(fat_bal(p) < -1){
            if(fat_bal(p->esq) < 0)
                p = rot_dir(p);
            else
                p = rot_esq_dir(p);
        }
        else if(fat_bal(p) > 1){
            if(fat_bal(p->dir) > 0)
                p = rot_esq(p);
            else
                p = rot_dir_esq(p);
        }
    }

    return p;
}

arv * inserir(arv *p, arv *elemento){
    if(!p)
        return elemento;
    else{
        if(elemento->info[0] > p->info[0]){
          p->dir = inserir(p->dir, elemento);
          p = balanceia_arv(p);
        }
        else{
          p->esq = inserir(p->esq, elemento);
          p = balanceia_arv(p);
        }
    }

    return p;
}

arv * descarrega(arv *a){
    if(a){
      descarrega(a->esq);
      descarrega(a->dir);
      free(a);
    }

    return NULL;
}

bool busca_arv(arv *a, const char *palavra){
    // if(a){
    //     if(strcasecmp(a->info, palavra) == 0)
    //         return true;
    //     else if(a->info[0] > palavra[0])
    //         return busca_arv(a->esq, palavra);
    //     else
    //         return busca_arv(a->dir, palavra);
    // }

    return false;
}

/* Retorna true se a palavra estah no dicionario. Do contrario, retorna false */
bool conferePalavra(const char *palavra) {
    if(busca_arv(a, palavra) == true)
        return true;

    return false;
} /* fim-conferePalavra */

/* Carrega dicionario na memoria. Retorna true se sucesso; senao retorna false. */
bool carregaDicionario(const char *dicionario) {
    FILE *fd = fopen(dicionario, "r");
    char palavra[TAM_MAX];

    if(fd == NULL){
        printf("Nao foi possivel abrir o arquivo dicionario %s\n", dicionario);
        return false;
    }

    while(!feof(fd)){
        fscanf(fd,"%s",palavra);
        a = inserir(a, novo_no(palavra));
        a = balanceia_arv(a);
        printf("%s\n", palavra);
        qtd_nos++;
    }

    if(feof(fd))
        return true;

    return false;
} /* fim-carregaDicionario */

/* Retorna qtde palavras do dicionario, se carregado; senao carregado retorna zero */
unsigned int contaPalavrasDic(void) {
    return qtd_nos;
} /* fim-contaPalavrasDic */

/* Descarrega dicionario da memoria. Retorna true se ok e false se algo deu errado */
bool descarregaDicionario(void) {
  // a = descarrega(a);
  //
  // if(!a)
  //   return true;
  // else
  //   return false;

  return true;

} /* fim-descarregaDicionario */

/* Retorna o numero de segundos entre a e b */
double calcula_tempo(const struct rusage *b, const struct rusage *a) {
    if (b == NULL || a == NULL)
        return 0;
    else
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                 (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                 (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
} /* fim-calcula_tempo */


int main(int argc, char *argv[]) {
    struct rusage tempo_inicial, tempo_final; /* structs para dados de tempo do processo */
    double tempo_carga = 0.0, tempo_check = 0.0, tempo_calc_tamanho_dic = 0.0, tempo_limpeza_memoria = 0.0;
    /* determina qual dicionario usar; o default eh usar o arquivo dicioPadrao */
    char *dicionario = (argc == 3) ? argv[1] : NOME_DICIONARIO;
    int  indice, totPalErradas, totPalavras, c;
    char palavra[TAM_MAX+1];
    bool palavraErrada, descarga, carga;
    unsigned int qtdePalavrasDic;
    char *arqTexto;
    FILE *fd;

    /* Confere se o numero de argumentos de chamada estah correto */
    if (argc != 2 && argc != 3) {
        printf("Uso: %s [nomeArquivoDicionario] nomeArquivoTexto\n", argv[0]);
        return NUM_ARGS_INCORRETO;
    } /* fim-if */

    /* carrega o dicionario na memoria, c/ anotacao de tempos inicial e final */
    getrusage(RUSAGE_SELF, &tempo_inicial);
       carga = carregaDicionario(dicionario);
    getrusage(RUSAGE_SELF, &tempo_final);

    /* aborta se o dicionario nao estah carregado */
    if (!carga) {
        printf("Dicionario nao carregado!\n");
        return ARQDICIO_NAOCARREGADO;
    } /* fim-if */

    /* calcula_tempo para carregar o dicionario */
    tempo_carga = calcula_tempo(&tempo_inicial, &tempo_final);

    /* tenta abrir um arquivo-texto para corrigir */
    arqTexto = (argc == 3) ? argv[2] : argv[1];
    fd = fopen(arqTexto, "r");
    if (fd == NULL) {
        printf("Nao foi possivel abrir o arquivo de texto %s.\n", arqTexto);
        descarregaDicionario();
        return ARQTEXTO_ERROABERTURA;
    } /* fim-if */

    /* Reportando palavras erradas de acordo com o dicionario */
    printf("\nPALAVRAS NAO ENCONTRADAS NO DICIONARIO \n\n");

    /* preparando para checar cada uma das palavras do arquivo-texto */
    indice = 0, totPalErradas = 0, totPalavras = 0;

    /* checa cada palavra do arquivo-texto  */
    for (c = fgetc(fd); c != EOF; c = fgetc(fd)) {
        /* permite apenas palavras c/ caracteres alfabeticos e apostrofes */
        if (isalpha(c) || (c == '\'' && indice > 0)) {
            /* recupera um caracter do arquivo e coloca no vetor palavra */
            palavra[indice] = c;
            indice++;
            /* ignora palavras longas demais (acima de 45) */
            if (indice > TAM_MAX) {
                /* nesse caso, consome o restante da palavra e "reseta" o indice */
                while ((c = fgetc(fd)) != EOF && isalpha(c));
                indice = 0;
            } /* fim-if */
        } /* fim-if */
        /* ignora palavras que contenham numeros */
        else if (isdigit(c)) {
            /* nesse caso, consome o restante da palavra e "reseta" o indice */
            while ((c = fgetc(fd)) != EOF && isalnum(c));
            indice = 0;
        } /* fim-if */
        /* encontra uma palavra completa */
        else if (indice > 0) { /* termina a palavra corrente */
            palavra[indice] = '\0';
            totPalavras++;
            /* confere o tempo de busca da palavra */
            getrusage(RUSAGE_SELF, &tempo_inicial);
              palavraErrada = !conferePalavra(palavra);
            getrusage(RUSAGE_SELF, &tempo_final);
            /* atualiza tempo de checagem */
            tempo_check += calcula_tempo(&tempo_inicial, &tempo_final);
            /* imprime palavra se nao encontrada no dicionario */
            if (palavraErrada) {
                printf("%s\n", palavra);
                totPalErradas++;
            } /* fim-if */
            /* faz "reset" no indice para recuperar nova palavra no arquivo-texto*/
            indice = 0;
        } /* fim-if */
    } /* fim-for */

    /* verifica se houve um erro na leitura do arquivo-texto */
    if (ferror(fd)) {
        fclose(fd);
        printf("Erro de leitura %s.\n", arqTexto);
        descarregaDicionario();
        return ARQTEXTO_ERROLEITURA;
    } /* fim-if */

    /* fecha arquivo */
    fclose(fd);

    /* determina a quantidade de palavras presentes no dicionario, c anotacao de tempos */
    getrusage(RUSAGE_SELF, &tempo_inicial);
      qtdePalavrasDic = contaPalavrasDic();
    getrusage(RUSAGE_SELF, &tempo_final);

    /* calcula tempo p determinar o tamanho do dicionario */
    tempo_calc_tamanho_dic = calcula_tempo(&tempo_inicial, &tempo_final);

    /* limpa a memoria - descarrega o dicionario, c anotacao de tempos */
    getrusage(RUSAGE_SELF, &tempo_inicial);
      descarga = descarregaDicionario();
    getrusage(RUSAGE_SELF, &tempo_final);

    /* aborta se o dicionario nao estiver carregado */
    if (!descarga) {
        printf("Nao foi necessario fazer limpeza da memoria\n");
        return ERRO_DICIO_NAOCARREGADO;
    } /* fim-if */

    /* calcula tempo para descarregar o dicionario */
    tempo_limpeza_memoria = calcula_tempo(&tempo_inicial, &tempo_final);

    /* RESULTADOS ENCONTRADOS */
    printf("\nTOTAL DE PALAVRAS ERRADAS NO TEXTO    : %d\n",   totPalErradas);
    printf("TOTAL DE PALAVRAS DO DICIONARIO         : %d\n",   qtdePalavrasDic);
    printf("TOTAL DE PALAVRAS DO TEXTO              : %d\n",   totPalavras);
    printf("TEMPO GASTO COM CARGA DO DICIONARIO     : %.2f\n", tempo_carga);
    printf("TEMPO GASTO COM CHECK DO ARQUIVO        : %.2f\n", tempo_check);
    printf("TEMPO GASTO P CALCULO TAMANHO DICIONARIO: %.2f\n", tempo_calc_tamanho_dic);
    printf("TEMPO GASTO COM LIMPEZA DA MEMORIA      : %.2f\n", tempo_limpeza_memoria);
    printf("------------------------------------------------------\n");
    printf("T E M P O   T O T A L                   : %.2f\n\n",
     tempo_carga + tempo_check + tempo_calc_tamanho_dic + tempo_limpeza_memoria);
    printf("------------------------------------------------------\n");

   return SUCESSO;
} /* fim-main */
