#include "AgendaCupos.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>

using namespace std;

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.

AgendaCupos::AgendaCupos(vector<Cupo> cupos, vector<Reserva> reservas)
    : cupos(std::move(cupos)), reservas(std::move(reservas)) {}

const vector<Cupo> &AgendaCupos::getCupos() const { return cupos; }

const vector<Reserva> &AgendaCupos::getReservas() const { return reservas; }

int AgendaCupos::indiceCupo(const string &punto, const string &turno) const {
  for (int i = 0; i < static_cast<int>(cupos.size()); ++i) {
    if (cupos[i].esDe(punto, turno)) return i;
  }
  return -1;
}

int AgendaCupos::indiceReserva(const string &codigo) const {
  for (int i = 0; i < static_cast<int>(reservas.size()); ++i) {
    if (reservas[i].getCodigo() == codigo) return i;
  }
  return -1;
}

int AgendaCupos::capacidad(const string &punto, const string &turno) const {
  int i = indiceCupo(punto, turno);
  return i == -1 ? -1 : cupos[i].getCapacidadMaxima();
}

int AgendaCupos::ocupados(const string &punto, const string &fecha,
                          const string &turno) const {
  int total = 0;
  for (const Reserva &reserva : reservas) {
    if (reserva.estaConfirmada() && reserva.esDelTurno(punto, fecha, turno)) {
      total += reserva.getPersonas();
    }
  }
  return total;
}

vector<int> AgendaCupos::indicesEnEspera(const string &punto,
                                         const string &fecha,
                                         const string &turno) const {
  vector<int> indices;
  for (int i = 0; i < static_cast<int>(reservas.size()); ++i) {
    if (reservas[i].estaEnEspera() && reservas[i].esDelTurno(punto, fecha, turno)) {
      indices.push_back(i);
    }
  }
  stable_sort(indices.begin(), indices.end(), [this](int a, int b) {
    return reservas[a].getOrdenEspera() < reservas[b].getOrdenEspera();
  });
  return indices;
}

vector<Reserva> AgendaCupos::enEspera(const string &punto, const string &fecha,
                                      const string &turno) const {
  vector<Reserva> cola;
  for (int i : indicesEnEspera(punto, fecha, turno)) cola.push_back(reservas[i]);
  return cola;
}

bool AgendaCupos::tieneReservaActiva(const string &solicitud,
                                     const string &fecha,
                                     const string &turno) const {
  for (const Reserva &reserva : reservas) {
    if (reserva.estaActiva() && reserva.getSolicitud() == solicitud
        && reserva.getFecha() == fecha && reserva.getTurno() == turno) {
      return true;
    }
  }
  return false;
}

string AgendaCupos::siguienteCodigo() const {
  long long mayor = 0;
  for (const Reserva &reserva : reservas) {
    string codigo = reserva.getCodigo();
    if (codigo.size() < 2 || codigo[0] != 'R') continue;
    stringstream lector(codigo.substr(1));
    long long numero;
    char sobrante;
    if (lector >> numero && !(lector >> sobrante) && numero > mayor) {
      mayor = numero;
    }
  }
  if (mayor == numeric_limits<long long>::max()) return "";
  stringstream nuevo;
  nuevo << 'R' << setw(3) << setfill('0') << mayor + 1;
  return nuevo.str();
}

bool AgendaCupos::reservar(const string &solicitud, const string &punto,
                           const string &fecha, const string &turno,
                           int personas, Reserva &creada) {
  int maximo = capacidad(punto, turno);
  if (maximo == -1 || personas <= 0 || personas > maximo) return false;

  string codigo = siguienteCodigo();
  if (codigo.empty() || indiceReserva(codigo) != -1) return false;
  if (personas <= maximo - ocupados(punto, fecha, turno)) {
    creada = Reserva(codigo, solicitud, punto, fecha, turno, personas,
                     EstadoReserva::Confirmada, 0);
  } else {
    int ultimo = 0;
    for (const Reserva &enCola : enEspera(punto, fecha, turno)) {
      ultimo = max(ultimo, enCola.getOrdenEspera());
    }
    creada = Reserva(codigo, solicitud, punto, fecha, turno, personas,
                     EstadoReserva::EnEspera, ultimo + 1);
  }
  reservas.push_back(creada);
  return true;
}

void AgendaCupos::promover(const string &punto, const string &fecha,
                           const string &turno, vector<string> &promovidas) {
  int maximo = capacidad(punto, turno);
  if (maximo == -1) return;
  int libres = maximo - ocupados(punto, fecha, turno);
  for (int i : indicesEnEspera(punto, fecha, turno)) {
    if (reservas[i].getPersonas() > libres) continue;  // no cabe y conserva su lugar
    reservas[i].confirmar();
    libres -= reservas[i].getPersonas();
    promovidas.push_back(reservas[i].getCodigo());
  }
  renumerarEspera(punto, fecha, turno);
}

void AgendaCupos::renumerarEspera(const string &punto, const string &fecha,
                                  const string &turno) {
  vector<int> indices = indicesEnEspera(punto, fecha, turno);
  for (int posicion = 0; posicion < static_cast<int>(indices.size()); ++posicion) {
    reservas[indices[posicion]].ponerEnEspera(posicion + 1);
  }
}

bool AgendaCupos::cancelar(const string &codigo, vector<string> &promovidas) {
  int i = indiceReserva(codigo);
  if (i == -1 || !reservas[i].estaActiva()) return false;

  bool liberaCupos = reservas[i].estaConfirmada();
  string punto = reservas[i].getPunto();
  string fecha = reservas[i].getFecha();
  string turno = reservas[i].getTurno();
  reservas[i].cancelar();
  if (liberaCupos) {
    promover(punto, fecha, turno, promovidas);
  } else {
    renumerarEspera(punto, fecha, turno);
  }
  return true;
}

bool AgendaCupos::fijarCupo(const string &punto, const string &turno,
                            int capacidadNueva, vector<string> &promovidas) {
  if (capacidadNueva <= 0) return false;
  vector<string> fechas;
  for (const Reserva &reserva : reservas) {
    if (reserva.getPunto() != punto || reserva.getTurno() != turno) continue;
    if (ocupados(punto, reserva.getFecha(), turno) > capacidadNueva
        || (reserva.estaEnEspera() && reserva.getPersonas() > capacidadNueva)) {
      return false;
    }
    if (reserva.estaEnEspera()
        && find(fechas.begin(), fechas.end(), reserva.getFecha())
               == fechas.end()) {
      fechas.push_back(reserva.getFecha());
    }
  }

  int i = indiceCupo(punto, turno);
  if (i == -1) {
    cupos.push_back(Cupo(punto, turno, capacidadNueva));
  } else {
    cupos[i].cambiarCapacidad(capacidadNueva);
  }
  sort(fechas.begin(), fechas.end());
  for (const string &fecha : fechas) promover(punto, fecha, turno, promovidas);
  return true;
}

vector<string> AgendaCupos::normalizar() {
  vector<string> promovidas;
  vector<int> turnosVistos;
  for (int i = 0; i < static_cast<int>(reservas.size()); ++i) {
    if (!reservas[i].estaEnEspera()) continue;
    const string punto = reservas[i].getPunto();
    const string fecha = reservas[i].getFecha();
    const string turno = reservas[i].getTurno();
    bool visto = false;
    for (int j : turnosVistos) {
      if (reservas[j].esDelTurno(punto, fecha, turno)) visto = true;
    }
    if (visto) continue;
    turnosVistos.push_back(i);
    promover(punto, fecha, turno, promovidas);
  }
  return promovidas;
}
