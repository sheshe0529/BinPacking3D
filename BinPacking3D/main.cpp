#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
#include <fstream>
#include <iostream>
using namespace std;
#define M 9
#define NVEH 5
#define NDESTI 100

int inda = 0;

struct Paquete{
    int ancho;
    int largo;
    int alto;
    double pesoPaquete;
    bool rotado;        // Si ha sido rotado para ingresar
    int id;
};

struct Vehiculo{
    int largo;  // Largo estático/real
    int ancho;  // Ancho estático/real
    int alto;   // Alto estático/real
    double pesoMax; // Máximo peso que puede cargar el vehículo
    int ACVehiculo; // Ancho restante del vehículo
    int LCVehiculo; // Largo restante del vehículo
    double pesoAcum;    // Peso acumulado con todos los paquetes
    int distancia;
    int trafico;
    vector<Paquete> paquetes;   // Paquetes cargados en el vehículo
};

void leerArchivoProductos(const char *nombArch, vector<Paquete> &LPaquetes){
    ifstream arch(nombArch, ios::in);
    if(not arch.is_open()){
        cout<<"Error: No se pudo abrir el archivo "<<nombArch<<endl;
        exit(true);
    }
    Paquete paq;
    srand(time(NULL));
    int cont = 1;
    while(true){
        arch>>paq.largo;
        if(arch.eof())break;
        arch>>paq.ancho;
        arch>>paq.alto;
        arch>>paq.pesoPaquete;
        paq.rotado = false;
        paq.id = cont;
        LPaquetes.push_back(paq);
        cont++;
    }
}

void leerArchivoVehiculos(const char *nombArch, Vehiculo &vehiculo){
    ifstream arch(nombArch, ios::in);
    if(not arch.is_open()){
        cout<<"Error: No se pudo abrir el archivo "<<nombArch<<endl;
        exit(true);
    }

    while(true){
        arch>>vehiculo.largo;
        if(arch.eof())break;
        arch>>vehiculo.ancho;
        arch>>vehiculo.alto;
        arch>>vehiculo.pesoMax;
        vehiculo.pesoAcum = 0;
        vehiculo.ACVehiculo = vehiculo.ancho;
        vehiculo.LCVehiculo = vehiculo.largo;
    }
}

void imprimirSolucion(Vehiculo vehiculo){
//    cout<<"Vehiculo obtenido: "<<endl;
//    cout<<"Peso acumulado: "<<vehiculo.pesoAcum
//            <<" Capacidad maxima del vehiculo: "<<vehiculo.pesoMax<<endl;
//    cout<<"Lista de paquetes del vehiculo: "<<endl;
//    for(Paquete paq : vehiculo.paquetes){
//        cout<<paq.pesoPaquete<<" ";
//    }
//    cout<<endl;
    double porcentajeLleno = (vehiculo.pesoAcum / vehiculo.pesoMax) * 100;
    double volVehiculo = (double)vehiculo.alto*vehiculo.ancho*vehiculo.largo;
    double volPaquetes = 0;
    for(Paquete paq: vehiculo.paquetes)
        volPaquetes += paq.alto*paq.ancho*paq.largo;
    // Imprimimos información general del vehículo
    cout << "==================== REPORTE DEL VEHÍCULO ====================\n";
    cout << "Capacidad máxima: " << vehiculo.pesoMax << " kg" << endl;
    cout << "Peso acumulado: " << vehiculo.pesoAcum << " kg" << endl;
    cout << "Volumen del Vehículo: " << volVehiculo << " m^3" << endl;
    cout << "Volumen acumulado: " << volPaquetes << " m^3" << endl;
    cout << "Dimensiones Vehículo (Largo/Ancho/Alto): " << vehiculo.largo << " x " << vehiculo.ancho << " x " << vehiculo.alto << " m\n";
    cout << "Lista de paquetes en el vehículo (Peso): ";
    for (Paquete paq : vehiculo.paquetes) {
        cout << paq.id<<"-"<<paq.pesoPaquete << " ";
    }
    cout << endl << endl << "Barra Procentual del Vehículo: " << endl;
    // Representación gráfica del camión
    cout << "  _______________________________________________________\n";
    cout << " |                                                       |\n";
    cout << " |   ";

    // Dibujamos una barra que represente el porcentaje de llenado
    int barrasLlenas = static_cast<int>(porcentajeLleno / 2); // Escalamos al tamaño de la barra (50 caracteres máx)
    for (int i = 0; i < 50; i++) {
        if (i < barrasLlenas)
            cout << "="; // Parte llena
        else
            cout << " "; // Parte vacía
    }
    cout << "  |\n";

    cout << " |_______________________________________________________|\n";
    cout << "        |______|                           |______|\n";
    cout << "\nPorcentaje de llenado: " << fixed << setprecision(2) << porcentajeLleno << "%\n";
}

