#include <stdio.h>	// libreria estandar
#include <stdlib.h> // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>   // para usar threads
#include <semaphore.h> // para usar semaforos
#include <unistd.h>

#define LIMITE 50

pthread_mutex_t m_salero;
pthread_mutex_t m_sarten;
pthread_mutex_t m_horno;

// creo estructura de semaforos
struct semaforos
{
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_empanar;
	sem_t sem_cocinar;
	sem_t sem_hornear;
	sem_t sem_armar;
	sem_t sem_pan;
	sem_t sem_entregar;
};

// creo los pasos con los ingredientes
struct paso
{
	char accion[LIMITE];
	char ingredientes[4][LIMITE];
};

// creo los parametros de los hilos
struct parametro
{
	int equipo_param;
	struct semaforos semaforos_param;
	struct paso pasos_param[8];
};

// funcion para imprimir las acciones y los ingredientes de la accion
void *imprimirAccion(void *data, char *accionIn)
{
	struct parametro *mydata = data;
	// calculo la longitud del array de pasos
	int sizeArray = (int)(sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	// indice para recorrer array de pasos
	int i;
	for (i = 0; i < sizeArray; i++)
	{
		if (strcmp(mydata->pasos_param[i].accion, accionIn) == 0)
		{
			printf("\tEquipo %d - accion %s \n ", mydata->equipo_param, mydata->pasos_param[i].accion);
			// calculo la longitud del array de ingredientes
			int sizeArrayIngredientes = (int)(sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]));
			// indice para recorrer array de ingredientes
			int h;
			printf("\tEquipo %d -----------ingredientes : ----------\n", mydata->equipo_param);
			for (h = 0; h < sizeArrayIngredientes; h++)
			{
				// consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion
				if (strlen(mydata->pasos_param[i].ingredientes[h]) != 0)
				{
					printf("\tEquipo %d ingrediente  %d : %s \n", mydata->equipo_param, h, mydata->pasos_param[i].ingredientes[h]);
				}
			}
		}
	}
}

// funcion para tomar de ejemplo
void *cortar(void *data)
{
	// creo el nombre de la accion de la funcion
	char *accion = "cortar";
	// creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
	struct parametro *mydata = data;
	// llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata, accion);
	// uso sleep para simular que que pasa tiempo
	usleep(20000);
	// doy la señal a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_mezclar);

	pthread_exit(NULL);
}

void *mezclar(void *data)
{
	char *accion = "mezclar";
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_mezclar);
	imprimirAccion(mydata, accion);
	usleep(1000000);
	sem_post(&mydata->semaforos_param.sem_salar);

	pthread_exit(NULL);
}

void *salar(void *data)
{
	struct parametro *mydata = data;
	int equipo = *((int *)&mydata->equipo_param);

	sem_wait(&mydata->semaforos_param.sem_salar);
	pthread_mutex_lock(&m_salero);
	printf("\nEl equipo %d está usando el salero\n", equipo);
	usleep(20000);
	printf("\nEl equipo %d terminó de usar el salero\n", equipo);
	sem_post(&mydata->semaforos_param.sem_empanar);
	pthread_mutex_unlock(&m_salero);
	pthread_exit(NULL);
}

void *empanar(void *data)
{
	char *accion = "empanar";
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_empanar);
	imprimirAccion(mydata, accion);
	usleep(1000000);
	sem_post(&mydata->semaforos_param.sem_cocinar);

	pthread_exit(NULL);
}

void *cocinar(void *data)
{
	struct parametro *mydata = data;
	int equipo = *((int *)&mydata->equipo_param);

	sem_wait(&mydata->semaforos_param.sem_cocinar);
	pthread_mutex_lock(&m_sarten);
	printf("\nEl equipo %d está usando la sartén\n", equipo);
	usleep(5000000);
	printf("\nEl equipo %d terminó de usar la sartén\n", equipo);
	sem_post(&mydata->semaforos_param.sem_armar);
	sem_post(&mydata->semaforos_param.sem_hornear);
	pthread_mutex_unlock(&m_sarten);
	pthread_exit(NULL);
}

void *hornear(void *data)
{
	struct parametro *mydata = data;
	int equipo = *((int *)&mydata->equipo_param);

	sem_wait(&mydata->semaforos_param.sem_hornear);
	pthread_mutex_lock(&m_horno);
	printf("\nEl equipo %d está calentando el pan\n", equipo);
	usleep(10000000);
	printf("\nEquipo %d, el pan está listo\n", equipo);
	sem_post(&mydata->semaforos_param.sem_pan);
	pthread_mutex_unlock(&m_horno);
	pthread_exit(NULL);
}

void *armar(void *data)
{
	char *accion = "armar";
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armar);
	sem_wait(&mydata->semaforos_param.sem_pan);
	imprimirAccion(mydata, accion);
	usleep(1000000);
	sem_post(&mydata->semaforos_param.sem_entregar);

	pthread_exit(NULL);
}

