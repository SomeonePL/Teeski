//
// Created by Andrzej on 19.12.2018.
//


#include <cmath>
#include "client.h"
#include "converters.h"


/*
- OPERACJA		XXX
-- ODPOWIEDZ		XXX
- polacz        000
--  zadanie         000 (C)
--  przyjecie       001 (S)
--  odmowa          010 (S)
--  blad serwera    011 (S)
--  chce id         100 (C)
--  masz id         101 (S)
- zapros        001
--  zadanie         000 (C)
--  przyjecie       001 (C)
--  odmowa          010 (C)
--  nie ma klienta  011 (S)
- wyslij		010
--  zadaj	        000 (C)
--  potwierdzenie   001 (S)
--  odmowa			010 (C)
--	ACK				111
- zakoncz		011
--  zadanie         000 (C)
--  odpowiedz		001 (S)
--	odmow			010	(C)
														   o-------------------------o
 --- FLAGI                                       X-DF X-MF | DF - Data Fragmentation |
---Brak fragmentacji                             0    0    | MF - More Fragments     |
---Fragmentacja, ostatni pakiet                  1    0    o-------------------------o
---Fragmentacja, wiecej pakietow za tym pakietem 1    1
*/

//KOMUNIKATY

void client::wyczysc() {
	operacja = 0;
	odpowiedz = 0;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
}

void client::zadaj_polaczenia() {
	//ZADANIE NAWIAZANIA POLACZENIA
	operacja = 0;
	odpowiedz = 0;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = 0;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}


void client::zadaj_zaproszenia() {
	//WYSLANIE ZAPROSZENIA
	operacja = 1;
	odpowiedz = 0;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}

void client::przyjmij_zaproszenie() {
	//PRZYJECIE ZAPROSZENIA
	operacja = 1;
	odpowiedz = 1;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}

void client::odrzuc_zaproszenie() {
	//ODRZUCENIE ZAPROSZENIA
	operacja = 1;
	odpowiedz = 2;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: "<<  buffer_size << std::endl;
}

void client::potwierdzenie() {
	//POTWIERDZENIE OTRZYMANIA WIADOMOSCI
	operacja = 2;
	odpowiedz = 1;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}

void client::zadaj_zakonczenia() {
	//ZADANIE ZAKONCZENIA SESJI
	operacja = 3;
	odpowiedz = 0;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}

void client::potwierdz_zakonczenie() {
	//POTWIERDZENIE OTRZYMANIA WIADOMOSCI
	operacja = 3;
	odpowiedz = 1;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
	//std::cout << "Buffer size: " << buffer_size << std::endl;
}

void client::ponow() {
	std::cout << "Co chcesz zrobic?\n[1] Sprobuj ponownie\n[2] Rozlacz\n";
	int reakcja;
	reakcja = _getch();
	if (reakcja == '1') {
		std::cout << "<--trwa proba ponownego polaczenia-->\n";
		wyczysc();
		zadaj_polaczenia();
		if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
		{
			std::cout << "Blad sendto nr" << errno << std::endl;
		}
		wyczysc();

	}
	if (reakcja == '2') {
		std::cout << "<--zadanie zakonczenia polaczenia-->\n";
		wyczysc();
		zadaj_zakonczenia();
		if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
		{
			std::cout << "Blad sendto nr" << errno << std::endl;
		}
		wyczysc();
	}
	else {
		wyczysc();
		Sleep(100);
	}
}

void client::odrzuc_zakonczenie() {
	operacja = 3;
	odpowiedz = 4;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
}

void client::wymus_koniec() {
	operacja = 7;
	odpowiedz = 7;
	dlugosc = 10;
	dane = 0;
	flagi = 0;
	id = identyfikator;
	spakuj();
}


