#include "RegistroInformacion.h"

#include "Conexion.h"
#include "Punto.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
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

bool leerTexto(const string &mensaje, string &texto) {
  cout << mensaje;
  if (!getline(cin, texto)) return false;
  if (texto == "" || soloEspacios(texto)
      || texto.find(',') != string::npos) {
    cout << "[ERROR] El dato es obligatorio y no puede contener comas.\n";
    return false;
  }
  return true;
}

bool leerCodigo(const string &mensaje, char prefijo, string &codigo) {
  if (!leerTexto(mensaje, codigo)) return false;
  codigo = mayusculas(codigo);
  if (codigo.size() < 2 || codigo[0] != prefijo) {
    cout << "[ERROR] El codigo debe comenzar con " << prefijo << ".\n";
    return false;
  }
  return true;
}

void actualizarMayor(int &mayor, const string &codigo, char prefijo) {
  if (codigo.size() < 2 || codigo[0] != prefijo) return;
  stringstream lector(codigo.substr(1));
  int numero;
  char sobrante;
  if (lector >> numero && !(lector >> sobrante) && numero > mayor) {
    mayor = numero;
  }
}

string siguienteCodigo(ArchivosCSV &archivosCSV, char prefijo) {
  int mayor = -1;
  if (prefijo == 'P') {
    vector<Punto> registros = archivosCSV.getPuntos();
    for (int i = 0; i < static_cast<int>(registros.size()); ++i) {
      actualizarMayor(mayor, registros[i].getCodigo(), prefijo);
    }
  } else if (prefijo == 'C') {
    vector<Conexion> registros = archivosCSV.getConexiones();
    for (int i = 0; i < static_cast<int>(registros.size()); ++i) {
      actualizarMayor(mayor, registros[i].getCodigo(), prefijo);
    }
  } else if (prefijo == 'U') {
    vector<Unidad> registros = archivosCSV.getUnidades();
    for (int i = 0; i < static_cast<int>(registros.size()); ++i) {
      actualizarMayor(mayor, registros[i].getCodigo(), prefijo);
    }
  } else {
    vector<Solicitud> registros = archivosCSV.getSolicitudes();
    for (int i = 0; i < static_cast<int>(registros.size()); ++i) {
      actualizarMayor(mayor, registros[i].getCodigo(), prefijo);
    }
  }

  stringstream codigo;
  codigo << prefijo << setw(3) << setfill('0') << mayor + 1;
  return codigo.str();
}

bool leerEnteroPositivo(const string &mensaje, int &numero) {
  string texto;
  if (!leerTexto(mensaje, texto)) return false;
  stringstream lector(texto);
  char sobrante;
  if (!(lector >> numero) || numero <= 0 || lector >> sobrante) {
    cout << "[ERROR] Ingresa un numero entero mayor que cero.\n";
    return false;
  }
  return true;
}

bool leerDecimalPositivo(const string &mensaje, double &numero) {
  string texto;
  if (!leerTexto(mensaje, texto)) return false;
  stringstream lector(texto);
  char sobrante;
  if (!(lector >> numero) || numero <= 0 || lector >> sobrante) {
    cout << "[ERROR] Ingresa un numero mayor que cero.\n";
    return false;
  }
  return true;
}

bool leerTipoPunto(TipoPunto &tipo) {
  cout << "Tipo de punto:\n"
       << "1. Agencia\n2. Hotel\n3. Atractivo\n"
       << "4. Mirador\n5. Espacio publico\n";
  string opcion;
  if (!leerTexto("Opcion: ", opcion)) return false;
  if (opcion == "1") tipo = TipoPunto::Agencia;
  else if (opcion == "2") tipo = TipoPunto::Hotel;
  else if (opcion == "3") tipo = TipoPunto::Atractivo;
  else if (opcion == "4") tipo = TipoPunto::Mirador;
  else if (opcion == "5") tipo = TipoPunto::EspacioPublico;
  else {
    cout << "[ERROR] Tipo de punto invalido.\n";
    return false;
  }
  return true;
}

bool leerTipoUnidad(TipoUnidad &tipo) {
  cout << "Tipo de unidad:\n"
       << "1. Guia peatonal\n2. Taxi turistico\n3. Bus turistico\n";
  string opcion;
  if (!leerTexto("Opcion: ", opcion)) return false;
  if (opcion == "1") tipo = TipoUnidad::GuiaPeatonal;
  else if (opcion == "2") tipo = TipoUnidad::TaxiTuristico;
  else if (opcion == "3") tipo = TipoUnidad::BusTuristico;
  else {
    cout << "[ERROR] Tipo de unidad invalido.\n";
    return false;
  }
  return true;
}

bool leerEstadoConexion(EstadoConexion &estado) {
  cout << "Estado de la conexion:\n1. Habilitada\n2. Cerrada\n";
  string opcion;
  if (!leerTexto("Opcion: ", opcion)) return false;
  if (opcion == "1") estado = EstadoConexion::Habilitada;
  else if (opcion == "2") estado = EstadoConexion::Cerrada;
  else {
    cout << "[ERROR] Estado de conexion invalido.\n";
    return false;
  }
  return true;
}

bool leerPrioridad(Prioridad &prioridad) {
  cout << "Prioridad:\n1. Baja\n2. Media\n3. Alta\n";
  string opcion;
  if (!leerTexto("Opcion: ", opcion)) return false;
  if (opcion == "1") prioridad = Prioridad::Baja;
  else if (opcion == "2") prioridad = Prioridad::Media;
  else if (opcion == "3") prioridad = Prioridad::Alta;
  else {
    cout << "[ERROR] Prioridad invalida.\n";
    return false;
  }
  return true;
}

