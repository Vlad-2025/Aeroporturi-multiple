#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <queue>
#include <fstream>
#include <cmath>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

using namespace std;

//      STRUCTURI   ========
//  + alte chestii legate de ele

struct zonaSpecialaCerc
{
    int tipZona = 0;
	int raza = 0;
};

struct aeroport
{
	int x = 0, y = 0;
	int numar = 0;
	zonaSpecialaCerc cerc;
};

vector <aeroport> vectorAeroporturi;

struct zbor
{
    string IDzbor;
    int plecare = 0, destinatie = 0;    // aeroport plecare, aeroport de sosire  -  adik numarul ala de la fiecare aeroport
    int unitatiCombustibil = 0;     // cate patratele poate sa mearga inainte sa ramana fara benzenoida 
    int vCroaziera = 0;         // pt cand vreau sa fac calcule d-alea bengoase

    string tipZbor;     // militar / civil || m / c
    /*
        mai vedem
    */
};

vector <zbor> vectorZboruri;

//==========================================================================================================================


//      FUNCTII     ========================================================================================================

void cerc(int x0, int y0, int r, int** matriceHarta, int tip, int n, int m)
{
    if (r == 0)
        return;
    int x, y, xx, rr;

    //      xx si rr inseama x * x, respectiv r * r;
    //      
    //      ia -r pentru ca intervalul [-r, r] reprezinta diametrul;
    //
    //      r * r se face la inceput pentru ca nu se mai schimba, 
    //      x * x se face in 'for'-ul dupa cel in care a fost declarat
    //      pentru ca se schimba, la fel si y * y, doar ca e in 'if'

    for (rr = r * r, x = -r; x <= r; x++)
        for (xx = x * x, y = -r; y <= r; y++)
            if ((x0 + x) < n && (y0 + y) < m && xx + (y * y) <= rr && matriceHarta[x0 + x][y0 + y] == 0)
            {
                if (tip == 0)
                    matriceHarta[x0 + x][y0 + y] = -5;
                else if (tip == 1)
                    matriceHarta[x0 + x][y0 + y] = -6;
            }
}

int** creare_matrice(int randuri, int coloane)
{
    int** matrice = new int* [randuri];

    for (int i = 0; i < randuri; ++i)
        matrice[i] = new int[coloane];

    for (int i = 0; i < randuri; i++)
        for (int j = 0; j < coloane; j++)
            matrice[i][j] = 0;

    return matrice;
}

void dealocareMatrice(int** matrice, int randuri)
{
    for (int i = 0; i < randuri; i++)
        delete matrice[i];

    delete[] matrice;
}

int** creareMatrieHartaNumere(int &randuri, int &coloane)
{
    /*
        in matricebaza.txt avem coordonatele aeroporturilor, cat si daca 
        este cerc sau nu;
        
        daca este cerc, avem raza cercului;

        aeroporturile le tin minte intr-un vector;

        primele 2 valori din fisier sunt marimea matricei, randuri - coloane;
    
    */
	ifstream fin("matriceBaza.txt");

    fin >> randuri >> coloane;
    
    int** matriceHartaBaza = creare_matrice(randuri, coloane);

    do
    {
        // 3 valori din fisier sunt x, y si numarul aeroportului;
        // urmatoarea e ce fel de zona, militara sau civila (0 pt militar, 1 pt civil);
        // ultima este raza cercului;
        
        aeroport aux;

        fin >> aux.x >> aux.y >> aux.numar;
        
        fin >> aux.cerc.tipZona;
        fin >> aux.cerc.raza;
        
        cerc(aux.x, aux.y, aux.cerc.raza, matriceHartaBaza, aux.cerc.tipZona, randuri, coloane);

        vectorAeroporturi.push_back(aux);

        // adaugare aeroporturi pe matricea de baza

        matriceHartaBaza[aux.x][aux.y] = aux.numar;
        

    } while (!fin.eof());

    fin.close();
    
    return matriceHartaBaza;
}