void client::zadaj_wyslania() {

	//WYSLANIE WIADOMOSCI
	getline(std::cin, text);
	if (text == "#") {
		zadaj_zakonczenia();

		//std::cout << "Buffer size: " << buffer_size << std::endl;
		if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
		{
			std::cout << "Blad sendto nr" << errno << std::endl;
		}
		wyczysc();
		return;
	}
	else if (text == "###") {
		wymus_koniec();
		if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
		{
			std::cout << "Blad sendto nr" << errno << std::endl;
		}

	}
	else {
		operacja = 2;
		odpowiedz = 0;
		int d = text.size() * 8 + 10;//rozmiar
		//buffer_size = 6 + text.size();

		//std::string ciag = "";
		//for (int i = 0; i < (int) text.length(); i++) {
		// ciag = ciag;
		//wiadomosc = wiadomosc + znak;
		//}

		if (d <= 1019 * 8) {
			flagi = 0;
			id = identyfikator;//identyfikator
			dlugosc = d;
			zapisz(text);
			//std::cout << "<WYSYLAM> DZIALANIE: " << dzialanie << " ODPOWIEDZ:" << odpowiedz << " DLUGOSC:" << dlugosc << " TEKST:" << wiadomosc;
			
			spakuj();

			//std::cout << "Buffer size: " << buffer_size << std::endl;

			if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
			{
				std::cout << "Blad sendto nr" << errno << std::endl;
			}

		} // DZIELENIE NA PACZKI
		else {
			float paczki = d / (1019 * 8);
			paczki = ceil(paczki);

			int wyslane = 0;


			for (int i = 0; i < paczki - 1; i++) {
				
				flagi = 3;
				id = identyfikator;
				dlugosc = 1019;
				zapisz(text.substr(i * (1019), 1019));
				wyslane++;

				spakuj();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;

				}

			
				while (operacja != 2 && odpowiedz != 7)
				{
					
				}
			}

			

			std::cout << std::endl << wyslane << "< wyslane " << paczki << " < paczki" ;
			flagi = 2;
			id = identyfikator;


			int temp = d - (wyslane * 1019 * 8);
			zapisz(text.substr((wyslane + 1) * (1019), temp / 8));

			spakuj();
			wyslane++;
			if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
			{
				std::cout << "Blad sendto nr" << errno << std::endl;
			}

			std::cout << std::endl << wyslane << "< wyslane " << paczki << " < paczki";

		}

	}
}
//

//OPERACJE NA DANYCH
void client::spakuj() {

	std::string pomocnicza = "";

	if (z2na10(dlugosc.to_string()) == 10) {
		pomocnicza = operacja.to_string();
		pomocnicza += odpowiedz.to_string();
		pomocnicza += dlugosc.to_string();
		//pomocnicza += wiadomosc.to_string();
		pomocnicza += flagi.to_string();
		pomocnicza += id.to_string();
	}
	else {
		pomocnicza = operacja.to_string();
		pomocnicza += odpowiedz.to_string();
		pomocnicza += dlugosc.to_string();
		pomocnicza += dane.to_string().substr(0,z2na10(dlugosc.to_string())-10);
		pomocnicza += flagi.to_string();
		pomocnicza += id.to_string();
	}
	buffer_size = pomocnicza.length()/8;
	std::cout << buffer_size << std::endl;
	ZeroMemory(buffer, 1024);
	//std::cout << pomocnicza << std::endl;
	for (int i = 0; i < buffer_size; i++) {
		std::string bajt_1 = pomocnicza.substr(i * 8, 8);
		buffer[i] = bit_to_int(bajt_1);
	}

	std::cout << std::endl << pomocnicza << std::endl;
	//std::cout << std::endl << "OP " << z2na10(operacja.to_string()) << "  OD " << z2na10(odpowiedz.to_string()) << "  DL " << z2na10(dlugosc.to_string()) << "  D " << z2na10(dane.to_string()) << "  FL " << z2na10(flagi.to_string()) << "  ID " << z2na10(id.to_string()) << std::endl;

}

