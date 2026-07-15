# 🛒 POLI POS - Tech Store

Un sistema integral de Punto de Venta (POS) y gestión de inventario desarrollado completamente en **C** nativo. 

## 🚀 Características Principales

* **Gestión de Memoria Dinámica:** Administración de Productos, Clientes, Usuarios y Ventas mediante listas enlazadas simples (nodos) creadas en tiempo de ejecución con malloc y destruidas de forma segura con free.
* **Persistencia de Datos (Archivos Binarios):** El estado del sistema (inventario, usuarios, base de clientes) se congela y recupera automáticamente utilizando archivos .dat mediante fread y fwrite.
* **Generación de Facturas Físicas:** Emisión de tickets de venta detallados en archivos de texto plano (.txt) generados dinámicamente usando fprintf.
* **Algoritmos Avanzados:**
  * **Quicksort:** Implementación de este algoritmo de ordenamiento para generar un ranking en tiempo real de los mejores clientes del local.
  * **Recursividad:** Reportes de cierre de caja (filtros semanales, mensuales y anuales) procesados mediante lógica recursiva.
* **Sistema de Roles:** Autenticación de usuarios con niveles de acceso (Administrador, Supervisor, Vendedor).



  
