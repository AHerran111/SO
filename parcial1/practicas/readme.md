Compilacion:
    a.Compilar manualmente con:
    gcc shell.c -o a.out
    b.Compilar con makefile, corriendo comando:
    make
        i.Es necesario la libreria make para realizar este paso,
        instalar con:
        sudo apt install make

Ejecucion:
    ./a.out

Diseño:
    El shell funciona de manera modular, se fue construyendo de abajo hacia
    arriba, empezando primero con lo basico, la lectura y ejecucion de comandos
    basica con excevp(),siguiendole con diferentes retos y lineamientos

    Lectura/ejecucion:
    Empieza con un fgets el cual es ideal para leer cadenas de texto, utilizando
    strtok(), para obtener cada string del comando, pasandolo a un array.
    Una vez obtenido, el array es pasado a excevp con el formato:
    excevp(command[0],command);

    
    Errores/limites:
    Al caer en error excevp(), se imprime el error y sigue con la ejecucion,esto 
    cubre la mayoria de casos.
    NO utilizar comillas dobles o simples,solo funciona sin ellas.
    NO sobrepasar limite de input/command, no se implemento deteccion de overflow.
    NO combinar operadores |,<>,& funcionalidad combinada no fue testeada.
    

    Retos:
    Los retos se manejan de manera individual(por lo que no se pueden combinar) durante la lectura de comando si un operador |,<,>,& es detectado se alza activa una bandera.

    Pipes:
    Los pipes son basicamente dos comandos siendo ejecutados de manera consecuente,
    el stdout del primero es el stdin del segundo.Se crean con pipe() y se redirige 
    el stdin/out con dup2()

    Background:
    Se crea una nueva sesion con setsid(), y se cierran stdin/out/err, asi 
    creando un daemon que corre en el fondo, dejando que el shell siga

    Redirects:
    Utilizando una logica parecida a los pipes, se redirige el stdin/out al archivo
    que se indica, solo se implementan redirects simples, no dobles (>>,<<)

    Recoleccion de zombies:
    Utilizando signal(SIGCHLD, sigchild_handler) se escucha a la señal de terminacion
    del hijo, al "acacharla", el kernel hara la limpieza necesaria

    Ignorar SIGINT en shell:
    Se utilizo  signal(SIGINT, SIG_IGN), el cual hace que se ignore CTRL-C, y lo restauramos en los hijos signal(SIGINT, child_sigint_handler), la funcion mata al
    hijo con kill(), el funcionamiento de SIG_IGN se restaura al regresar al prompt



