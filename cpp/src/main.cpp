#include "ArchivosCSV.h"
#include "ConsultasBasicas.h"
#include "FA01.h"
#include "FA02.h"
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
        ejecutarConsultasBasicas(archivosCSV);
      } else if (opcion == "4") {
        mostrarReporteGeneral(archivosCSV);
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
