#ifndef class_Population_Status_h
#define class_Population_Status_h

#include <iostream>
#include "struct_Node_Data.h"
#include "class_Country.h"
#include "struct_List.h"
#include "class_Vaccine_Status.h"

class populationStatus :public nodeData {
		const country& cntr;
		list vaccineStatuses;
		
	public:
		populationStatus(const country& cntr);
		
		populationStatus(const populationStatus& popStatus);
		
		std::string GetCountryName() const;
		
		vaccineStatus *GetVaccineStatus(const vaccineStatus& vacStatusTemp);
		
		vaccineStatus *Insert(const vaccineStatus& vacStatusTemp);
		
		void Remove(const vaccineStatus& vacStatusTemp);
		
		void PrintPopulationStatusBetween(tm *date1, tm *date2) const;
		
		void PrintPopStatusByAgeBetween(tm *date1, tm *date2) const;
		
		virtual populationStatus *Clone() const {
			return new populationStatus(*this);
		}
		
		virtual populationStatus *Search(const nodeData& data) {
			std::string name = data.GetKey();
			if(this->cntr.GetName() == name)
				return this;
			return NULL;
		}
		
		virtual std::string GetKey() const {
			return cntr.GetName();
		}
};

#endif
