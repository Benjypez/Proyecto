#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOM 50
#define MAX_CED 15

/* --- ESTRUCTURAS --- */
typedef struct Usuario {
    char user[20];
    char pass[20];
    int rol; // 1: Admin, 2: Supervisor, 3: Vendedor
    struct Usuario* sig;
} Usuario;

typedef struct Producto {
    int id;
    char nombre[MAX_NOM];
    float precio;
    int stock;
    int iva; 
    struct Producto* sig; 
} Producto;

typedef struct Cliente {
    char cedula[MAX_CED];
    char nombre[MAX_NOM];
    float totalComprado; 
    struct Cliente* sig;
} Cliente;

typedef struct Venta {
    int id;
    char fecha[15]; // Formato DD/MM/AAAA
    char cedulaCli[MAX_CED];
    float subtotal;
    float totalIva;
    float totalFinal;
    struct Venta* sig;
} Venta;

/* --- PROTOTIPOS --- */
void cargarDatos(Producto** p, Cliente** c, Venta** v, Usuario** u);
void guardarDatos(Producto* p, Cliente* c, Venta* v, Usuario* u);
void datosDemo(Producto** p, Cliente** c, Usuario** u);
void limpiarMemoria(Producto* p, Cliente* c, Venta* v, Usuario* u);

void leerCedula(char* buffer);
void aMinusculas(char* cadena);
Producto* buscarProducto(Producto* cabeza);
Cliente* buscarCliente(Cliente* cabeza, char* cedula);

Usuario* login(Usuario* cabeza);
void crearProducto(Producto** cabeza);
void menuUsuarios(Usuario** cabeza, int rolActual);
void menuInventario(Producto* cabeza, int rolActual);
void menuVentas(Venta** ventas, Cliente* clientes, Producto* productos);
void menuReportes(Cliente* clientes, Producto* productos, Venta* ventas);

// Funciones Recursivas de Cierre de Caja
float sumaAnual(Venta* n, int anioBuscado);
float sumaMensual(Venta* n, int mesBuscado, int anioBuscado);
float sumaSemanal(Venta* n, int semBuscada, int mesBuscado, int anioBuscado);

void ordenarClientes(Cliente** arr, int low, int high);

/* --- MAIN --- */
int main() {
    Producto* listaProductos = NULL; 
    Cliente* listaClientes = NULL;
    Venta* listaVentas = NULL;
    Usuario* listaUsuarios = NULL;

    cargarDatos(&listaProductos, &listaClientes, &listaVentas, &listaUsuarios);
    datosDemo(&listaProductos, &listaClientes, &listaUsuarios);

    Usuario* usrActual = NULL;
    int opcion;

    do {
        system("cls");
        printf("=== POLI POS - TECH STORE ===\n");
        usrActual = login(listaUsuarios);
        if(!usrActual) {
            printf("Error: Usuario o clave incorrecta.\n");
            system("pause");
        }
    } while (!usrActual);

    do {
        system("cls");
        printf("--- MENU PRINCIPAL | USER: %s | ROL: %d ---\n", usrActual->user, usrActual->rol);
        printf("1. Crear Producto\n");
        printf("2. Inventario e IVA Global\n");
        printf("3. Caja (Facturar)\n");
        printf("4. Registrar Cliente\n");
        printf("5. Reportes y Cierres de Caja\n");
        printf("6. Gestionar Usuarios\n");
        printf("7. Salir\n");
        printf("Opcion: ");
        
        scanf("%d", &opcion);
        while (getchar() != '\n'); 

        if (usrActual->rol == 3 && opcion != 3 && opcion != 7) {
            printf("Acceso Denegado: Solo tienes permisos de Ventas.\n");
            system("pause");
            continue;
        }

        switch(opcion) {
            case 1:
                crearProducto(&listaProductos);
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                system("pause");
                break;
            case 2:
                menuInventario(listaProductos, usrActual->rol);
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                system("pause");
                break;
            case 3:
                menuVentas(&listaVentas, listaClientes, listaProductos);
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                system("pause");
                break;
            case 4: {
                Cliente* c = (Cliente*)malloc(sizeof(Cliente));
                printf("\n--- NUEVO CLIENTE ---\n");
                leerCedula(c->cedula);
                if (buscarCliente(listaClientes, c->cedula)) {
                    printf("Error: Cliente ya existe.\n"); 
                    free(c);
                } else {
                    printf("Nombre: "); fgets(c->nombre, MAX_NOM, stdin); c->nombre[strcspn(c->nombre, "\n")] = 0;
                    c->totalComprado = 0.0f; c->sig = listaClientes; listaClientes = c;
                    printf("Cliente guardado.\n");
                }
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                system("pause");
                break;
            }
            case 5:
                menuReportes(listaClientes, listaProductos, listaVentas);
                system("pause");
                break;
            case 6:
                menuUsuarios(&listaUsuarios, usrActual->rol);
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                system("pause");
                break;
            case 7:
                guardarDatos(listaProductos, listaClientes, listaVentas, listaUsuarios);
                limpiarMemoria(listaProductos, listaClientes, listaVentas, listaUsuarios);
                printf("Saliendo del sistema. !Hasta luego!\n");
                break;
            default:
                printf("Opcion invalida.\n"); 
                system("pause");
        }
    } while (opcion != 7);

    return 0;
}

