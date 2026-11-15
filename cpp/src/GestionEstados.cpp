#include "GestionEstados.h"

#include "Conexion.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {

string mayusculas(string texto) {
  for (int i = 0; i < static_cast<int>(texto.size()); ++i) {
    texto[i] = static_cast<char>(
        toupper(static_cast<unsigned char>(texto[i])));
  }
  return texto;
}

bool soloEspacios(const string &texto) {
  for (int i = 0; i < static_cast<int>(texto.size()); ++i) {
    if (!isspace(static_cast<unsigned char>(texto[i]))) return false;
  }
  return true;
}

bool leerCodigo(const string &mensaje, string &codigo) {
  cout << mensaje;
  if (!getline(cin, codigo)) return false;
  codigo = mayusculas(codigo);
  if (codigo == "" || soloEspacios(codigo)
      || codigo.find(',') != string::npos) {
    cout << "[ERROR] Ingresa un codigo valido.\n";
    return false;
  }
  return true;
}

int buscarConexion(const vector<Conexion> &conexiones, const string &codigo) {
  for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
    if (conexiones[i].getCodigo() == codigo) return i;
  }
  return -1;
}

int buscarUnidad(const vector<Unidad> &unidades, const string &codigo) {
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].getCodigo() == codigo) return i;
  }
  return -1;
}

int buscarSolicitud(const vector<Solicitud> &solicitudes,
                    const string &codigo) {
  for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
    if (solicitudes[i].getCodigo() == codigo) return i;
  }
  return -1;
}

void cambiarEstadoConexion(ArchivosCSV &archivosCSV) {
  vector<Conexion> conexiones = archivosCSV.getConexiones();
  string codigo;
  if (!leerCodigo("Codigo de la conexion: ", codigo)) return;
  int posicion = buscarConexion(conexiones, codigo);
  if (posicion == -1) {
    cout << "[ERROR] No existe esa conexion.\n";
    return;
  }

  cout << "Nuevo estado:\n1. Habilitada\n2. Cerrada\nOpcion: ";
  string opcion;
  getline(cin, opcion);
  if (opcion == "1") conexiones[posicion].habilitar();
  else if (opcion == "2") conexiones[posicion].cerrar();
  else {
    cout << "[ERROR] Estado invalido.\n";
    return;
  }

  if (archivosCSV.guardarConexiones(conexiones)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Estado de la conexion actualizado.\n";
  } else {
    cout << "[ERROR] No se pudo actualizar la conexion.\n";
  }
}

vector<int> unidadesCompatibles(const vector<Unidad> &unidades,
                                int cantidadTuristas) {
  vector<int> posiciones;
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].estaDisponible()
        && unidades[i].tieneCapacidad(cantidadTuristas)) {
      posiciones.push_back(i);
    }
  }
  return posiciones;
}

void mostrarUnidadesCompatibles(ArchivosCSV &archivosCSV,
                                const vector<Unidad> &unidades,
                                const vector<int> &posiciones) {
  cout << "\nUnidades compatibles:\n" << fixed << setprecision(2);
  for (int i = 0; i < static_cast<int>(posiciones.size()); ++i) {
    const Unidad &unidad = unidades[posiciones[i]];
    cout << unidad.getCodigo() << " | "
         << archivosCSV.textoTipoUnidad(unidad.getTipo())
         << " | capacidad: " << unidad.getCapacidad()
         << " | costo/km: " << unidad.getCostoKm() << "\n";
  }
}

bool esCompatible(int posicion, const vector<int> &posiciones) {
  for (int i = 0; i < static_cast<int>(posiciones.size()); ++i) {
    if (posiciones[i] == posicion) return true;
  }
  return false;
}

void asignarSolicitud(ArchivosCSV &archivosCSV) {
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  string codigoSolicitud;
  if (!leerCodigo("Codigo de la solicitud: ", codigoSolicitud)) return;
  int posicionSolicitud = buscarSolicitud(solicitudes, codigoSolicitud);
  if (posicionSolicitud == -1) {
    cout << "[ERROR] No existe esa solicitud.\n";
    return;
  }
  if (!solicitudes[posicionSolicitud].estaPendiente()) {
    cout << "[ERROR] La solicitud no esta pendiente.\n";
    return;
  }

  vector<int> compatibles = unidadesCompatibles(
      unidades, solicitudes[posicionSolicitud].getCantidadTuristas());
  if (compatibles.empty()) {
    cout << "[ERROR] No hay unidades disponibles con capacidad suficiente.\n";
    return;
  }
  mostrarUnidadesCompatibles(archivosCSV, unidades, compatibles);

  string codigoUnidad;
  if (!leerCodigo("Codigo de la unidad: ", codigoUnidad)) return;
  int posicionUnidad = buscarUnidad(unidades, codigoUnidad);
  if (posicionUnidad == -1 || !esCompatible(posicionUnidad, compatibles)) {
    cout << "[ERROR] La unidad seleccionada no es compatible.\n";
    return;
  }

  solicitudes[posicionSolicitud].asignarUnidad(codigoUnidad);
  unidades[posicionUnidad].ocupar();
  if (archivosCSV.guardarUnidadesYSolicitudes(unidades, solicitudes)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Solicitud asignada a " << codigoUnidad << ".\n";
  } else {
    cout << "[ERROR] No se pudo guardar la asignacion.\n";
  }
}

void entregarSolicitud(ArchivosCSV &archivosCSV) {
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  string codigoSolicitud;
  if (!leerCodigo("Codigo de la solicitud: ", codigoSolicitud)) return;
  int posicionSolicitud = buscarSolicitud(solicitudes, codigoSolicitud);
  if (posicionSolicitud == -1) {
    cout << "[ERROR] No existe esa solicitud.\n";
    return;
  }
  if (solicitudes[posicionSolicitud].getEstado()
      != EstadoSolicitud::Asignada) {
    cout << "[ERROR] La solicitud no esta asignada.\n";
    return;
  }

  string codigoUnidad = solicitudes[posicionSolicitud].getUnidadAsignada();
  int posicionUnidad = buscarUnidad(unidades, codigoUnidad);
  if (posicionUnidad == -1 || unidades[posicionUnidad].estaDisponible()) {
    cout << "[ERROR] La unidad asignada no esta ocupada.\n";
    return;
  }

  solicitudes[posicionSolicitud].marcarEntregada();
  unidades[posicionUnidad].liberar();
  if (archivosCSV.guardarUnidadesYSolicitudes(unidades, solicitudes)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Solicitud entregada y unidad " << codigoUnidad
         << " liberada.\n";
  } else {
    cout << "[ERROR] No se pudo guardar la entrega.\n";
  }
}

} // namespace

void ejecutarGestionEstados(ArchivosCSV &archivosCSV) {
  cout << "\n=== Gestionar estados ===\n"
       << "1. Cerrar o habilitar conexion\n"
       << "2. Asignar solicitud pendiente\n"
       << "3. Marcar solicitud como entregada\n"
       << "0. Volver\n"
       << "Opcion: ";
  string opcion;
  getline(cin, opcion);
  if (opcion == "1") cambiarEstadoConexion(archivosCSV);
  else if (opcion == "2") asignarSolicitud(archivosCSV);
  else if (opcion == "3") entregarSolicitud(archivosCSV);
  else if (opcion != "0") cout << "[ERROR] Opcion invalida.\n";
}
