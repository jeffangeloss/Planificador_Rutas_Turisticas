#include "Reserva.h"

#include <array>
#include <cctype>
#include <sstream>

// FA03 Reservas de cupos por turno con lista de espera. Responsable: Jefferson Sanchez.

Reserva::Reserva()
    : personas(0), estado(EstadoReserva::Cancelada), ordenEspera(0) {}

Reserva::Reserva(std::string codigo, std::string solicitud, std::string punto,
                 std::string fecha, std::string turno, int personas,
                 EstadoReserva estado, int ordenEspera) {
  this->codigo = codigo;
  this->solicitud = solicitud;
  this->punto = punto;
  this->fecha = fecha;
  this->turno = turno;
  this->personas = personas;
  this->estado = estado;
  this->ordenEspera = ordenEspera;
}

std::string Reserva::getCodigo() const { return codigo; }

std::string Reserva::getSolicitud() const { return solicitud; }

std::string Reserva::getPunto() const { return punto; }

std::string Reserva::getFecha() const { return fecha; }

std::string Reserva::getTurno() const { return turno; }

int Reserva::getPersonas() const { return personas; }

EstadoReserva Reserva::getEstado() const { return estado; }

int Reserva::getOrdenEspera() const { return ordenEspera; }

bool Reserva::estaConfirmada() const {
  return estado == EstadoReserva::Confirmada;
}

bool Reserva::estaEnEspera() const { return estado == EstadoReserva::EnEspera; }

bool Reserva::estaActiva() const { return estado != EstadoReserva::Cancelada; }

bool Reserva::esDelTurno(const std::string &puntoBuscado,
                         const std::string &fechaBuscada,
                         const std::string &turnoBuscado) const {
  return punto == puntoBuscado && fecha == fechaBuscada && turno == turnoBuscado;
}

void Reserva::confirmar() {
  estado = EstadoReserva::Confirmada;
  ordenEspera = 0;
}

void Reserva::ponerEnEspera(int orden) {
  estado = EstadoReserva::EnEspera;
  ordenEspera = orden;
}

void Reserva::cancelar() {
  estado = EstadoReserva::Cancelada;
  ordenEspera = 0;
}

namespace {

// Valor de los digitos de texto entre las posiciones desde y hasta (sin incluir
// hasta), o -1 si alguno no es digito.
int numeroEn(const std::string &texto, int desde, int hasta) {
  int valor = 0;
  for (int i = desde; i < hasta; ++i) {
    if (!std::isdigit(static_cast<unsigned char>(texto[i]))) return -1;
    valor = valor * 10 + (texto[i] - '0');
  }
  return valor;
}

}  // namespace

bool fechaValida(const std::string &fecha) {
  if (fecha.size() != 10 || fecha[4] != '-' || fecha[7] != '-') return false;
  int anio = numeroEn(fecha, 0, 4);
  int mes = numeroEn(fecha, 5, 7);
  int dia = numeroEn(fecha, 8, 10);
  if (anio < 1 || mes < 1 || mes > 12 || dia < 1) return false;
  static constexpr std::array<int, 12> diasPorMes = {31, 28, 31, 30, 31, 30,
                                                     31, 31, 30, 31, 30, 31};
  bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || anio % 400 == 0;
  int maximo = diasPorMes[static_cast<std::size_t>(mes - 1)]
               + ((mes == 2 && bisiesto) ? 1 : 0);
  return dia <= maximo;
}

bool turnoValido(const std::string &turno) {
  if (turno.size() != 5 || turno[2] != ':') return false;
  int horas = numeroEn(turno, 0, 2);
  int minutos = numeroEn(turno, 3, 5);
  return horas >= 0 && horas < 24 && minutos >= 0 && minutos < 60;
}

bool codigoReservaValido(const std::string &codigo) {
  if (codigo.size() < 2 || codigo[0] != 'R') return false;
  for (std::size_t i = 1; i < codigo.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(codigo[i]))) return false;
  }
  return true;
}

bool leerEntero(const std::string &texto, int &numero) {
  std::stringstream lector(texto);
  char sobrante;
  return static_cast<bool>(lector >> numero) && !(lector >> sobrante);
}
