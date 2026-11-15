#include "ReporteGeneral.h"

#include "ArchivosCSV.h"
#include "Conexion.h"
#include "Punto.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"

#include <iostream>
#include <vector>

using namespace std;

void mostrarReporteGeneral(const ArchivosCSV &archivosCSV) {
  vector<Punto> puntos = archivosCSV.getPuntos();
  vector<Conexion> conexiones = archivosCSV.getConexiones();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  int habilitadas = 0;
  int cerradas = 0;
  int disponibles = 0;
  int ocupadas = 0;
  int pendientes = 0;
  int asignadas = 0;
  int entregadas = 0;

  for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
    if (conexiones[i].estaHabilitada()) habilitadas++;
    else cerradas++;
  }
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].estaDisponible()) disponibles++;
    else ocupadas++;
  }
  for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
    if (solicitudes[i].getEstado() == EstadoSolicitud::Pendiente) pendientes++;
    if (solicitudes[i].getEstado() == EstadoSolicitud::Asignada) asignadas++;
    if (solicitudes[i].getEstado() == EstadoSolicitud::Entregada) entregadas++;
  }

  cout << "\n=== Reporte general ===\n"
       << "Puntos: " << puntos.size() << "\n"
       << "Conexiones: " << conexiones.size() << " (habilitadas: "
       << habilitadas << ", cerradas: " << cerradas << ")\n"
       << "Unidades: " << unidades.size() << " (disponibles: "
       << disponibles << ", ocupadas: " << ocupadas << ")\n"
       << "Solicitudes: " << solicitudes.size() << " (pendientes: "
       << pendientes << ", asignadas: " << asignadas << ", entregadas: "
       << entregadas << ")\n";
}
