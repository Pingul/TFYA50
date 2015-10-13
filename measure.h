#ifndef _MEASURE_
#define _MEASURE_

#include <vector>

class MDBox;
class Atom;

class Measure
{
	public:
		Measure() = default;
		virtual ~Measure() = default;

		virtual void calculate(double t, const MDBox&) = 0;
		virtual void saveToFile(const std::string&);

	protected:
		// Accessor functions that are necessary in order to use the friendship property of the MDBox
		const std::vector<Atom*>& atoms(const MDBox&);
		const std::vector<std::vector<Atom*>>& verletList(const MDBox&);
	
		std::vector<double> timestamps;
		std::vector<double> values;
};

class KineticEnergy : public Measure
{
	public:
		KineticEnergy() = default;
		virtual ~KineticEnergy() = default;

		virtual void calculate(double, const MDBox&);
	private:
};

#endif