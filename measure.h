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
		// return value for timestep. Returns 0.0 if no value for the timestamp could be found
		static double value(Measure* measure, double timestamp);

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

class TotalEnergy : public Measure
{
	public:
		TotalEnergy(KineticEnergy* kinetic, PotentialEnergy* potential) 
			: kineticEnergy{kinetic}, potentialEnergy{potential} {}
		TotalEnergy(PotentialEnergy* potential, KineticEnergy* kinetic) 
			: kineticEnergy{kinetic}, potentialEnergy{potential} {}
		virtual ~TotalEnergy() = default;

		virtual std::string name() { return "totalEnergy"; }
		virtual void calculate(double, const SimulationParams& params, const MDBox& box);

	private:
		KineticEnergy* kineticEnergy;
		PotentialEnergy* potentialEnergy;
};

class Temperature : public Measure
{
	public:
		Temperature(KineticEnergy* kinetic)
			: kineticEnergy{kinetic} {}
		virtual ~Temperature() = default;

		virtual std::string name() { return "temperature"; }
		virtual void calculate(double, const SimulationParams&, const MDBox&);

	private:
		KineticEnergy* kineticEnergy;

};

class MSD : public Measure
{
public:
	MSD() = default;
	virtual ~MSD() = default;

	virtual std::string name() { return "msd"; }
	virtual void calculate(double, const SimulationParams&, const MDBox&);
};


#endif