double objetivoBPP3D(Paquete paq, Vehiculo vehiculo){
    return (double)(paq.ancho*paq.largo*paq.alto)/(double)(vehiculo.alto*
            vehiculo.ACVehiculo*vehiculo.LCVehiculo);
}

double costoBPP3D(Vehiculo vehiculo){
    double volTp = 0;
    
    // Obtener volumen acumulado de los paquetes del vehiculo
    for(Paquete paq: vehiculo.paquetes)
        volTp += paq.alto * paq.ancho * paq.largo;
    
    double volVeh = vehiculo.alto * vehiculo.ancho * vehiculo.largo;
    double pesoTp = vehiculo.pesoAcum;
    double pesoVeh = vehiculo.pesoMax;
    
    return (double)(((volTp/volVeh)*2 + pesoTp/pesoVeh))/3*100;
}

void copiar(Vehiculo &nuevoVehiculo,Vehiculo &vehiculo){
    nuevoVehiculo = vehiculo;
}

int obtenerCorteLargoInicial(Vehiculo vehiculo){
    if(vehiculo.paquetes.size() == 0)
        return 0;
    return vehiculo.largo - 1;
}

int obtenerMayorLargo(vector<Paquete> LPaquetes){
    int maxLargo = INT_MIN;
    for(Paquete paq: LPaquetes){
        if(paq.largo > maxLargo){
            maxLargo = paq.largo;
            paq.rotado = false;
        }
        if(paq.ancho > maxLargo){
            maxLargo = paq.ancho;
            paq.rotado = true;
        }
    }
    return maxLargo;
}

bool hayEspacio(Vehiculo veh,vector<Paquete> LPaquetes){
    for(Paquete paq: LPaquetes)
        if(paq.ancho*paq.largo*paq.alto <= veh.ACVehiculo*veh.LCVehiculo*veh.alto)
            return true;
    return false;
}

bool entrePaquetesPorPeso(Vehiculo vehiculo,vector<Paquete> LPaquetes){
    for(Paquete paq: LPaquetes)
        if(paq.pesoPaquete + vehiculo.pesoAcum <= vehiculo.pesoMax)
            return true;
    return false;
}

void obtenerMejorV(vector<Paquete> LPaquetes,Vehiculo v,double &mejorV,
        double &peorV){
    double mejor = INT_MIN, peor = INT_MAX;
    for(Paquete paq: LPaquetes){
        double FObjetivo = objetivoBPP3D(paq,v);
        if(FObjetivo < peor)
            peor = FObjetivo;
        if(FObjetivo > mejor)
            mejor = FObjetivo;
    }
    mejorV = mejor;
    peorV = peor;
}

bool hayEspacioParaPaquete(Vehiculo &vehiculo, Paquete &paq){
    
    if (paq.ancho > vehiculo.ACVehiculo or 
            paq.largo > vehiculo.LCVehiculo or paq.alto > vehiculo.alto) {
        return false; // No cabe en las dimensiones restantes
    }
    if (paq.pesoPaquete + vehiculo.pesoAcum > vehiculo.pesoMax) {
        return false; // Se pasa  del peso máximo permitido
    }
    
    return true;
}

int buscarPaquete(Paquete ele,vector<Paquete> LPaquetes){
    int indice = 0;
    for(Paquete paq: LPaquetes){
        if(ele.id == paq.id)
            return indice;
        indice++;
    }
    return 0;
}

vector<Paquete> generarListaCandidatosRestringida(double mejorV,double peorV,
        double cstRelaj,vector<Paquete> paquetes,Vehiculo v){
    vector<Paquete> listaCandidata;
    double limite = peorV + cstRelaj*(mejorV-peorV); 
    for(Paquete paq: paquetes){
        double FObj = objetivoBPP3D(paq,v);
        // Prioriza los mejores y valida que entre en el espacio sobrante
        if(FObj > limite and hayEspacioParaPaquete(v, paq))  
            listaCandidata.push_back(paq);
    }
    // Prioriza y añade los paquetes rotados
    for(Paquete paq: paquetes)
        if(paq.rotado)
            if(!buscarPaquete(paq,listaCandidata))    // Valida que no este
                listaCandidata.push_back(paq); 
    
    return listaCandidata;
}

Paquete seleccionAleatoria(vector<Paquete> RCL){
    inda = rand()%RCL.size();
    return RCL[inda];
}

