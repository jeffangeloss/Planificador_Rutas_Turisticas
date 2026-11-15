// Grupo 4: ruta turística en Lima. Módulo imperativo (FA01).
// FA01 Recomendador explicable de atractivos por perfil. Responsable: Samantha Rodriguez.
// Enunciado FA01: registrar datos y coordinar el acceso al recomendador.

#include "FA01.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {

class RegistroFA01 {
private:
  string carpetaDatos;

  string rutaArchivo(const string &nombre) const {
    return carpetaDatos + "/" + nombre;
  }

  string leerTexto(const string &mensaje) const {
    string respuesta;
    while (true) {
      cout << mensaje;
      if (!getline(cin, respuesta))
        return "";
      if (respuesta.find(',') == string::npos)
        return respuesta;
      cout << "No uses comas. Si son varios valores, separalos con punto y "
              "coma.\n";
    }
  }

  vector<string> separarCampos(const string &linea) const {
    vector<string> campos;
    string campo;
    for (int i = 0; i < static_cast<int>(linea.size()); ++i) {
      if (linea[i] == ',') {
        campos.push_back(campo);
        campo = "";
      } else if (linea[i] != '\r') {
        campo += linea[i];
      }
    }
    campos.push_back(campo);
    return campos;
  }

  bool leerFilas(const string &nombre, vector<string> &filas) const {
    ifstream archivo(rutaArchivo(nombre).c_str());
    if (!archivo)
      return false;
    string linea;
    getline(archivo, linea);
    while (getline(archivo, linea)) {
      if (linea != "")
        filas.push_back(linea);
    }
    return true;
  }

  string minusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(tolower(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  string mayusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  string limpiarEspacios(const string &texto) const {
    int inicio = 0;
    int fin = static_cast<int>(texto.size()) - 1;
    while (inicio <= fin && (texto[inicio] == ' ' || texto[inicio] == '\t'))
      inicio++;
    while (fin >= inicio && (texto[fin] == ' ' || texto[fin] == '\t'))
      fin--;
    string resultado;
    for (int i = inicio; i <= fin; ++i)
      resultado += texto[i];
    return resultado;
  }

  vector<string> separarLista(const string &texto) const {
    vector<string> valores;
    string valor;
    for (int i = 0; i <= static_cast<int>(texto.size()); ++i) {
      if (i == static_cast<int>(texto.size()) || texto[i] == ';') {
        string limpio = limpiarEspacios(valor);
        if (limpio != "")
          valores.push_back(minusculas(limpio));
        valor = "";
      } else {
        valor += texto[i];
      }
    }
    return valores;
  }

  bool existeCodigo(const string &archivo, const string &codigo) const {
    vector<string> filas;
    if (!leerFilas(archivo, filas))
      return false;
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 0 && campos[0] == codigo)
        return true;
    }
    return false;
  }