/* --- LOGICA ROLES Y USUARIOS --- */
Usuario* login(Usuario* cabeza) {
    char u[20], p[20];
    printf("Usuario: "); fgets(u, 20, stdin); u[strcspn(u, "\n")] = 0;
    printf("Clave: "); fgets(p, 20, stdin); p[strcspn(p, "\n")] = 0;
    
    Usuario* aux = cabeza;
    while(aux) {
        if(strcmp(aux->user, u) == 0 && strcmp(aux->pass, p) == 0) return aux;
        aux = aux->sig;
    }
    return NULL;
}

void menuUsuarios(Usuario** cabeza, int rolActual) {
    if (rolActual == 3) { printf("Vendedores no pueden crear usuarios.\n"); return; }
    
    Usuario* nuevo = (Usuario*)malloc(sizeof(Usuario));
    printf("\n--- CREAR USUARIO ---\nNombre: "); 
    fgets(nuevo->user, 20, stdin); nuevo->user[strcspn(nuevo->user, "\n")] = 0;
    printf("Clave: "); 
    fgets(nuevo->pass, 20, stdin); nuevo->pass[strcspn(nuevo->pass, "\n")] = 0;
    
    if (rolActual == 1) {
        printf("Rol (1:Admin, 2:Super, 3:Vendedor): ");
        scanf("%d", &nuevo->rol); while(getchar()!='\n');
    } else {
        printf("Asignando rol de Vendedor por defecto.\n");
        nuevo->rol = 3;
    }
    nuevo->sig = *cabeza; *cabeza = nuevo;
    printf("Usuario creado.\n");
}

/* --- UTILIDADES Y BUSQUEDAS --- */
void leerCedula(char* buffer) {
    int ok = 0;
    do {
        printf("Cedula (10 digitos exactos): ");
        fgets(buffer, MAX_CED, stdin);
        
        if (strchr(buffer, '\n') == NULL) while (getchar() != '\n'); 
        else buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) != 10) {
            printf("Error: Deben ser 10 numeros (ingreso %zu).\n", strlen(buffer)); continue;
        }
        ok = 1;
        for (int i = 0; i < 10; i++) {
            if (!isdigit(buffer[i])) {
                printf("Error: Solo se permiten numeros.\n"); ok = 0; break;
            }
        }
    } while (!ok);
}

void aMinusculas(char* cad) {
    for(int i = 0; cad[i]; i++) cad[i] = tolower(cad[i]);
}

Producto* buscarProducto(Producto* cabeza) {
    int tipo;
    printf("Buscar por [1] ID o [2] Nombre: ");
    scanf("%d", &tipo); while(getchar()!='\n');

    Producto* aux = cabeza;
    if (tipo == 1) {
        int id; printf("ID: "); scanf("%d", &id); while(getchar()!='\n');
        while (aux) { if (aux->id == id) return aux; aux = aux->sig; }
    } else {
        char nom[MAX_NOM], actual[MAX_NOM];
        printf("Nombre (o parte): "); fgets(nom, MAX_NOM, stdin); nom[strcspn(nom, "\n")] = 0;
        aMinusculas(nom);
        while (aux) {
            strcpy(actual, aux->nombre); aMinusculas(actual);
            if (strstr(actual, nom)) return aux;
            aux = aux->sig;
        }
    }
    return NULL;
}

