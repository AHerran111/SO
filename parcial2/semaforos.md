Semaforos Posix:
    No Nombrados: Usan variables en memoria compartida (ej: variables globales usadas
    entre threads)

    Nombrados: Se usan cuando no existe memoria compartida, o sea, cuando se usa un semaforo entre procesos. Se asocian a un archivo temporal que tiene nombre.