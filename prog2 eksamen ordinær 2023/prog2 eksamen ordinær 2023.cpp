#include <iostream>
#include <ctime>
#include <exception>
#include <vector>

class Dato {
private:
	int dag;
	int maaned;
	int aar;

public:
	Dato(){
		tm date;
		time_t now = time(0);
		localtime_s(&date, &now);

		dag = date.tm_mday;
		maaned = date.tm_mon + 1;
		aar = date.tm_year + 1900;
	}
	friend std::ostream& operator<<(std::ostream& ost, Dato d);
};

class Transaksjon {
private:
	int m_belop;
	Dato dato;
	std::string m_tekst;

public:
	Transaksjon(int belop, std::string tekst) :
		m_belop{ belop }, m_tekst{ tekst } {
		dato = Dato();
	}

	friend std::ostream& operator <<(std::ostream& ost, Transaksjon t);
};


std::ostream& operator<<(std::ostream& ost, Dato d) {
	return ost << d.dag << "/" << d.maaned << " " << d.aar;

};

std::ostream& operator <<(std::ostream& ost, Transaksjon t) {
	return ost << t.dato << "\tbelop: " << t.m_belop << "kr. \tmelding: " << t.m_tekst;

};

class IkkeDekningException : public std::exception {

};

class IkkeMuligMedNegativtBelopException : public std::exception {

};


class Konto {
private:
	unsigned int m_kontoNummer;
	double m_saldo;
	double m_rente;
	double m_akkumulertRente;
	std::vector<std::shared_ptr<Transaksjon>> m_transaksjonsliste;

public:
	Konto(unsigned int kontoNummer, double rente_pr_aar) :
		m_kontoNummer{ kontoNummer }, m_rente{ rente_pr_aar }, m_saldo{ 0.0 }, m_akkumulertRente{ 0.0 } {}

	void utforTransaksjon(double belop, std::string tekst) {
		if (m_saldo + belop < 0) {
			throw IkkeDekningException();
		}

		else {
			m_saldo += belop;
			m_transaksjonsliste.push_back(std::make_shared<Transaksjon>(belop, tekst));
		};
	};

	void innskudd(double in) {
		if (in < 0.0) {
			throw IkkeMuligMedNegativtBelopException();

		}
		else {
			utforTransaksjon(in, "innskudd");
		}
	}

	void uttak(double ut) {
		if (ut < 0.0) {
			throw IkkeMuligMedNegativtBelopException();

		}
		else {
			utforTransaksjon(-ut, "uttak");
		}
	}

	void gebyr(double belop) {
		if (belop < 0.0) {
			throw IkkeMuligMedNegativtBelopException();

		}
		else {
			utforTransaksjon(-belop, "gebyr");
		}
	}
	void kontoUtskrift();
};

class brukskonto : public Konto {

public:
	brukskonto(unsigned int knr) : Konto{ knr, 0.25 } {}

};

class SpareKonto : public Konto {
private:
	int antallGratisUttak;
	int antallUttak;

public:
	SpareKonto(unsigned int knr, int antallGratisUttak = 5) :
		Konto{ knr, 1.6 }, antallGratisUttak{ antallGratisUttak }, antallUttak{ 0 } {};

	void uttak(double belop) {
		Konto::uttak(belop);
		if (antallUttak >= antallGratisUttak) {
			if ((belop * 0.1) > 250) {
				double gebyr_belop = belop * 0.1;
				gebyr(gebyr_belop);
			}
			else {
				double gebyr_belop = 250;
				gebyr(gebyr_belop);
			}
		}
		antallUttak++;
	}
};

void Konto::kontoUtskrift() {
	std::cout << "Kontoutskrift kontonummer " << m_kontoNummer << std::endl;
	for (auto trans : m_transaksjonsliste) {
		std::cout << *trans << std::endl;
	}
	std::cout << "Saldo: " << m_saldo << std::endl;
}

int main() {

	brukskonto b1(123456789);
	SpareKonto s1(987654321);

	b1.innskudd(100000);
	s1.innskudd(100000);

	for (int i = 0; i < 7; i++) {
		b1.uttak(50000);
		s1.uttak(50000);

	}
	b1.kontoUtskrift();
	s1.kontoUtskrift();

	return 0;
};
