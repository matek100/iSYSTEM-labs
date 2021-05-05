#include <stdio.h>
#include <math.h>
#define RADIUS_EARTH 6371000
#define SPEED 50
//naprava1 je vedno trenutna naprava
//sprejme podatko o lokacijah dveh naprav, naprava1 je trenutna, naprava2 je neka druga
//kompas_1 vrne kot v stopninjah kje se nahaja naprava2 gelde na napravao1, kompas je vedno usmerjen proti severu
//razdalja_skala vrne logaritmsko skalo, kako blizu sta si napravi 
//privzaemo da je tabela naprava = {longituted, latitude, velocity}

double radian(double degree){
    return  degree * (M_PI /180.0);
}

double haversine(double naprava1[], double naprava2[]){ //po haversine formuli izracuna razdaljo med dvema tockama
    double lon1 = naprava1[0];
    double lat1 = naprava1[1];

    double lon2 = naprava2[0];
    double lat2 = naprava2[0];

    double phi_1 = radian(lat1);
    double phi_2 = radian(lat2);

    double delt_phi = radian(lat2-lat1);
    double delta_lambda = radian(lon2-lon1);

    double a = pow(sin(delt_phi/2.0),2) + cos(phi_1)*cos(phi_2) * pow(sin(delta_lambda/2.0), 2);

    double c = 2*atan2(sqrt(a), sqrt(1-a));

    return RADIUS_EARTH*c; //vrne razdaljo v metrih

}

int razdalja_skala(double naprava1[], double naprava2[]){ // z funkcijo f(x) = 11 - log_e(x), izračunamo skalo, kako velika mora bit ikonca na kompaasu (vecja stevilka =  vecja ikona), predlagam recimo, da za f(x) <= 4 sploh ne prikazujemo, to je crika 1000
    //vrne tip int, ker se ta vrednost uporablja sam za določanje skale, int je zaokrozen navzgor
    double x = haversine(naprava1, naprava2);
    double y = 11 - 1.2* log(x);
    int size_icon = 0; // 0 da ne prikazujes
    if(y >= 4){
    size_icon = ceil(11 - 1.2* log(x - 10)) - 4;
    }
    return size_icon;
    //funkcijo lahko se malo boljse fitamo tistmo kar rabimo
    //ce zelimo se upostaveat hitrost naprava1, recimo da je zgornji izracun (t.j. vidljivost do 1km primerna za hitrsot 50 na uro, potem sorazmerno spreminjamo f(X))
    //double speed_inverse = 1/(naprava1[2]/SPEED);
    //y = 11 - speed_inverse*log(x);
    //if(y >= 4){
    //    size_icon = ceil(11-speed_inverse* 1.2 * log(x-10)-4);
    //}
    
}

double kompas_1(double naprava1[], double naprava2[]){
    double lon1 = radian(naprava1[0]);
    double lat1 = radian(naprava1[1]);

    double lon2 = radian(naprava2[0]);
    double lat2 = radian(naprava2[0];)
    double delta_lon = lon2-lon1;

    double x = cos(lat2) * sin(delta_lon);
    double y = cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(delta_lon);

    double kompas = atan2(y, x);
    kompas = kompas - M_PI/2; 
    kompas = kompas % 2*M_PI;
    return kompas; // bi moglo vrnet tako kot si hotu
}

double kompas(double naprava1_stara[], double nparava1, double naprava2[]){
    double heading_n1 = kompas_1(naprava1_stara, naprava1); //heading naprave1 
    double heading = kompas_1(naprava1, naprava2);  //kot med napravama 1 in 2, sever je fiksn
    double kompas = abs(heading - heading_n1)
    return kompas;
    //da dobis poziciniras kompas, ga premaknes za -kot ki ga da kompas_1
    //funkcija kompas ti da za kak je kot med naprav2 in smerjo v katero gre naprava1
}





int main(){ //For testing purposes

}