void *entregar(void *data)
{
	struct parametro *mydata = data;
	int equipo = *((int *)&mydata->equipo_param);
	sem_wait(&mydata->semaforos_param.sem_entregar);
	printf("\n¡Equipo %d ganó!\n", equipo);
	exit(-1);
	pthread_exit(NULL);
}

void *ejecutarReceta(void *i)
{

	// variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_empanar;
	sem_t sem_cocinar;
	sem_t sem_hornear;
	sem_t sem_armar;
	sem_t sem_pan;
	sem_t sem_entregar;

	// variables hilos
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;

	// numero del equipo (casteo el puntero a un int)
	int p = *((int *)i);

	printf("Ejecutando equipo %d \n", p);

	// reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	// seteo los valores al struct

	// seteo numero de grupo
	pthread_data->equipo_param = p;

	// seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	// setear demas semaforos al struct aqui

	// seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?
	strcpy(pthread_data->pasos_param[0].accion, "cortar");
	strcpy(pthread_data->pasos_param[0].ingredientes[0], "ajo");
	strcpy(pthread_data->pasos_param[0].ingredientes[1], "perejil");

	strcpy(pthread_data->pasos_param[1].accion, "mezclar");
	strcpy(pthread_data->pasos_param[1].ingredientes[0], "ajo");
	strcpy(pthread_data->pasos_param[1].ingredientes[1], "perejil");
	strcpy(pthread_data->pasos_param[1].ingredientes[2], "huevo");
	strcpy(pthread_data->pasos_param[1].ingredientes[3], "carne");

	strcpy(pthread_data->pasos_param[2].accion, "empanar");
	strcpy(pthread_data->pasos_param[2].ingredientes[0], "pan rayado");
	strcpy(pthread_data->pasos_param[2].ingredientes[1], "carne");

	strcpy(pthread_data->pasos_param[3].accion, "armar");
	strcpy(pthread_data->pasos_param[3].ingredientes[0], "pan");
	strcpy(pthread_data->pasos_param[3].ingredientes[1], "lechuga");
	strcpy(pthread_data->pasos_param[3].ingredientes[2], "tomate");
	strcpy(pthread_data->pasos_param[3].ingredientes[3], "cebolla morada");
	strcpy(pthread_data->pasos_param[3].ingredientes[4], "pepino");
	strcpy(pthread_data->pasos_param[3].ingredientes[5], "milanesa");

	// inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_salar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_empanar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear), 0, 1);
	sem_init(&(pthread_data->semaforos_param.sem_armar), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_pan), 0, 0);
	sem_init(&(pthread_data->semaforos_param.sem_entregar), 0, 0);

	// creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos
	int rc;
	rc = pthread_create(&p1, NULL, cortar, pthread_data);
	rc = pthread_create(&p2, NULL, hornear, pthread_data);
	rc = pthread_create(&p3, NULL, mezclar, pthread_data);
	rc = pthread_create(&p4, NULL, salar, pthread_data);
	rc = pthread_create(&p5, NULL, empanar, pthread_data);
	rc = pthread_create(&p6, NULL, cocinar, pthread_data);
	rc = pthread_create(&p7, NULL, armar, pthread_data);
	rc = pthread_create(&p8, NULL, entregar, pthread_data);

	// join de todos los hilos
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(p4, NULL);
	pthread_join(p5, NULL);
	pthread_join(p6, NULL);
	pthread_join(p7, NULL);
	pthread_join(p8, NULL);

	// valido que el hilo se alla creado bien
	if (rc)
	{
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	// destruccion de los semaforos
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_empanar);
	sem_destroy(&sem_cocinar);
	sem_destroy(&sem_hornear);
	sem_destroy(&sem_armar);
	sem_destroy(&sem_entregar);

	// salida del hilo
	pthread_exit(NULL);
}

int main()
{
	// creo los nombres de los equipos
	int rc;
	int *equipoNombre1 = malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 = malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 = malloc(sizeof(*equipoNombre2));
	int *equipoNombre4 = malloc(sizeof(*equipoNombre2));
	// faltan equipos

	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;
	*equipoNombre4 = 4;

	// creo las variables los hilos de los equipos
	pthread_t equipo1;
	pthread_t equipo2;
	pthread_t equipo3;
	pthread_t equipo4;
	// faltan hilos

	// inicializo los hilos de los equipos
	rc = pthread_create(&equipo1, NULL, ejecutarReceta, equipoNombre1);
	rc = pthread_create(&equipo2, NULL, ejecutarReceta, equipoNombre2);
	rc = pthread_create(&equipo3, NULL, ejecutarReceta, equipoNombre3);
	rc = pthread_create(&equipo4, NULL, ejecutarReceta, equipoNombre4);

	if (rc)
	{
		printf("Error:unable to create thread, %d \n", rc);
		exit(-1);
	}

	// join de todos los hilos
	pthread_join(equipo1, NULL);
	pthread_join(equipo2, NULL);
	pthread_join(equipo3, NULL);
	pthread_join(equipo4, NULL);

	pthread_exit(NULL);
}