Cliente* buscarCliente(Cliente* cabeza, char* ced) {
    Cliente* aux = cabeza;
    while (aux) {
        if (strcmp(aux->cedula, ced) == 0) return aux;
        aux = aux->sig;
    }
    return NULL;
}

/* --- MODULO PRODUCTOS E INVENTARIO --- */
void crearProducto(Producto** cabeza) {
    Producto* p = (Producto*)malloc(sizeof(Producto));
    if (!p) return;
    
    printf("\n--- NUEVO PRODUCTO ---\n");
    printf("ID del producto (Num): "); scanf("%d", &p->id); while(getchar()!='\n');
    
    Producto* aux = *cabeza;
    while(aux) {
        if (aux->id == p->id) {
            printf("Error: Ya existe un producto con ID %d.\n", p->id);
            free(p); return;
        }
        aux = aux->sig;
    }

    printf("Nombre: "); fgets(p->nombre, MAX_NOM, stdin); p->nombre[strcspn(p->nombre, "\n")] = 0;
    printf("Precio: $"); scanf("%f", &p->precio); while(getchar()!='\n');
    printf("Stock inicial: "); scanf("%d", &p->stock); while(getchar()!='\n');
    printf("IVA %% (Ej: 0, 12, 15): "); scanf("%d", &p->iva); while(getchar()!='\n');
    
    p->sig = *cabeza; *cabeza = p;
    printf("Producto creado exitosamente.\n");
}

void menuInventario(Producto* cabeza, int rolActual) {
    if (!cabeza) return;
    int op;
    printf("\n1. Ajustar Stock\n2. Cambiar IVA Global de la Tienda\nOpcion: "); 
    scanf("%d", &op); while(getchar()!='\n');

    if (op == 1) {
        Producto* p = buscarProducto(cabeza);
        if (p) {
            int cant = 0;
            printf("Producto: %s | Stock: %d\nAjuste (Ej: 5 para sumar, -3 para restar): ", p->nombre, p->stock);
            if (scanf("%d", &cant) != 1) {
                printf("Error: Debes ingresar un valor numerico.\n");
            } else {
                p->stock += cant;
                if(p->stock < 0) p->stock = 0;
                printf("Stock actualizado a %d.\n", p->stock);
            }
            while (getchar() != '\n');
        } else printf("No encontrado.\n");
    } else if (op == 2) {
        if (rolActual == 3) { printf("No tienes permiso.\n"); return; }
        int nIva; printf("Nuevo IVA %% para todos los productos: "); scanf("%d", &nIva); while(getchar()!='\n');
        Producto* aux = cabeza;
        while(aux) { aux->iva = nIva; aux = aux->sig; }
        printf("IVA actualizado globalmente al %d%%.\n", nIva);
    }
}

