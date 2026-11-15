// FA03 - Autor: Jefferson Sanchez.
// Reservas de cupos por turno con lista de espera (opcion 9 del menu).
// Registra el cupo de cada atractivo o mirador por turno y reserva una fecha y
// un turno para una solicitud cuyo destino es ese punto. La reserva queda
// confirmada si cabe en los cupos libres y, si no, entra a la lista de espera.
// Al cancelar una reserva confirmada o al subir un cupo, se confirma en orden
// de llegada cada reserva en espera que cabe, y la que no cabe conserva su
// lugar. Cada cambio se guarda de inmediato en cupos.csv o reservas.csv.

#include "FA03.h"

#include "AgendaCupos.h"
#include "Punto.h"
#include "Solicitud.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

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

bool admiteCupo(TipoPunto tipo) {
  return tipo == TipoPunto::Atractivo || tipo == TipoPunto::Mirador;
}

class MenuReservas {
private:
  const ArchivosCSV &archivosCSV;
  vector<Punto> puntos;
  vector<Solicitud> solicitudes;
  bool soloLectura;
  AgendaCupos agenda;

  // Lee una linea sin espacios en los extremos. Rechaza las comas, porque el
  // formato CSV no las admite dentro de un campo.
  bool pedir(const string &mensaje, string &respuesta) const {
    cout << mensaje;
    if (!getline(cin, respuesta)) return false;
    size_t inicio = respuesta.find_first_not_of(" \t\r");
    size_t fin = respuesta.find_last_not_of(" \t\r");
    respuesta = inicio == string::npos ? "" : respuesta.substr(inicio, fin - inicio + 1);
    if (respuesta.find(',') == string::npos) return true;
    cout << "[ERROR] No uses comas: el formato CSV no las admite.\n";
    return false;
  }

  bool pedirCodigo(const string &mensaje, string &codigo) const {
    if (!pedir(mensaje, codigo)) return false;
    codigo = mayusculas(codigo);
    return !codigo.empty();
  }

  int buscarPunto(const string &codigo) const {
    for (int i = 0; i < static_cast<int>(puntos.size()); ++i) {
      if (puntos[i].getCodigo() == codigo) return i;
    }
    return -1;
  }

