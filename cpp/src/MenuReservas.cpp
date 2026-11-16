// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Submenu de FA03: define los metodos de MenuReservas.

#include "MenuReservas.h"

#include "FA03.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Ayudantes que solo usa este archivo. Van en un namespace anonimo para que
// mayusculas no choque con la de GestionEstados.cpp al enlazar.
namespace {

string mayusculas(string texto) {
  for (char &letra : texto) {
    letra = static_cast<char>(toupper(static_cast<unsigned char>(letra)));
  }
  return texto;
}

string cuposLibres(int libres) {
  return libres == 1 ? "1 cupo libre" : to_string(libres) + " cupos libres";
}

}  // namespace

// Lee una linea sin espacios en los extremos. Rechaza las comas, porque el
// formato CSV no las admite dentro de un campo.
bool MenuReservas::pedir(const string &mensaje, string &respuesta) const {
  cout << mensaje;
  if (!getline(cin, respuesta)) return false;
  size_t inicio = respuesta.find_first_not_of(" \t\r");
  size_t fin = respuesta.find_last_not_of(" \t\r");
  respuesta = inicio == string::npos ? "" : respuesta.substr(inicio, fin - inicio + 1);
  if (respuesta.find(',') == string::npos) return true;
  cout << "[ERROR] No uses comas: el formato CSV no las admite.\n";
  return false;
}

bool MenuReservas::pedirCodigo(const string &mensaje, string &codigo) const {
  if (!pedir(mensaje, codigo)) return false;
  codigo = mayusculas(codigo);
  return !codigo.empty();
}

int MenuReservas::buscarPunto(const string &codigo) const {
  return indicePunto(puntos, codigo);
}

int MenuReservas::buscarSolicitud(const string &codigo) const {
  return indiceSolicitud(solicitudes, codigo);
}

string MenuReservas::nombrePunto(const string &codigo) const {
  int i = buscarPunto(codigo);
  return i == -1 ? codigo : codigo + " " + puntos[i].getNombre();
}

int MenuReservas::personasDe(const string &codigoReserva) const {
  for (const Reserva &reserva : agenda.getReservas()) {
    if (reserva.getCodigo() == codigoReserva) return reserva.getPersonas();
  }
  return 0;
}

// Carga los CSV de FA03 con armarCargaFA03. Si algo se descarta, FA03 queda
// en solo lectura para no borrar esas filas al guardar.
AgendaCupos MenuReservas::cargarAgenda() {
  vector<Cupo> cuposLeidos = archivosCSV.cargarCupos();
  vector<Reserva> reservasLeidas = archivosCSV.cargarReservas();
  bool malFormadas =
      static_cast<int>(cuposLeidos.size()) != archivosCSV.contarFilas("cupos.csv")
      || static_cast<int>(reservasLeidas.size())
             != archivosCSV.contarFilas("reservas.csv");
  CargaFA03 carga = armarCargaFA03(cuposLeidos, reservasLeidas, malFormadas,
                                   puntos, solicitudes);
  for (const string &error : carga.errores) cout << "[ERROR] " << error << "\n";
  soloLectura = carga.soloLectura;
  if (soloLectura) {
    cout << "[ERROR] Corrige cupos.csv o reservas.csv. Mientras tanto FA03 solo "
            "muestra el reporte, para no perder filas al guardar.\n";
    return carga.agenda;
  }
  if (carga.promovidas.empty()) return carga.agenda;
  if (!archivosCSV.guardarReservas(carga.agenda.getReservas())) {
    soloLectura = true;
    cout << "[ERROR] No se pudo guardar en reservas.csv las reservas que ya "
            "caben. FA03 queda en solo lectura y muestra los datos del archivo.\n";
    return AgendaCupos(carga.cupos, carga.reservas);
  }
  for (const string &codigo : carga.promovidas) {
    cout << "[OK] Al cargar, la reserva " << codigo
         << " pasa a confirmada porque ya cabe en su turno.\n";
  }
  return carga.agenda;
}

bool MenuReservas::puedeModificar() const {
  if (!soloLectura) return true;
  cout << "[ERROR] FA03 esta en solo lectura por el problema que ya informo. "
          "Resuelvelo y vuelve a abrir la opcion 9.\n";
  return false;
}

