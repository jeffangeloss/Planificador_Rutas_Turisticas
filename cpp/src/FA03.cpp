// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.
// Carga y guardado de cupos y reservas, y entrada al submenu de FA03.

#include "FA03.h"

#include "AgendaCupos.h"
#include "MenuReservas.h"
#include "Punto.h"
#include "Solicitud.h"

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

using namespace std;

// Ayudantes declarados en FA03.h.
// Indica si el tipo de punto admite cupos por turno.
bool admiteCupo(TipoPunto tipo) {
  return tipo == TipoPunto::Atractivo || tipo == TipoPunto::Mirador;
}

// Devuelve la posicion del punto con ese codigo, o -1 si no existe.
int indicePunto(const vector<Punto> &puntos, const string &codigo) {
  for (int i = 0; i < static_cast<int>(puntos.size()); ++i) {
    if (puntos[i].getCodigo() == codigo) return i;
  }
  return -1;
}

// Devuelve la posicion de la solicitud con ese codigo, o -1 si no existe.
int indiceSolicitud(const vector<Solicitud> &solicitudes, const string &codigo) {
  for (int i = 0; i < static_cast<int>(solicitudes.size()); ++i) {
    if (solicitudes[i].getCodigo() == codigo) return i;
  }
  return -1;
}

namespace {

// Motivo por el que una reserva leida no cumple las reglas de
// _datos/README.md, o texto vacio si las cumple.
string motivoReservaInvalida(const Reserva &reserva,
                             const vector<Solicitud> &solicitudes,
                             const AgendaCupos &conCupos) {
  int s = indiceSolicitud(solicitudes, reserva.getSolicitud());
  if (s == -1) return "la solicitud no existe";
  if (solicitudes[s].getDestino() != reserva.getPunto()) {
    return "el punto no es el destino de la solicitud";
  }
  int capacidad = conCupos.capacidad(reserva.getPunto(), reserva.getTurno());
  if (capacidad == -1) return "el turno no tiene capacidad registrada";
  if (reserva.getPersonas() > solicitudes[s].getCantidadTuristas()) {
    return "tiene mas personas que la solicitud";
  }
  // Una reserva cancelada ya no ocupa cupos, asi que no se compara con la
  // capacidad actual, que pudo bajar despues de cancelarla.
  if (reserva.estaActiva() && reserva.getPersonas() > capacidad) {
    return "el grupo supera la capacidad del turno";
  }
  return "";
}

}  // namespace

// Valida los cupos y reservas leidos y arma la agenda de FA03 con sus errores.
CargaFA03 armarCargaFA03(const vector<Cupo> &cuposLeidos,
                         const vector<Reserva> &reservasLeidas,
                         bool hayFilasMalFormadas, const vector<Punto> &puntos,
                         const vector<Solicitud> &solicitudes) {
  vector<string> errores;
  bool descartes = hayFilasMalFormadas;

  vector<Cupo> cupos;
  for (const Cupo &cupo : cuposLeidos) {
    int i = indicePunto(puntos, cupo.getPunto());
    if (i == -1 || !admiteCupo(puntos[i].getTipo())) {
      errores.push_back("Capacidad de " + cupo.getPunto() + " a las " + cupo.getTurno()
                        + " descartada: el punto no es un atractivo ni un mirador.");
      descartes = true;
    } else {
      cupos.push_back(cupo);
    }
  }

  AgendaCupos soloCupos(cupos, {});
  vector<Reserva> reservas;
  vector<string> activas;  // solicitud, fecha y turno de cada reserva activa aceptada
  for (const Reserva &reserva : reservasLeidas) {
    string motivo = motivoReservaInvalida(reserva, solicitudes, soloCupos);
    if (motivo.empty() && reserva.estaActiva()) {
      string clave = reserva.getSolicitud() + " " + reserva.getFecha() + " " + reserva.getTurno();
      if (find(activas.begin(), activas.end(), clave) != activas.end()) {
        motivo = "la solicitud ya tiene otra reserva activa en esa fecha y ese turno";
      } else {
        activas.push_back(clave);
      }
    }
    if (motivo.empty()) {
      reservas.push_back(reserva);
    } else {
      errores.push_back("Reserva " + reserva.getCodigo() + " descartada: " + motivo + ".");
      descartes = true;
    }
  }

  AgendaCupos agenda(cupos, reservas);
  for (const Reserva &reserva : agenda.getReservas()) {
    if (reserva.estaConfirmada()
        && agenda.ocupados(reserva.getPunto(), reserva.getFecha(), reserva.getTurno())
               > agenda.capacidad(reserva.getPunto(), reserva.getTurno())) {
      errores.push_back("El turno de " + reserva.getPunto() + " del " + reserva.getFecha()
                        + " a las " + reserva.getTurno()
                        + " tiene mas personas confirmadas que su capacidad.");
      descartes = true;
      break;
    }
  }

  // En solo lectura la agenda queda como en los archivos, para que el reporte
  // muestre lo mismo que reservas.csv.
  vector<string> promovidas;
  if (!descartes) promovidas = agenda.normalizar();
  return CargaFA03{cupos, reservas, agenda, errores, promovidas, descartes};
}

// Guarda cupos y reservas segun el cambio y restaura cupos.csv si falla reservas.csv.
GuardadoFA03 guardarCambioFA03(bool cupos, bool reservas,
                               const function<bool()> &escribirCupos,
                               const function<bool()> &escribirReservas,
                               const function<bool()> &restaurarCupos) {
  bool cuposGuardados = !cupos || escribirCupos();
  bool reservasGuardadas = cuposGuardados && (!reservas || escribirReservas());
  if (cuposGuardados && reservasGuardadas) return GuardadoFA03::Guardado;
  // guardarCSV deja intacto el archivo que no logra escribir, asi que solo hay
  // que restaurar cupos.csv cuando ya se guardo y falla reservas.csv.
  if (cupos && cuposGuardados && !restaurarCupos()) return GuardadoFA03::SinRestaurar;
  return GuardadoFA03::Revertido;
}

// Elige la agenda que queda en memoria segun el resultado del guardado.
AgendaCupos agendaSegunGuardadoFA03(GuardadoFA03 resultado, const AgendaCupos &nueva,
                                    const AgendaCupos &anterior) {
  if (resultado == GuardadoFA03::Guardado) return nueva;
  if (resultado == GuardadoFA03::Revertido) return anterior;
  // cupos.csv quedo con la capacidad nueva y reservas.csv sin cambios.
  return AgendaCupos(nueva.getCupos(), anterior.getReservas());
}

// Abre el submenu de reservas de cupos de FA03.
void ejecutarFA03(const ArchivosCSV &archivosCSV) {
  MenuReservas menu(archivosCSV);
  menu.ejecutar();
}