/* --- VENTAS --- */
void menuVentas(Venta** ventas, Cliente* clientes, Producto* productos) {
    char ced[MAX_CED];
    printf("\n--- NUEVA FACTURA ---\n");
    leerCedula(ced); 

    Cliente* c = buscarCliente(clientes, ced);
    if (!c) { printf("Cliente no registrado en el sistema. Registrelo primero.\n"); return; }

    Venta* v = (Venta*)malloc(sizeof(Venta));
    strcpy(v->cedulaCli, c->cedula);
    printf("Nro de Factura: "); scanf("%d", &v->id); while (getchar() != '\n');
    
    // --- VALIDACION ESTRICTA DE FECHA ---
    int fechaValida = 0;
    do {
        printf("Fecha (DD/MM/AAAA): ");
        fgets(v->fecha, 15, stdin);
        v->fecha[strcspn(v->fecha, "\n")] = 0;

        if (strlen(v->fecha) == 10 && v->fecha[2] == '/' && v->fecha[5] == '/') {
            int d, m, a;
            if (sscanf(v->fecha, "%d/%d/%d", &d, &m, &a) == 3) {
                if (d >= 1 && d <= 31 && m >= 1 && m <= 12 && a >= 2000 && a <= 2100) {
                    fechaValida = 1;
                }
            }
        }
        if (!fechaValida) {
            printf("Error: Formato invalido. Use exactamente DD/MM/AAAA (Ej: 14/07/2026).\n");
        }
    } while (!fechaValida);
    // ------------------------------------

    v->subtotal = 0.0f; v->totalIva = 0.0f;
    
    char nombreTxt[50]; sprintf(nombreTxt, "Factura_%d.txt", v->id);
    FILE* ticket = fopen(nombreTxt, "w");
    if(ticket) {
        fprintf(ticket, "=== POLI POS ===\nFactura: %d | Fecha: %s\nCliente: %s\n", v->id, v->fecha, c->nombre);
        fprintf(ticket, "--------------------\n");
    }

    while (1) {
        printf("\n[0] Finalizar e Imprimir Factura | [1] Buscar Articulo: ");
        int accion; scanf("%d", &accion); while(getchar()!='\n');
        if (accion == 0) break;

        Producto* p = buscarProducto(productos);
        if (!p) { printf("Producto no encontrado.\n"); continue; }

        int cant;
        printf("Cantidad de %s (Stock: %d): ", p->nombre, p->stock);
        scanf("%d", &cant); while (getchar() != '\n');

        if (cant > p->stock || cant <= 0) printf("Stock insuficiente o cantidad invalida.\n");
        else {
            p->stock -= cant; 
            float sub = p->precio * cant;
            float iva = sub * (p->iva / 100.0f);
            
            v->subtotal += sub; v->totalIva += iva;
            printf("Agregado %d unidades de %s a la factura.\n", cant, p->nombre);
            
            if(ticket) fprintf(ticket, "%dx %s - $%.2f\n", cant, p->nombre, sub);
        }
    }

    v->totalFinal = v->subtotal + v->totalIva;
    c->totalComprado += v->totalFinal;

    if(ticket) {
        fprintf(ticket, "--------------------\nSUBTOTAL: $%.2f\nTOTAL IVA: $%.2f\nTOTAL FINAL: $%.2f\n", v->subtotal, v->totalIva, v->totalFinal);
        fclose(ticket);
        printf("\nTicket generado exitosamente: %s\n", nombreTxt);
    }

    v->sig = *ventas; *ventas = v;
}