void client::zapisz(std::string napis) {
	std::string binarnytekst = "";


	for (int i = 0; i < (int)napis.size(); i++) {
		binarnytekst = binarnytekst + zCna2(napis[i]);
	}

	dane = 0;
	for (int i = binarnytekst.size() - 1; i >= 0; i--) {
		if (binarnytekst[i] == '1') {
			dane.set(8143 - i, 1);
		}
	}
}


void client::odpakuj() {

	std::string pomocnicza = "";
	/*
	std::string pom = zCna2(buffer[0]);
	operacja = bit_to_int(pom.substr(0, 3));
	odpowiedz = bit_to_int(pom.substr(3, 3));
	pom = zCna2(buffer[5]);
	id = bit_to_int(pom.substr(0, 8));
	*/

	for (int i = 0; i < 1024; i++) {
		pomocnicza += zCna2(buffer[i]);
	}

		operacja = bit_to_int(pomocnicza.substr(0, 3));
		odpowiedz = bit_to_int(pomocnicza.substr(3, 3));
		dlugosc = bit_to_int(pomocnicza.substr(6, 32));
		//dane = bit_to_int(pomocnicza.substr(38, bit_to_int(dlugosc.to_string()) - 10));
		flagi = bit_to_int(pomocnicza.substr(bit_to_int(dlugosc.to_string()) + 28, 2));
		id = bit_to_int(pomocnicza.substr(bit_to_int(dlugosc.to_string()) + 30, 8));

		std::string s = "";
		s = pomocnicza.substr(38, z2na10(dlugosc.to_string()) - 10);
		dane = bit_to_int(s);
		std::cout << "otrzymano :" << s << std::endl << " dl " << z2na10(dlugosc.to_string()) -10 << std::endl;

	//	std::cout << std::endl << "OP " << z2na10(operacja.to_string()) << "  OD " << z2na10(odpowiedz.to_string()) << "  DL " << z2na10(dlugosc.to_string()) << "  D " << z2na10(dane.to_string()) << "  FL " << z2na10(flagi.to_string()) << "  ID " << z2na10(id.to_string()) << std::endl;
	}


void client::odczytaj() {
	std::string bajt;
	std::string wynik = "";
	std::string pomocnicza = dane.to_string();
	std::cout << pomocnicza;
	for (int i = 0; i < (int)pomocnicza.length(); i = i + 8) {
		bajt = pomocnicza.substr(i, 8);
		wynik = wynik + bit_to_char(bajt);
	}
	std::cout << wynik;
}



