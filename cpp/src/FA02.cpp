// FA02 Selección de atractivos dentro de un presupuesto. Responsable: Melissa Ruíz.
#include "FA02.h"

#include "ArchivosCSV.h"
#include "Solicitud.h"
#include "Tipos.h"
#include "Unidad.h"
#include "Visitante.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct AlternativaFA02 {
  vector<string> ruta;
  TipoUnidad tipoTransporte;
  int cantidadVisitas;
  double costoTotal;
  int tiempoTotal;
};

class CotizadorFA02 {
private:
  string carpetaDatos;
  ArchivosCSV archivosCSV;

  string leerTexto(const string &mensaje);
  bool leerDoublePositivo(const string &mensaje, double &numero);
  bool leerEnteroPositivo(const string &mensaje, int &numero);
  bool leerEnteroNoNegativo(const string &mensaje, int &numero);
  vector<string> separarPor(const string &texto, char separador);
  bool parsearAlternativa(const string &linea, AlternativaFA02 &alternativa);
  string comandoHaskell(int generales, int estudiantes, int ninos, double presupuesto, int tiempoMaximo);
  bool generarAlternativas(int generales, int estudiantes, int ninos, double presupuesto, int tiempoMaximo,
                           vector<AlternativaFA02> &alternativas);
  string siguienteCodigoSolicitud(const vector<Solicitud> &solicitudes);
  int buscarUnidadDisponible(const vector<Unidad> &unidades, TipoUnidad tipoTransporte, int cantidadPersonas);
  void agregarVisitantes(vector<Visitante> &visitantes, const string &codigoSolicitud,
                         TipoVisitante tipoVisitante, int cantidad);
  void registrarPlan(int generales, int estudiantes, int ninos, const AlternativaFA02 &alternativa);

public:
  CotizadorFA02(const string &rutaDatos);
  void ejecutar();
};

CotizadorFA02::CotizadorFA02(const string &rutaDatos) : carpetaDatos(rutaDatos), archivosCSV(rutaDatos) {}

// Lee una entrada que no contenga comas para no alterar los CSV.
string CotizadorFA02::leerTexto(const string &mensaje) {
  string respuesta;
  while (true) {
    cout << mensaje;
    if (!getline(cin, respuesta)) return "";
    if (respuesta.find(',') == string::npos) return respuesta;
    cout << "No uses comas." << endl;
  }
}

bool CotizadorFA02::leerDoublePositivo(const string &mensaje, double &numero) {
  stringstream lector(leerTexto(mensaje));
  char sobrante;
  if (!(lector >> numero) || numero <= 0 || lector >> sobrante) {
    cout << "Ingresa un numero mayor que cero." << endl;
    return false;
  }
  return true;
}

bool CotizadorFA02::leerEnteroPositivo(const string &mensaje, int &numero) {
  stringstream lector(leerTexto(mensaje));
  char sobrante;
  if (!(lector >> numero) || numero <= 0 || lector >> sobrante) {
    cout << "Ingresa un numero entero mayor que cero." << endl;
    return false;
  }
  return true;
}

bool CotizadorFA02::leerEnteroNoNegativo(const string &mensaje, int &numero) {
  stringstream lector(leerTexto(mensaje));
  char sobrante;
  if (!(lector >> numero) || numero < 0 || lector >> sobrante) {
    cout << "Ingresa un numero entero mayor o igual que cero." << endl;
    return false;
  }
  return true;
}

// Separa los campos de la respuesta interna de Haskell.
vector<string> CotizadorFA02::separarPor(const string &texto, char separador) {
  vector<string> partes;
  string parte;
  for (int i = 0; i <= static_cast<int>(texto.size()); i++) {
    if (i == static_cast<int>(texto.size()) || texto[i] == separador) {
      partes.push_back(parte);
      parte = "";
    } else {
      parte += texto[i];
    }
  }
  return partes;
}

// Convierte una línea FA02_DATA en una alternativa temporal.
bool CotizadorFA02::parsearAlternativa(const string &linea, AlternativaFA02 &alternativa) {
  vector<string> campos = separarPor(linea, '|');
  if (campos.size() != 7 || campos[0] != "FA02_DATA") return false;

  stringstream lectorCantidad(campos[4]);
  stringstream lectorCosto(campos[5]);
  stringstream lectorTiempo(campos[6]);
  alternativa.ruta = separarPor(campos[2], '>');
  if (!archivosCSV.convertirTipoUnidad(campos[3], alternativa.tipoTransporte)) {
    return false;
  }
  lectorCantidad >> alternativa.cantidadVisitas;
  lectorCosto >> alternativa.costoTotal;
  lectorTiempo >> alternativa.tiempoTotal;

  return !alternativa.ruta.empty() && !lectorCantidad.fail()
         && !lectorCosto.fail() && !lectorTiempo.fail();
}

