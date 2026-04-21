# Sistema de Gestión de Parqueadero

## Integrantes

- Kanny Fernanda Gutiérrez Infante

---

## Descripción General

Este proyecto consiste en el desarrollo de un sistema de gestión de parqueadero en consola.

El sistema controla el ingreso y salida de vehículos, calcula tarifas según el tiempo de permanencia y muestra en tiempo real el estado del parqueadero mediante un mapa visual.

---

## Funcionalidades Principales

- Generación de un mapa de parqueadero de 18x18 posiciones
- Registro de ingreso de vehículos (placa, tipo de vehiculo y hora de entrada)
- Cálculo automático del valor a pagar según tiempo de llegada.
- Visualización en tiempo real de espacios ocupados y disponibles
- Búsqueda de vehículos por placa
- Estadísticas del parqueadero (ocupación e ingresos)

---

## Extras

Para superar los requisitos mínimos, se añadieron las siguientes mejoras:

### 1. Persistencia de datos

El sistema guarda el estado del parqueadero en un archivo (`Informe.txt`), permitiendo recuperar la información incluso después de cerrar el programa.

**¿Por qué?**
Permite simular un sistema real que no pierde información al reiniciarse.

---

### 2. Interfaz visual mejorada

Se implementó el uso de colores en consola para diferenciar:

- Espacios libres
- Espacios ocupados
- Vías
- Entradas y salidas

**¿Por qué?**
Mejora la experiencia del usuario y facilita la lectura del mapa.

---

### 3. Sistema de cobro realista

El cálculo del pago se basa en el tiempo real transcurrido, incluyendo:

- Conversión de segundos a horas
- Tarifa por tipo de vehículo
- Tiempo mínimo de cobro (10 minutos)

---

### 4. Recibo de salida

Al retirar un vehículo, el sistema genera un resumen con:

- Hora de entrada
- Hora de salida
- Tiempo total
- Valor a pagar

---

### 5. Búsqueda por placa

Permite localizar un vehículo dentro del parqueadero y mostrar su información actual.

---

## Estructura del Programa

El sistema está organizado en funciones:

- `mostrarMapa()` → visualización del parqueadero
- `ingresarVehiculo()` → registro de entrada
- `pagarSalida()` → salida y cobro
- `calcularPrecio()` → cálculo de tarifa

Además, se utilizan estructuras (`struct`) para representar los datos del parqueadero y los vehículos.

---

## Tecnologías y conceptos utilizados

- Arreglos y matrices (mapa del parqueadero)
- Punteros para manipulación de estructuras
- Funciones para modularidad
- Manejo de archivos (`ifstream`, `ofstream`)
- Manejo de tiempo (`time_t`, `difftime`)

---

## Cómo ejecutar

1. Compilar el programa:

```
g++ main.cpp -o parqueadero
```

2. Ejecutar:

```
./parqueadero
```
