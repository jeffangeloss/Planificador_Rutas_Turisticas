#ifndef FA03_H
#define FA03_H

#include "AgendaCupos.h"
#include "ArchivosCSV.h"

#include <functional>
#include <string>
#include <vector>

// FA03 - Autor: Jefferson Sanchez.
// Reservas de cupos por turno con lista de espera (opcion 9 del menu).
void ejecutarFA03(const ArchivosCSV &archivosCSV);

// Agenda que FA03 arma al abrirse, a partir de las filas que ya leyeron
// cargarCupos y cargarReservas. Descarta lo que no cumple las reglas de
// _datos/README.md y deja FA03 en solo lectura si descarta algo o si un turno
// tiene mas personas confirmadas que su capacidad.
struct CargaFA03 {
  std::vector<Cupo> cupos;              // capacidades que pasan la validacion
  std::vector<Reserva> reservas;        // reservas que pasan, sin normalizar
  AgendaCupos agenda;                   // normalizada fuera de solo lectura
  std::vector<std::string> errores;     // un motivo por cada problema
  std::vector<std::string> promovidas;  // reservas que la carga confirma
  bool soloLectura;
};

CargaFA03 armarCargaFA03(const std::vector<Cupo> &cuposLeidos,
                         const std::vector<Reserva> &reservasLeidas,
                         bool hayFilasMalFormadas,
                         const std::vector<Punto> &puntos,
                         const std::vector<Solicitud> &solicitudes);

// Resultado de guardar un cambio del submenu.
enum class GuardadoFA03 { Guardado, Revertido, SinRestaurar };

// Escribe cupos.csv y despues reservas.csv, segun lo que toque el cambio. Si
// reservas.csv falla despues de guardar cupos.csv, restaura cupos.csv.
GuardadoFA03 guardarCambioFA03(bool cupos, bool reservas,
                               const std::function<bool()> &escribirCupos,
                               const std::function<bool()> &escribirReservas,
                               const std::function<bool()> &restaurarCupos);

// Agenda que queda en memoria despues de guardar un cambio: la del cambio si
// se guardo, la anterior si se revirtio, y la de los archivos (capacidades
// nuevas y reservas de antes) si no se pudo restaurar cupos.csv.
AgendaCupos agendaSegunGuardadoFA03(GuardadoFA03 resultado,
                                    const AgendaCupos &nueva,
                                    const AgendaCupos &anterior);

#endif