  bool guardarFila(const string &nombre, const string &encabezado,
                   const string &codigo, const string &nuevaFila) const {
    vector<string> filas;
    leerFilas(nombre, filas);
    bool actualizada = false;
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 0 && campos[0] == codigo) {
        filas[i] = nuevaFila;
        actualizada = true;
      }
    }
    if (!actualizada)
      filas.push_back(nuevaFila);

    ofstream archivo(rutaArchivo(nombre).c_str());
    if (!archivo)
      return false;
    archivo << encabezado << "\n";
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      archivo << filas[i] << "\n";
    }
    return true;
  }

  void reemplazarRelaciones(const string &nombre, const string &encabezado,
                            const string &codigo,
                            const vector<string> &valores) const {
    vector<string> filas;
    leerFilas(nombre, filas);
    vector<string> nuevasFilas;
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 0 && campos[0] != codigo) {
        nuevasFilas.push_back(filas[i]);
      }
    }
    for (int i = 0; i < static_cast<int>(valores.size()); ++i) {
      nuevasFilas.push_back(codigo + "," + valores[i]);
    }
    ofstream archivo(rutaArchivo(nombre).c_str());
    if (!archivo)
      return;
    archivo << encabezado << "\n";
    for (int i = 0; i < static_cast<int>(nuevasFilas.size()); ++i) {
      archivo << nuevasFilas[i] << "\n";
    }
  }

  string leerSiNo(const string &pregunta) const {
    while (true) {
      string respuesta = minusculas(leerTexto(pregunta + " (si/no): "));
      if (respuesta == "si" || respuesta == "no")
        return respuesta;
      cout << "Escribe si o no.\n";
    }
  }

  bool horaValida(const string &hora) const {
    if (hora.size() != 5 || hora[2] != ':')
      return false;
    for (int i = 0; i < 5; ++i) {
      if (i != 2 && !isdigit(static_cast<unsigned char>(hora[i])))
        return false;
    }
    int horas = (hora[0] - '0') * 10 + (hora[1] - '0');
    int minutos = (hora[3] - '0') * 10 + (hora[4] - '0');
    return horas < 24 && minutos < 60;
  }

  void registrarPerfil() {
    string codigo = mayusculas(leerTexto("Codigo del perfil: "));
    if (existeCodigo("perfiles_turista.csv", codigo)) {
      cout << "Ese perfil ya existe.\n";
      return;
    }
    string movilidad = leerSiNo("Movilidad reducida");
    string ninos = leerSiNo("Viaja con ninos");
    string hora = leerTexto("Hora de visita (HH:MM): ");
    if (codigo == "" || !horaValida(hora)) {
      cout << "El codigo y una hora valida son obligatorios.\n";
      return;
    }
    string fila = codigo + "," + movilidad + "," + ninos + "," + hora;
    if (guardarFila("perfiles_turista.csv",
                    "codigo,movilidad_reducida,viaja_con_ninos,hora_visita",
                    codigo, fila)) {
      string intereses = leerTexto("Intereses separados por punto y coma: ");
      vector<string> lista = separarLista(intereses);
      reemplazarRelaciones("intereses_turista.csv", "codigo,interes", codigo,
                           lista);
      cout << "Perfil e intereses guardados.\n";
    } else {
      cout << "No se pudo guardar el perfil.\n";
    }
  }

  void registrarCaracteristicasPunto() {
    string codigo = mayusculas(leerTexto("Codigo de un punto existente: "));
    if (!existeCodigo("puntos.csv", codigo)) {
      cout << "No existe ese punto.\n";
      return;
    }
    string movilidad = leerSiNo("Accesible para movilidad reducida");
    string ninos = leerSiNo("Adecuado para ninos");
    string seguroDia = leerSiNo("Seguro durante el dia");
    string seguroNoche = leerSiNo("Seguro durante la noche");
    string inicio = leerTexto("Hora de apertura (HH:MM): ");
    string fin = leerTexto("Hora de cierre (HH:MM): ");
    if (!horaValida(inicio) || !horaValida(fin)) {
      cout << "Ingresa horarios validos en formato HH:MM.\n";
      return;
    }
    string fila = codigo + "," + movilidad + "," + ninos + "," + seguroDia +
                  "," + seguroNoche + "," + inicio + "," + fin;
    if (!guardarFila("caracteristicas_puntos.csv",
                     "codigo,accesible_movilidad,apto_ninos,seguro_dia,seguro_"
                     "noche,horario_inicio,horario_fin",
                     codigo, fila)) {
      cout << "No se pudieron guardar las caracteristicas.\n";
      return;
    }
    string categorias = leerTexto("Categorias separadas por punto y coma: ");
    vector<string> lista = separarLista(categorias);
    reemplazarRelaciones("categorias_puntos.csv", "codigo,categoria", codigo,
                         lista);
    cout << "Caracteristicas y categorias guardadas.\n";
  }

  void registrarAccesibilidadConexion() {
    string codigo = mayusculas(leerTexto("Codigo de una conexion existente: "));
    if (!existeCodigo("conexiones.csv", codigo)) {
      cout << "No existe esa conexion.\n";
      return;
    }
    string accesible = leerSiNo("Accesible para movilidad reducida");
    string fila = codigo + "," + accesible;
    if (guardarFila("accesibilidad_conexiones.csv",
                    "codigo,accesible_movilidad", codigo, fila)) {
      cout << "Accesibilidad de la conexion guardada.\n";
    } else {
      cout << "No se pudo guardar la accesibilidad.\n";
    }
  }

public:
  explicit RegistroFA01(const string &rutaDatos) : carpetaDatos(rutaDatos) {}

  void ejecutar() {
    while (true) {
      cout << "\n=== Datos para el recomendador FA01 ===\n"
           << "1. Registrar perfil e intereses\n"
           << "2. Registrar caracteristicas y categorias de un punto\n"
           << "3. Registrar accesibilidad de una conexion\n"
           << "0. Volver\n";
      string opcion = leerTexto("Opcion: ");
      if (opcion == "1")
        registrarPerfil();
      else if (opcion == "2")
        registrarCaracteristicasPunto();
      else if (opcion == "3")
        registrarAccesibilidadConexion();
      else if (opcion != "0")
        cout << "Opcion invalida.\n";
      if (opcion == "0")
        break;
    }
  }
};

} // namespace

void ejecutarFA01(const string &carpetaDatos) {
  RegistroFA01 modulo(carpetaDatos);
  modulo.ejecutar();
}
