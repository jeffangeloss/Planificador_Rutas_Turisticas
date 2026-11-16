#include "ArchivosCSV.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <system_error>

using namespace std;

ArchivosCSV::ArchivosCSV(string carpetaDatos) {
  this->carpetaDatos = carpetaDatos;
}

// 4.1.2 Carga inicial. Responsable: Jefferson Sanchez.
void ArchivosCSV::cargarDatosIniciales() {
  const vector<string> archivos = {"puntos.csv", "conexiones.csv",
                                   "unidades.csv", "solicitudes.csv"};
  // Avisa si falta algun CSV; la carga sigue con los archivos que si existen.
  for (const string &nombre : archivos) {
    const string ruta = rutaArchivo(nombre);
    ifstream archivo(ruta.c_str());
    if (!archivo) cerr << "No se pudo abrir " << ruta << "\n";
  }

  actualizarDatos(); // Lee los cuatro CSV y los guarda en memoria.

  cout << "Puntos cargados: " << puntos.size() << '\n'
       << "Conexiones cargadas: " << conexiones.size() << '\n'
       << "Unidades cargadas: " << unidades.size() << '\n'
       << "Solicitudes cargadas: " << solicitudes.size() << '\n';
}

void ArchivosCSV::actualizarDatos() {
  puntos = cargarPuntos();
  conexiones = cargarConexiones();
  unidades = cargarUnidades();
  solicitudes = cargarSolicitudes();
}

string ArchivosCSV::rutaArchivo(const string &nombre) const {
  return carpetaDatos + "/" + nombre;
}

vector<string> ArchivosCSV::separarCampos(const string &linea) const {
  // Corta en cada coma; los CSV del proyecto no usan comillas.
  vector<string> campos;
  string campo;
  for (char caracter : linea) {
    if (caracter == ',') {
      campos.push_back(campo);
      campo = "";
    } else if (caracter != '\r') {
      campo += caracter;
    }
  }
  campos.push_back(campo);
  return campos;
}

bool ArchivosCSV::leerFilas(const string &nombre, vector<string> &filas) const {
  ifstream archivo(rutaArchivo(nombre).c_str());
  if (!archivo) return false;

  string linea;
  getline(archivo, linea); // Saltar encabezado
  while (getline(archivo, linea)) {
    // Quita el '\r' final que dejan los CSV guardados en Windows.
    if (!linea.empty() && linea.back() == '\r') linea.pop_back();
    if (linea != "") filas.push_back(linea);
  }
  return true;
}

// Los cargadores pasan i + 2: la fila 1 del archivo es el encabezado e i
// empieza en 0 (las lineas vacias no se cuentan).
void ArchivosCSV::mostrarFilaInvalida(const string &archivo,
                                      int numeroFila) const {
  cout << "[ERROR] Fila " << numeroFila << " invalida en " << archivo
       << ".\n";
}

bool ArchivosCSV::convertirTipoPunto(const string &texto,
                                     TipoPunto &tipo) const {
  if (texto == "agencia") tipo = TipoPunto::Agencia;
  else if (texto == "hotel") tipo = TipoPunto::Hotel;
  else if (texto == "atractivo") tipo = TipoPunto::Atractivo;
  else if (texto == "mirador") tipo = TipoPunto::Mirador;
  else if (texto == "espacio_publico") tipo = TipoPunto::EspacioPublico;
  else return false;
  return true;
}

bool ArchivosCSV::convertirTipoUnidad(const string &texto,
                                      TipoUnidad &tipo) const {
  if (texto == "guia_peatonal") tipo = TipoUnidad::GuiaPeatonal;
  else if (texto == "taxi_turistico") tipo = TipoUnidad::TaxiTuristico;
  else if (texto == "bus_turistico") tipo = TipoUnidad::BusTuristico;
  else return false;
  return true;
}

