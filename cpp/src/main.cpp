#include "ArchivosCSV.h"
#include "Conexion.h"
#include "FA01.h"
#include "FA02.h"
#include "Punto.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class SistemaTurismo {
public:
  explicit SistemaTurismo(const string &carpetaDatos)
      : carpetaDatos(carpetaDatos), archivosCSV(carpetaDatos) {}

  void ejecutar() {
    archivosCSV.cargarDatosIniciales();
    string opcion;
    do {
      cout << "\n=== Sistema de turismo ===\n"
           << "1. Registrar informacion base\n"
           << "2. Gestionar estados\n"
           << "3. Consultas basicas\n"
           << "4. Reporte general\n"
           << "5. Abrir modulo Haskell\n"
           << "6. Abrir modulo Prolog\n"
           << "7. Registrar datos para FA01\n"
           << "8. Cotizar plan turistico FA02\n"
           << "0. Salir\n"
           << "Opcion: ";
      getline(cin, opcion);

      if (opcion == "1") {
        cout << "La gestion de informacion base corresponde a su modulo.\n";
      } else if (opcion == "2") {
        cout << "La gestion de estados corresponde a su modulo.\n";
      } else if (opcion == "3") {
        menuConsultas();
      } else if (opcion == "4") {
        mostrarReporteGeneral();
      } else if (opcion == "5") {
        abrirHaskell();
      } else if (opcion == "6") {
        abrirProlog();
      } else if (opcion == "7") {
        ejecutarFA01(carpetaDatos);
      } else if (opcion == "8") {
        ejecutarFA02(carpetaDatos);
        archivosCSV.actualizarDatos();
      } else if (opcion == "0") {
        cout << "Programa finalizado.\n";
      } else {
        cout << "Opcion invalida.\n";
      }
    } while (opcion != "0");
  }

private:
  string carpetaDatos;
  ArchivosCSV archivosCSV;

  string rutaArchivo(const string &nombre) const {
    return carpetaDatos + "/" + nombre;
  }

  // Los CSV separan los campos con comas, asi que ningun dato puede llevarlas.
  string leerTexto(const string &mensaje) const {
    string respuesta;
    while (true) {
      cout << mensaje;
      if (!getline(cin, respuesta))
        return "";
      if (respuesta.find(',') == string::npos)
        return respuesta;
      cout << "No uses comas. Si son varios valores, separalos con punto y "
              "coma.\n";
    }
  }

  vector<string> separarCampos(const string &linea) const {
    vector<string> campos;
    string campo;
    for (int i = 0; i < static_cast<int>(linea.size()); ++i) {
      if (linea[i] == ',') {
        campos.push_back(campo);
        campo = "";
      } else if (linea[i] != '\r') {
        campo += linea[i];
      }
    }
    campos.push_back(campo);
    return campos;
  }

  bool leerFilas(const string &nombre, vector<string> &filas) const {
    ifstream archivo(rutaArchivo(nombre).c_str());
    if (!archivo)
      return false;

    string linea;
    getline(archivo, linea); // Saltar encabezado
    while (getline(archivo, linea)) {
      if (linea != "")
        filas.push_back(linea);
    }
    return true;
  }

  void menuConsultas() const {
    cout << "\n=== Consultas basicas ===\n"
         << "1. Punto\n2. Conexion\n3. Unidad\n4. Solicitud\n0. Volver\n";
    string opcion = leerTexto("Opcion: ");
    if (opcion == "1")
      consultarPunto();
    else if (opcion == "2")
      consultarConexion();
    else if (opcion == "3")
      consultarUnidad();
    else if (opcion == "4")
      consultarSolicitud();
    else if (opcion != "0")
      cout << "Opcion invalida.\n";
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

  void mostrarNoEncontrado(bool encontrado, const string &tipo,
                           const string &codigo) const {
    if (!encontrado)
      cout << "No se encontro " << tipo << " " << codigo << ".\n";
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

  void mostrarReporteGeneral() const {
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
      if (conexiones[i].estaHabilitada())
        habilitadas++;
      else
        cerradas++;
    }
    for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
      if (unidades[i].estaDisponible())
        disponibles++;
      else
        ocupadas++;
    }
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
      if (solicitudes[i].getEstado() == EstadoSolicitud::Pendiente)
        pendientes++;
      if (solicitudes[i].getEstado() == EstadoSolicitud::Asignada)
        asignadas++;
      if (solicitudes[i].getEstado() == EstadoSolicitud::Entregada)
        entregadas++;
    }

    cout << "\n=== Reporte general ===\n"
         << "Puntos: " << puntos.size() << "\n"
         << "Conexiones: " << conexiones.size()
         << " (habilitadas: " << habilitadas << ", cerradas: " << cerradas
         << ")\n"
         << "Unidades: " << unidades.size() << " (disponibles: " << disponibles
         << ", ocupadas: " << ocupadas << ")\n"
         << "Solicitudes: " << solicitudes.size()
         << " (pendientes: " << pendientes << ", asignadas: " << asignadas
         << ", entregadas: " << entregadas << ")\n";
  }

  string mayusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  void abrirHaskell() const {
    string comando;
    if (carpetaDatos == "_datos") {
      comando = "cabal run --project-dir=haskell turismo-haskell -- _datos";
    } else {
      comando =
          "cabal run --project-dir=../haskell turismo-haskell -- ../_datos";
    }
    int resultado = system(comando.c_str());
    if (resultado != 0)
      cout << "No se pudo iniciar Haskell. Revisa Cabal y la ruta.\n";
  }

  void abrirProlog() const {
    string comando;
    if (carpetaDatos == "_datos") {
      comando = "swipl prolog/app/main.pl -- _datos";
    } else {
      comando = "swipl ../prolog/app/main.pl -- ../_datos";
    }
    int resultado = system(comando.c_str());
    if (resultado != 0)
      cout << "No se pudo iniciar Prolog. Revisa SWI-Prolog y la ruta.\n";
  }
};

int main() {
  string carpetaDatos = "_datos";
  ifstream datos((carpetaDatos + "/puntos.csv").c_str());
  if (!datos)
    carpetaDatos = "../_datos";

  SistemaTurismo sistema(carpetaDatos);
  sistema.ejecutar();
  return 0;
}
