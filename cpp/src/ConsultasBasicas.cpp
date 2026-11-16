// 4.1.6 Consultas básicas. Responsable: Samantha Rodriguez.
// Consulta la información turística registrada en los CSV.

#include "ConsultasBasicas.h"

#include "ArchivosCSV.h"
#include "Conexion.h"
#include "Punto.h"
#include "Solicitud.h"
#include "Unidad.h"

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {

class MenuConsultas {
private:
  const ArchivosCSV &archivosCSV;

  // pide texto y evita comas que rompan la lectura csv.
  string leerTexto(const string &mensaje) const {
    string respuesta;
    while (true) {
      cout << mensaje;
      if (!getline(cin, respuesta))
        return "";
      if (respuesta.find(',') == string::npos)
        return respuesta;
      cout << "No uses comas.\n";
    }
  }

  // convierte el criterio a mayusculas para comparar codigos.
  string mayusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  // muestra los datos principales de una conexion.
  void mostrarConexion(const Conexion &conexion) const {
    cout << " - " << conexion.getCodigo() << " | " << conexion.getOrigen()
         << " -> " << conexion.getDestino() << " | " << conexion.getDistancia()
         << " km | " << conexion.getTiempo() << " min | "
         << archivosCSV.textoEstadoConexion(conexion.getEstado()) << "\n";
  }

  // muestra los datos principales de una unidad.
  void mostrarUnidad(const Unidad &unidad) const {
    cout << " - " << unidad.getCodigo() << " | "
         << archivosCSV.textoTipoUnidad(unidad.getTipo()) << " | "
         << unidad.getCapacidad() << " turistas | costo/km: "
         << unidad.getCostoKm() << " | "
         << archivosCSV.textoEstadoUnidad(unidad.getEstado()) << "\n";
  }

  // muestra los datos principales de una solicitud.
  void mostrarSolicitud(const Solicitud &solicitud) const {
    cout << solicitud.getCodigo() << " | " << solicitud.getOrigen() << " -> "
         << solicitud.getDestino() << " | " << solicitud.getCantidadTuristas()
         << " turistas | prioridad "
         << archivosCSV.textoPrioridad(solicitud.getPrioridad()) << " | estado "
         << archivosCSV.textoEstadoSolicitud(solicitud.getEstado())
         << " | unidad "
         << (solicitud.getUnidadAsignada().empty()
                 ? "sin asignar"
                 : solicitud.getUnidadAsignada())
         << "\n";
  }

  // avisa cuando una busqueda no encuentra registros.
  void mostrarNoEncontrado(bool encontrado, const string &tipo,
                           const string &criterio) const {
    if (!encontrado)
      cout << "No se encontro " << tipo << " " << criterio << ".\n";
  }

  // muestra todos los puntos disponibles.
  void mostrarListaPuntos(const vector<Punto> &puntos) const {
    cout << "\nPuntos registrados (" << puntos.size() << "):\n";
    for (int i = 0; i < static_cast<int>(puntos.size()); ++i) {
      cout << " - " << puntos[i].getCodigo() << " | " << puntos[i].getNombre()
           << " | " << archivosCSV.textoTipoPunto(puntos[i].getTipo()) << " | "
           << puntos[i].getZona() << "\n";
    }
  }

  // lista puntos o busca uno por su codigo.
  void consultarPunto() const {
    vector<Punto> puntos = archivosCSV.getPuntos();
    string codigo = mayusculas(leerTexto(
        "Codigo de punto (Enter: listar todos; Ej: P001): "));
    if (codigo.empty()) {
      mostrarListaPuntos(puntos);
      return;
    }
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(puntos.size()); ++i) {
      if (puntos[i].getCodigo() == codigo) {
        cout << puntos[i].getCodigo() << " | " << puntos[i].getNombre() << " | "
             << archivosCSV.textoTipoPunto(puntos[i].getTipo()) << " | "
             << puntos[i].getZona() << "\n";
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "punto", codigo);
  }

  // lista conexiones o filtra por codigo de punto o conexion.
  void consultarConexion() const {
    vector<Conexion> conexiones = archivosCSV.getConexiones();
    string criterio = mayusculas(leerTexto(
        "Codigo de conexion o punto (Enter: listar todas): "));
    if (criterio.empty()) {
      cout << "\nConexiones registradas (" << conexiones.size() << "):\n";
      for (int i = 0; i < static_cast<int>(conexiones.size()); ++i)
        mostrarConexion(conexiones[i]);
      return;
    }
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
      if (conexiones[i].getCodigo() == criterio ||
          conexiones[i].getOrigen() == criterio ||
          conexiones[i].getDestino() == criterio) {
        mostrarConexion(conexiones[i]);
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "conexion o punto", criterio);
  }

  // muestra las solicitudes vinculadas a una unidad.
  void mostrarSolicitudesUnidad(const vector<Solicitud> &solicitudes,
                                const string &codigoUnidad) const {
    int cantidad = 0;
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i)
      if (solicitudes[i].getUnidadAsignada() == codigoUnidad) cantidad++;
    cout << "   Solicitudes asignadas (" << cantidad << "):";
    if (cantidad == 0) {
      cout << " ninguna\n";
      return;
    }
    cout << "\n";
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
      if (solicitudes[i].getUnidadAsignada() == codigoUnidad) {
        cout << "   ";
        mostrarSolicitud(solicitudes[i]);
      }
    }
  }

  // lista solicitudes o filtra por codigo o estado.
  void consultarSolicitud() const {
    vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
    string criterio = mayusculas(leerTexto(
        "Codigo o estado (pendiente/asignada/entregada; Enter: listar todas): "));
    if (criterio.empty()) {
      cout << "\nSolicitudes registradas (" << solicitudes.size() << "):\n";
      for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i)
        mostrarSolicitud(solicitudes[i]);
      return;
    }
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
      if (solicitudes[i].getCodigo() == criterio ||
          mayusculas(archivosCSV.textoEstadoSolicitud(
              solicitudes[i].getEstado())) == criterio) {
        mostrarSolicitud(solicitudes[i]);
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "solicitud o estado", criterio);
  }
  // lista unidades o muestra una unidad con sus solicitudes.
  void consultarUnidad() const {
    vector<Unidad> unidades = archivosCSV.getUnidades();
    vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
    string codigo = mayusculas(leerTexto(
        "Codigo de unidad (Enter: listar todas): "));
    if (codigo.empty()) {
      cout << "\nUnidades y solicitudes asignadas:\n";
      for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
        mostrarUnidad(unidades[i]);
        mostrarSolicitudesUnidad(solicitudes, unidades[i].getCodigo());
      }
      return;
    }
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
      if (unidades[i].getCodigo() == codigo) {
        mostrarUnidad(unidades[i]);
        mostrarSolicitudesUnidad(solicitudes, unidades[i].getCodigo());
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "unidad", codigo);
  }

public:
  // recibe una referencia de solo lectura a los datos csv.
  MenuConsultas(const ArchivosCSV &datos) : archivosCSV(datos) {}

  // muestra el menu y llama a la consulta elegida.
  void ejecutar() const {
    cout << "\n=== Consultas basicas ===\n"
         << "Escribe un codigo o criterio. Presiona Enter para listar todo.\n"
         << "1. Puntos\n2. Conexiones\n3. Solicitudes por estado\n"
         << "4. Unidades y solicitudes asignadas\n0. Volver\n";
    string opcion = leerTexto("Opcion: ");
    if (opcion == "1")
      consultarPunto();
    else if (opcion == "2")
      consultarConexion();
    else if (opcion == "3")
      consultarSolicitud();
    else if (opcion == "4")
      consultarUnidad();
    else if (opcion != "0")
      cout << "Opcion invalida.\n";
  }
};

} // namespace

// crea el menu y ejecuta una consulta.
void ejecutarConsultasBasicas(const ArchivosCSV &archivosCSV) {
  MenuConsultas menu(archivosCSV);
  menu.ejecutar();
}