void afisareMatrice(int** matrice, int randuri, int coloane)
{
    for (int i = 0; i < randuri; i++)
    {
        for (int j = 0; j < coloane; j++)
        {

            SetConsoleTextAttribute(hConsole, 15);
            if (matrice[i][j] == 0)
            {
                cout << 0;
            }
            else if (matrice[i][j] == -5)
            {
                SetConsoleTextAttribute(hConsole, 12);
                cout << '*';
            }
            else if (matrice[i][j] == -6)
            {
                SetConsoleTextAttribute(hConsole, 11);
                cout << '#';
            }
            else
            {
                SetConsoleTextAttribute(hConsole, 14);
                cout << matrice[i][j];
            }
            cout << ' ';
        }
        cout << endl;
    }
}

void introducereZboruri()
{
    ifstream fin("zboruri.txt");

    zbor aux;   

    do
    {
        fin >> aux.IDzbor;
        fin >> aux.plecare >> aux.destinatie;
        fin >> aux.unitatiCombustibil;
        fin >> aux.vCroaziera;

        fin >> aux.tipZbor;

        vectorZboruri.push_back(aux);

    } while (!fin.eof());

    fin.close();
}

void bordareMatrice(int randuri, int coloane, int** matrice)
{
    for (int i = 0; i < coloane; i++)
        matrice[0][i] = matrice[randuri-1][i] = - 1;
    for (int i = 0; i < randuri; i++)
        matrice[i][0] = matrice[i][coloane-1] = - 1;
}

int** submatrice(int** matrice, int iSus, int iJos, int jStanga, int jDreapta, int&randuri, int&coloane)
{
    randuri = abs(iSus - iJos) + 3, coloane = abs(jStanga - jDreapta) + 3;

    int** matriceMica = creare_matrice(randuri, coloane);

    bordareMatrice(randuri, coloane, matriceMica);

    for (int i = iSus, ii = 1; i <= iJos; i++, ii++)
    {
        for (int j = jStanga, jj = 1; j <= jDreapta; j++, jj++)
        {
            if (matrice[i][j] <= 0)
                matriceMica[ii][jj] = matrice[i][j];
            else
                matriceMica[ii][jj] = 0;
        }
    }    
    ///*
    for (int i = 0; i < randuri; i++)
    {
        for (int j = 0; j < coloane;j++)
        {
            SetConsoleTextAttribute(hConsole, 15);
            if (matriceMica[i][j] == 0)
            {
                cout << " 0";
            }
            else if (matriceMica[i][j] == -5)
            {
                SetConsoleTextAttribute(hConsole, 12);
                cout << " *";
            }
            else if (matriceMica[i][j] == -6)
            {
                SetConsoleTextAttribute(hConsole, 11);
                cout << " #";
            }
            else if (matriceMica[i][j] == -1)
            {
                SetConsoleTextAttribute(hConsole, 15);
                cout << -1;
            }
            else
            {
                SetConsoleTextAttribute(hConsole, 14);
                cout << ' ' << matriceMica[i][j];
            }
            cout << ' ';
        }
        cout << endl;
    }
    SetConsoleTextAttribute(hConsole, 15);
    //*/
    
    return matriceMica;
}

