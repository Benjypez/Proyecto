# 🛒 POLI POS — Tech Store
Sistema de punto de venta desarrollado en **C** para administrar una tienda de productos tecnológicos.

El programa permite gestionar usuarios, productos, clientes, inventario, ventas, facturas y reportes de caja mediante estructuras, listas enlazadas, punteros, memoria dinámica, archivos binarios, recursividad y Quicksort.

---

## 📌 Descripción

**POLI POS** es una aplicación de consola creada como proyecto académico de Programación en C. Su objetivo es simular el funcionamiento básico de un punto de venta para una tienda tecnológica.

El sistema está dividido en funciones especializadas para facilitar la lectura, reutilización y mantenimiento del código.

---

## ✨ Funcionalidades

### 👤 Gestión de usuarios

- Inicio de sesión con usuario y contraseña.
- Manejo de tres roles:
  - **Administrador**
  - **Supervisor**
  - **Vendedor**
- Restricción de opciones según el rol.
- Creación dinámica de nuevos usuarios.

### 📦 Productos e inventario

- Registro de productos.
- Validación de identificadores duplicados.
- Búsqueda por ID o por nombre.
- Actualización de stock.
- Cambio global del porcentaje de IVA.
- Consulta general del inventario.

### 🧾 Ventas y facturación

- Validación del cliente antes de facturar.
- Validación del formato de fecha `DD/MM/AAAA`.
- Verificación de stock disponible.
- Cálculo automático de subtotal, IVA y total final.
- Actualización del inventario después de cada venta.
- Generación de una factura en formato `.txt`.

### 👥 Clientes

- Registro de clientes.
- Validación de cédulas de 10 dígitos.
- Prevención de clientes duplicados.
- Acumulación del valor total comprado.

### 📊 Reportes

- Ranking de clientes mediante **Quicksort**.
- Cierre de caja semanal.
- Cierre de caja mensual.
- Cierre de caja anual.
- Cálculo de cierres mediante funciones recursivas.

### 💾 Persistencia de datos

La información se almacena en archivos binarios:

```text
productos.dat
clientes.dat
ventas.dat
usuarios.dat
```

Al iniciar el sistema, estos archivos son leídos para reconstruir las listas enlazadas.

---


## 🔑 Usuarios de demostración

Cuando no existe un archivo previo de usuarios, el sistema crea automáticamente:

| Usuario | Contraseña | Rol |
|---|---|---|
| `admin` | `123` | Administrador |
| `vende` | `123` | Vendedor |

También se generan clientes y productos de prueba para facilitar la demostración.

---


## 🧾 Ejemplo de factura generada

```text
=== POLI POS ===
Factura: 1001 | Fecha: 14/07/2026
Cliente: Juan Perez
--------------------
1x Laptop Gamer Gigabyte - $1200.00
2x Mouse Inalambrico Logitech - $50.00
--------------------
SUBTOTAL: $1250.00
TOTAL IVA: $187.50
TOTAL FINAL: $1437.50
```

El nombre del archivo sigue el formato:

```text
Factura_<numero>.txt
```

---

## 🔄 Recursividad en los cierres

Las funciones de cierre recorren la lista de ventas nodo por nodo:

```c
float sumaAnual(Venta* n, int anioBuscado);
float sumaMensual(Venta* n, int mesBuscado, int anioBuscado);
float sumaSemanal(Venta* n, int semBuscada, int mesBuscado, int anioBuscado);
```

Cada función tiene:

- un **caso base**, cuando el nodo es `NULL`;
- una llamada recursiva al siguiente nodo;
- una condición para sumar únicamente las ventas del período solicitado.

---

## 🏆 Ranking de clientes

El ranking convierte temporalmente la lista enlazada en un arreglo de punteros y lo ordena mediante **Quicksort**.

El criterio de ordenamiento es:

```text
Mayor total comprado → menor total comprado
```

De esta manera, los mejores clientes aparecen en las primeras posiciones.

---

