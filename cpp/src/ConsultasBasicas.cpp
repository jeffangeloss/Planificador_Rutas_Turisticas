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

  string leerTexto(const string &mensaje) const {
    string respuesta;
    while (true) {
      cout << mensaje;
      if (!getline(cin, respuesta)) return "";
      if (respuesta.find(',') == string::npos) return respuesta;
      cout << "No uses comas.\n";
    }
  }

  string mayusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] = static_cast<char>(toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  void mostrarNoEncontrado(bool encontrado, const string &tipo,
                           const string &codigo) const {
    if (!encontrado) cout << "No se encontro " << tipo << " " << codigo << ".\n";
  }

  void mostrarSolicitud(const Solicitud &solicitud) const {
    cout << solicitud.getCodigo() << " | " << solicitud.getOrigen() << " -> "
         << solicitud.getDestino() << " | " << solicitud.getCantidadTuristas()
         << " turistas | prioridad "
         << archivosCSV.textoPrioridad(solicitud.getPrioridad()) << " | estado "
         << archivosCSV.textoEstadoSolicitud(solicitud.getEstado())
         << " | unidad "
         << (solicitud.getUnidadAsignada() == ""
                 ? "sin asignar"
                 : solicitud.getUnidadAsignada())
         << "\n";
  }

  void consultarPunto() const {
    vector<Punto> puntos = archivosCSV.getPuntos();
    string codigo = mayusculas(leerTexto("Codigo del punto: "));
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

  void consultarConexion() const {
    vector<Conexion> conexiones = archivosCSV.getConexiones();
    string codigo = mayusculas(leerTexto("Codigo de la conexion: "));
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
      if (conexiones[i].getCodigo() == codigo) {
        cout << conexiones[i].getCodigo() << " | " << conexiones[i].getOrigen()
             << " -> " << conexiones[i].getDestino() << " | "
             << conexiones[i].getDistancia() << " km | "
             << conexiones[i].getTiempo() << " min | "
             << archivosCSV.textoEstadoConexion(conexiones[i].getEstado())
             << "\n";
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "conexion", codigo);
  }

  void consultarUnidad() const {
    vector<Unidad> unidades = archivosCSV.getUnidades();
    string codigo = mayusculas(leerTexto("Codigo de la unidad: "));
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
      if (unidades[i].getCodigo() == codigo) {
        cout << unidades[i].getCodigo() << " | "
             << archivosCSV.textoTipoUnidad(unidades[i].getTipo()) << " | "
             << unidades[i].getCapacidad()
             << " turistas | costo/km: " << unidades[i].getCostoKm() << " | "
             << archivosCSV.textoEstadoUnidad(unidades[i].getEstado()) << "\n";
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "unidad", codigo);
  }

  void consultarSolicitud() const {
    vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
    string codigo = mayusculas(leerTexto("Codigo de la solicitud: "));
    bool encontrado = false;
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
      if (solicitudes[i].getCodigo() == codigo) {
        mostrarSolicitud(solicitudes[i]);
        encontrado = true;
      }
    }
    mostrarNoEncontrado(encontrado, "solicitud", codigo);
  }

public:
  explicit MenuConsultas(const ArchivosCSV &datos) : archivosCSV(datos) {}

  void ejecutar() const {
    cout << "\n=== Consultas basicas ===\n"
         << "1. Punto\n2. Conexion\n3. Unidad\n4. Solicitud\n0. Volver\n";
    string opcion = leerTexto("Opcion: ");
    if (opcion == "1") consultarPunto();
    else if (opcion == "2") consultarConexion();
    else if (opcion == "3") consultarUnidad();
    else if (opcion == "4") consultarSolicitud();
    else if (opcion != "0") cout << "Opcion invalida.\n";
  }
};

} // namespace

void ejecutarConsultasBasicas(const ArchivosCSV &archivosCSV) {
  MenuConsultas menu(archivosCSV);
  menu.ejecutar();
}