/* --- REPORTES Y CIERRES FILTRADOS --- */
void menuReportes(Cliente* cl, Producto* pr, Venta* vt) {
    int op;
    printf("\n--- REPORTES AVANZADOS ---\n");
    printf("1. Ranking Clientes (Quicksort)\n");
    printf("2. Cierre de Caja con Filtros (Recursividad)\n");
    printf("3. Inventario General\nOpcion: ");
    scanf("%d", &op); while (getchar() != '\n'); 

    if (op == 1) {
        int c = 0; Cliente* aux = cl;
        while (aux) { c++; aux = aux->sig; }
        if (c == 0) { printf("No hay clientes.\n"); return; }

        Cliente** arr = (Cliente**)malloc(c * sizeof(Cliente*));
        aux = cl; for (int i=0; i<c; i++) { arr[i] = aux; aux = aux->sig; }
        
        ordenarClientes(arr, 0, c - 1);

        printf("\n--- MEJORES CLIENTES ---\n");
        for (int i = 0; i < c; i++) printf("%d. %s - CI: %s - Compras: $%.2f\n", i+1, arr[i]->nombre, arr[i]->cedula, arr[i]->totalComprado);
        free(arr); 
    } 
    else if (op == 2) {
        int tipoFiltro;
        printf("\n--- SELECCIONE EL TIPO DE CIERRE DE CAJA ---\n");
        printf("1. Cierre Semanal\n2. Cierre Mensual\n3. Cierre Anual\nOpcion: ");
        scanf("%d", &tipoFiltro); while(getchar()!='\n');

        float totalFiltrado = 0.0f;

        if (tipoFiltro == 1) {
            int sem, mes, anio;
            printf("Ingrese el Anio (AAAA): "); scanf("%d", &anio);
            printf("Ingrese el Mes (1-12): "); scanf("%d", &mes);
            printf("Ingrese la Semana (1 a 4): "); scanf("%d", &sem); while(getchar()!='\n');
            
            totalFiltrado = sumaSemanal(vt, sem, mes, anio);
            
            if (totalFiltrado == 0.0f) printf("\n[AVISO] No se encontraron facturas en la semana %d de %d/%d.\n", sem, mes, anio);
            else printf("\n--- CIERRE SEMANAL ---\nTotal de la semana %d: $%.2f\n", sem, totalFiltrado);
        } 
        else if (tipoFiltro == 2) {
            int mes, anio;
            printf("Ingrese el Anio (AAAA): "); scanf("%d", &anio);
            printf("Ingrese el Mes (1-12): "); scanf("%d", &mes); while(getchar()!='\n');
            
            totalFiltrado = sumaMensual(vt, mes, anio);
            
            if (totalFiltrado == 0.0f) printf("\n[AVISO] No se encontraron facturas en el periodo %d/%d.\n", mes, anio);
            else printf("\n--- CIERRE MENSUAL ---\nTotal facturado en el mes %d/%d: $%.2f\n", mes, anio, totalFiltrado);
        } 
        else if (tipoFiltro == 3) {
            int anio;
            printf("Ingrese el Anio a consultar (AAAA): "); scanf("%d", &anio); while(getchar()!='\n');
            
            totalFiltrado = sumaAnual(vt, anio);
            
            if (totalFiltrado == 0.0f) printf("\n[AVISO] No se encontraron facturas registradas en el anio %d.\n", anio);
            else printf("\n--- CIERRE ANUAL ---\nTotal facturado en el anio %d: $%.2f\n", anio, totalFiltrado);
        }
    }
    else if (op == 3) {
        printf("\n--- INVENTARIO ---\n");
        Producto* aux = pr;
        while(aux) {
            printf("[%d] %s - Stock: %d - $%.2f - IVA: %d%%\n", aux->id, aux->nombre, aux->stock, aux->precio, aux->iva);
            aux = aux->sig;
        }
    }
}

/* --- LOGICA RECURSIVA CON FILTROS --- */
float sumaAnual(Venta* n, int anioBuscado) {
    if (!n) return 0.0f; 
    int d, m, a;
    sscanf(n->fecha, "%d/%d/%d", &d, &m, &a); 
    
    if (a == anioBuscado) {
        return n->totalFinal + sumaAnual(n->sig, anioBuscado); 
    }
    return sumaAnual(n->sig, anioBuscado); 
}

float sumaMensual(Venta* n, int mesBuscado, int anioBuscado) {
    if (!n) return 0.0f;
    int d, m, a;
    sscanf(n->fecha, "%d/%d/%d", &d, &m, &a);
    
    if (m == mesBuscado && a == anioBuscado) {
        return n->totalFinal + sumaMensual(n->sig, mesBuscado, anioBuscado);
    }
    return sumaMensual(n->sig, mesBuscado, anioBuscado);
}

float sumaSemanal(Venta* n, int semBuscada, int mesBuscado, int anioBuscado) {
    if (!n) return 0.0f;
    int d, m, a;
    sscanf(n->fecha, "%d/%d/%d", &d, &m, &a);
    
    int semActual = (d - 1) / 7 + 1;
    if (semActual > 4) semActual = 4; 
    
    if (semActual == semBuscada && m == mesBuscado && a == anioBuscado) {
        return n->totalFinal + sumaSemanal(n->sig, semBuscada, mesBuscado, anioBuscado);
    }
    return sumaSemanal(n->sig, semBuscada, mesBuscado, anioBuscado);
}

/* --- ARCHIVOS Y PRECARGA --- */
void guardarDatos(Producto* p, Cliente* c, Venta* v, Usuario* u) {
    FILE* f;
    f = fopen("productos.dat", "wb"); while(p) { fwrite(p, sizeof(Producto), 1, f); p = p->sig; } if(f) fclose(f);
    f = fopen("clientes.dat", "wb"); while(c) { fwrite(c, sizeof(Cliente), 1, f); c = c->sig; } if(f) fclose(f);
    f = fopen("ventas.dat", "wb"); while(v) { fwrite(v, sizeof(Venta), 1, f); v = v->sig; } if(f) fclose(f);
    f = fopen("usuarios.dat", "wb"); while(u) { fwrite(u, sizeof(Usuario), 1, f); u = u->sig; } if(f) fclose(f);
}