// Construye el comando que ejecuta FA02 en Haskell.
string CotizadorFA02::comandoHaskell(int generales, int estudiantes, int ninos,
                                     double presupuesto, int tiempoMaximo) {
  stringstream comando;
  comando << fixed << setprecision(2);
  if (carpetaDatos == "_datos") {
    comando << "cabal run --project-dir=haskell turismo-haskell -- _datos";
  } else {
    comando << "cabal run --project-dir=../haskell turismo-haskell -- ../_datos";
  }
  comando << " --fa02 " << generales << " " << estudiantes << " " << ninos
          << " " << presupuesto << " " << tiempoMaximo << " 2>&1";
  return comando.str();
}

// Ejecuta Haskell y recoge las alternativas devueltas por el protocolo FA02.
bool CotizadorFA02::generarAlternativas(int generales, int estudiantes, int ninos, double presupuesto,
                                        int tiempoMaximo, vector<AlternativaFA02> &alternativas) {
  string comando = comandoHaskell(generales, estudiantes, ninos, presupuesto, tiempoMaximo);
#ifdef _WIN32
  FILE *proceso = _popen(comando.c_str(), "r");
#else
  FILE *proceso = popen(comando.c_str(), "r");
#endif
  if (proceso == NULL) {
    cout << "[ERROR] No se pudo iniciar Haskell." << endl;
    return false;
  }

  bool hayError = false;
  char buffer[512];
  while (fgets(buffer, sizeof(buffer), proceso) != NULL) {
    string linea(buffer);
    while (!linea.empty() && (linea.back() == '\n' || linea.back() == '\r')) {
      linea.pop_back();
    }

    if (linea.find("FA02_DATA|") == 0) {
      AlternativaFA02 alternativa;
      if (parsearAlternativa(linea, alternativa)) {
        alternativas.push_back(alternativa);
      } else {
        hayError = true;
        cout << "[ERROR] Haskell devolvio una alternativa invalida." << endl;
      }
    } else if (linea.find("FA02_ERROR|") == 0) {
      hayError = true;
      cout << "[ERROR] " << linea.substr(12) << endl;
    } else {
      cout << linea << endl;
    }
  }

#ifdef _WIN32
  int resultado = _pclose(proceso);
#else
  int resultado = pclose(proceso);
#endif
  if (resultado != 0) {
    cout << "[ERROR] Haskell no pudo completar FA02." << endl;
    return false;
  }
  return !hayError;
}

string CotizadorFA02::siguienteCodigoSolicitud(const vector<Solicitud> &solicitudes) {
  int mayor = 0;
  for (int i = 0; i < static_cast<int>(solicitudes.size()); i++) {
    string codigoActual = solicitudes[i].getCodigo();
    if (codigoActual.size() > 1 && codigoActual[0] == 'S') {
      stringstream lector(codigoActual.substr(1));
      int numero;
      char sobrante;
      if (lector >> numero && !(lector >> sobrante) && numero > mayor) {
        mayor = numero;
      }
    }
  }

  stringstream codigo;
  codigo << "S" << setw(3) << setfill('0') << mayor + 1;
  return codigo.str();
}

// Busca la unidad disponible más económica del tipo recomendado.
int CotizadorFA02::buscarUnidadDisponible(const vector<Unidad> &unidades, TipoUnidad tipoTransporte,
                                          int cantidadPersonas) {
  int posicion = -1;
  for (int i = 0; i < static_cast<int>(unidades.size()); i++) {
    bool compatible = unidades[i].estaDisponible() && unidades[i].getTipo() == tipoTransporte
                      && unidades[i].tieneCapacidad(cantidadPersonas);
    bool menorCosto = posicion == -1 || unidades[i].getCostoKm() < unidades[posicion].getCostoKm();
    bool desempate = posicion != -1 && unidades[i].getCostoKm() == unidades[posicion].getCostoKm()
                     && unidades[i].getCodigo() < unidades[posicion].getCodigo();
    if (compatible && (menorCosto || desempate)) posicion = i;
  }
  return posicion;
}