// Guarda los archivos indicados con guardarCambioFA03. Si alguno falla, la
// agenda vuelve al estado anterior al cambio.
bool MenuReservas::guardar(const AgendaCupos &anterior, bool cupos, bool reservas) {
  GuardadoFA03 resultado = guardarCambioFA03(
      cupos, reservas,
      [this]() { return archivosCSV.guardarCupos(agenda.getCupos()); },
      [this]() { return archivosCSV.guardarReservas(agenda.getReservas()); },
      [this, &anterior]() { return archivosCSV.guardarCupos(anterior.getCupos()); });
  agenda = agendaSegunGuardadoFA03(resultado, agenda, anterior);
  if (resultado == GuardadoFA03::Guardado) return true;
  if (resultado == GuardadoFA03::Revertido) {
    cout << "[ERROR] No se pudo guardar el cambio; los archivos y la agenda "
            "quedan como antes.\n";
  } else {
    soloLectura = true;
    cout << "[ERROR] No se pudo guardar el cambio ni restaurar cupos.csv, que "
            "queda con la capacidad nueva. FA03 pasa a solo lectura y muestra lo "
            "que hay en los archivos. Al volver a abrir la opcion 9, la carga "
            "confirma las reservas en espera que quepan con esa capacidad.\n";
  }
  return false;
}

// Turnos del punto con su capacidad, por ejemplo "10:00 (20), 15:00 (12)",
// o texto vacio si el punto no tiene cupos.
string MenuReservas::turnosConCapacidad(const string &punto) const {
  string turnos;
  for (const Cupo &cupo : agenda.getCupos()) {
    if (cupo.getPunto() != punto) continue;
    turnos += (turnos.empty() ? "" : ", ") + cupo.getTurno() + " ("
              + to_string(cupo.getCapacidadMaxima()) + ")";
  }
  return turnos;
}

void MenuReservas::mostrarPromovidas(const vector<string> &promovidas) const {
  for (const string &codigo : promovidas) {
    cout << "[OK] La reserva " << codigo << " pasa de la espera a confirmada ("
         << personasDe(codigo) << " personas).\n";
  }
}

// Fechas distintas y ordenadas en que el turno tiene reservas activas.
vector<string> MenuReservas::fechasConReservas(const string &punto,
                                               const string &turno) const {
  vector<string> fechas;
  for (const Reserva &reserva : agenda.getReservas()) {
    if (reserva.estaActiva() && reserva.getPunto() == punto
        && reserva.getTurno() == turno) {
      bool fechaNueva = true;
      for (const string &fecha : fechas) {
        if (fecha == reserva.getFecha()) fechaNueva = false;
      }
      if (fechaNueva) fechas.push_back(reserva.getFecha());
    }
  }
  sort(fechas.begin(), fechas.end());
  return fechas;
}

// Opcion 1: crea o cambia la capacidad de un turno de un atractivo o mirador.
void MenuReservas::registrarCupo() {
  if (!puedeModificar()) return;
  string punto;
  string turno;
  string texto;
  int capacidad = 0;
  if (!pedirCodigo("Codigo del punto: ", punto)) return;
  int i = buscarPunto(punto);
  if (i == -1 || !admiteCupo(puntos[i].getTipo())) {
    cout << "[ERROR] El punto no existe o no es un atractivo ni un mirador.\n";
    return;
  }
  if (!pedir("Turno (HH:MM): ", turno) || !turnoValido(turno)) {
    cout << "[ERROR] El turno debe tener el formato HH:MM.\n";
    return;
  }
  if (!pedir("Capacidad maxima del turno: ", texto)
      || !leerEntero(texto, capacidad) || capacidad <= 0) {
    cout << "[ERROR] La capacidad debe ser un entero mayor que cero.\n";
    return;
  }

  // Si la capacidad sube, fijarCupo confirma las reservas en espera que ya
  // caben; solo en ese caso cambia tambien reservas.csv.
  AgendaCupos anterior = agenda;
  vector<string> promovidas;
  if (!agenda.fijarCupo(punto, turno, capacidad, promovidas)) {
    cout << "[ERROR] La capacidad no puede quedar por debajo de las personas "
            "confirmadas ni del grupo mas grande en espera.\n";
    return;
  }
  if (!guardar(anterior, true, !promovidas.empty())) return;
  cout << "[OK] Capacidad de " << nombrePunto(punto) << " a las " << turno << ": "
       << capacidad << " personas por fecha.\n";
  mostrarPromovidas(promovidas);
}

