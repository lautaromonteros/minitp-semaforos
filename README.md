## MiniTP SOR 

#### Consgina
En la cocina de Subway Argento compiten cuatro equipos de cocineros para ganarse el puesto de trabajo en el local. ¿Quién será el primero en entregar 1 sándwich de milanesa terminado, de 15 cm, al dueño del local?

La dificultad de la competencia es que la cocina no cuenta con el equipamiento individual para cada equipo, sino que algunos de ellos son compartidos. Se tienen que ordenar para cocinar las milanesas ya que solo tienen un sartén para todos los equipos y este cocina una milanesa a la vez, existe solo un horno que cocina dos panes a la vez y un solo salero.

¿Cómo se arma un sándwich de milanesa?

Para armar un sándwich necesito cortar 2 dientes de ajo y un puñado de perejil, para luego mezclarlos con 1 huevo. Después utilizar sal a gusto, agregar la carne y luego empanar las milanesas. Una vez que tengo todo armado se cocinan por 5’
en el sartén bien caliente.

Como corresponde una buena milanesa tiene panes recién horneados (por 10’ aproximadamente) junto a lechuga fresca, tomate, cebolla morada y pepino recién cortados. Finalmente puedo armar el sándwich con todos los ingredientes. 

#### Explicación pseudocódigo y prueba de escritorio

Para el desarrollo de este trabajo se tubo en cuenta las siguientes funciones: 

|Nombre |Descripción|
|:-|:-|
|cortar|Corta los ingredientes principales|
|hornear|Hornea los panes|
|mezclar|Mezcla el ajo y perejil con los huevos|
|salar|Tira sal a la mezcla|
|empanar| Empana las milanesas|
|cocinar| Cocina las milanesas en una sartén|
|armar| Arma el sanguche con la milanesa cocinada y los ingredientes |
|entregar|El equipo entrega el sangunche|

Los semáforos:
|Nombre| Se llama para ... | Valor inicial |
|--|--| -- |
|sem_mezclar|Realizar la mezcla de ingredientes| 0 |
|sem_hornear|Simular el horneado del pan| 1 |
|sem_salar | Salar los ingredientes mezclados | 0 |
|sem_empanar| empanar las milanesas | 0 |
|sem_cocinar|Cocinar la milanesa en la sarten| 0 |
|sem_armar| Cuando termina de cocinarse la milanesa | 0 |
|sem_pan| Cuando termina de cocinarse el pan | 0 |
|sem_entregar| Entregar el pedido | 0 |

Y los mutex:
|Nombre| Se utiliza para |
|--|--|
|m_salero| Simular el uso del salero |
|m_horno| Simular el uso del horno |
|m_sarten| Simular el uso de la sartén|

Teniendo en cuenta que:
P() --> llama al semaforo
V() --> habilita el semaforo
LOCK() --> bloquea el mutex
UNLOCK() --> desbloquea el mutex

```
cortar(){
    imprimirAccion
    V(sem_mezclar)
}

hornear() {
    P(sem_hornear)
    LOCK(m_horno) 
    //simulo 10 segundos el uso del horno para la cocción del pan
    V(sem_pan)
    UNLOCK(m_horno)
}

mezclar(){
    P(sem_mezclar)
    imprimirAccion
    V(sem_salar)
}

salar(){
    P(sem_salar)
    LOCK(m_salero) 
    //simulo 1 segundo de usar el salero
    V(sem_empanar)
    UNLOCK(m_salero)
}

empanar() {
    P(sem_empanar)
    imprimirAccion
    V(sem_cocinar)
}

cocinar(){
    P(sem_cocinar)
    LOCK(m_sarten)
    //simulo 5 segundos el uso de la sarten
    V(sem_armar)
    UNLOCK(m_sarten)
}

armar(){
    P(sem_pan)
    P(sem_armar)
    imprimirAccion
    V(sem_entregar)
}

entregar(){
    P(sem_entregar)
    Fin de la tarea
}
```

#### Prueba de escritorio
Sea Pn, con n igual a los naturales para cada función de crear el sandwich:

|P1| P2 |P3| P4 | P5 | P6 | P7 | P8 |
|:-|:-|:-|:-|:-|:-|:-|:-|
|||||||P(sem_pan)|||
|| P(sem_hornear) |P(sem_mezclar)| P(sem_salar) | P(sem_empanar) | P(sem_cocinar) | P(sem_armar) | P(sem_entregar) |
|cortar| hornear |mezclar| salar | empanar | cocinar | armar | entregar |
|V(sem_mezclar)| V(sem_pan) | V(sem_salar) |V(sem_empanar)| V(sem_cocinar) | V(sem_armar) | V(sem_entregar) | V(sem_mezclar) |