bool ArchivosCSV::convertirTipoVisitante(const string &texto,
                                         TipoVisitante &tipo) const {
  if (texto == "general") tipo = TipoVisitante::General;
  else if (texto == "estudiante") tipo = TipoVisitante::Estudiante;
  else if (texto == "nino") tipo = TipoVisitante::Nino;
  else return false;
  return true;
}

bool ArchivosCSV::convertirEstadoConexion(const string &texto,
                                          EstadoConexion &estado) const {
  if (texto == "habilitada") estado = EstadoConexion::Habilitada;
  else if (texto == "cerrada") estado = EstadoConexion::Cerrada;
  else return false;
  return true;
}

bool ArchivosCSV::convertirEstadoUnidad(const string &texto,
                                        EstadoUnidad &estado) const {
  if (texto == "disponible") estado = EstadoUnidad::Disponible;
  else if (texto == "ocupada") estado = EstadoUnidad::Ocupada;
  else return false;
  return true;
}

bool ArchivosCSV::convertirPrioridad(const string &texto,
                                     Prioridad &prioridad) const {
  if (texto == "baja") prioridad = Prioridad::Baja;
  else if (texto == "media") prioridad = Prioridad::Media;
  else if (texto == "alta") prioridad = Prioridad::Alta;
  else return false;
  return true;
}

bool ArchivosCSV::convertirEstadoSolicitud(const string &texto,
                                           EstadoSolicitud &estado) const {
  if (texto == "pendiente") estado = EstadoSolicitud::Pendiente;
  else if (texto == "asignada") estado = EstadoSolicitud::Asignada;
  else if (texto == "entregada") estado = EstadoSolicitud::Entregada;
  else return false;
  return true;
}

// Cantidad de campos que debe tener cada fila de los CSV de la carga inicial.
const int CAMPOS_PUNTO = 4;
const int CAMPOS_CONEXION = 6;
const int CAMPOS_UNIDAD = 5;
const int CAMPOS_SOLICITUD = 7;

vector<Punto> ArchivosCSV::cargarPuntos() const {
  vector<Punto> puntosCargados;
  vector<string> filas;
  if (!leerFilas("puntos.csv", filas)) return puntosCargados;

  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    TipoPunto tipo;
    if (campos.size() == CAMPOS_PUNTO && convertirTipoPunto(campos[2], tipo)) {
      puntosCargados.push_back(Punto(campos[0], campos[1], tipo, campos[3]));
    } else {
      mostrarFilaInvalida("puntos.csv", i + 2);
    }
  }
  return puntosCargados;
}

namespace {

// Lee un numero decimal que ocupa todo el texto, sin caracteres sobrantes
// (por ejemplo "2.5" vale y "2.5km" no). Para enteros se usa leerEntero,
// de Reserva.h, que aplica la misma regla.
bool leerDecimal(const string &texto, double &numero) {
  stringstream lector(texto);
  char sobrante;
  return static_cast<bool>(lector >> numero) && !(lector >> sobrante);
}

// Dice si texto ya esta en la lista; los cargadores de FA03 lo usan para
// rechazar claves y codigos repetidos.
bool estaEnLista(const vector<string> &lista, const string &texto) {
  for (int i = 0; i < static_cast<int>(lista.size()); i++) {
    if (lista[i] == texto) return true;
  }
  return false;
}

} // namespace

vector<Conexion> ArchivosCSV::cargarConexiones() const {
  vector<Conexion> conexionesCargadas;
  vector<string> filas;
  if (!leerFilas("conexiones.csv", filas)) return conexionesCargadas;

  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    double distancia;
    int tiempo;
    EstadoConexion estado;
    // Primero se cuentan las columnas, asi nunca se lee un campo que no existe.
    bool datosValidos = campos.size() == CAMPOS_CONEXION
                        && leerDecimal(campos[3], distancia)
                        && leerEntero(campos[4], tiempo)
                        && distancia >= 0 && tiempo >= 0
                        && convertirEstadoConexion(campos[5], estado);
    if (datosValidos) {
      conexionesCargadas.push_back(Conexion(
          campos[0], campos[1], campos[2], distancia, tiempo, estado));
    } else {
      mostrarFilaInvalida("conexiones.csv", i + 2);
    }
  }
  return conexionesCargadas;
}