  int buscarSolicitud(const string &codigo) const {
    for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
      if (solicitudes[i].getCodigo() == codigo) return i;
    }
    return -1;
  }

  string nombrePunto(const string &codigo) const {
    int i = buscarPunto(codigo);
    return i == -1 ? codigo : codigo + " " + puntos[i].getNombre();
  }

  int personasDe(const string &codigoReserva) const {
    for (const Reserva &reserva : agenda.getReservas()) {
      if (reserva.getCodigo() == codigoReserva) return reserva.getPersonas();
    }
    return 0;
  }

  // Motivo por el que una reserva leida no cumple las reglas de
  // _datos/README.md, o texto vacio si las cumple.
  string motivoInvalida(const Reserva &reserva, const AgendaCupos &conCupos) const {
    int s = buscarSolicitud(reserva.getSolicitud());
    if (s == -1) return "la solicitud no existe";
    if (solicitudes[s].getDestino() != reserva.getPunto()) {
      return "el punto no es el destino de la solicitud";
    }
    int capacidad = conCupos.capacidad(reserva.getPunto(), reserva.getTurno());
    if (capacidad == -1) return "el turno no tiene cupo";
    if (reserva.getPersonas() > solicitudes[s].getCantidadTuristas()) {
      return "tiene mas personas que la solicitud";
    }
    if (reserva.getPersonas() > capacidad) return "el grupo supera el cupo";
    return "";
  }

  // Carga los CSV de FA03 y descarta lo que no cumple sus reglas. Si algo se
  // descarta, FA03 queda en solo lectura para no borrar esas filas al guardar.
  AgendaCupos cargarAgenda() {
    vector<Cupo> cuposLeidos = archivosCSV.cargarCupos();
    vector<Reserva> reservasLeidas = archivosCSV.cargarReservas();
    bool descartes =
        static_cast<int>(cuposLeidos.size()) != archivosCSV.contarFilas("cupos.csv")
        || static_cast<int>(reservasLeidas.size())
               != archivosCSV.contarFilas("reservas.csv");

    vector<Cupo> cupos;
    for (const Cupo &cupo : cuposLeidos) {
      int i = buscarPunto(cupo.getPunto());
      if (i == -1 || !admiteCupo(puntos[i].getTipo())) {
        cout << "[ERROR] Cupo de " << cupo.getPunto() << " " << cupo.getTurno()
             << " descartado: el punto no es un atractivo ni un mirador.\n";
        descartes = true;
      } else {
        cupos.push_back(cupo);
      }
    }
    AgendaCupos soloCupos(cupos, {});
    vector<Reserva> reservas;
    for (const Reserva &reserva : reservasLeidas) {
      string motivo = motivoInvalida(reserva, soloCupos);
      if (motivo.empty()) {
        reservas.push_back(reserva);
      } else {
        cout << "[ERROR] Reserva " << reserva.getCodigo() << " descartada: "
             << motivo << ".\n";
        descartes = true;
      }
    }

    AgendaCupos cargada(cupos, reservas);
    for (const Reserva &reserva : cargada.getReservas()) {
      if (reserva.estaConfirmada()
          && cargada.ocupados(reserva.getPunto(), reserva.getFecha(),
                              reserva.getTurno())
                 > cargada.capacidad(reserva.getPunto(), reserva.getTurno())) {
        cout << "[ERROR] El turno de " << reserva.getPunto() << " del "
             << reserva.getFecha() << " a las " << reserva.getTurno()
             << " tiene mas confirmados que cupo.\n";
        descartes = true;
        break;
      }
    }
    if (descartes) {
      soloLectura = true;
      cout << "[ERROR] Corrige cupos.csv o reservas.csv. Mientras tanto FA03 solo "
              "muestra el reporte, para no perder filas al guardar.\n";
    }

    vector<string> promovidas = cargada.normalizar();
    for (const string &codigo : promovidas) {
      cout << "[OK] Al cargar, la reserva " << codigo
           << " pasa a confirmada porque ya cabe en su turno.\n";
    }
    if (!promovidas.empty() && !soloLectura) {
      archivosCSV.guardarReservas(cargada.getReservas());
    }
    return cargada;
  }

  bool puedeModificar() const {
    if (!soloLectura) return true;
    cout << "[ERROR] FA03 esta en solo lectura hasta corregir cupos.csv o "
            "reservas.csv.\n";
    return false;
  }

  // Guarda los archivos indicados. Si alguno falla, los archivos y la agenda
  // vuelven al estado anterior al cambio.
  bool guardar(const AgendaCupos &anterior, bool cupos, bool reservas) {
    bool cuposGuardados = !cupos || archivosCSV.guardarCupos(agenda.getCupos());
    bool reservasGuardadas =
        cuposGuardados
        && (!reservas || archivosCSV.guardarReservas(agenda.getReservas()));
    if (cuposGuardados && reservasGuardadas) return true;
    if (cupos && cuposGuardados) archivosCSV.guardarCupos(anterior.getCupos());
    agenda = anterior;
    cout << "[ERROR] No se pudo guardar el cambio; los archivos y la agenda "
            "quedan como antes.\n";
    return false;
  }

  void mostrarPromovidas(const vector<string> &promovidas) const {
    for (const string &codigo : promovidas) {
      cout << "[OK] La reserva " << codigo << " pasa de la espera a confirmada ("
           << personasDe(codigo) << " personas).\n";
    }
  }

  void registrarCupo() {
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

    AgendaCupos anterior = agenda;
    vector<string> promovidas;
    if (!agenda.fijarCupo(punto, turno, capacidad, promovidas)) {
      cout << "[ERROR] El cupo no puede quedar por debajo de las personas "
              "confirmadas ni del grupo mas grande en espera.\n";
      return;
    }
    if (!guardar(anterior, true, !promovidas.empty())) return;
    cout << "[OK] Cupo de " << nombrePunto(punto) << " a las " << turno << ": "
         << capacidad << " personas por fecha.\n";
    mostrarPromovidas(promovidas);
  }

  void reservarTurno() {
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
    string turnos;
    for (const Cupo &cupo : agenda.getCupos()) {
      if (cupo.getPunto() != punto) continue;
      turnos += (turnos.empty() ? "" : ", ") + cupo.getTurno() + " ("
                + to_string(cupo.getCapacidadMaxima()) + ")";
    }
    if (turnos.empty()) {
      cout << "[ERROR] El destino " << nombrePunto(punto)
           << " no tiene cupos por turno.\n";
      return;
    }
    cout << "Destino " << nombrePunto(punto) << ". Turnos con su cupo: "
         << turnos << ".\n";

    string fecha;
    string turno;
    string texto;
    if (!pedir("Fecha (AAAA-MM-DD): ", fecha) || !fechaValida(fecha)) {
      cout << "[ERROR] La fecha debe existir y tener el formato AAAA-MM-DD.\n";
      return;
    }
    if (!pedir("Turno (HH:MM): ", turno) || agenda.capacidad(punto, turno) == -1) {
      cout << "[ERROR] El destino no tiene cupo en ese turno.\n";
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
      cout << "[ERROR] El grupo supera el cupo del turno (" << capacidad
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
      cout << "[OK] El turno no tiene cupo libre para " << personas
           << " personas. La reserva " << creada.getCodigo()
           << " queda en espera en la posicion " << creada.getOrdenEspera()
           << ".\n";
    }
  }

  void cancelarReserva() {
    if (!puedeModificar()) return;
    string codigo;
    if (!pedirCodigo("Codigo de la reserva: ", codigo)) return;
    Reserva antes;
    for (const Reserva &reserva : agenda.getReservas()) {
      if (reserva.getCodigo() == codigo) antes = reserva;
    }
    int ocupadosAntes = agenda.ocupados(antes.getPunto(), antes.getFecha(),
                                        antes.getTurno());
    AgendaCupos anterior = agenda;
    vector<string> promovidas;
    if (antes.getCodigo() != codigo || !agenda.cancelar(codigo, promovidas)) {
      cout << "[ERROR] No hay una reserva activa con el codigo " << codigo
           << ".\n";
      return;
    }
    if (!guardar(anterior, false, true)) return;

    const string punto = antes.getPunto();
    const string fecha = antes.getFecha();
    const string turno = antes.getTurno();
    int capacidad = agenda.capacidad(punto, turno);
    cout << "[OK] Reserva " << codigo << " cancelada.";
    if (antes.estaConfirmada()) {
      cout << " Se liberan " << antes.getPersonas() << " cupos y el turno queda con "
           << cuposLibres(capacidad - (ocupadosAntes - antes.getPersonas()))
           << " para la lista de espera.";
    }
    cout << "\n";
    mostrarPromovidas(promovidas);
    int libres = capacidad - agenda.ocupados(punto, fecha, turno);
    for (const Reserva &enCola : agenda.enEspera(punto, fecha, turno)) {
      cout << "      " << enCola.getCodigo() << " (" << enCola.getPersonas()
           << " personas) no cabe y conserva la posicion "
           << enCola.getOrdenEspera() << ". Ahora hay " << cuposLibres(libres)
           << ".\n";
    }
  }

  void reporteOcupacion() const {
    string filtro;
    if (!pedir("Codigo del punto (Enter para todos): ", filtro)) return;
    filtro = mayusculas(filtro);

    bool hayCupos = false;
    for (const Cupo &cupo : agenda.getCupos()) {
      if (!filtro.empty() && cupo.getPunto() != filtro) continue;
      hayCupos = true;
      const string punto = cupo.getPunto();
      const string turno = cupo.getTurno();
      cout << "\n" << nombrePunto(punto) << ", turno " << turno << ", cupo "
           << cupo.getCapacidadMaxima() << "\n";

      vector<string> fechas;
      for (const Reserva &reserva : agenda.getReservas()) {
        if (reserva.estaActiva() && reserva.getPunto() == punto
            && reserva.getTurno() == turno
            && find(fechas.begin(), fechas.end(), reserva.getFecha())
                   == fechas.end()) {
          fechas.push_back(reserva.getFecha());
        }
      }
      sort(fechas.begin(), fechas.end());
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
      cout << "[ERROR] No hay cupos registrados"
           << (filtro.empty() ? "" : " para " + filtro) << ".\n";
    }
  }

public:
  explicit MenuReservas(const ArchivosCSV &datos)
      : archivosCSV(datos), puntos(datos.getPuntos()),
        solicitudes(datos.getSolicitudes()), soloLectura(false),
        agenda(cargarAgenda()) {}

  void ejecutar() {
    string opcion;
    do {
      cout << "\n=== Reservas de cupos por turno FA03 ===\n"
           << "1. Registrar o actualizar un cupo\n"
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
};

}  // namespace

void ejecutarFA03(const ArchivosCSV &archivosCSV) {
  MenuReservas menu(archivosCSV);
  menu.ejecutar();
}
