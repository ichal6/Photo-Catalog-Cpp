#include <iostream>
#include <conio.h>
#include <fstream>
#include <string>
#include <regex>


#include <windows.h>
#include <Shobjidl.h>
#include <winnls.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h> 


#define C 10
#define M 100
#define D 1000

std::string string_konfiguracyjny[M];
std::string string_sciezka_zapisu;


HRESULT CreateLink(LPCTSTR lpszPathObj, LPCSTR lpszPathLink, LPCTSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl ; 

	CoInitialize(NULL);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	//std::cout <<std::hex << hres << " 1\n";
	if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
		
        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
 
        // Query IShellLink for the IPersistFile interface, used for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
		//std::cout << std::hex << hres << " 2\n";
        if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
            
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE);
			//std::cout <<std::hex << hres << " 3\n";
            ppf->Release(); 
        } 
        psl->Release(); 
    } 
	//std::cout <<std::hex<< hres << " 4\n";
    return hres; 
}


int DzielWczytaneDane(char* TablicaZnakow)
{

	char * pwc;
	char *next_token1 = NULL;
	char *next_token2 = NULL;
	char znak  = '/';
	char znak2 = '>';
	static int i = 0;
	std::string c;


	pwc = strtok_s(TablicaZnakow, ";\r\n", &next_token1);

	while (pwc != NULL)
	{

		if ((pwc[0] != znak) && (pwc[0] != znak2))
		{
			
			string_konfiguracyjny[i] = pwc;	
			i++;
			if (i >= 99)
			{
				return i;
			}

		}
		if (pwc[0] == znak2)
		{
			pwc = strtok_s(pwc, ">", &next_token2);
			string_sciezka_zapisu = pwc;
		}
		pwc = strtok_s(NULL, ";\r\n", &next_token1);

	}
	return i;
}

int DzielUstawienia(char* TablicaZnakow)
{
	char * pwc;
	char *next_token1 = NULL;
	char znak = '/';
	int i;

	pwc = strtok_s(TablicaZnakow, "?", &next_token1);

	
		while (pwc != NULL)
		{

			i = DzielWczytaneDane(pwc);

			pwc = strtok_s(NULL, "?", &next_token1);

		
		}
		return i;
}

int WalidacjaDanych(char* SciezkaDoFolderu, int WersjaProgramu) // Wersja Programu - 0 dla sprawdzania ca³ego adresu; 1 - dla sprawdzania tylko nazwy folderu
{
	std::string string_a;


	char *Tablica[7];
	size_t znalezionaPozycja = NULL;
	int i = 0;
	std::regex wzorzec("[a-zA-Z]");
	std::smatch wynik;

	Tablica[0] = "*";
	Tablica[1] = "?";
	Tablica[2] = "\"";
	Tablica[3] = "<";
	Tablica[4] = ">";
	Tablica[5] = "|";
	
	if (SciezkaDoFolderu == 0)
	{
		
		std::cout << "\nNiewprowadzono œcie¿ki!\n";

		return 0;
	}
	

	string_a = SciezkaDoFolderu;

	if (!std::regex_search(string_a, wynik, wzorzec))
	{
		std::cout << "\nB³êdna litera dysku!\n";
		return 0;
	}
	

	// Przerwij program jeœli w adresie znajduj¹ siê niedozwolone znaki:
	while ( i < 6)
	{

		znalezionaPozycja = string_a.find(Tablica[i]);
		
		if (znalezionaPozycja != std::string::npos)
		{
			std::cout << "\nB³êdna œcie¿ka folderu. Wykryto niepoprawny znak\n";
			return 0;
		}

		i++;
		
	}
	
	// Jeœli adres nie jest adresem sieciowym wykonaj:
	if (WersjaProgramu == 0)
		if ((string_a[0] != '\\') || (string_a[1] != '\\'))
	{

		if (string_a[1] != ':')
			{
				std::cout << "\nNiepoprawna œcie¿ka folderu!\n";
				return 0;
			}

		if (string_a[2] != '\\')
			if (string_a[2] != '/')
			{
				std::cout << "\nNiepoprawna œcie¿ka folderu!\n";
				return 0;
			}
	}

	if (WersjaProgramu == 1)
	{
		Tablica[6] = "\\";

		znalezionaPozycja = string_a.find(Tablica[6]);

		if (znalezionaPozycja != std::string::npos)
		{
			
			std::cout << "\nB³êdna œcie¿ka folderu. Wykryty znak to \\. Jeœli chcesz zmieniæ folder zapisu uczyñ to w polu Lokalizacji Katalogu\n";

			return 0;
		}
	}


	if (znalezionaPozycja == std::string::npos)
	{

		return 1;
	}
	return 0;
}