vector<Unidad> ArchivosCSV::cargarUnidades() const {
  vector<Unidad> unidadesCargadas;
  vector<string> filas;
  if (!leerFilas("unidades.csv", filas)) return unidadesCargadas;

  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    TipoUnidad tipo;
    int capacidad;
    double costoKm;
    EstadoUnidad estado;
    bool datosValidos = campos.size() == CAMPOS_UNIDAD
                        && convertirTipoUnidad(campos[1], tipo)
                        && leerEntero(campos[2], capacidad)
                        && leerDecimal(campos[3], costoKm)
                        && capacidad > 0 && costoKm >= 0
                        && convertirEstadoUnidad(campos[4], estado);
    if (datosValidos) {
      unidadesCargadas.push_back(
          Unidad(campos[0], tipo, capacidad, costoKm, estado));
    } else {
      mostrarFilaInvalida("unidades.csv", i + 2);
    }
  }
  return unidadesCargadas;
}

vector<Solicitud> ArchivosCSV::cargarSolicitudes() const {
  vector<Solicitud> solicitudesCargadas;
  vector<string> filas;
  if (!leerFilas("solicitudes.csv", filas)) return solicitudesCargadas;

  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    int cantidadTuristas;
    Prioridad prioridad;
    EstadoSolicitud estado;
    bool datosValidos = campos.size() == CAMPOS_SOLICITUD
                        && leerEntero(campos[3], cantidadTuristas)
                        && cantidadTuristas > 0
                        && convertirPrioridad(campos[4], prioridad)
                        && convertirEstadoSolicitud(campos[5], estado);
    if (datosValidos) {
      solicitudesCargadas.push_back(Solicitud(
          campos[0], campos[1], campos[2], cantidadTuristas, prioridad,
          estado, campos[6]));
    } else {
      mostrarFilaInvalida("solicitudes.csv", i + 2);
    }
  }
  return solicitudesCargadas;
}

vector<Visitante> ArchivosCSV::cargarVisitantes() const {
  vector<Visitante> visitantes;
  vector<string> filas;
  if (!leerFilas("visitantes.csv", filas)) return visitantes;

  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    TipoVisitante tipo;
    if (campos.size() == 2 && convertirTipoVisitante(campos[1], tipo)) {
      visitantes.push_back(Visitante(campos[0], tipo));
    } else {
      mostrarFilaInvalida("visitantes.csv", i + 2);
    }
  }
  return visitantes;
}

vector<Punto> ArchivosCSV::getPuntos() const { return puntos; }

vector<Conexion> ArchivosCSV::getConexiones() const { return conexiones; }

vector<Unidad> ArchivosCSV::getUnidades() const { return unidades; }

vector<Solicitud> ArchivosCSV::getSolicitudes() const { return solicitudes; }

string ArchivosCSV::textoTipoPunto(TipoPunto tipo) const {
  if (tipo == TipoPunto::Agencia) return "agencia";
  if (tipo == TipoPunto::Hotel) return "hotel";
  if (tipo == TipoPunto::Atractivo) return "atractivo";
  if (tipo == TipoPunto::Mirador) return "mirador";
  return "espacio_publico";
}

string ArchivosCSV::textoTipoUnidad(TipoUnidad tipo) const {
  if (tipo == TipoUnidad::GuiaPeatonal) return "guia_peatonal";
  if (tipo == TipoUnidad::TaxiTuristico) return "taxi_turistico";
  return "bus_turistico";
}

