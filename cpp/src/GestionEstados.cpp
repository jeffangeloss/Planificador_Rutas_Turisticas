// 4.1.4 Gestión de estados. Responsable: Melissa Ruíz.
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

// Convierte los códigos ingresados a mayúsculas.
string mayusculas(string texto) {
  for (int i = 0; i < static_cast<int>(texto.size()); ++i) {
    texto[i] = static_cast<char>(toupper(static_cast<unsigned char>(texto[i])));
  }
  return texto;
}

// Comprueba si una entrada contiene únicamente espacios.
bool soloEspacios(const string &texto) {
  for (int i = 0; i < static_cast<int>(texto.size()); ++i) {
    if (!isspace(static_cast<unsigned char>(texto[i]))) return false;
  }
  return true;
}

// Lee un código obligatorio que no contenga comas.
bool leerCodigo(const string &mensaje, string &codigo) {
  cout << mensaje;
  if (!getline(cin, codigo)) return false;
  codigo = mayusculas(codigo);
  if (codigo == "" || soloEspacios(codigo) || codigo.find(',') != string::npos) {
    cout << "[ERROR] Ingresa un codigo valido." << endl;
    return false;
  }
  return true;
}

// Busca una conexión por su código.
int buscarConexion(const vector<Conexion> &conexiones, const string &codigo) {
  for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
    if (conexiones[i].getCodigo() == codigo) return i;
  }
  return -1;
}

// Busca una unidad por su código.
int buscarUnidad(const vector<Unidad> &unidades, const string &codigo) {
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].getCodigo() == codigo) return i;
  }
  return -1;
}

// Busca una solicitud por su código.
int buscarSolicitud(const vector<Solicitud> &solicitudes, const string &codigo) {
  for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
    if (solicitudes[i].getCodigo() == codigo) return i;
  }
  return -1;
}

// Cambia una conexión entre habilitada y cerrada.
void cambiarEstadoConexion(ArchivosCSV &archivosCSV) {
  vector<Conexion> conexiones = archivosCSV.getConexiones();
  string codigo;
  if (!leerCodigo("Codigo de la conexion: ", codigo)) return;
  int posicion = buscarConexion(conexiones, codigo);
  if (posicion == -1) {
    cout << "[ERROR] No existe esa conexion." << endl;
    return;
  }

  cout << "Nuevo estado:" << endl;
  cout << "1. Habilitada" << endl;
  cout << "2. Cerrada" << endl;
  cout << "Opcion: ";
  string opcion;
  getline(cin, opcion);
  if (opcion == "1") conexiones[posicion].habilitar();
  else if (opcion == "2") conexiones[posicion].cerrar();
  else {
    cout << "[ERROR] Estado invalido." << endl;
    return;
  }

  if (archivosCSV.guardarConexiones(conexiones)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Estado de la conexion actualizado." << endl;
  } else {
    cout << "[ERROR] No se pudo actualizar la conexion." << endl;
  }
}

// Reúne las unidades disponibles que tienen capacidad suficiente.
vector<int> unidadesCompatibles(const vector<Unidad> &unidades, int cantidadTuristas) {
  vector<int> posiciones;
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].estaDisponible() && unidades[i].tieneCapacidad(cantidadTuristas)) {
      posiciones.push_back(i);
    }
  }
  return posiciones;
}

// Muestra las unidades que pueden atender la solicitud.
void mostrarUnidadesCompatibles(const ArchivosCSV &archivosCSV,
                                const vector<Unidad> &unidades, const vector<int> &posiciones) {
  cout << endl << "Unidades compatibles:" << endl;
  cout << fixed << setprecision(2);
  for (int i = 0; i < static_cast<int>(posiciones.size()); ++i) {
    const Unidad &unidad = unidades[posiciones[i]];
    cout << unidad.getCodigo() << " | " << archivosCSV.textoTipoUnidad(unidad.getTipo())
         << " | capacidad: " << unidad.getCapacidad() << " | costo/km: " << unidad.getCostoKm() << endl;
  }
}