// Opcion 2: reserva un turno para una solicitud; si no cabe, queda en espera.
void MenuReservas::reservarTurno() {
  if (!puedeModificar()) return;
  string codigo;
  if (!pedirCodigo("Codigo de la solicitud: ", codigo)) return;
  int s = buscarSolicitud(codigo);
  if (s == -1) {
    cout << "[ERROR] No existe la solicitud " << codigo << ".\n";
    return;
  }
  const Solicitud &solicitud = solicitudes[s];
  const string punto = solicitud.getDestino();
  string turnos = turnosConCapacidad(punto);
  if (turnos.empty()) {
    cout << "[ERROR] El destino " << nombrePunto(punto)
         << " no tiene turnos con capacidad registrada.\n";
    return;
  }
  cout << "Destino " << nombrePunto(punto) << ". Turnos con su capacidad: "
       << turnos << ".\n";

  string fecha;
  string turno;
  string texto;
  if (!pedir("Fecha (AAAA-MM-DD): ", fecha) || !fechaValida(fecha)) {
    cout << "[ERROR] La fecha debe existir y tener el formato AAAA-MM-DD.\n";
    return;
  }
  if (!pedir("Turno (HH:MM): ", turno) || agenda.capacidad(punto, turno) == -1) {
    cout << "[ERROR] El destino no tiene capacidad registrada en ese turno.\n";
    return;
  }
  if (agenda.tieneReservaActiva(codigo, fecha, turno)) {
    cout << "[ERROR] La solicitud ya tiene una reserva activa en ese turno.\n";
    return;
  }
  int maximoGrupo = solicitud.getCantidadTuristas();
  int personas = maximoGrupo;
  if (!pedir("Personas (Enter para " + to_string(maximoGrupo) + "): ", texto)) {
    return;
  }
  if (!texto.empty()
      && (!leerEntero(texto, personas) || personas <= 0 || personas > maximoGrupo)) {
    cout << "[ERROR] Las personas deben ser entre 1 y " << maximoGrupo << ".\n";
    return;
  }
  int capacidad = agenda.capacidad(punto, turno);
  if (personas > capacidad) {
    cout << "[ERROR] El grupo supera la capacidad del turno (" << capacidad
         << " personas).\n";
    return;
  }

  AgendaCupos anterior = agenda;
  Reserva creada;
  if (!agenda.reservar(codigo, punto, fecha, turno, personas, creada)) {
    cout << "[ERROR] No se pudo crear la reserva.\n";
    return;
  }
  if (!guardar(anterior, false, true)) return;
  if (creada.estaConfirmada()) {
    cout << "[OK] Reserva " << creada.getCodigo() << " confirmada. Ahora hay "
         << cuposLibres(capacidad - agenda.ocupados(punto, fecha, turno))
         << " en ese turno.\n";
  } else {
    cout << "[OK] El turno no tiene cupos libres para " << personas
         << " personas. La reserva " << creada.getCodigo()
         << " queda en espera en la posicion " << creada.getOrdenEspera()
         << ".\n";
  }
}