void cargarDatos(Producto** p, Cliente** c, Venta** v, Usuario** u) {
    FILE* f;
    f = fopen("productos.dat", "rb"); if(f) { Producto t; while(fread(&t, sizeof(Producto), 1, f)) { Producto* n = malloc(sizeof(Producto)); *n=t; n->sig=*p; *p=n; } fclose(f); }
    f = fopen("clientes.dat", "rb"); if(f) { Cliente t; while(fread(&t, sizeof(Cliente), 1, f)) { Cliente* n = malloc(sizeof(Cliente)); *n=t; n->sig=*c; *c=n; } fclose(f); }
    f = fopen("ventas.dat", "rb"); if(f) { Venta t; while(fread(&t, sizeof(Venta), 1, f)) { Venta* n = malloc(sizeof(Venta)); *n=t; n->sig=*v; *v=n; } fclose(f); }
    f = fopen("usuarios.dat", "rb"); if(f) { Usuario t; while(fread(&t, sizeof(Usuario), 1, f)) { Usuario* n = malloc(sizeof(Usuario)); *n=t; n->sig=*u; *u=n; } fclose(f); }
}

void datosDemo(Producto** p, Cliente** c, Usuario** u) {
    if (*u == NULL) {
        printf("Generando datos de prueba para la exposicion...\n");
        Usuario* u1 = malloc(sizeof(Usuario)); strcpy(u1->user, "admin"); strcpy(u1->pass, "123"); u1->rol = 1; u1->sig = *u; *u = u1;
        Usuario* u2 = malloc(sizeof(Usuario)); strcpy(u2->user, "vende"); strcpy(u2->pass, "123"); u2->rol = 3; u2->sig = *u; *u = u2;

        Cliente* c1 = malloc(sizeof(Cliente)); strcpy(c1->cedula, "9999999999"); strcpy(c1->nombre, "Consumidor Final"); c1->totalComprado = 0; c1->sig = *c; *c = c1;
        Cliente* c2 = malloc(sizeof(Cliente)); strcpy(c2->cedula, "1711111111"); strcpy(c2->nombre, "Juan Perez"); c2->totalComprado = 0; c2->sig = *c; *c = c2;

        char* noms[] = { "Laptop Gamer Gigabyte", "Mando PlayStation 4", "Tableta Grafica Ugee", "Mouse Inalambrico Logitech", "Monitor Asus 144Hz" };
        float pres[] = { 1200.0f, 65.0f, 45.0f, 25.0f, 250.0f };
        for(int i=0; i<5; i++){
            Producto* pr = malloc(sizeof(Producto));
            pr->id = i+1; strcpy(pr->nombre, noms[i]); pr->precio = pres[i]; pr->stock = 15; pr->iva = 15; pr->sig = *p; *p = pr;
        }
        guardarDatos(*p, *c, NULL, *u);
    }
}

/* --- ORDENAMIENTO (QUICKSORT) --- */
void intercambiar(Cliente** a, Cliente** b) { Cliente* t = *a; *a = *b; *b = t; }

int partirArreglo(Cliente** arr, int low, int high) {
    float piv = arr[high]->totalComprado; int i = (low - 1);
    for (int j = low; j < high; j++) { if (arr[j]->totalComprado >= piv) { i++; intercambiar(&arr[i], &arr[j]); } }
    intercambiar(&arr[i + 1], &arr[high]); return (i + 1);
}

void ordenarClientes(Cliente** arr, int low, int high) {
    if (low < high) { 
        int pi = partirArreglo(arr, low, high); 
        ordenarClientes(arr, low, pi - 1); 
        ordenarClientes(arr, pi + 1, high); 
    }
}

void limpiarMemoria(Producto* p, Cliente* c, Venta* v, Usuario* u) {
    while(p) { Producto* t = p; p = p->sig; free(t); }
    while(c) { Cliente* t = c; c = c->sig; free(t); }
    while(v) { Venta* t = v; v = v->sig; free(t); }
    while(u) { Usuario* t = u; u = u->sig; free(t); }
}