string ArchivosCSV::textoTipoVisitante(TipoVisitante tipo) const {
  if (tipo == TipoVisitante::General) return "general";
  if (tipo == TipoVisitante::Estudiante) return "estudiante";
  return "nino";
}

string ArchivosCSV::textoEstadoConexion(EstadoConexion estado) const {
  if (estado == EstadoConexion::Habilitada) return "habilitada";
  return "cerrada";
}

string ArchivosCSV::textoEstadoUnidad(EstadoUnidad estado) const {
  if (estado == EstadoUnidad::Disponible) return "disponible";
  return "ocupada";
}

string ArchivosCSV::textoPrioridad(Prioridad prioridad) const {
  if (prioridad == Prioridad::Baja) return "baja";
  if (prioridad == Prioridad::Media) return "media";
  return "alta";
}

string ArchivosCSV::textoEstadoSolicitud(EstadoSolicitud estado) const {
  if (estado == EstadoSolicitud::Pendiente) return "pendiente";
  if (estado == EstadoSolicitud::Asignada) return "asignada";
  return "entregada";
}

bool ArchivosCSV::archivoExiste(const string &nombre) const {
  ifstream archivo(rutaArchivo(nombre).c_str());
  return static_cast<bool>(archivo);
}

// 4.1.5 Generación de archivos CSV. Responsable: Jefferson Sanchez.
// Guarda puntos y conexiones usando archivos temporales.
namespace {

// Escritura segura: primero escribe el CSV completo en una carpeta temporal y
// solo si la escritura no fallo lo mueve sobre el original (rename). Si algo
// falla, el CSV original queda como estaba. Al final borra lo temporal.
// contenido es el texto completo del CSV (encabezado y filas) ya armado.
bool guardarCSV(const filesystem::path &destino, const string &contenido) {
  const long long marca =
      chrono::steady_clock::now().time_since_epoch().count();
  const filesystem::path carpetaTemporal = destino.parent_path() /
      (destino.stem().string() + ".tmp." + to_string(marca));
  error_code error;
  if (!filesystem::create_directory(carpetaTemporal, error)) return false;

  const filesystem::path temporal = carpetaTemporal / destino.filename();
  ofstream archivo(temporal, ios::binary);
  if (!archivo) {
    filesystem::remove(carpetaTemporal, error);
    return false;
  }

  archivo << contenido;
  archivo.close();

  if (archivo) { // Reemplaza el original solo si se escribio todo bien.
    filesystem::rename(temporal, destino, error);
  }
  const bool guardado = archivo && !error;
  filesystem::remove(temporal, error);
  filesystem::remove(carpetaTemporal, error);
  return guardado;
}

} // namespace

bool ArchivosCSV::guardarPuntos(
    const vector<Punto> &puntosActualizados) const {
  ostringstream contenido;
  contenido << "codigo,nombre,tipo,zona\n";
  for (const Punto &punto : puntosActualizados) {
    contenido << punto.getCodigo() << ',' << punto.getNombre() << ','
              << textoTipoPunto(punto.getTipo()) << ',' << punto.getZona()
              << '\n';
  }
  return guardarCSV(rutaArchivo("puntos.csv"), contenido.str());
}

bool ArchivosCSV::guardarConexiones(
    const vector<Conexion> &conexionesActualizadas) const {
  ostringstream contenido;
  contenido << "codigo,origen,destino,distancia_km,tiempo_min,estado\n";
  for (const Conexion &conexion : conexionesActualizadas) {
    contenido << conexion.getCodigo() << ',' << conexion.getOrigen() << ','
              << conexion.getDestino() << ',' << conexion.getDistancia() << ','
              << conexion.getTiempo() << ','
              << textoEstadoConexion(conexion.getEstado()) << '\n';
  }
  return guardarCSV(rutaArchivo("conexiones.csv"), contenido.str());
}

bool ArchivosCSV::existePunto(const string &codigo) const {
  for (int i = 0; i < static_cast<int>(puntos.size()); ++i) {
    if (puntos[i].getCodigo() == codigo) return true;
  }
  return false;
}

