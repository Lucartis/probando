/*
Fecha:02/04/2024
Materia:Sistemas Operativos
Autor: Lucas Rivera Mora
Titulo: Procesamiento paralelo
Fichero: Principal
Objetivo: Crear una aplicacion que use el algoritmo de Multiplicacion de
matrices clasica (filas por columnas). La aplicacion presentara diversos pasos
como metodologia para la implementacion de un algoritmo. La implementacion se
hara para matrices cuadradas, es decir el numero de filas = columnas

A continuacion se presentan las siguientes etapas:
  -Reserva de memoria
  -Se crea el main para ingresar argumentos de entrada
      -Arg1 dimensión matriz
      -Arg2 cantidad de hilos
  -Crear punteros para las matrices
  -Apuntar a la reserva de memoria las matrices NxN
  -Inicializar las matrices
  -Funciones (programacion modular)
      -Inicializar
      Imprimir (solo si N < 10)
  -Algoritmo Clasico Multiplicacion Matrices
  -Estructura de datos que encapsule datos
      -N, Th, y las matrices
      -se crea una variable del tipo struct creado
      -se asignan valores
  **************faltante*******************************
  -crear un vector de hilos segun el Th
  -validar el numero de hilos
  -Se hace el envio de trabajo para cada hilo

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define RESERVA (1024 * 128 * 64 * 8)
// estructura de datos
struct datos_MM {
  int N;
  int th;
  int IDhilo;
  double *mA;
  double *mB;
  double *mC;
};
// **************funciones*****************

void init_matrices(int n, double *m1, double *m2, double *m3) {
  for (int i = 0; i < n * n; i++) {
    m1[i] = i * 1.1;
    m2[i] = i * 2.2;
    m3[i] = i;
  }
}
void impr_matrices(int N, double *m) {
  if (N < 10) {
    for (int i = 0; i < N * N; i++) {
      if (i % N == 0) {
        printf("\n");
      }
      printf(" %f ", m[i]);
    }
  }
  printf("\n---------------------------------------------\n");
}
void multi_matrices(void *argMM) {
  // Desempacar la estructura de datos
  struct datos_MM *val = (struct datos_MM *)argMM;
  int N;
  double *mA;
  double *mB;
  double *mC;
  N = val->N;
  mA = val->mA;
  mB = val->mB;
  mC = val->mC;
  int hilo = val->IDhilo;
  int Numh = val->th;

  // dividir la matriz por hilos
  for (int i = hilo * N / Numh; i < (hilo + 1) * (N / Numh); i++) {
    // filas
    for (int i = 0; i < N; i++) {
      // columnas
      for (int j = 0; j < N; j++) {
        double sumTemp, *pA, *pB;
        sumTemp = 0.0;
        pA = mA + i * N;
        pB = mB + j;

        // producto suma
        for (int k = 0; k < N; k++, pA++, pB += N) {
          sumTemp += *pA * *pB;
        }
        mC[i * N + j] = sumTemp;
      }
    }
  }
}
// *******************************
// main
static double MEM_CHUNK[RESERVA];

int main(int argc, char *argv[]) {

  // validacion de argumentos
  if (argc <= 2) {
    printf("\n Argumentos de entrada \n");
    printf("\n\t $./ejecutable.exe [Dim] [Hilos] \n");
    return -1;
  }
  int N = (int)atof(argv[1]);
  int Th = (int)atof(argv[2]);

  // validacion de datos
  if (N <= 0) {
    printf("\n Dimensión matriz incorrecta \n");
    printf("\n\t Debe ser mayor que cero \n");
    return -1;
  }

  // se crean los hilos
  pthread_t vectorHilo[Th];

  // mis matrices
  double *mA, *mB, *mC;

  // asignacion de su espacio especifico de memoria
  mA = MEM_CHUNK;
  mB = mA + N * N;
  mC = mB + N * N;

  // iniciacion de matrices
  init_matrices(N, mA, mB, mC);

  // impresion de matrices
  impr_matrices(N, mA);
  impr_matrices(N, mB);

  // estructura para encapsular valores
  struct datos_MM *encValores =
      (struct datos_MM *)malloc(sizeof(struct datos_MM));
  encValores->N = N;
  encValores->mA = mA;
  encValores->mB = mB;
  encValores->mC = mC;
  encValores->th = Th;

  for (int h = 0; h < Th; h++) {
    encValores->IDhilo = h;
    pthread_create(&vectorHilo[h], NULL, (void *)multi_matrices,
                   (void *)encValores);
  }
  for (int h = 0; h < Th; h++) {
    pthread_join(vectorHilo[h], NULL);
  }

  // impresion de resultado
  printf("\n---------------------------------------------\n");
  impr_matrices(N, encValores->mC);

  printf("\n\n\n Fin del programa \n");

  pthread_exit(NULL);
  return 0;  
}
