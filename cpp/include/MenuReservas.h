#ifndef MENU_RESERVAS_H
#define MENU_RESERVAS_H

#include "AgendaCupos.h"
#include "ArchivosCSV.h"
#include "Punto.h"
#include "Solicitud.h"

#include <string>
#include <vector>

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Submenu de FA03 (opcion 9): capacidades, reservas, cancelaciones y reporte.
class MenuReservas {
private:
  const ArchivosCSV &archivosCSV;
  std::vector<Punto> puntos;
  std::vector<Solicitud> solicitudes;
  bool soloLectura;   // true: FA03 solo muestra el reporte y no guarda cambios
  AgendaCupos agenda; // va al final: se carga con los atributos de arriba

  // Entrada por consola.
  bool pedir(const std::string &mensaje, std::string &respuesta) const;
  bool pedirCodigo(const std::string &mensaje, std::string &codigo) const;

  // Busquedas por codigo. buscarPunto y buscarSolicitud devuelven la posicion
  // en el vector, o -1 si no existe.
  int buscarPunto(const std::string &codigo) const;
  int buscarSolicitud(const std::string &codigo) const;
  std::string nombrePunto(const std::string &codigo) const;
  int personasDe(const std::string &codigoReserva) const;

  // Carga y guardado de la agenda.
  AgendaCupos cargarAgenda();
  bool puedeModificar() const;
  bool guardar(const AgendaCupos &anterior, bool cupos, bool reservas);

  // Apoyo para mostrar datos en las opciones.
  std::string turnosConCapacidad(const std::string &punto) const;
  void mostrarPromovidas(const std::vector<std::string> &promovidas) const;
  std::vector<std::string> fechasConReservas(const std::string &punto,
                                             const std::string &turno) const;

  // Las cuatro opciones del submenu.
  void registrarCupo();           // opcion 1
  void reservarTurno();           // opcion 2
  void cancelarReserva();         // opcion 3
  void reporteOcupacion() const;  // opcion 4

public:
  // Lee los datos y carga la agenda de cupos.csv y reservas.csv.
  explicit MenuReservas(const ArchivosCSV &datos);

  // Muestra el submenu hasta que se elige 0.
  void ejecutar();
};

#endif
