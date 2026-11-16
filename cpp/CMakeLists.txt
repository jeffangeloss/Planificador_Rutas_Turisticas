cmake_minimum_required(VERSION 3.16)

project(turismo_cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(turismo_lib
    src/ArchivosCSV.cpp
    src/Punto.cpp
    src/Conexion.cpp
    src/Unidad.cpp
    src/Solicitud.cpp
    src/Visitante.cpp
    src/RegistroInformacion.cpp
    src/GestionEstados.cpp
    src/ConsultasBasicas.cpp
    src/ReporteGeneral.cpp
    src/FA01.cpp
    src/FA02.cpp
    src/Cupo.cpp
    src/Reserva.cpp
    src/AgendaCupos.cpp
    src/FA03.cpp
)

target_include_directories(turismo_lib PUBLIC include)

add_executable(turismo_cpp src/main.cpp)
target_link_libraries(turismo_cpp PRIVATE turismo_lib)

# FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
enable_testing()
add_executable(prueba_fa03 tests/prueba_fa03.cpp)
target_link_libraries(prueba_fa03 PRIVATE turismo_lib)
add_test(NAME prueba_fa03 COMMAND prueba_fa03
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