// Comprueba que la unidad elegida pertenezca a la lista compatible.
bool esCompatible(int posicion, const vector<int> &posiciones) {
  for (int i = 0; i < static_cast<int>(posiciones.size()); ++i) {
    if (posiciones[i] == posicion) return true;
  }
  return false;
}

// Asigna una solicitud pendiente a una unidad compatible.
void asignarSolicitud(ArchivosCSV &archivosCSV) {
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  string codigoSolicitud;
  if (!leerCodigo("Codigo de la solicitud: ", codigoSolicitud)) return;
  int posicionSolicitud = buscarSolicitud(solicitudes, codigoSolicitud);
  if (posicionSolicitud == -1) {
    cout << "[ERROR] No existe esa solicitud." << endl;
    return;
  }
  if (!solicitudes[posicionSolicitud].estaPendiente()) {
    cout << "[ERROR] La solicitud no esta pendiente." << endl;
    return;
  }

  vector<int> compatibles = unidadesCompatibles(unidades, solicitudes[posicionSolicitud].getCantidadTuristas());
  if (compatibles.empty()) {
    cout << "[ERROR] No hay unidades disponibles con capacidad suficiente." << endl;
    return;
  }
  mostrarUnidadesCompatibles(archivosCSV, unidades, compatibles);

  string codigoUnidad;
  if (!leerCodigo("Codigo de la unidad: ", codigoUnidad)) return;
  int posicionUnidad = buscarUnidad(unidades, codigoUnidad);
  if (posicionUnidad == -1 || !esCompatible(posicionUnidad, compatibles)) {
    cout << "[ERROR] La unidad seleccionada no es compatible." << endl;
    return;
  }

  solicitudes[posicionSolicitud].asignarUnidad(codigoUnidad);
  unidades[posicionUnidad].ocupar();
  if (archivosCSV.guardarUnidadesYSolicitudes(unidades, solicitudes)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Solicitud asignada a " << codigoUnidad << "." << endl;
  } else {
    cout << "[ERROR] No se pudo guardar la asignacion." << endl;
  }
}

// Marca una solicitud como entregada y libera su unidad.
void entregarSolicitud(ArchivosCSV &archivosCSV) {
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  string codigoSolicitud;
  if (!leerCodigo("Codigo de la solicitud: ", codigoSolicitud)) return;
  int posicionSolicitud = buscarSolicitud(solicitudes, codigoSolicitud);
  if (posicionSolicitud == -1) {
    cout << "[ERROR] No existe esa solicitud." << endl;
    return;
  }
  if (solicitudes[posicionSolicitud].getEstado() != EstadoSolicitud::Asignada) {
    cout << "[ERROR] La solicitud no esta asignada." << endl;
    return;
  }

  string codigoUnidad = solicitudes[posicionSolicitud].getUnidadAsignada();
  int posicionUnidad = buscarUnidad(unidades, codigoUnidad);
  if (posicionUnidad == -1 || unidades[posicionUnidad].estaDisponible()) {
    cout << "[ERROR] La unidad asignada no esta ocupada." << endl;
    return;
  }

  solicitudes[posicionSolicitud].marcarEntregada();
  unidades[posicionUnidad].liberar();
  if (archivosCSV.guardarUnidadesYSolicitudes(unidades, solicitudes)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Solicitud entregada y unidad " << codigoUnidad << " liberada." << endl;
  } else {
    cout << "[ERROR] No se pudo guardar la entrega." << endl;
  }
}

} // namespace

// Muestra las operaciones disponibles para gestionar estados.
void ejecutarGestionEstados(ArchivosCSV &archivosCSV) {
  cout << endl << "=== Gestionar estados ===" << endl;
  cout << "1. Cerrar o habilitar conexion" << endl;
  cout << "2. Asignar solicitud pendiente" << endl;
  cout << "3. Marcar solicitud como entregada" << endl;
  cout << "0. Volver" << endl;
  cout << "Opcion: ";
  string opcion;
  getline(cin, opcion);
  if (opcion == "1") cambiarEstadoConexion(archivosCSV);
  else if (opcion == "2") asignarSolicitud(archivosCSV);
  else if (opcion == "3") entregarSolicitud(archivosCSV);
  else if (opcion != "0") cout << "[ERROR] Opcion invalida." << endl;
}
