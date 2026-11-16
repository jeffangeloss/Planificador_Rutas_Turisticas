// 4.1.1 Menú principal. Responsable: Samantha Rodriguez.
// Integra el menú y delega cada opción a su módulo.

#include "ArchivosCSV.h"
#include "ConsultasBasicas.h"
#include "FA01.h"
#include "FA02.h"
#include "FA03.h"
#include "GestionEstados.h"
#include "RegistroInformacion.h"
#include "ReporteGeneral.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class SistemaTurismo {
public:
  explicit SistemaTurismo(const string &carpetaDatos)
      : carpetaDatos(carpetaDatos), archivosCSV(carpetaDatos) {}

  // muestra el menu principal y deriva cada opcion.
  void ejecutar() {
    // 4.1.2 Carga inicial. Responsable: Jefferson Sanchez.
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
           << "9. Reservas de cupos por turno FA03\n"
           << "0. Salir\n"
           << "Opcion: ";
      if (!getline(cin, opcion)) {
        cout << "\nPrograma finalizado.\n";
        break;
      }

      if (opcion == "1") {
        // 4.1.3 Registro de información con validaciones. Responsable: Melissa Ruíz.
        ejecutarRegistroInformacion(archivosCSV);
      } else if (opcion == "2") {
        // 4.1.4 Gestión de estados. Responsable: Melissa Ruíz.
        ejecutarGestionEstados(archivosCSV);
      } else if (opcion == "3") {
        // 4.1.6 Consultas básicas. Responsable: Samantha Rodriguez.
        ejecutarConsultasBasicas(archivosCSV);
      } else if (opcion == "4") {
        // 4.1.7 Reporte general. Responsable: Samantha Rodriguez.
        mostrarReporteGeneral(archivosCSV);
      } else if (opcion == "5") {
        abrirHaskell();
      } else if (opcion == "6") {
        abrirProlog();
      } else if (opcion == "7") {
        // FA01 Recomendador explicable de atractivos por perfil. Responsable: Samantha Rodriguez.
        ejecutarFA01(carpetaDatos);
      } else if (opcion == "8") {
        // FA02 Selección de atractivos dentro de un presupuesto. Responsable: Melissa Ruíz.
        ejecutarFA02(carpetaDatos);
        archivosCSV.actualizarDatos();
      } else if (opcion == "9") {
        // FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
        ejecutarFA03(archivosCSV);
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

  // inicia el modulo haskell con la carpeta de datos.
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

  // inicia el modulo prolog con la carpeta de datos.
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

// crea el sistema turistico y pone en marcha el menu.
int main() {
  string carpetaDatos = "_datos";
  ifstream datos((carpetaDatos + "/puntos.csv").c_str());
  if (!datos)
    carpetaDatos = "../_datos";

  SistemaTurismo sistema(carpetaDatos);
  sistema.ejecutar();
  return 0;
}
