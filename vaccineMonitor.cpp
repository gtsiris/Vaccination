#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "class_Citizen.h"
#include "class_Vaccine_Status.h"
#include "class_Country.h"
#include "class_Population_Status.h"
#include "class_Virus.h"
#include "struct_List.h"
#include "struct_List_Node.h"
#include "struct_Hash_Table.h"

#define OK 0
#define ERROR !OK
#define DEFAULT_BUCKET_COUNT 1000  /* Rule of thumb: logN, where N is the expected number of records */

using namespace std;

bool Configure(const int& argc, char** argv, string& citizenRecordsFile, unsigned int& bloomSize);

void PrintProperExec();

bool InputFromFile(const string& filename, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize);

bool InputFromLine(string line, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize);

country *RegisterCountry(const string& countryName, list& countries);

citizen *RegisterCitizen(const string& citizenID, const string& firstName, const string& lastName, const country& cntr, const unsigned int& age, hashTable& citizens);

virus *RegisterVirus(const string& virusName, const unsigned int& bloomSize, list& viruses);

string CreateAttribute(string& line, const char& delimiter = ' ');

tm CreateDate(string& line);

void PrintDate(const tm& date);

void ReceiveCommands(hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize);

void PrintProperUse();

unsigned int CountArguments(const string& command);

void VaccineStatusBloom(const string& citizenID, const string& virusName, list& viruses);

void VaccineStatus(const string& citizenID, const string& virusName, list& viruses);

void VaccineStatuses(const string& citizenID, list& viruses);

void PopulationStatus(const string& countryName, const string& virusName, tm *date1, tm *date2, list& viruses);

void PopulationStatuses(const string& virusName, tm *date1, tm *date2, list& viruses);

void PopStatusByAge(const string& countryName, const string& virusName, tm *date1, tm *date2, list& viruses);

void PopStatusesByAge(const string& virusName, tm *date1, tm *date2, list& viruses);

void InsertCitizenRecord(const string& citizenID, const string& firstName, const string& lastName, const string& countryName, const unsigned int& age, const string& virusName, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize, const tm *date = NULL);

void ListNonVaccinatedPersons(const string& virusName, list& viruses);

int main (int argc, char** argv) {
	string citizenRecordsFile;
	unsigned int bloomSize;
	if (Configure(argc, argv, citizenRecordsFile, bloomSize) == ERROR) {
		PrintProperExec();
		return ERROR;
	}
	hashTable citizens(DEFAULT_BUCKET_COUNT);
	list countries;
	list viruses;
	if (InputFromFile(citizenRecordsFile, citizens, countries, viruses, bloomSize) == ERROR) {
		cout << "ERROR: THERE IS NOT FILE " << citizenRecordsFile << " IN THIS DIRECTORY\n";
		return ERROR;
	}
	ReceiveCommands(citizens, countries, viruses, bloomSize);
	return OK;
}

bool Configure(const int& argc, char** argv, string& citizenRecordsFile, unsigned int& bloomSize) {
	if (argc == 5) {
		srand(time(NULL));
		for (unsigned i = 1; i < argc; i += 2) {
			if (string(argv[i]) == "-c")
				citizenRecordsFile = string(argv[i + 1]);
			else if (string(argv[i]) == "-b")
				bloomSize = atoi(argv[i + 1]);
		}
		return OK;
	}
	return ERROR;
}

void PrintProperExec() {
	cout << "Please try: ./vaccineMonitor -c citizenRecordsFile -b bloomSize\n";
}

bool InputFromFile(const string& filename, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize) {
	ifstream file(filename.c_str());
	if (file.is_open()) {
		cout << "Loading records...\n\n";
		string line;
		while(getline(file, line)) {
			//cout << line << "\n";
			if (InputFromLine(line, citizens, countries, viruses, bloomSize) == ERROR)
				cout << "ERROR IN RECORD " << line << "\n";
		}
		file.close();
		return OK;
	}
	return ERROR;
}