bool ArchivosCSV::existeConexion(const string &codigo) const {
  for (int i = 0; i < static_cast<int>(conexiones.size()); ++i) {
    if (conexiones[i].getCodigo() == codigo) return true;
  }
  return false;
}

bool ArchivosCSV::existeUnidad(const string &codigo) const {
  for (int i = 0; i < static_cast<int>(unidades.size()); ++i) {
    if (unidades[i].getCodigo() == codigo) return true;
  }
  return false;
}

bool ArchivosCSV::existeSolicitud(const string &codigo) const {
  for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
    if (solicitudes[i].getCodigo() == codigo) return true;
  }
  return false;
}

bool ArchivosCSV::agregarPunto(const Punto &punto) const {
  ofstream archivo(rutaArchivo("puntos.csv").c_str(), ios::app);
  if (!archivo) return false;
  archivo << punto.getCodigo() << "," << punto.getNombre() << ","
          << textoTipoPunto(punto.getTipo()) << "," << punto.getZona() << "\n";
  return archivo.good();
}

bool ArchivosCSV::agregarConexion(const Conexion &conexion) const {
  ofstream archivo(rutaArchivo("conexiones.csv").c_str(), ios::app);
  if (!archivo) return false;
  archivo << fixed << setprecision(2) << conexion.getCodigo() << ","
          << conexion.getOrigen() << "," << conexion.getDestino() << ","
          << conexion.getDistancia() << "," << conexion.getTiempo() << ","
          << textoEstadoConexion(conexion.getEstado()) << "\n";
  return archivo.good();
}

bool ArchivosCSV::agregarUnidad(const Unidad &unidad) const {
  ofstream archivo(rutaArchivo("unidades.csv").c_str(), ios::app);
  if (!archivo) return false;
  archivo << fixed << setprecision(2) << unidad.getCodigo() << ","
          << textoTipoUnidad(unidad.getTipo()) << "," << unidad.getCapacidad()
          << "," << unidad.getCostoKm() << ","
          << textoEstadoUnidad(unidad.getEstado()) << "\n";
  return archivo.good();
}

bool ArchivosCSV::agregarSolicitud(const Solicitud &solicitud) const {
  ofstream archivo(rutaArchivo("solicitudes.csv").c_str(), ios::app);
  if (!archivo) return false;
  archivo << solicitud.getCodigo() << "," << solicitud.getOrigen() << ","
          << solicitud.getDestino() << "," << solicitud.getCantidadTuristas()
          << "," << textoPrioridad(solicitud.getPrioridad()) << ","
          << textoEstadoSolicitud(solicitud.getEstado()) << ","
          << solicitud.getUnidadAsignada() << "\n";
  return archivo.good();
}

bool ArchivosCSV::guardarUnidadesYSolicitudes(
    const vector<Unidad> &nuevasUnidades,
    const vector<Solicitud> &nuevasSolicitudes) const {
  ofstream archivoUnidades(rutaArchivo("unidades.csv").c_str());
  ofstream archivoSolicitudes(rutaArchivo("solicitudes.csv").c_str());
  if (!archivoUnidades || !archivoSolicitudes) return false;

  archivoUnidades << "codigo,tipo,capacidad,costo_km,estado\n";
  archivoUnidades << fixed << setprecision(2);
  for (int i = 0; i < static_cast<int>(nuevasUnidades.size()); ++i) {
    archivoUnidades << nuevasUnidades[i].getCodigo() << ","
                    << textoTipoUnidad(nuevasUnidades[i].getTipo()) << ","
                    << nuevasUnidades[i].getCapacidad() << ","
                    << nuevasUnidades[i].getCostoKm() << ","
                    << textoEstadoUnidad(nuevasUnidades[i].getEstado()) << "\n";
  }

  archivoSolicitudes
      << "codigo,origen,destino,cantidad_turistas,prioridad,estado,unidad_asignada\n";
  for (int i = 0; i < static_cast<int>(nuevasSolicitudes.size()); ++i) {
    archivoSolicitudes << nuevasSolicitudes[i].getCodigo() << ","
                       << nuevasSolicitudes[i].getOrigen() << ","
                       << nuevasSolicitudes[i].getDestino() << ","
                       << nuevasSolicitudes[i].getCantidadTuristas() << ","
                       << textoPrioridad(nuevasSolicitudes[i].getPrioridad())
                       << ","
                       << textoEstadoSolicitud(nuevasSolicitudes[i].getEstado())
                       << "," << nuevasSolicitudes[i].getUnidadAsignada()
                       << "\n";
  }

  return archivoUnidades.good() && archivoSolicitudes.good();
}

