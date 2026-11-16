// 4.1.1 menu principal. responsable: samantha rodriguez.
// integra el menu y delega cada opcion a su modulo.

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
  // guarda la carpeta de datos y prepara el gestor de archivos csv.
  SistemaTurismo(const string &carpetaDatos)
      : carpetaDatos(carpetaDatos), archivosCSV(carpetaDatos) {}

  // muestra el menu principal y deriva cada opcion.
  void ejecutar() {
    // 4.1.2 carga inicial. responsable: jefferson sanchez.
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

      // convierte la opcion de un solo digito para usar el menu switch.
      int numeroOpcion = -1;
      if (opcion.size() == 1 && opcion[0] >= '0' && opcion[0] <= '9')
        numeroOpcion = opcion[0] - '0';

      // deriva cada opcion a la funcionalidad correspondiente.
      switch (numeroOpcion) {
      case 1:
        // 4.1.3 registro de informacion con validaciones. responsable: melissa ruiz.
        ejecutarRegistroInformacion(archivosCSV);
        break;
      case 2:
        // 4.1.4 gestion de estados. responsable: melissa ruiz.
        ejecutarGestionEstados(archivosCSV);
        break;
      case 3:
        // 4.1.6 consultas basicas. responsable: samantha rodriguez.
        ejecutarConsultasBasicas(archivosCSV);
        break;
      case 4:
        // 4.1.7 reporte general. responsable: samantha rodriguez.
        mostrarReporteGeneral(archivosCSV);
        break;
      case 5:
        abrirHaskell();
        break;
      case 6:
        abrirProlog();
        break;
      case 7:
        // fa01 registro de datos del recomendador. responsable: samantha rodriguez.
        ejecutarFA01(carpetaDatos);
        break;
      case 8:
        // fa02 seleccion de atractivos dentro de un presupuesto.
        ejecutarFA02(carpetaDatos);
        archivosCSV.actualizarDatos();
        break;
      case 9:
        // fa03 reservas de cupos por turno con lista de espera.
        ejecutarFA03(archivosCSV);
        break;
      case 0:
        cout << "Programa finalizado.\n";
        break;
      default:
        cout << "Opcion invalida.\n";
        break;
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