/// <summary>
/// 
/// istart si jstart sunt pt aeroportul de plecare.
/// iSus si restu sunt pentru submatrice.
/// 
/// tipZbor e pt gen militar sau civil pentru zonele alea speciale.
/// 
/// start e coltu din care pleaca
/// 
/// </summary>
void algLee(int** matriceBaza, int iSus, int iJos, int jStanga, int jDreapta, string tipZbor, int&suma,
            int istart, int jstart)
{ 
    int randuri=0, coloane=0;
    int** A = submatrice(matriceBaza, iSus, iJos, jStanga, jDreapta, randuri, coloane);

    if (istart == 0)
        istart = 1;
    else
        istart = randuri - 2;

    if (jstart == 0)
        jstart = 1;
    else
        jstart = coloane - 2;
    
    int di[10] = { 0, 0, 1, -1 };       //|
    int dj[10] = { 1, -1, 0, 0 };       //|
    queue<pair<int, int>> Q;            //|

    Q.push(make_pair(istart, jstart));//initializare coada
    A[istart][jstart] = 1;//marcare pozitie de start
    
    while (!Q.empty()) // cat timp coada este nevida
    {
        int i = Q.front().first, j = Q.front().second; // determinam elementul de la inceputul cozii

        for (int k = 0; k < 4; k++)
        {
            int iv = i + di[k], jv = j + dj[k]; // coordonatele vecinului

            if (iv >= 1 && iv <= randuri && jv >= 1 && jv <= coloane) 
            {
                if((A[iv][jv] == -6 || A[iv][jv] == 0) && tipZbor == "civil")
                {
                    A[iv][jv] = 0;
                    A[iv][jv] = A[i][j] + 1;    // marcam elementul vecin cu o valoare mai mare
                    Q.push(make_pair(iv, jv));  // il adaugam in coada
                    
                }
                else if((A[iv][jv] == -5 || A[iv][jv] == -6 || A[iv][jv] == 0) && tipZbor == "militar")
                {
                    A[iv][jv] = 0;
                    A[iv][jv] = A[i][j] + 1;    
                    Q.push(make_pair(iv, jv));  
                }
            }
        }
        Q.pop(); // eliminam din coada
    }
    cout << endl;
    /*
    for (int i = 0; i < randuri; i++)
    {
        for (int j = 0; j < coloane; j++)
        {
            if (A[i][j] <= -1 || A[i][j] >= 10)
                cout << A[i][j] << " ";
            else
                cout << A[i][j] << "  ";
        }
        cout << endl;
    }
    */
    int val = 0;

    if (A[1][1] == 1)
        val = A[randuri - 2][coloane - 2];

    else if (A[randuri - 2][coloane - 2] == 1)
        val = A[1][1];

    else if (A[randuri - 2][1] == 1)
        val = A[1][coloane - 2];
    else
        val = A[randuri - 2][1];

    cout << endl << "Lungimea traseului: " << val << endl;
    
    suma += val;
    dealocareMatrice(A, randuri);
}

//==========================================================================================================================
// TODO: implementare traseu
/*
void Traseu2(int istop, int jstop)
{
    int di[10] = { 0, 0, 1, -1 };       //|
    int dj[10] = { 1, -1, 0, 0 };       //|
    vector<pair<int, int>> V;
    int i = istop, j = jstop;
    V.push_back(make_pair(i, j));
    do
    {
        int p = -1;
        for (int k = 0; k < 4 && p == -1; k++)
            if (A[i][j] == A[i + di[k]][j + dj[k]] + 1)
                p = k;
        i = i + di[p], j = j + dj[p];
        V.push_back(make_pair(i, j));
    } while (A[i][j] != 1);
}
*/
void traseuCompus(int** matrice, string tip)
{
    int nrAeroporturi;
    int suma = 0;
Alegere1:
    cout << endl << "Cate opriri?\n";
    cin >> nrAeroporturi;
    if (!cin)
    {
        cin.clear();
        cin.ignore();
        cout << "Invalid";
        goto Alegere1;
    }

    int* v = new int[nrAeroporturi];

    for (int i = 0; i < nrAeroporturi; i++)
    {
    Alegere2:
        cout << i + 1 << ": ";
        cin >> v[i];
        if (!cin)
        {
            cin.clear();
            cin.ignore();
            cout << "Invalid";
            goto Alegere2;
        }

    }

    int iSus, iJos, jStanga, jDreapta;
    int istart, jstart, istop, jstop;
    int is, js;

    /*

        comparam indicii ca sa vedem unde se afla start-ul in raport cu stop-ul;

    */

    for (int i = 0; i < nrAeroporturi - 1; i++)
    {
        istart = iSus = vectorAeroporturi.at(v[i] - 1).x, istop = iJos = vectorAeroporturi.at(v[i + 1] - 1).x;

        jstart = jStanga = vectorAeroporturi.at(v[i] - 1).y, jstop = jDreapta = vectorAeroporturi.at(v[i + 1] - 1).y;

        {
            if (istart < istop)
                is = 0;//sus
            else
                is = 1;//jos

            if (jstart < jstop)
                js = 0;//stanga
            else
                js = 1;//dreapta
        }

        if (iSus > iJos)
            swap(iSus, iJos);
        if (jStanga > jDreapta)
            swap(jStanga, jDreapta);

        cout << "==========\n";
        cout << "# " << vectorAeroporturi.at(v[i] - 1).numar << " " << vectorAeroporturi.at(v[i + 1] - 1).numar << " #" << endl;
        cout << "==========\n";

        algLee(matrice, iSus, iJos, jStanga, jDreapta, tip, suma, is, js);

        cout << endl;
        cout << endl << "Lungime totala: " << suma << endl << endl;
    }

}