bool ArchivosCSV::guardarPlanFA02(
    const vector<Unidad> &nuevasUnidades,
    const vector<Solicitud> &nuevasSolicitudes,
    const vector<Visitante> &visitantes) const {
  ofstream archivoUnidades(rutaArchivo("unidades.csv").c_str());
  ofstream archivoSolicitudes(rutaArchivo("solicitudes.csv").c_str());
  ofstream archivoVisitantes(rutaArchivo("visitantes.csv").c_str());
  if (!archivoUnidades || !archivoSolicitudes || !archivoVisitantes) {
    return false;
  }

  archivoUnidades << "codigo,tipo,capacidad,costo_km,estado\n";
  archivoUnidades << fixed << setprecision(2);
  for (int i = 0; i < static_cast<int>(nuevasUnidades.size()); ++i) {
    archivoUnidades << nuevasUnidades[i].getCodigo() << ","
                    << textoTipoUnidad(nuevasUnidades[i].getTipo()) << ","
                    << nuevasUnidades[i].getCapacidad() << ","
                    << nuevasUnidades[i].getCostoKm() << ","
                    << textoEstadoUnidad(nuevasUnidades[i].getEstado()) << "\n";
  }

  archivoSolicitudes
      << "codigo,origen,destino,cantidad_turistas,prioridad,estado,unidad_asignada\n";
  for (int i = 0; i < static_cast<int>(nuevasSolicitudes.size()); ++i) {
    archivoSolicitudes
        << nuevasSolicitudes[i].getCodigo() << ","
        << nuevasSolicitudes[i].getOrigen() << ","
        << nuevasSolicitudes[i].getDestino() << ","
        << nuevasSolicitudes[i].getCantidadTuristas() << ","
        << textoPrioridad(nuevasSolicitudes[i].getPrioridad()) << ","
        << textoEstadoSolicitud(nuevasSolicitudes[i].getEstado()) << ","
        << nuevasSolicitudes[i].getUnidadAsignada() << "\n";
  }

  archivoVisitantes << "solicitud,tipo_visitante\n";
  for (int i = 0; i < static_cast<int>(visitantes.size()); ++i) {
    archivoVisitantes << visitantes[i].getSolicitud() << ","
                      << textoTipoVisitante(visitantes[i].getTipo()) << "\n";
  }

  return archivoUnidades.good() && archivoSolicitudes.good()
         && archivoVisitantes.good();
}

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Lee y guarda cupos y reservas sin dejar archivos incompletos.

bool ArchivosCSV::convertirEstadoReserva(const string &texto,
                                         EstadoReserva &estado) const {
  if (texto == "confirmada") estado = EstadoReserva::Confirmada;
  else if (texto == "en_espera") estado = EstadoReserva::EnEspera;
  else if (texto == "cancelada") estado = EstadoReserva::Cancelada;
  else return false;
  return true;
}

string ArchivosCSV::textoEstadoReserva(EstadoReserva estado) const {
  if (estado == EstadoReserva::Confirmada) return "confirmada";
  if (estado == EstadoReserva::EnEspera) return "en_espera";
  return "cancelada";
}

