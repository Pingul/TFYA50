#ifndef _MEASURE_
#define _MEASURE_

#include <vector>
#include <map>
#include <string>

class MDBox;
class Atom;
class Vector3;
class SimulationParams;

class Measure
{
	public:
		Measure() = default;
		virtual ~Measure() = default;

		virtual std::string name() { return "UNDEFINED"; }
		virtual void calculate(double t, const SimulationParams&, const MDBox&) = 0;
		virtual void saveToFile(const std::string&);

	protected:
		// Accessor functions that are necessary in order to use the friendship property of the MDBox
		const std::vector<Atom*>& atoms(const MDBox&);
		// Cannot use the typedef VerletList here apparently
		const std::vector<std::vector<std::pair<Atom*, Vector3>>>& verletList(const MDBox&);
	
		std::vector<double> timestamps;
		std::vector<double> values;
};

class AggregateMeasure : public Measure
{
	public:
		AggregateMeasure() = default;
		virtual ~AggregateMeasure() = default;

		virtual void addDependency(std::string, const Measure&);

	protected:
		virtual double dValue(std::string, double); // get value from dependency
		std::map<std::string, const Measure*> dependencies;
};

class TotalEnergy : public AggregateMeasure
{
	public:
		TotalEnergy() = default;
		virtual ~TotalEnergy() = default;

		virtual std::string name() { return "totalEnergy"; }
		virtual std::vector<std::string> dependencies() { return std::vector<std::string>{"KineticEnergy", "PotentialEnergy"}; } 
		virtual void calculate(double, const SimulationParams& params, const MDBox& box);
};


class KineticEnergy : public Measure
{
	public:
		KineticEnergy() = default;
		virtual ~KineticEnergy() = default;

		virtual std::string name() { return "kinetic"; }
		virtual void calculate(double t, const SimulationParams&, const MDBox&);
};

class PotentialEnergy : public Measure
{
	public:
		PotentialEnergy() = default;
		virtual ~PotentialEnergy() = default;

		virtual std::string name() { return "potential"; }
		virtual void calculate(double, const SimulationParams&, const MDBox&);
};
#endif