//KLIENT
int client::UDP() {
	//WINSOCK
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(55555);
	inet_pton(AF_INET, "127.0.0.1", &dest_addr.sin_addr);

	//ZADANIE POLACZENIA
	std::cout << "<---proba nawiazania polaczenia--->\n";
	zadaj_polaczenia();
	sendto(client_socket, buffer, buffer_size, 0, (sockaddr*)&dest_addr, sin_size);

	std::cout << "<-wyslano->\n";
	wyczysc();

	std::thread ODBIOR(&client::odbierz_wiadomosc, this, client_socket, buffer, dest_addr, sin_size);
	ODBIOR.detach();

	while (true) {
		while (operacja == 0 && odpowiedz == 0) {

		}


		if (operacja == 0 && odpowiedz == 1) {

			std::cout << "<----nawiazano polaczenie---->\n";
			std::cout << "<----otrzymano id sesji---->\n";
			identyfikator = z2na10(id.to_string());
			std::cout << "Co chcesz zrobic?\n[1] Zapros do rozmowy\n[2] Czekaj\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<----trwa zapraszanie innego uytkownika do rozmowy---->\n";
				wyczysc();
				zadaj_zaproszenia();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
				while (operacja == 1 && odpowiedz == 0) {
				}
			}
			if (reakcja == '2') {
				std::cout << "<----oczekuje na zaproszenie---->\n";
				wyczysc();
			}
		}

		if (operacja == 0 && odpowiedz == 2) {
			std::cout << "<----odmowa nawiazania polaczenia---->\n";
			ponow();
		}

		if (operacja == 0 && odpowiedz == 3) {
			std::cout << "<----problem po stronie serwera lub serwer nieosiagalny---->\n";
			ponow();
		}

		if (operacja == 1 && odpowiedz == 0) {
			std::cout << "<----otrzymano zaproszenie do rozmowy---->\n";
			std::cout << "Co chcesz zrobic?\n[1] Przyjmij\n[2] Odrzuc\n[3] Rozlacz - nieaktywne\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<--przyjmowanie zaproszenia-->\n";
				wyczysc();
				przyjmij_zaproszenie();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
				rozmowa();
			}
			if (reakcja == '2') {
				std::cout << "<----odrzucanie zaproszenia---->\n";
				wyczysc();
				odrzuc_zaproszenie();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
				wyczysc();
			}
		}


		if (operacja == 1 && odpowiedz == 1) {
			std::cout << "<----uzytkownik przyjal zaproszenie---->\n";
			/*wyczysc();
			zadaj_wyslania();
			spakuj();
			sendto(client_socket, buffer, buffer_size / 8, 0, (sockaddr*)&dest_addr, sin_size);*/
			rozmowa();
		}

		if (operacja == 1 && odpowiedz == 2) {
			std::cout << "<----uzytkownik odrzucil zaproszenie---->\n";
			std::cout << "Co chcesz zrobic?\n[1] Zapros ponownie\n[2] Rozlacz\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<----trwa ponowne zapraszaine do rozmowy---->\n";
				wyczysc();
				zadaj_zaproszenia();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
			}
			if (reakcja == '2') {
				std::cout << "<----zakonczenie polaczenia---->\n";
				wyczysc();
				potwierdz_zakonczenie();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
			}
		}

		if (operacja == 1 && odpowiedz == 3) {
			std::cout << "<----nie udalo sie zaprosic uzytkownika---->\n";
			std::cout << "Co chcesz zrobic?\n[1] Sprobuj ponownie\n[2] Rozlacz\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<----trwa proba ponownego zaproszenia uzytkownika---->\n";
				wyczysc();
				zadaj_zaproszenia();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
			}
			if (reakcja == '2') {
				std::cout << "<----zadanie zakonczenia polaczenia---->\n";
				wyczysc();
				zadaj_zakonczenia();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
				wyczysc();
			}
		}

		if (operacja == 2 && odpowiedz == 7) {
			std::cout << "<----dostarczono komunikat---->\n";
		}

		if (operacja == 3 && odpowiedz == 0) {
			std::cout << "<----rozmowca chce zakonczyc sesje---->\n[1] Pozwol odejsc\n[2] Utrzymaj polaczenie\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1') {
				std::cout << "<----zezwolono na rozlaczenie---->\n";
				potwierdz_zakonczenie();
				if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
				{
					std::cout << "Blad sendto nr" << errno << std::endl;
				}
				break;
			}
			if (reakcja == '2') {
				std::cout << "<----wyslano komunikat o checi utrzymania polaczenia---->\n";

				wyczysc();
			}
		}

		if (operacja == 3 && odpowiedz == 1) {
			std::cout << "<----zezwolono na rozlaczenie--->\n";
			std::cout << "<----trwa rozlaczanie--->\n";
			break;
		}

		if (operacja == 7 && odpowiedz == 7) {
			std::cout << "<---- wymuszono zakonczenie sesji---->\n";
			std::cout << "<----rozlaczono---->\n";
			break;
		}

		int i = sizeof(dest_addr);

	}

	closesocket(client_socket);
	WSACleanup();
	std::cout << "<----rozlaczono---->\n";
	return 1;
}

