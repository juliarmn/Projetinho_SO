cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -Wall -lncurses -g")

project (GatuOS C)

add_executable(GatuOS
    main.c
    processo/processo.h
    processo/process.c
    escalonador/rr.c
    escalonador/rr.h
    memoria/memoria.h
    memoria/memoria.c
    semaforo/semaforo.h
    semaforo/semaforo.c
)

target_link_libraries(GatuOS ${CURSES_LIBRARIES})
