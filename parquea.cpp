#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>

using namespace std;

const int FILAS = 18;
const int COLS = 18;
const int MAX_CUPOS = 50;

// Precios de pago
const int PAGO_CARRO = 5000;
const int PAGO_MOTO  = 2500;
const int PAGO_BICI  = 1000;

const int TIPO_CARRO = 1;
const int TIPO_MOTO  = 2;
const int TIPO_BICI  = 3;

struct Puesto {
    int    esta_ocupado;
    int    tipo_vehiculo;
    string placa;
    time_t hora_entrada;
    string hora_entrada_texto;
    int    fila;
    int    col;
};

struct Parqueadero {
    Puesto puestos[MAX_CUPOS];
    int    cupos_totales;
    int    carros_actuales;
    int    motos_actuales;
    int    bicis_actuales;
};

// El mapa del lugar
char mapa[FILAS][COLS] = {
    {'W','W','E','W','W','W','W','W','W','W','W','W','W','W','W','S','W','W'},
    {'W','X','V','X','X','X','X','X','X','X','X','X','X','X','X','V','X','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','C','V','C','C','V','C','C','V','C','C','V','C','C','V','C','C','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','M','V','M','M','V','M','M','V','M','M','V','M','M','V','M','M','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','B','V','B','B','V','B','B','V','B','B','V','B','B','V','B','B','W'},
    {'W','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','V','W'},
    {'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'},
};

void limpiarPantalla() { cin.ignore(1000, '\n'); }

string obtenerHora(time_t t) {
    struct tm *info = localtime(&t);
    char buffer[30];
    sprintf(buffer, "%02d:%02d:%02d", info->tm_hour, info->tm_min, info->tm_sec);
    return string(buffer);
}

void inicializarParqueadero(Parqueadero *p) {
    p->cupos_totales = 0;
    p->carros_actuales = 0;
    p->motos_actuales = 0;
    p->bicis_actuales = 0;

    int contador = 0;
    for (int i = 0; i < FILAS && contador < MAX_CUPOS; i++) {
        for (int j = 0; j < COLS && contador < MAX_CUPOS; j++) {
            char letra = mapa[i][j];
            if (letra == 'C' || letra == 'M' || letra == 'B') {
                Puesto *espacio = &p->puestos[contador];
                espacio->esta_ocupado = 0;
                
                if (letra == 'C') {
                    espacio->tipo_vehiculo = TIPO_CARRO;
                } else if (letra == 'M') {
                    espacio->tipo_vehiculo = TIPO_MOTO;
                } else {
                    espacio->tipo_vehiculo = TIPO_BICI;
                }

                espacio->placa = "";
                espacio->fila = i;
                espacio->col = j;
                contador++;
                p->cupos_totales++;
            }
        }
    }
}

double calcularPrecio(int tipo, time_t entrada, time_t salida) {
    double segundos = difftime(salida, entrada);
    double horas = segundos / 3600.0;
    
    // Si entró hace nada, se cobra al menos un cuarto de hora
    if (horas < 0.25) {
        horas = 0.25;
    }

    double tarifa;
    if (tipo == TIPO_CARRO) {
        tarifa = PAGO_CARRO;
    } else if (tipo == TIPO_MOTO) {
        tarifa = PAGO_MOTO;
    } else {
        tarifa = PAGO_BICI;
    }
    
    return horas * tarifa;
}

void ingresarVehiculo(Parqueadero *p) {
    int tipo;
    cout << "\n¿Qué vehículo ingresa? (1. Carro, 2. Moto, 3. Bici): ";
    cin >> tipo; limpiarPantalla();

    int indice = -1;
    for (int i = 0; i < p->cupos_totales; i++) {
        if (p->puestos[i].esta_ocupado == 0 && p->puestos[i].tipo_vehiculo == tipo) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        cout << "No hay puestos libres para ese tipo de vehículo.\n";
        return;
    }

    cout << "Escriba la placa: ";
    string placa;
    cin >> placa; limpiarPantalla();

    Puesto *lugar = &p->puestos[indice];
    lugar->esta_ocupado = 1;
    lugar->placa = placa;
    lugar->hora_entrada = time(nullptr);
    lugar->hora_entrada_texto = obtenerHora(lugar->hora_entrada);
    mapa[lugar->fila][lugar->col] = 'O'; // O de Ocupado

    if (tipo == TIPO_CARRO) p->carros_actuales++;
    else if (tipo == TIPO_MOTO) p->motos_actuales++;
    else p->bicis_actuales++;

    cout << "Vehículo registrado correctamente.\n";
}

void pagarSalida(Parqueadero *p) {
    cout << "Ingrese la placa del vehículo que sale: ";
    string placa_buscar;
    cin >> placa_buscar; limpiarPantalla();

    for (int i = 0; i < p->cupos_totales; i++) {
        Puesto *lugar = &p->puestos[i];
        if (lugar->esta_ocupado == 1 && lugar->placa == placa_buscar) {
            time_t ahora = time(nullptr);
            double total = calcularPrecio(lugar->tipo_vehiculo, lugar->hora_entrada, ahora);

            cout << "\n--- RECIBO DE PAGO ---" << endl;
            cout << "Placa: " << lugar->placa << endl;
            cout << "Entrada: " << obtenerHora(lugar->hora_entrada) << endl;
            cout << "Salida: " << obtenerHora(ahora) << endl;
            cout << "TOTAL A PAGAR: $" << fixed << setprecision(0) << total << " pesos." << endl;
            cout << "----------------------" << endl;

            // Dejar el mapa como estaba antes
            if (lugar->tipo_vehiculo == TIPO_CARRO) {
                mapa[lugar->fila][lugar->col] = 'C';
                p->carros_actuales--;
            } else if (lugar->tipo_vehiculo == TIPO_MOTO) {
                mapa[lugar->fila][lugar->col] = 'M';
                p->motos_actuales--;
            } else {
                mapa[lugar->fila][lugar->col] = 'B';
                p->bicis_actuales--;
            }

            lugar->esta_ocupado = 0;
            return;
        }
    }
    cout << "No se encontró ningún vehículo con esa placa.\n";
}

int main() {
    Parqueadero miParqueadero;
    inicializarParqueadero(&miParqueadero);

    int opcion = -1;
    while (opcion != 0) {
        cout << "\n--- CONTROL DEL PARQUEADERO ---" << endl;
        cout << "1. Ver el mapa\n";
        cout << "2. Registrar entrada\n";
        cout << "3. Registrar salida y pago\n";
        cout << "0. Salir del programa\n";
        cout << "Elija una opción: ";
        cin >> opcion; limpiarPantalla();

        if (opcion == 1) {
            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLS; j++) cout << mapa[i][j] << " ";
                cout << "\n";
            }
        } 
        else if (opcion == 2) ingresarVehiculo(&miParqueadero);
        else if (opcion == 3) pagarSalida(&miParqueadero);
    }

    cout << "Cerrando el sistema. ¡Buen día!\n";
    return 0;
}
