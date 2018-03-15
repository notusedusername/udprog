//Helytakarékosan ábrázolt szimmetrikus mátrix 28 elemű vektorban felírt formáját
//alakítja szembarátabb mátrixos ábrázolásba. Nem feltétlen helyes algoritmus, but
// I guess I will pass. Továbbfejleszthető custom vektor beolvasására, és a méret alapján, 
//hiba vagy a mátrix kiírására.

#include <iostream>
#include <vector>
#include <math.h>


using namespace std;

int rek_osszead(int szam){

	if (szam > 0)

		szam += rek_osszead(szam - 1);

	else 
		return szam;
}

int main (){
	
std::vector<int> v;

 
v={94, 43, 90, 49, 38, 88, 50, 60, 28, 93, 82, 52, 13, 48, 23, 96, 84, 54, 61, 32, 81, 64, 76, 99, 18, 92, 78, 11};



int megoldas1 = 0;

int megoldas2 = 0;

int i,j, harom, ot, negy, hat;
int szamlalo = 0;
 
 int M[8][8];
	for( i = 1; i <=  7; i++){

		for(  j = 1; j <= i; j++){
			
			
			if ( i == j){
				// cout<< "M[" << i << "," << j << "]= " << v[szamlalo] <<endl;
				M[i][j]= v[szamlalo];
			}
			else{

				//cout<< "M[" << i << "," << j << "]= " << v[szamlalo] <<"\t és M["<< j <<"," << i << "]= " << v[szamlalo] <<endl;
				M[i][j]= v[szamlalo];
				M[j][i]= v[szamlalo];
			}
			int i1 = i;
			int j1 = j;
			
			if ( (i1 == 6 && j1 == 5) || (i1 == 5 && j1 == 6)){
				megoldas1= v[szamlalo];
				ot = j;
				harom = i;
			}

			if ( (i1 == 6 && j1 == 6) || (i1 == 6 && j1 == 6)) {
				megoldas2 = v[szamlalo];
				hat = j;
				negy = i;
			}	
		szamlalo++;
		}
	}








int x= 1, y=1;
for( i = 0; i <= 7; i++){
	for( j = 0; j <= 7 ; j++){
		if (i == 0 && j == 0)
			cout<<"index"<<'\t';
		else if( i == 0 && j != 0){
			cout<<x<<'\t';
			x++;
		}

		else if( j == 0 && i != 0){ 
			cout<<y<<'\t';
			y++;
		}
		else if( j != 0 && i != 0)
			cout<<M[i][j]<<'\t';




	if (j == 7 )
		cout<<'\n'<<endl;
	
	}
}

cout<< "M[6,5]= "<<megoldas1<<endl;

cout<< "M[6,6]= "<<megoldas2<<endl;

cout<<megoldas1+megoldas2<<'\n'<<endl;

return 0;
}