// Cantidad de campos que debe tener cada fila de cupos.csv y reservas.csv.
const int CAMPOS_CUPO = 3;
const int CAMPOS_RESERVA = 8;

vector<Cupo> ArchivosCSV::cargarCupos() const {
  vector<Cupo> cupos;
  vector<string> filas;
  if (!leerFilas("cupos.csv", filas)) return cupos;

  vector<string> claves;
  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    int capacidad = 0;
    string clave =
        campos.size() == CAMPOS_CUPO ? campos[0] + " " + campos[1] : "";
    bool valida = campos.size() == CAMPOS_CUPO && campos[0] != ""
                  && turnoValido(campos[1])
                  && leerEntero(campos[2], capacidad) && capacidad > 0
                  && !estaEnLista(claves, clave);
    if (valida) {
      claves.push_back(clave);
      cupos.push_back(Cupo(campos[0], campos[1], capacidad));
    } else {
      mostrarFilaInvalida("cupos.csv", i + 2);
    }
  }
  return cupos;
}

vector<Reserva> ArchivosCSV::cargarReservas() const {
  vector<Reserva> reservas;
  vector<string> filas;
  if (!leerFilas("reservas.csv", filas)) return reservas;

  vector<string> codigos;
  for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
    vector<string> campos = separarCampos(filas[i]);
    int personas = 0;
    int orden = 0;
    EstadoReserva estado = EstadoReserva::Cancelada;
    // Ocho campos con formato valido, personas mayor que 0, orden_espera mayor
    // que 0 solo si esta en_espera (0 en otro caso) y codigo no repetido.
    bool valida = campos.size() == CAMPOS_RESERVA
                  && codigoReservaValido(campos[0]) && campos[1] != ""
                  && campos[2] != "" && fechaValida(campos[3])
                  && turnoValido(campos[4]) && leerEntero(campos[5], personas)
                  && personas > 0 && convertirEstadoReserva(campos[6], estado)
                  && leerEntero(campos[7], orden)
                  && (estado == EstadoReserva::EnEspera ? orden > 0 : orden == 0)
                  && !estaEnLista(codigos, campos[0]);
    if (valida) {
      codigos.push_back(campos[0]);
      reservas.push_back(Reserva(campos[0], campos[1], campos[2], campos[3],
                                 campos[4], personas, estado, orden));
    } else {
      mostrarFilaInvalida("reservas.csv", i + 2);
    }
  }
  return reservas;
}

bool ArchivosCSV::guardarCupos(const vector<Cupo> &cupos) const {
  ostringstream contenido;
  contenido << "punto,turno,capacidad_maxima\n";
  for (const Cupo &cupo : cupos) {
    contenido << cupo.getPunto() << ',' << cupo.getTurno() << ','
              << cupo.getCapacidadMaxima() << '\n';
  }
  return guardarCSV(rutaArchivo("cupos.csv"), contenido.str());
}

bool ArchivosCSV::guardarReservas(const vector<Reserva> &reservas) const {
  ostringstream contenido;
  contenido << "codigo,solicitud,punto,fecha,turno,personas,estado,"
               "orden_espera\n";
  for (const Reserva &reserva : reservas) {
    contenido << reserva.getCodigo() << ',' << reserva.getSolicitud() << ','
              << reserva.getPunto() << ',' << reserva.getFecha() << ','
              << reserva.getTurno() << ',' << reserva.getPersonas() << ','
              << textoEstadoReserva(reserva.getEstado()) << ','
              << reserva.getOrdenEspera() << '\n';
  }
  return guardarCSV(rutaArchivo("reservas.csv"), contenido.str());
}

int ArchivosCSV::contarFilas(const string &nombre) const {
  vector<string> filas;
  leerFilas(nombre, filas);
  return static_cast<int>(filas.size());
}