bool InputFromLine(string line, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize) {
	string citizenID = CreateAttribute(line);
	string firstName = CreateAttribute(line);
	string lastName = CreateAttribute(line);
	string countryName = CreateAttribute(line);
	unsigned int age = atoi(CreateAttribute(line).c_str());
	string virusName = CreateAttribute(line);
	string vaccinated = CreateAttribute(line);
	
	country *cntrPtr = RegisterCountry(countryName, countries);
	citizen *ctznPtr = RegisterCitizen(citizenID, firstName, lastName, *cntrPtr, age, citizens);
	virus *vrsPtr = RegisterVirus(virusName, bloomSize, viruses);
	
	if (vaccinated == "YES") {
		tm date = CreateDate(line);
		vrsPtr->Vaccinated(*ctznPtr, date);
	}
	else if (vaccinated == "NO") {
		if (line != "")
			return ERROR;
		vrsPtr->NotVaccinated(*ctznPtr);
	}
	else
		return ERROR;
	return OK;
}

country *RegisterCountry(const string& countryName, list& countries) {
	country cntrTemp(countryName);
	nodeData *dataPtr = countries.Search(cntrTemp);
	if (dataPtr == NULL) {
		countries.Insert(cntrTemp);
		dataPtr = countries.Search(cntrTemp);
	}
	country *cntrPtr = dynamic_cast<country *>(dataPtr);
	return cntrPtr;
}

citizen *RegisterCitizen(const string& citizenID, const string& firstName, const string& lastName, const country& cntr, const unsigned int& age, hashTable& citizens) {
	citizen ctznTemp(citizenID, firstName, lastName, cntr, age);
	nodeData *dataPtr = citizens.Search(ctznTemp);
	if (dataPtr == NULL) {
		citizens.Insert(ctznTemp);
		dataPtr = citizens.Search(ctznTemp);
	}
	citizen *ctznPtr = dynamic_cast<citizen *>(dataPtr);
	return ctznPtr;
}

