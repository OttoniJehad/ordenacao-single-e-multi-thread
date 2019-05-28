#include "iostream"
#include "process.h"
#include "Windows.h"
#include "vector"
#include "time.h"
#include "stdio.h"
//#include "YcStdAfx.h"
using namespace std;
int TAM = 1000000;


int vet[1000000]; // Tamanho do vetor para ordenar = 1.000.000
int seed = 21;


const int num_threads = 256;

typedef struct {
	int i_ini;
	int i_fim;
	vector<int> vet_thread;
}parametros;

HANDLE hthread[num_threads];
parametros p[num_threads];
HANDLE hSemafaro;
HANDLE hMutex;


//Gera o vetor com os numeros aleatorios
void geravetor()
{
	srand(seed); //definido um seed para o rand, no caso 21.

	for (int i = 0; i < TAM; i++)
	{
		/* Matriz recebendo valores entre zero e 30000 aleatoriamente */
		vet[i] = rand() % 1000000;
		//printf("%i\n", vet[i]);  //imprime os numeros do vetor
	}
	printf("NUMEROS ALEATORIOS GERADOS...\n");
}
// Ordena utilizando o MergeSort
void mergeSort(int *vetor, int posicaoInicio, int posicaoFim) {
	int i, j, k, metadeTamanho, *vetorTemp;
	if (posicaoInicio == posicaoFim) return;
	metadeTamanho = (posicaoInicio + posicaoFim) / 2;

	mergeSort(vetor, posicaoInicio, metadeTamanho);
	mergeSort(vetor, metadeTamanho + 1, posicaoFim);

	i = posicaoInicio;
	j = metadeTamanho + 1;
	k = 0;
	vetorTemp = (int *)malloc(sizeof(int) * (posicaoFim - posicaoInicio + 1));

	while (i < metadeTamanho + 1 || j  < posicaoFim + 1) {
		if (i == metadeTamanho + 1) {
			vetorTemp[k] = vetor[j];
			j++;
			k++;
		}
		else {
			if (j == posicaoFim + 1) {
				vetorTemp[k] = vetor[i];
				i++;
				k++;
			}
			else {
				if (vetor[i] < vetor[j]) {
					vetorTemp[k] = vetor[i];
					i++;
					k++;
				}
				else {
					vetorTemp[k] = vetor[j];
					j++;
					k++;
				}
			}
		}

	}
	for (i = posicaoInicio; i <= posicaoFim; i++) {
		vetor[i] = vetorTemp[i - posicaoInicio];
	}
	free(vetorTemp);
}
//Ordena o vetor
void Ordena(void *param) {



	parametros *pr = (parametros*)param;
	int i_ini = pr->i_ini;
	int i_fim = pr->i_fim;
	vector<int> vet_thread = pr->vet_thread;

	//system("PAUSE");


	for (int i = i_ini; i < i_fim; i++) {  // Adicionando todos os numeros do intervalo selecionado de cada thread
		vet_thread.push_back(vet[i]);
	}

	//mergeSort(vet, i_ini, i_fim);

	int aux;
	//printf("Realizando a ordenacao Thread...\n");
	for (size_t i = 0; i < vet_thread.size(); i++) {
		for (size_t j = i + 1; j < vet_thread.size(); j++) {
			if (vet_thread[i] > vet_thread[j]) { // Realizando a ordenação local no vetor da thread
				aux = vet_thread[i];
				vet_thread[i] = vet_thread[j];
				vet_thread[j] = aux;
				//printf("%d\n", vet_thread[j]);
			}
		}
	}


	int j = 0;
	for (int i = i_ini; i < i_fim; i++) {
		vet[i] = vet_thread[j];
		j++;
		//printf("%d\n",vet_thread[j]);		
	}

	_endthread(); // Finalizando a Thread
}
// Ordena com threads usando MergeSort
void OrdenaMergeSort(void *param) {
	parametros *pr = (parametros*)param;
	int i_ini = pr->i_ini;
	int i_fim = pr->i_fim;

	mergeSort(vet, i_ini, i_fim);

	_endthread(); // Finalizando a Thread
}
// Distribui os intervalos que os numeros irão ordenar
void Divide()// função que divide pra as threads o tamanho que cada um vai processar
{
	int qndThread = TAM / num_threads;

	for (int i = 0; i < num_threads; i++) { //criando as threads

		if (i == 0) {					   // Caso seja a primeira thread
			p[i].i_ini = 0;
			p[i].i_fim = qndThread;
		}
		else if (i < num_threads - 1) {       // Threads do inicio + 1 e fim -1 (Ex. de 0 a 9 trata as 1 a 8)
			p[i].i_ini = qndThread * i + 1;
			p[i].i_fim = qndThread * (i + 1);
		}
		else if (i < num_threads) {		   // Caso expecial para ultima thread, recebe todos dados até o final para ordenar
			p[i].i_ini = qndThread * i + 1;
			p[i].i_fim = TAM;
		}

		//printf("%d\n%d\n\n",p[i].i_ini, p[i].i_fim);

		hthread[i] = (HANDLE)_beginthread(Ordena, 0, &p[i]); // Usando thread para ordenar com o BobbleSort
															 //hthread[i] = (HANDLE)_beginthread(OrdenaMergeSort, 0, &p[i]); // Para usar as threads no MergSort

	}

	//printf("Processing......");
	WaitForMultipleObjects(num_threads, hthread, TRUE, 1000000);


}
// Ordena utilizando o BubbleSort
void bubbleSort() {
	int aux;
	for (int i = 0; i < TAM; i++) {
		for (int j = i; j < TAM; j++) {
			if (vet[i] > vet[j]) {
				aux = vet[i];
				vet[i] = vet[j];
				vet[j] = aux;
			}
		}
	}
}
//Main
int main()
{
	time_t tinicial, tfinal; // Variaveis utilizadas para cronometrar o tempo gasto para ordenação
	geravetor();
	tinicial = time(0);
	Divide();
	WaitForMultipleObjects(num_threads, hthread, TRUE, 1000000);

	// Realizando a ordenaçaõ final
	printf("Realizando a ordenacao final...\n");

	bubbleSort();
	//mergeSort(vet,0,TAM);

	tfinal = time(0);


	/*for (int i = 0; i < TAM; i++) {

	printf(" %d\n", vet[i]);
	}*/
	cout << endl << endl << "O tempo total de execa?o foi " << tfinal - tinicial << endl;
	system("PAUSE");

}