int main()
{
    // TODO: curatare main()

    int randuri = 0, coloane = 0;
    int** matrice = creareMatrieHartaNumere(randuri, coloane);
    
    cout << randuri << " " << coloane << endl << endl;

    afisareMatrice(matrice, randuri, coloane);

    cout << endl << endl;

    /*
    for (auto i = vectorAeroporturi.begin(); i != vectorAeroporturi.end(); i++)
        cout << "r = " << i->cerc.raza << ", Tip: " << i->cerc.tipZona << "; ";
    cout << endl;
    for (auto i = vectorAeroporturi.begin(); i != vectorAeroporturi.end(); i++)
        cout << "N: " << i->numar << ", x = " << i->x << " y = " << i->y << "; ";
    
    
    cout << endl << endl;
    introducereZboruri();
    for (auto i = vectorZboruri.begin(); i != vectorZboruri.end(); i++)
    {
        cout << "Zborul " << i->IDzbor << ", pleaca de la aeroportul " << i->plecare << ""
            " catre aeroportul " << i->destinatie << ", cu " << i->unitatiCombustibil << ""
            " unitati de combustibil, \ncalatorind cu o viteza de ~" << i->vCroaziera << "km/h" << endl;
        
        cout << endl;
    }
    */

    cout << endl;
   
    cout << "Aeroporturi disponibile:\n";

    cout << endl;

    for (int i = 0; i <= vectorAeroporturi.size()/2-2; i++)
        cout << "== == ";
    
    cout << endl;
    
    for (int i = 0; i < vectorAeroporturi.size()-1; i++)
    {
        cout << "| ";
        SetConsoleTextAttribute(hConsole, 11); 
        cout << vectorAeroporturi.at(i).numar;
        SetConsoleTextAttribute(hConsole, 15); 
        cout << " |,";

        if ((i + 1) % 4 == 0)
        {
            cout << endl;
            for (int i = 0; i <= vectorAeroporturi.size() / 2-2; i++)
                cout << "== == ";
            cout << endl;
        }
    }
    cout << "| ";
    SetConsoleTextAttribute(hConsole, 11);
    cout << vectorAeroporturi.at(vectorAeroporturi.size()-1).numar;
    SetConsoleTextAttribute(hConsole, 15);
    cout << " |";
    cout << endl;
    for (int i = 0; i <= vectorAeroporturi.size() / 2-2; i++)
        cout << "== == ";

    
    cout << endl << endl << "Ce tip de zbor?\n";
    zbor tip;
IntroducereTip:

    cin >> tip.tipZbor;
    if (tip.tipZbor != "m" && tip.tipZbor != "c" && tip.tipZbor != "militar" && tip.tipZbor != "civil")
    {
        cout << "Invalid\n";
        cin.clear();
        cin.ignore();
        goto IntroducereTip;
    }
    if (tip.tipZbor[0] == 'm')
        tip.tipZbor = "militar";
    else
        tip.tipZbor = "civil";
    traseuCompus(matrice, tip.tipZbor);
    
    return 0;
}

/*

left top corner: arr[0][0]
right top corner: arr[0][m-1]
left bottom corner: arr[n-1][0]
right bottom corner: arr[n-1][m-1]

*/

