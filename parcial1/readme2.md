Compilacion:
    a.Compilar manualmente con:
    gcc primes.c -o a.out -lm
    b.Compilar con makefile, corriendo comando:
    make
        i.Es necesario la libreria make para realizar este paso,
        instalar con:
        sudo apt install make

Ejecucion:
    ./a.out -flags <inicio> <fin>

Uso detallado:
    Opciones:
    -N <num>       Define el número de hilos a usar (por defecto: número de CPUs).
    -s             Modo secuencial (desactiva el multithreading).
    -t A..B        Modo de prueba: ejecuta con cantidad de hilos desde A hasta B-1.
    -h             Muestra este mensaje de ayuda y termina.

    Argumentos:
    <inicio>       Inicio del rango de enteros.
    <fin>          Fin del rango de enteros.

    Ejemplos:
    programa -N 4 1 1000000    # Cuenta primos entre 1 y 1,000,000 usando 4 hilos
    programa -s 1 1000000      # Cuenta primos de manera secuencial
    programa -t 1..8 1 1000000 # Ejecuta pruebas con entre 1 y 7 hilos


    Limitaciones:
    0..<inicio>,<fin>..2,147,483,647
    MAX_THREADS:Definidas por CPU



Diseño

    El diseño del programa es modular, con una lógica simple.
    Se utilizan banderas para indiciar modo testing y paralaelo,
    asi como los threads, todas estas variables son globales.
    Se cuenta unicamente el tiempo de retorno de la cantidad de los primos. Para la lectura de los argumentos se utiliza la función
    getopt() que funciona para obtener las banderas y argumentos desde argv[], pero aun asi hay edge cases donde no funciona como esperado.
    Para la paralelización se utilizo pthread_create(), donde t_func distrbuye el rango de numeros que va a checar cada thread, tomando en cuenta el rango de min/max y la cantidad de threads.Cada Thread guarda el resultado en su bloque asignado en un array, para al final sumar el resultado de cada thread.

Performace:
1-1,000
1 THREAD: 0.000223 seconds
2 THREAD: 0.000267 seconds
4 THREAD: 0.000476 seconds
11 THREADS: 0.000705 seconds

1-10,000
1 THREAD: 0.000544 seconds
2 THREAD: 0.000542 seconds
4 THREAD: 0.000525 seconds
11 THREADS: 0.000949 second

1-100,000
1 THREAD: 0.004139 seconds
2 THREAD: 0.000542 seconds
4 THREAD: 0.001952 seconds
11 THREADS: 0.002189 seconds

1-1,000,000
1 THREAD: 0.090672 seconds
2 THREAD: 0.057050 seconds
4 THREAD: 0.033218 seconds
11 THREADS: 0.019658 seconds

1-10,000,000
1 THREAD: 1.958829 seconds
2 THREAD: 1.325638 seconds
4 THREAD: 0.720458 seconds
11 THREADS: 0.402036 seconds

1-100,000,000
1 THREAD: 50.871951 seconds
2 THREAD: 34.665152 seconds
4 THREAD: 19.791021 seconds
11 THREADS: 11.328129 seconds

1-1,000,000,000
1 THREAD: 1351.85 seconds
11 THREAD: 77.670233 seconds

Analisis:
Podemos ver que la ventaja de los threads se demuestra hasta los 100,000 nums, antes de esto la vetnaja es minima, o en mayor cantidad de threads, resulta aun mas tardado los calculos, pero al igual, podemos ver como en magnitudes mas grandes, las ventajas son claramente visibles, tal como se ve en el ultimo test, en el que se como se ejecuta el codigo mas de 10 veces mas rapido.


