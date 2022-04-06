# MiniTP SOR Semáforos

El presente proyecto simula la competencia de cuatro equipos de Subway para la realización de un sanguche cada uno.
Para el desarrollo del mismo, no se cuentan con los materiales necesarios para que todos los equipos puedan cocinar a la par, por lo tanto con un programa en C, se hace una simulación de la realización de los mismos usando semáforos e hilos.

## Desarrollo

### Pseudocódigo

Para el desarrollo de este decidí crear los siguientes semáforos:

|Nombre Semáforo|Estado inicial|
| :------------ | :------------
| sem_mezclar | 0 |
| sem_salar | 0 |
| sem_empanar | 0 |
| sem_cocinar | 0 |
| sem_hornear | 1 |
| sem_armar | 0 |
| sem_entregar | 0 |

Asimismo se toman los siguientes procesos para la simulación de las actividades y/o acciones a realizar:

|Nombre| Característica |
| :------------ | :------------
| P1 | Cortar ingredientes|
| P2 | Hornear pan |
| P3 | Mezclar ingredientes cortados |
| P4 | Salar los ingredientes |
| P5 | Empanar las milanesas |
| P6 | Cocinar en la sarten |
| P7 | Armar el sanguche |
| P8 | Entregar |

### Prueba de escritorio

Teniendo en cuenta que: 
P() --> wait
V() --> signal

| P1 | P2 | P3 | P4 | P5 | P6 | P7 | P8 |
|  | P(sem_hornear)  | P(sem_mezclar) | P(sem_salar) | P(sem_empanar) | P(sem_cocinar) | P(sem_armar) | P(sem_entregar) |
| cortar  | hornear | mezclar | salar | empanar | cocinar | armar | entregar |
| V(sem_mezclar) | V(sem_armar) | V(sem_salar) | V(sem_empanar) | V(sem_cocinar) | V(sem_armar) | |