void registrarPunto(ArchivosCSV &archivosCSV) {
  string codigo;
  string nombre;
  string zona;
  TipoPunto tipo;
  string sugerido = siguienteCodigo(archivosCSV, 'P');
  if (!leerCodigo("Codigo del punto (sugerido: " + sugerido + "): ", 'P',
                  codigo)) return;
  if (archivosCSV.existePunto(codigo)) {
    cout << "[ERROR] Ya existe un punto con ese codigo.\n";
    return;
  }
  if (!leerTexto("Nombre: ", nombre) || !leerTipoPunto(tipo)
      || !leerTexto("Zona: ", zona)) return;

  if (archivosCSV.agregarPunto(Punto(codigo, nombre, tipo, zona))) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Punto registrado.\n";
  } else {
    cout << "[ERROR] No se pudo guardar el punto.\n";
  }
}

void registrarConexion(ArchivosCSV &archivosCSV) {
  string codigo;
  string origen;
  string destino;
  double distancia;
  int tiempo;
  EstadoConexion estado;
  string sugerido = siguienteCodigo(archivosCSV, 'C');
  if (!leerCodigo("Codigo de la conexion (sugerido: " + sugerido + "): ",
                  'C', codigo)) return;
  if (archivosCSV.existeConexion(codigo)) {
    cout << "[ERROR] Ya existe una conexion con ese codigo.\n";
    return;
  }
  if (!leerCodigo("Codigo de origen: ", 'P', origen)
      || !leerCodigo("Codigo de destino: ", 'P', destino)) return;
  if (!archivosCSV.existePunto(origen)
      || !archivosCSV.existePunto(destino)) {
    cout << "[ERROR] El origen y el destino deben ser puntos existentes.\n";
    return;
  }
  if (origen == destino) {
    cout << "[ERROR] El origen y el destino deben ser diferentes.\n";
    return;
  }
  if (!leerDecimalPositivo("Distancia en km: ", distancia)
      || !leerEnteroPositivo("Tiempo en minutos: ", tiempo)
      || !leerEstadoConexion(estado)) return;

  Conexion conexion(codigo, origen, destino, distancia, tiempo, estado);
  if (archivosCSV.agregarConexion(conexion)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Conexion registrada.\n";
  } else {
    cout << "[ERROR] No se pudo guardar la conexion.\n";
  }
}

void registrarUnidad(ArchivosCSV &archivosCSV) {
  string codigo;
  TipoUnidad tipo;
  int capacidad;
  double costoKm;
  string sugerido = siguienteCodigo(archivosCSV, 'U');
  if (!leerCodigo("Codigo de la unidad (sugerido: " + sugerido + "): ", 'U',
                  codigo)) return;
  if (archivosCSV.existeUnidad(codigo)) {
    cout << "[ERROR] Ya existe una unidad con ese codigo.\n";
    return;
  }
  if (!leerTipoUnidad(tipo)
      || !leerEnteroPositivo("Capacidad: ", capacidad)
      || !leerDecimalPositivo("Costo por km: ", costoKm)) return;

  Unidad unidad(codigo, tipo, capacidad, costoKm, EstadoUnidad::Disponible);
  if (archivosCSV.agregarUnidad(unidad)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Unidad registrada como disponible.\n";
  } else {
    cout << "[ERROR] No se pudo guardar la unidad.\n";
  }
}

void registrarSolicitud(ArchivosCSV &archivosCSV) {
  string codigo;
  string origen;
  string destino;
  int cantidadTuristas;
  Prioridad prioridad;
  string sugerido = siguienteCodigo(archivosCSV, 'S');
  if (!leerCodigo("Codigo de la solicitud (sugerido: " + sugerido + "): ",
                  'S', codigo)) return;
  if (archivosCSV.existeSolicitud(codigo)) {
    cout << "[ERROR] Ya existe una solicitud con ese codigo.\n";
    return;
  }
  if (!leerCodigo("Codigo de origen: ", 'P', origen)
      || !leerCodigo("Codigo de destino: ", 'P', destino)) return;
  if (!archivosCSV.existePunto(origen)
      || !archivosCSV.existePunto(destino)) {
    cout << "[ERROR] El origen y el destino deben ser puntos existentes.\n";
    return;
  }
  if (origen == destino) {
    cout << "[ERROR] El origen y el destino deben ser diferentes.\n";
    return;
  }
  if (!leerEnteroPositivo("Cantidad de turistas: ", cantidadTuristas)
      || !leerPrioridad(prioridad)) return;

  Solicitud solicitud(codigo, origen, destino, cantidadTuristas, prioridad,
                      EstadoSolicitud::Pendiente, "");
  if (archivosCSV.agregarSolicitud(solicitud)) {
    archivosCSV.actualizarDatos();
    cout << "[OK] Solicitud registrada como pendiente.\n";
  } else {
    cout << "[ERROR] No se pudo guardar la solicitud.\n";
  }
}

} // namespace

void ejecutarRegistroInformacion(ArchivosCSV &archivosCSV) {
  cout << "\n=== Registrar informacion base ===\n"
       << "1. Registrar punto\n"
       << "2. Registrar conexion\n"
       << "3. Registrar unidad\n"
       << "4. Registrar solicitud\n"
       << "0. Volver\n";
  string opcion;
  cout << "Opcion: ";
  getline(cin, opcion);
  if (opcion == "1") registrarPunto(archivosCSV);
  else if (opcion == "2") registrarConexion(archivosCSV);
  else if (opcion == "3") registrarUnidad(archivosCSV);
  else if (opcion == "4") registrarSolicitud(archivosCSV);
  else if (opcion != "0") cout << "[ERROR] Opcion invalida.\n";
}
