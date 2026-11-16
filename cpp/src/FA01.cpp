// FA01 Recomendador explicable de atractivos por perfil. Responsable: Samantha Rodriguez.
// Registra los datos y coordina el acceso al recomendador.

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

  // combina la carpeta de datos con el nombre del csv.
  string rutaArchivo(const string &nombre) const {
    return carpetaDatos + "/" + nombre;
  }

  // lee texto ingresado y evita comas dentro del dato.
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

  // separa una fila csv usando la coma como delimitador.
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

  // lee las filas de datos y omite el encabezado del csv.
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

  // convierte un texto a minusculas para comparar valores.
  string minusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(tolower(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  // convierte codigos a mayusculas antes de buscarlos.
  string mayusculas(const string &texto) const {
    string resultado = texto;
    for (int i = 0; i < static_cast<int>(resultado.size()); ++i) {
      resultado[i] =
          static_cast<char>(toupper(static_cast<unsigned char>(resultado[i])));
    }
    return resultado;
  }

  // quita espacios al inicio y al final de un texto.
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

  // separa valores escritos con punto y coma.
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

  // revisa si un codigo ya aparece en el archivo indicado.
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

  // actualiza una fila existente o agrega una fila nueva.
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
    for (int i = 0; i < static_cast<int>(filas.size()); ++i)
      archivo << filas[i] << "\n";
    archivo.close();
    return !archivo.fail();
  }
  // reemplaza las relaciones anteriores asociadas al codigo.
  bool reemplazarRelaciones(const string &nombre, const string &encabezado,
                            const string &codigo,
                            const vector<string> &valores) const {
    vector<string> filas;
    leerFilas(nombre, filas);
    vector<string> nuevasFilas;
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 0 && campos[0] != codigo)
        nuevasFilas.push_back(filas[i]);
    }
    for (int i = 0; i < static_cast<int>(valores.size()); ++i)
      nuevasFilas.push_back(codigo + "," + valores[i]);

    ofstream archivo(rutaArchivo(nombre).c_str());
    if (!archivo)
      return false;
    archivo << encabezado << "\n";
    for (int i = 0; i < static_cast<int>(nuevasFilas.size()); ++i)
      archivo << nuevasFilas[i] << "\n";
    archivo.close();
    return !archivo.fail();
  }
  // repite la pregunta hasta recibir si o no.
  string leerSiNo(const string &pregunta) const {
    cout << pregunta << "\n"
         << "  1. Si\n"
         << "  2. No\n";
    while (true) {
      string respuesta = minusculas(leerTexto("Selecciona 1 o 2: "));
      if (respuesta == "1" || respuesta == "si")
        return "si";
      if (respuesta == "2" || respuesta == "no")
        return "no";
      cout << "Elige 1 (Si) o 2 (No).\n";
    }
  }
  // comprueba que la hora use el formato hh:mm.
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

  // comprueba si una lista ya contiene el valor.
  bool contiene(const vector<string> &valores, const string &buscado) const {
    for (int i = 0; i < static_cast<int>(valores.size()); ++i) {
      if (valores[i] == buscado)
        return true;
    }
    return false;
  }

  // prepara una lista para mostrarla en pantalla.
  string textoLista(const vector<string> &valores) const {
    if (valores.empty())
      return "ninguno";
    string texto;
    for (int i = 0; i < static_cast<int>(valores.size()); ++i) {
      if (i > 0)
        texto += ", ";
      texto += valores[i];
    }
    return texto;
  }

  // obtiene las categorias registradas para los puntos.
  vector<string> obtenerCategorias() const {
    vector<string> filas;
    vector<string> categorias;
    leerFilas("categorias_puntos.csv", filas);
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 1) {
        string categoria = minusculas(limpiarEspacios(campos[1]));
        if (categoria != "" && !contiene(categorias, categoria))
          categorias.push_back(categoria);
      }
    }
    return categorias;
  }

  // obtiene los intereses que pueden elegirse para un perfil.
  vector<string> obtenerInteresesDisponibles() const {
    vector<string> intereses = obtenerCategorias();
    vector<string> filas;
    leerFilas("intereses_turista.csv", filas);
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 1) {
        string interes = minusculas(limpiarEspacios(campos[1]));
        if (interes != "" && !contiene(intereses, interes))
          intereses.push_back(interes);
      }
    }
    return intereses;
  }

  void mostrarValores(const string &titulo,
                     const vector<string> &valores) const {
    cout << titulo << "\n";
    if (valores.empty()) {
      cout << "  No hay valores registrados.\n";
      return;
    }
    for (int i = 0; i < static_cast<int>(valores.size()); ++i)
      cout << "  - " << valores[i] << "\n";
  }

  // lista los perfiles que ya estan guardados.
  void mostrarPerfilesRegistrados() const {
    vector<string> perfiles;
    vector<string> relaciones;
    leerFilas("perfiles_turista.csv", perfiles);
    leerFilas("intereses_turista.csv", relaciones);
    cout << "\nPerfiles registrados (codigo | movilidad | ninos | hora | intereses):\n";
    if (perfiles.empty()) {
      cout << "  Aun no hay perfiles registrados.\n";
      return;
    }
    for (int i = 0; i < static_cast<int>(perfiles.size()); ++i) {
      vector<string> campos = separarCampos(perfiles[i]);
      if (campos.size() >= 4) {
        vector<string> intereses;
        for (int j = 0; j < static_cast<int>(relaciones.size()); ++j) {
          vector<string> datosInteres = separarCampos(relaciones[j]);
          if (datosInteres.size() > 1 && datosInteres[0] == campos[0])
            intereses.push_back(datosInteres[1]);
        }
        cout << "  " << campos[0] << " | movilidad: " << campos[1]
             << " | ninos: " << campos[2] << " | hora: " << campos[3]
             << " | intereses: " << textoLista(intereses) << "\n";
      }
    }
  }

  // calcula el siguiente codigo disponible para un perfil.
  string generarCodigoPerfil() const {
    vector<string> filas;
    leerFilas("perfiles_turista.csv", filas);
    int mayorNumero = 0;
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() > 0 && campos[0].size() > 1 &&
          campos[0][0] == 'T') {
        int numero = 0;
        bool valido = true;
        for (int j = 1; j < static_cast<int>(campos[0].size()); ++j) {
          if (!isdigit(static_cast<unsigned char>(campos[0][j]))) {
            valido = false;
            break;
          }
          numero = numero * 10 + (campos[0][j] - '0');
        }
        if (valido && numero > mayorNumero)
          mayorNumero = numero;
      }
    }

    string codigo;
    do {
      mayorNumero++;
      string numero = to_string(mayorNumero);
      codigo = "T";
      for (int i = static_cast<int>(numero.size()); i < 3; ++i)
        codigo += "0";
      codigo += numero;
    } while (existeCodigo("perfiles_turista.csv", codigo));
    return codigo;
  }

  string leerCodigoExistente(const string &archivo,
                             const string &mensaje) const {
    while (true) {
      string codigo = mayusculas(leerTexto(mensaje));
      if (codigo == "0")
        return "";
      if (existeCodigo(archivo, codigo))
        return codigo;
      cout << "Ese codigo no existe. Elige uno de la lista o escribe 0 para cancelar.\n";
    }
  }

  // solicita una hora hasta que tenga formato valido.
  string leerHoraValida(const string &mensaje) const {
    while (true) {
      string hora = leerTexto(mensaje);
      if (hora == "")
        return "";
      if (horaValida(hora))
        return hora;
      cout << "Hora invalida. Usa el formato HH:MM, por ejemplo 18:30.\n";
    }
  }

  // lista puntos para que la persona elija uno conocido.
  void mostrarPuntosRegistrados() const {
    vector<string> filas;
    leerFilas("puntos.csv", filas);
    cout << "\nPuntos registrados (codigo | nombre | tipo | zona):\n";
    if (filas.empty()) {
      cout << "  No hay puntos registrados.\n";
      return;
    }
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() >= 4)
        cout << "  " << campos[0] << " | " << campos[1] << " | "
             << campos[2] << " | " << campos[3] << "\n";
    }
  }

  // lista conexiones para que la persona elija una conocida.
  void mostrarConexionesRegistradas() const {
    vector<string> filas;
    leerFilas("conexiones.csv", filas);
    cout << "\nConexiones registradas (codigo | origen -> destino | estado):\n";
    if (filas.empty()) {
      cout << "  No hay conexiones registradas.\n";
      return;
    }
    for (int i = 0; i < static_cast<int>(filas.size()); ++i) {
      vector<string> campos = separarCampos(filas[i]);
      if (campos.size() >= 6)
        cout << "  " << campos[0] << " | " << campos[1] << " -> "
             << campos[2] << " | " << campos[5] << "\n";
    }
  }

  // muestra las categorias permitidas para un punto.
  void mostrarCategorias(const vector<string> &categorias) const {
    mostrarValores("Categorias conocidas (puedes agregar otra):", categorias);
  }

  // muestra intereses que se pueden asignar al perfil.
  void mostrarIntereses(const vector<string> &intereses) const {
    mostrarValores("Intereses sugeridos (puedes agregar otro):", intereses);
  }
  // registra un perfil turistico y sus intereses.
  void registrarPerfil() {
    mostrarPerfilesRegistrados();
    string codigo = generarCodigoPerfil();
    cout << "\nCodigo asignado automaticamente: " << codigo << "\n";

    string movilidad = leerSiNo("Perfil: movilidad reducida?");
    string ninos = leerSiNo("Perfil: viaja con ninos?");
    string hora = leerHoraValida("Hora de visita (ejemplo 18:30): ");
    if (hora == "") {
      cout << "Registro cancelado.\n";
      return;
    }

    vector<string> interesesDisponibles = obtenerInteresesDisponibles();
    mostrarIntereses(interesesDisponibles);
    cout << "Escribe varios intereses separados por punto y coma.\n";
    cout << "Deja el campo vacio si no deseas agregar intereses.\n";
    vector<string> intereses =
        separarLista(leerTexto("Intereses del perfil: "));

    cout << "\n=== Resumen del perfil ===\n"
         << "Codigo: " << codigo << "\n"
         << "Movilidad reducida: " << movilidad << "\n"
         << "Viaja con ninos: " << ninos << "\n"
         << "Hora de visita: " << hora << "\n"
         << "Intereses: " << textoLista(intereses) << "\n";
    if (leerSiNo("Guardar este perfil?") != "si") {
      cout << "Registro cancelado.\n";
      return;
    }

    string fila = codigo + "," + movilidad + "," + ninos + "," + hora;
    if (!guardarFila("perfiles_turista.csv",
                     "codigo,movilidad_reducida,viaja_con_ninos,hora_visita",
                     codigo, fila)) {
      cout << "No se pudo guardar el perfil.\n";
      return;
    }
    if (reemplazarRelaciones("intereses_turista.csv", "codigo,interes",
                             codigo, intereses)) {
      cout << "Perfil e intereses guardados correctamente.\n";
    } else {
      cout << "El perfil se guardo, pero no se pudieron guardar sus intereses.\n";
    }
  }
  // registra accesibilidad, horario, seguridad y categorias.
  void registrarCaracteristicasPunto() {
    mostrarPuntosRegistrados();
    string codigo = leerCodigoExistente(
        "puntos.csv", "Codigo del punto (0 para volver): ");
    if (codigo == "")
      return;

    bool actualizar = existeCodigo("caracteristicas_puntos.csv", codigo);
    if (actualizar)
      cout << "Este punto ya tiene caracteristicas; se actualizaran.\n";

    string movilidad = leerSiNo("Punto: accesible para movilidad reducida?");
    string ninos = leerSiNo("Punto: adecuado para ninos?");
    string seguroDia = leerSiNo("Punto: seguro durante el dia?");
    string seguroNoche = leerSiNo("Punto: seguro durante la noche?");
    string inicio = leerHoraValida("Hora de apertura (ejemplo 09:00): ");
    if (inicio == "") {
      cout << "Registro cancelado.\n";
      return;
    }
    string fin = leerHoraValida("Hora de cierre (ejemplo 18:30): ");
    if (fin == "") {
      cout << "Registro cancelado.\n";
      return;
    }

    vector<string> categoriasDisponibles = obtenerCategorias();
    mostrarCategorias(categoriasDisponibles);
    cout << "Separa varias categorias con punto y coma.\n";
    vector<string> categorias =
        separarLista(leerTexto("Categorias del punto: "));

    cout << "\n=== Resumen del punto " << codigo << " ===\n"
         << "Movilidad reducida: " << movilidad << "\n"
         << "Adecuado para ninos: " << ninos << "\n"
         << "Seguro de dia: " << seguroDia << "\n"
         << "Seguro de noche: " << seguroNoche << "\n"
         << "Horario: " << inicio << " a " << fin << "\n"
         << "Categorias: " << textoLista(categorias) << "\n";
    if (leerSiNo("Guardar estos datos?") != "si") {
      cout << "Registro cancelado.\n";
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
    if (reemplazarRelaciones("categorias_puntos.csv", "codigo,categoria",
                             codigo, categorias)) {
      cout << "Caracteristicas y categorias guardadas correctamente.\n";
    } else {
      cout << "Las caracteristicas se guardaron, pero no se pudieron guardar las categorias.\n";
    }
  }
  // registra si una conexion permite movilidad accesible.
  void registrarAccesibilidadConexion() {
    mostrarConexionesRegistradas();
    string codigo = leerCodigoExistente(
        "conexiones.csv", "Codigo de conexion (0 para volver): ");
    if (codigo == "")
      return;

    bool actualizar = existeCodigo("accesibilidad_conexiones.csv", codigo);
    if (actualizar)
      cout << "Ya hay accesibilidad registrada; se actualizara.\n";

    string accesible = leerSiNo(
        "Conexion: accesible para movilidad reducida?");
    cout << "\n=== Resumen de accesibilidad ===\n"
         << "Conexion: " << codigo << "\n"
         << "Accesible para movilidad reducida: " << accesible << "\n";
    if (leerSiNo("Guardar estos datos?") != "si") {
      cout << "Registro cancelado.\n";
      return;
    }

    string fila = codigo + "," + accesible;
    if (guardarFila("accesibilidad_conexiones.csv",
                    "codigo,accesible_movilidad", codigo, fila)) {
      cout << "Accesibilidad de la conexion guardada correctamente.\n";
    } else {
      cout << "No se pudo guardar la accesibilidad.\n";
    }
  }
public:
  // guarda la carpeta donde se encuentran los csv de fa01.
  explicit RegistroFA01(const string &rutaDatos) : carpetaDatos(rutaDatos) {}

  // muestra las opciones de registro y atiende la seleccion.
  void ejecutar() {
    while (true) {
      cout << "\n=== Datos para el recomendador FA01 ===\n"
           << "1. Crear perfil turistico e intereses\n"
           << "2. Configurar caracteristicas y categorias de un punto\n"
           << "3. Configurar accesibilidad de una conexion\n"
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

// crea el registrador fa01 e inicia su menu.
void ejecutarFA01(const string &carpetaDatos) {
  RegistroFA01 modulo(carpetaDatos);
  modulo.ejecutar();
}