bool compara(Paquete pa,Paquete pb){
    return pa.alto*pa.ancho*pa.largo > pb.alto*pb.ancho*pb.largo;
}

void embalarPaquete(Vehiculo &vehiculo,Paquete paqueteElegido){
    
    if (not hayEspacioParaPaquete(vehiculo, paqueteElegido)) {
        cout << "Error: El paquete no puede ser "
                "embalado debido a que se pasa el espacio y/o peso." << endl;
        return;
    }
    
    vehiculo.paquetes.push_back(paqueteElegido);
    vehiculo.pesoAcum += paqueteElegido.pesoPaquete;
    
    if(paqueteElegido.rotado){
        vehiculo.ACVehiculo -= paqueteElegido.ancho;
    }
    else{
        vehiculo.ACVehiculo -= paqueteElegido.ancho; 
    }  
}

void eliminarPaquete(Paquete ele,vector<Paquete> &LPaquetes){
    int indice = buscarPaquete(ele,LPaquetes);
    LPaquetes.erase(LPaquetes.begin()+indice);
}

Vehiculo GRASPBPP3DConstruccion(Vehiculo vehiculoOrg,vector<Paquete> LPaquetes,
        double cstRelaj){
    
    Vehiculo vehiculo = vehiculoOrg;
    int corteLargoInicial = obtenerCorteLargoInicial(vehiculo);
    int corteLargoFinal = obtenerMayorLargo(LPaquetes);
    int anchoIndice = 0;
    
    while(LPaquetes.size() > 0 and hayEspacio(vehiculo,LPaquetes) and 
            entrePaquetesPorPeso(vehiculo,LPaquetes)){
        
        srand(time(NULL)); // Semilla del random
        
        while(LPaquetes.size() > 0 and vehiculo.ancho > anchoIndice
                and entrePaquetesPorPeso(vehiculo,LPaquetes)){
            
            double mejorV,peorV;
            obtenerMejorV(LPaquetes,vehiculo,mejorV,peorV);
            
            vector<Paquete> RCL = generarListaCandidatosRestringida(mejorV,
                    peorV,cstRelaj,LPaquetes,vehiculo);
            
            if(RCL.empty()) break;
            
            Paquete paqueteElegido = seleccionAleatoria(RCL);
            embalarPaquete(vehiculo,paqueteElegido);
            if(paqueteElegido.rotado){
                anchoIndice += paqueteElegido.largo;
            }else{
                anchoIndice += paqueteElegido.ancho;
            }
            eliminarPaquete(paqueteElegido,LPaquetes);
        }
        anchoIndice = 0;
        vehiculo.LCVehiculo -= corteLargoFinal;
        vehiculo.ACVehiculo = vehiculo.ancho;
        corteLargoInicial = corteLargoFinal;     
        corteLargoFinal = obtenerMayorLargo(LPaquetes); // Mayor Largo
    }
    return vehiculo;
}

void actualizarEmbalaje(Vehiculo &vehiculo,Vehiculo &nuevoVehiculo){
    vehiculo = nuevoVehiculo;
}

Vehiculo ProcedimientoGRASPBPP3D(Vehiculo vehiculo,vector<Paquete> LPaquetes,int numIter,
        double cstRelaj){
    Vehiculo nuevoVehiculo;
    copiar(nuevoVehiculo,vehiculo);
    Vehiculo copia = vehiculo, mejor;
    
    for(int i=0;i<numIter;i++){
        nuevoVehiculo = copia;
        nuevoVehiculo = GRASPBPP3DConstruccion(nuevoVehiculo,LPaquetes,cstRelaj);
        
        cout<<"Solución "<<i<<": ";
        for(Paquete paq : nuevoVehiculo.paquetes){
            cout<<paq.pesoPaquete<<" ";
        }
        cout<<endl<<endl;
                
        if(i == 0){
            actualizarEmbalaje(mejor,nuevoVehiculo);
        }
        else
            if(costoBPP3D(mejor) <= costoBPP3D(nuevoVehiculo)){
                actualizarEmbalaje(mejor,nuevoVehiculo);
            }
    }
    return mejor;
}

int main(int argc, char** argv) {
    
    Vehiculo vehiculo;
    vector<Paquete> LPaquetes;
    int numIter = 400;   // Considera mayor población de soluciones
    double cstRelaj = 0.7; // Solución más aleatoria que voráz 
    
    leerArchivoProductos("Paquetes.txt",LPaquetes);
    leerArchivoVehiculos("Vehiculo.txt",vehiculo);
    
    vehiculo = ProcedimientoGRASPBPP3D(vehiculo,LPaquetes,numIter,cstRelaj);
    
    imprimirSolucion(vehiculo);
    
    return 0;
}