virus *RegisterVirus(const string& virusName, const unsigned int& bloomSize, list& viruses) {
	virus vrsTemp(virusName, bloomSize);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (viruses.Search(vrsTemp) == NULL) {
		viruses.Insert(vrsTemp);
		dataPtr = viruses.Search(vrsTemp);
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	return vrsPtr;
}

string CreateAttribute(string& line, const char& delimiter) {  /* Extract substring before delimiter */
	unsigned int attrLength = line.find_first_of(delimiter);
	string attribute = line.substr(0, attrLength);
	if (line.length() > attrLength)
		line = line.substr(attrLength + 1);
	else
		line = "";
	return attribute;
}

tm CreateDate(string& line) {
	tm date = {0};
	date.tm_mday = atoi(CreateAttribute(line, '-').c_str());
	date.tm_mon = atoi(CreateAttribute(line, '-').c_str()) - 1;
	date.tm_year = atoi(CreateAttribute(line, '-').c_str()) - 1900;
	return date;
}

void PrintDate(const tm& date) {
	cout << date.tm_mday << "-" << 1 + date.tm_mon << "-" << 1900 + date.tm_year;
}

void ReceiveCommands(hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize) {
	time_t now = time(0);
	tm *todaysDate = localtime(&now);  /* Current date */
	cout << "Give any command:\n";
	string command;
	while (getline(cin, command)) {
		unsigned int argCount = CountArguments(command);
		istringstream sstream(command);
		string function, argument1, argument2, argument3, argument4, argument5, argument6, argument7, argument8;
		sstream >> function >> argument1 >> argument2 >> argument3 >> argument4 >> argument5 >> argument6 >> argument7 >> argument8;
		if (function == "/vaccineStatusBloom") {
			if (argCount == 2)
				VaccineStatusBloom(argument1, argument2, viruses);
			else
				PrintProperUse();
		}
		else if (function == "/vaccineStatus") {
			if (argCount == 1) {
				VaccineStatuses(argument1, viruses);
			}
			else if (argCount == 2)
				VaccineStatus(argument1, argument2, viruses);
			else
				PrintProperUse();
		}
		else if (function == "/populationStatus") {
			if (argCount == 3) {
				tm *date1 = new tm(CreateDate(argument2));
				tm *date2 = new tm(CreateDate(argument3));
				PopulationStatuses(argument1, date1, date2, viruses);
				delete date1;
				delete date2;
			}
			else if (argCount == 4) {
				tm *date1 = new tm(CreateDate(argument3));
				tm *date2 = new tm(CreateDate(argument4));
				PopulationStatus(argument1, argument2, date1, date2, viruses);
				delete date1;
				delete date2;
			}
			else
				PrintProperUse();
		}
		else if (function == "/popStatusByAge") {
			if (argCount == 3) {
				tm *date1 = new tm(CreateDate(argument2));
				tm *date2 = new tm(CreateDate(argument3));
				PopStatusesByAge(argument1, date1, date2, viruses);
				delete date1;
				delete date2;
			}
			else if (argCount == 4) {
				tm *date1 = new tm(CreateDate(argument3));
				tm *date2 = new tm(CreateDate(argument4));
				PopStatusByAge(argument1, argument2, date1, date2, viruses);
				delete date1;
				delete date2;
			}
			else
				PrintProperUse();
		}
		else if (function == "/insertCitizenRecord") {
			if (argCount == 7 && argument7 == "NO") {
				unsigned int age = atoi(CreateAttribute(argument5).c_str());
				InsertCitizenRecord(argument1, argument2, argument3, argument4, age, argument6, citizens, countries, viruses, bloomSize);
			}
			else if (argCount == 8 && argument7 == "YES") {
				unsigned int age = atoi(CreateAttribute(argument5).c_str());
				tm *date = new tm(CreateDate(argument8));
				InsertCitizenRecord(argument1, argument2, argument3, argument4, age, argument6, citizens, countries, viruses, bloomSize, date);
				delete date;
			}
			else
				PrintProperUse();
		}
		else if (function == "/vaccinateNow") {
			if (argCount == 6) {
				unsigned int age = atoi(CreateAttribute(argument5).c_str());
				InsertCitizenRecord(argument1, argument2, argument3, argument4, age, argument6, citizens, countries, viruses, bloomSize, todaysDate);
			}
			else
				PrintProperUse();
		}
		else if (function == "/list-nonVaccinated-Persons") {
			if (argCount == 1)
				ListNonVaccinatedPersons(argument1, viruses);
			else
				PrintProperUse();
		}
		else if (function == "/exit") {
			if (argCount == 0)
				return;
			else
				PrintProperUse();
		}
		else
			PrintProperUse();
	}
}

void PrintProperUse() {
	cout << "Please try one of the following:\n"
				<< "/vaccineStatusBloom citizenID virusName\n"
				<< "/vaccineStatus citizenID virusName\n"
				<< "/vaccineStatus citizenID\n"
				<< "/populationStatus [country] virusName date1 date2\n"
				<< "/popStatusByAge [country] virusName date1 date2\n"
				<< "/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]\n"
				<< "/vaccinateNow citizenID firstName lastName country age virusName\n"
				<< "/list-nonVaccinated-Persons virusName\n"
				<< "/exit\n";
}

unsigned int CountArguments(const string& command) {
	istringstream sstream(command);
	string function, argument;
	sstream >> function;
	unsigned int count = 0;
	do {
		argument = "";
		sstream >> argument;
		if (!argument.empty())
			count++;
	} while(!argument.empty());
	return count;
}

void VaccineStatusBloom(const string& citizenID, const string& virusName, list& viruses) {
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << "NOT VACCINATED\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	if (vrsPtr->SearchBloom(citizenID))
		cout << "MAYBE\n";
	else
		cout << "NOT VACCINATED\n";
}

void VaccineStatus(const string& citizenID, const string& virusName, list& viruses) {
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << "NOT VACCINATED\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	const tm *date = vrsPtr->SearchVaccinatedPersons(citizenID);
	if (date == NULL)
		cout << "NOT VACCINATED\n";
	else {
		cout << "VACCINATED ON ";
		PrintDate(*date);
		cout << "\n";
	}
}

void VaccineStatuses(const string& citizenID, list& viruses) {
	const listNode *node = viruses.GetHead();
	unsigned int count = 0;
	while (node != NULL) {
		const nodeData *dataPtr = node->GetData();
		const virus *vrsPtr = dynamic_cast<const virus *>(dataPtr);
		const tm *date = vrsPtr->SearchVaccinatedPersons(citizenID);
		if (date != NULL) {
			cout << vrsPtr->GetName() << " YES ";
			PrintDate(*date);
			cout << "\n";
			count++;
		}
		else if (vrsPtr->SearchNotVaccinatedPersons(citizenID)) {
			cout << vrsPtr->GetName() << " NO\n";
			count++;
		}
		node = node->GetNext();
	}
	if (count == 0)
		cout << "NOT VACCINATED AGAINST ANY VIRUS\n";
}

void PopulationStatus(const string& countryName, const string& virusName, tm *date1, tm *date2, list& viruses) {
	if (difftime(mktime(date1), mktime(date2)) > 0) {
		cout << "ERROR: date1 CANNOT BE AFTER date2\n";
		return;
	}
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << countryName << " 0 0%\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	vrsPtr->PrintPopulationStatus(countryName, date1, date2);
}

void PopulationStatuses(const string& virusName, tm *date1, tm *date2, list& viruses) {
	if (difftime(mktime(date1), mktime(date2)) > 0) {
		cout << "ERROR: date1 CANNOT BE AFTER date2\n";
		return;
	}
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << "NO VACCINATIONS AGAINST " << virusName << " AT ANY COUNTRY\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	vrsPtr->PrintPopulationStatuses(date1, date2);
}

void PopStatusByAge(const string& countryName, const string& virusName, tm *date1, tm *date2, list& viruses) {
	if (difftime(mktime(date1), mktime(date2)) > 0) {
		cout << "ERROR: date1 CANNOT BE AFTER date2\n";
		return;
	}
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << countryName << "\n" << "0-20 0 0%\n" << "20-40 0 0%\n" << "40-60 0 0%\n" << "60+ 0 0%\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	vrsPtr->PrintPopStatusByAge(countryName, date1, date2);
}

void PopStatusesByAge(const string& virusName, tm *date1, tm *date2, list& viruses) {
	if (difftime(mktime(date1), mktime(date2)) > 0) {
		cout << "ERROR: date1 CANNOT BE AFTER date2\n";
		return;
	}
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << "NO VACCINATIONS AGAINST " << virusName << " AT ANY COUNTRY\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	vrsPtr->PrintPopStatusesByAge(date1, date2);
}

void InsertCitizenRecord(const string& citizenID, const string& firstName, const string& lastName, const string& countryName, const unsigned int& age, const string& virusName, hashTable& citizens, list& countries, list& viruses, const unsigned int& bloomSize, const tm *date) {
	country *cntrPtr = RegisterCountry(countryName, countries);
	citizen *ctznPtr = RegisterCitizen(citizenID, firstName, lastName, *cntrPtr, age, citizens);
	virus *vrsPtr = RegisterVirus(virusName, bloomSize, viruses);
	vrsPtr->InsertCitizenRecord(*ctznPtr, date);
}

void ListNonVaccinatedPersons(const string& virusName, list& viruses) {
	virus vrsTemp(virusName, 0);
	nodeData *dataPtr = viruses.Search(vrsTemp);
	if (dataPtr == NULL) {  /* Not registered virus */
		cout << "NON VACCINATED LIST FOR " << virusName << " IS EMPTY\n";
		return;
	}
	virus *vrsPtr = dynamic_cast<virus *>(dataPtr);
	vrsPtr->PrintNotVaccinatedList();
}