// Opcion 3: cancela una reserva activa y avisa que paso con la espera.
void MenuReservas::cancelarReserva() {
  if (!puedeModificar()) return;
  string codigo;
  if (!pedirCodigo("Codigo de la reserva: ", codigo)) return;
  Reserva antes;
  for (const Reserva &reserva : agenda.getReservas()) {
    if (reserva.getCodigo() == codigo) antes = reserva;
  }
  const string punto = antes.getPunto();
  const string fecha = antes.getFecha();
  const string turno = antes.getTurno();
  int ocupadosAntes = agenda.ocupados(punto, fecha, turno);
  AgendaCupos anterior = agenda;
  vector<string> promovidas;
  if (antes.getCodigo() != codigo || !agenda.cancelar(codigo, promovidas)) {
    cout << "[ERROR] No hay una reserva activa con el codigo " << codigo
         << ".\n";
    return;
  }
  if (!guardar(anterior, false, true)) return;

  int capacidad = agenda.capacidad(punto, turno);
  cout << "[OK] Reserva " << codigo << " cancelada.";
  if (antes.estaConfirmada()) {
    cout << " Se liberan " << antes.getPersonas() << " cupos y el turno queda con "
         << cuposLibres(capacidad - (ocupadosAntes - antes.getPersonas()))
         << " para la lista de espera.";
  }
  cout << "\n";
  // Si la cancelada estaba confirmada, cancelar ya recorrio la espera en
  // orden de llegada y confirmo las que cabian; las que quedan no caben y
  // conservan su posicion.
  mostrarPromovidas(promovidas);
  int libres = capacidad - agenda.ocupados(punto, fecha, turno);
  for (const Reserva &enCola : agenda.enEspera(punto, fecha, turno)) {
    cout << "      " << enCola.getCodigo() << " (" << enCola.getPersonas()
         << " personas) no cabe y conserva la posicion "
         << enCola.getOrdenEspera() << ". Ahora hay " << cuposLibres(libres)
         << ".\n";
  }
}

// Opcion 4: ocupacion y lista de espera de cada turno, por fecha.
void MenuReservas::reporteOcupacion() const {
  string filtro;
  if (!pedir("Codigo del punto (Enter para todos): ", filtro)) return;
  filtro = mayusculas(filtro);

  bool hayCupos = false;
  for (const Cupo &cupo : agenda.getCupos()) {
    if (!filtro.empty() && cupo.getPunto() != filtro) continue;
    hayCupos = true;
    const string punto = cupo.getPunto();
    const string turno = cupo.getTurno();
    cout << "\n" << nombrePunto(punto) << ", turno " << turno << ", capacidad "
         << cupo.getCapacidadMaxima() << "\n";

    vector<string> fechas = fechasConReservas(punto, turno);
    if (fechas.empty()) cout << "  Sin reservas activas.\n";
    for (const string &fecha : fechas) {
      int ocupados = agenda.ocupados(punto, fecha, turno);
      vector<Reserva> cola = agenda.enEspera(punto, fecha, turno);
      cout << "  " << fecha << ": " << ocupados << " de "
           << cupo.getCapacidadMaxima() << " ocupados, "
           << cuposLibres(cupo.getCapacidadMaxima() - ocupados) << ", "
           << cola.size() << " en espera\n";
      for (const Reserva &enCola : cola) {
        cout << "    " << enCola.getOrdenEspera() << ". " << enCola.getCodigo()
             << " (" << enCola.getSolicitud() << ", " << enCola.getPersonas()
             << " personas)\n";
      }
    }
  }
  if (!hayCupos) {
    cout << "[ERROR] No hay capacidades registradas"
         << (filtro.empty() ? "" : " para " + filtro) << ".\n";
  }
}

// La agenda se inicializa al final porque cargarAgenda usa puntos y solicitudes.
MenuReservas::MenuReservas(const ArchivosCSV &datos)
    : archivosCSV(datos), puntos(datos.getPuntos()),
      solicitudes(datos.getSolicitudes()), soloLectura(false),
      agenda(cargarAgenda()) {}

// Muestra el submenu y llama a la opcion elegida hasta que se elige 0.
void MenuReservas::ejecutar() {
  string opcion;
  do {
    cout << "\n=== Reservas de cupos por turno FA03 ===\n"
         << "1. Registrar o actualizar la capacidad de un turno\n"
         << "2. Reservar un turno para una solicitud\n"
         << "3. Cancelar una reserva\n"
         << "4. Reporte de ocupacion\n"
         << "0. Volver\n"
         << "Opcion: ";
    if (!getline(cin, opcion)) return;
    if (opcion == "1") registrarCupo();
    else if (opcion == "2") reservarTurno();
    else if (opcion == "3") cancelarReserva();
    else if (opcion == "4") reporteOcupacion();
    else if (opcion != "0") cout << "Opcion invalida.\n";
  } while (opcion != "0");
}