int main()
{
	system("chcp 1250");
	system( "cls" );
	char dzien[M],komputer[D], pochodzenie[D], podfoldery[D], kosz[M], sciezka_zapisu[MAX_PATH];

	int wielkosc;
	
	std::fstream Plik_Konfiguracyjny;
	Plik_Konfiguracyjny.open("ustawienia.konf", std::ios::in);
	
	if (Plik_Konfiguracyjny.good() == false)
	{
		std::cout << "Plik konfiguracyjny nie zosta³ wczytany!\nZamykanie programu...";
		Sleep(1000);
		return 0;
		
	}
	else std::cout << "Ustawienia zosta³y wczytane pomyœlnie.\n";
	Sleep(1000);
	system("cls");
	
	char bufor[1024] = {'a'};
	

	Plik_Konfiguracyjny.read(bufor, 1024);

//	std::cout << "Wczytano " << Plik_Konfiguracyjny.gcount() << " bajtów do bufora" << std::endl;

	Plik_Konfiguracyjny.close();

	wielkosc = DzielUstawienia(bufor);

	sprintf_s(sciezka_zapisu, "%s", string_sciezka_zapisu.c_str());
	
	printf("Czy lokalizacja standardowa?  ");
	char odpowiedz[M];
	gets_s( odpowiedz);


//	sprintf_s(sciezka_zapisu, "H:\\DOM\\wspólne\\katalog zdjêæ\\" );

	if(odpowiedz[0]!='T' )
		if(odpowiedz[0]!='t')
	{
		
	std::cout << "Podaj œcie¿kê zapisu: ";

	gets_s(sciezka_zapisu);
	
	odpowiedz[0] ='T';
	}

	
	std::cout << "\n\nPodaj nazwê folderu: ";
	char nazwa[M];
	gets_s(nazwa);
	
	while (!WalidacjaDanych(nazwa, 1))
	{
		std::cout << "Podano nieprawid³ow¹ nazwê folderu. Niedozwolone znaki to \\/:*?<>|" << std::endl << "WprowadŸ nazwê jeszcze raz: \n";
		gets_s(nazwa);

	}
	
	std::cout << "\nWybrana œcie¿ka to: "<<sciezka_zapisu << "\n\n";
    
    std::cout << "Podaj nazwê komputera: \n";
   	
	unsigned int n = 0;
	for (unsigned int l = 1; wielkosc > 0; l++, wielkosc--)
	{
		std::cout << "  " << l << ":" << string_konfiguracyjny[n] << "\n";
		n++;
	}

	std::cout << "  0:rêcznie\n\n  ";

   	int numer1;
	char PC[M] = { '0' };
    std::cin >> numer1;
    gets_s(kosz);

	if (numer1)
	{
		numer1--;
		sprintf_s(PC, "%s", string_konfiguracyjny[numer1].c_str());
	}
	else
	{
		std::cout << "\nWprowadŸ rêcznie nazwê komputera:\n\n";
		gets_s(PC); 
		std::cout << "\n";
	}
	
	
	char command[D];
	sprintf_s(command, "mkdir \"%s%s\"",sciezka_zapisu, nazwa );
	system(command);
	

	char katalog[D];
	sprintf_s(katalog,"%s%s/Dane-%s.txt",sciezka_zapisu, nazwa, PC  );
	std::ofstream G(katalog);



	printf("\nData wykonania: ");
	gets_s(dzien);
	G << "Data wykonania: "<< dzien << ";" ;
	
	

	G << "\nLokalizacja: " << PC;
	 

	printf("Lokalizacja folderu: ");
	gets_s(pochodzenie);
	
	
	while(!WalidacjaDanych(pochodzenie, 0))
	{
		/*Sleep(1500);
		return 0; */
		
		std::cout << "\nWprowdŸ lokalizacjê folderu jeszcze raz: \n";
		gets_s(pochodzenie);
	}
	
	G << ": \""<< pochodzenie << "\";";	

	printf("\nZawierane podfoldery : ");
	gets_s(podfoldery);
	G << "\nPodfoldery: "<< podfoldery;
	
	char shortcut[D];
	sprintf_s(shortcut,"%s/%s/%s-%s.lnk",sciezka_zapisu, nazwa, nazwa, PC  );	
	
	
	sprintf_s(komputer, "folder: %s znajdujê siê na %s", nazwa, PC);
	
	CreateLink(pochodzenie, shortcut, komputer);

	G.close();
	//system("pause");
}


