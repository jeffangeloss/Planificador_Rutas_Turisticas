#ifndef FA03_H
#define FA03_H

#include "AgendaCupos.h"
#include "ArchivosCSV.h"
#include "Punto.h"
#include "Solicitud.h"

#include <functional>
#include <string>
#include <vector>

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Ejecuta el menú de reservas, cupos y lista de espera.
void ejecutarFA03(const ArchivosCSV &archivosCSV);

// Ayudantes de FA03 que usan el submenu y la carga.
// Los indices devuelven la posicion en el vector, o -1 si el codigo no existe.
int indicePunto(const std::vector<Punto> &puntos, const std::string &codigo);
int indiceSolicitud(const std::vector<Solicitud> &solicitudes,
                    const std::string &codigo);
bool admiteCupo(TipoPunto tipo);  // solo atractivos y miradores tienen cupos

// Construye la agenda desde cupos.csv y reservas.csv.
// Si descarta datos inválidos, deja FA03 en modo de solo lectura.
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
