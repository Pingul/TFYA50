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

		virtual void calculate(const MDBox&) = 0;
		//virtual void writeToFile(const std::string&) = 0;
		virtual double measureValue() { return value; }

	protected:
		// Accessor functions that are necessary in order to use the friendship property of the MDBox
		const std::vector<Atom*>& atoms(const MDBox&);
		const std::vector<std::vector<Atom*>>& verletList(const MDBox&);

		double value;
};

class KineticEnergy : public Measure
{
	public:
		KineticEnergy() = default;
		virtual ~KineticEnergy() = default;

		virtual void calculate(const MDBox&);
	private:
		std::vector<double> values;
};

#endif