void client::odbierz_wiadomosc(SOCKET client_socket, char buffer[1024], SOCKADDR_IN dest_addr, int sin_size)
{
	while (true)
	{
		recvfrom(client_socket, buffer, 1024, 0, (sockaddr*)&dest_addr, &sin_size);
		odpakuj();

		if (operacja == 2 && odpowiedz == 1) {
			std::cout << "<----dostarczono--->\n";
			wyczysc();
		}

		if (operacja == 2 && odpowiedz == 0) {
			//std::string pom = dane.to_string();
			char znak;
			std::string pom = "";
			pom = dane.to_string().substr(8154- (z2na10(dlugosc.to_string())), z2na10(dlugosc.to_string()));

			std::cout << pom << std::endl;
			std::cout << "\nRozmowca napisal: " << std::endl;
			
			for (int j = 0 ; j < pom.length(); j = j + 8) {
				//std::cout<< z2na10(pom.substr(j, 8));
				znak = z2na10(pom.substr(j, 8));
				std::cout << znak;
			}
			std::cout << std::endl;
			
			


			potwierdzenie();
			if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
			{
				std::cout << "Blad sendto nr" << errno << std::endl;
			}
			wyczysc();

	
			std::cout << std::endl;

			if (flagi == 3)
			{
				do {
					recvfrom(client_socket, buffer, 1024, 0, (sockaddr*)&dest_addr, &sin_size);
					odpakuj();
					std::cout << "\nRozmowca napisal: " << std::endl;

					potwierdzenie();
					if (sendto(client_socket, buffer, buffer_size, 0, (sockaddr *)&dest_addr, sin_size) == -1)
					{
						std::cout << "Blad sendto nr" << errno << std::endl;
					}
					wyczysc();

					std::cout << std::endl;

				} while (flagi == 3);
			}

		}
		Sleep(100);
	}
}


void client::rozmowa() {
	std::cout << "Przywitaj sie! Napisz\"hej\"\nAby zakonczyc rozmowe wpisz #\n";
	while (true) {
		zadaj_wyslania();
		wyczysc();

		if (operacja == 0 && odpowiedz == 0)
		{

		}

		if (operacja == 3 && odpowiedz == 0) {
			std::cout << "<----rozmowca chce zakonczyc sesje---->\n[1] Pozwol odejsc\n[2] Utrzymaj polaczenie\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1')
			{
				std::cout << "<----zezwolono na rozlaczenie--->\n";
				potwierdz_zakonczenie();
				sendto(client_socket, buffer, buffer_size, 0, (sockaddr*)&dest_addr, sin_size);
				break;
			}
			if (reakcja == '2')
			{
				std::cout << "<----wyslano komunikat o checi utrzymania polaczenia---->\n";
				odrzuc_zakonczenie();
				sendto(client_socket, buffer, buffer_size, 0, (sockaddr*)&dest_addr, sin_size);
				wyczysc();
			}
		}

		if (operacja == 3 && odpowiedz == 1) {
			std::cout << "<----zezwolono na rozlaczenie--->\n";
			std::cout << "<----trwa rozlaczanie--->\n";
			break;
		}

		if (operacja == 3 && odpowiedz == 4) {
			std::cout << "<----rozmowca chce kontynuowac sesje---->\n[1] Ponow zadanie\n[2] Wymus zakonczenie\n[3] Wroc do rozmowy\n";
			int reakcja;
			reakcja = _getch();
			if (reakcja == '1')
			{
				std::cout << "<----ponawiam zadanie--->\n";
				zadaj_zakonczenia();
				sendto(client_socket, buffer, buffer_size, 0, (sockaddr*)&dest_addr, sin_size);
				wyczysc();
			}
			if (reakcja == '2')
			{
				std::cout << "<----wymuszono zakonczenie polaczenia---->\n";
				wymus_koniec();
				sendto(client_socket, buffer, buffer_size, 0, (sockaddr*)&dest_addr, sin_size);
			}
			if (reakcja == '3')
			{
				std::cout << "<----wracam do rozmowy---->\n";
				wyczysc();
			}
		}

		if (operacja == 7 && odpowiedz == 7)
		{
			std::cout << "<---rozmowca wymusil zakonczenie sesji--->\n";
			std::cout << "<----rozlaczono--->\n";
			break;
		}
	}
}