// Agrega una fila por visitante asociado a la solicitud creada.
void CotizadorFA02::agregarVisitantes(vector<Visitante> &visitantes,
                                      const string &codigoSolicitud, TipoVisitante tipoVisitante, int cantidad) {
  for (int i = 0; i < cantidad; i++) {
    visitantes.push_back(Visitante{codigoSolicitud, tipoVisitante});
  }
}

// Registra la solicitud aceptada, sus visitantes y la unidad ocupada.
void CotizadorFA02::registrarPlan(int generales, int estudiantes, int ninos,
                                  const AlternativaFA02 &alternativa) {
  if (alternativa.ruta.size() < 2 || alternativa.ruta[0] != "P000") {
    cout << "[ERROR] Haskell devolvio una ruta invalida." << endl;
    return;
  }

  int cantidadPersonas = generales + estudiantes + ninos;
  archivosCSV.actualizarDatos();
  vector<Unidad> unidades = archivosCSV.getUnidades();
  vector<Solicitud> solicitudes = archivosCSV.getSolicitudes();
  vector<Visitante> visitantes = archivosCSV.cargarVisitantes();
  if (!archivosCSV.archivoExiste("visitantes.csv")) {
    cout << "[ERROR] No se pudo leer visitantes.csv." << endl;
    return;
  }

  int posicionUnidad = buscarUnidadDisponible(unidades, alternativa.tipoTransporte, cantidadPersonas);
  if (posicionUnidad == -1) {
    cout << "[ERROR] La unidad recomendada ya no esta disponible." << endl;
    cout << "Vuelve a generar las alternativas." << endl;
    return;
  }

  string codigoSolicitud = siguienteCodigoSolicitud(solicitudes);
  Solicitud solicitud{codigoSolicitud, "P000", alternativa.ruta.back(), cantidadPersonas,
                      Prioridad::Media, EstadoSolicitud::Pendiente, ""};
  solicitud.asignarUnidad(unidades[posicionUnidad].getCodigo());

  unidades[posicionUnidad].ocupar();
  solicitudes.push_back(solicitud);
  agregarVisitantes(visitantes, codigoSolicitud, TipoVisitante::General, generales);
  agregarVisitantes(visitantes, codigoSolicitud, TipoVisitante::Estudiante, estudiantes);
  agregarVisitantes(visitantes, codigoSolicitud, TipoVisitante::Nino, ninos);

  if (!archivosCSV.guardarPlanFA02(unidades, solicitudes, visitantes)) {
    cout << "[ERROR] No se pudieron actualizar los archivos CSV." << endl;
    return;
  }

  cout << "[OK] Alternativa seleccionada." << endl;
  cout << "[OK] Unidad " << unidades[posicionUnidad].getCodigo() << " asignada." << endl;
  cout << "[OK] Solicitud " << codigoSolicitud << " creada." << endl;
}

void CotizadorFA02::ejecutar() {
  int generales;
  int estudiantes;
  int ninos;
  if (!leerEnteroNoNegativo("Visitantes generales: ", generales)) return;
  if (!leerEnteroNoNegativo("Visitantes estudiantes: ", estudiantes)) return;
  if (!leerEnteroNoNegativo("Visitantes ninos: ", ninos)) return;
  if (generales + estudiantes + ninos == 0) {
    cout << "El grupo debe tener al menos una persona." << endl;
    return;
  }

  double presupuesto;
  int tiempoMaximo;
  if (!leerDoublePositivo("Presupuesto maximo: ", presupuesto)) return;
  if (!leerEnteroPositivo("Tiempo disponible en minutos: ", tiempoMaximo)) return;

  vector<AlternativaFA02> alternativas;
  if (!generarAlternativas(generales, estudiantes, ninos, presupuesto, tiempoMaximo, alternativas)) return;
  if (alternativas.empty()) return;

  stringstream lectorOpcion(leerTexto("Seleccione una alternativa (1-3) o 0 para cancelar: "));
  int opcion;
  char sobrante;
  if (!(lectorOpcion >> opcion) || lectorOpcion >> sobrante) {
    cout << "Opcion invalida." << endl;
    return;
  }
  if (opcion == 0) {
    cout << "Asignacion cancelada." << endl;
    return;
  }
  if (opcion < 1 || opcion > static_cast<int>(alternativas.size())) {
    cout << "Opcion invalida." << endl;
    return;
  }

  registrarPlan(generales, estudiantes, ninos, alternativas[opcion - 1]);
}

void ejecutarFA02(const string &carpetaDatos) {
  CotizadorFA02 cotizador(carpetaDatos);
  cotizador.ejecutar();
}
