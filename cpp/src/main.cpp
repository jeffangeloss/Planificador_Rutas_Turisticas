// Grupo 4: ruta turística en Lima. Módulo imperativo (sección 4.1).
// 4.1.1 Menú principal. Responsable: Samantha Rodriguez.
// Este archivo integra el flujo general y delega cada opción a su módulo
// correspondiente.

#include "ArchivosCSV.h"
#include "ConsultasBasicas.h"
#include "FA01.h"
#include "FA02.h"
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
           << "9. Reservas de cupo por lista de espera FA03\n"
           << "0. Salir\n"
           << "Opcion: ";
      getline(cin, opcion);

      if (opcion == "1") {
        ejecutarRegistroInformacion(archivosCSV);
      } else if (opcion == "2") {
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
        ejecutarFA01(carpetaDatos);
      } else if (opcion == "8") {
        ejecutarFA02(carpetaDatos);
        archivosCSV.actualizarDatos();
      } else if (opcion == "9") {
        cout << "El modulo de reservas de cupo por lista de espera corresponde "
                "a su modulo.\n";
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
