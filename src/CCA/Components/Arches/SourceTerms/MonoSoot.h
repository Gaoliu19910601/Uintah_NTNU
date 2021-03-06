#ifndef Uintah_Component_Arches_MonoSoot_h
#define Uintah_Component_Arches_MonoSoot_h

#include <Core/ProblemSpec/ProblemSpec.h>
#include <Core/Grid/MaterialManagerP.h>
#include <CCA/Components/Arches/SourceTerms/SourceTermBase.h>
#include <CCA/Components/Arches/SourceTerms/SourceTermFactory.h>
#include <CCA/Components/Arches/ArchesLabel.h>
#include <CCA/Components/Arches/Directives.h>

/**
* @class  MonoSoot, 2018 Reduction of a Detailed Soot Model for Simulation of Pyrolyzing Solid Fuel Combustion
* @author Alexander Josephson
* @date   March 2018 
*
* @brief Computes the soot formation and tar source terms for solid fuel flames.
*
* Alexander Josephson, Emily Hopkins, David Lignell, and Rod Linn, Combustion Theory and Modelling
*
* The input file interface for this property should like this in your UPS file:
* \code
*   <Sources>
*     <src label = "my_source" type = "BrownSootFromation_Tar" >
        <!-- Brown Soot Source Term -->
        <tar_label                  spec="OPTIONAL STRING" need_applies_to="type brown_soot"/> <!-- mass fraction label for tar (default = Tar) -->
        <A                          spec="REQUIRED DOUBLE" need_applies_to="type brown_soot"/> <!-- Pre-exponential factor -->
        <E_R                        spec="REQUIRED DOUBLE" need_applies_to="type brown_soot"/> <!-- Activation temperature, code multiplies the -1!! -->
        <o2_label                   spec="OPTIONAL STRING" need_applies_to="type brown_soot"/> <!-- o2 label (default = O2) -->
        <temperature_label          spec="OPTIONAL STRING" need_applies_to="type brown_soot"/> <!-- temperature label (default = temperature) -->
        <density_label              spec="OPTIONAL STRING" need_applies_to="type brown_soot"/> <!-- density label (default = "density) -->
        <num_density_src            spec="REQUIRED NO_DATA"
                                    attribute1="label REQUIRED STRING"
                                    need_applies_to="type brown_soot"/> <!-- User defined label for the num density src
                                                                        (as generated by this model) -->
        <soot_mass_src              spec="REQUIRED NO_DATA"
                                    attribute1="label REQUIRED STRING"
                                    need_applies_to="type brown_soot"/> <!-- User defined label for the soot mass src
                                                                        (as generated by this model) -->
        <mass_balance_src           spec="REQUIRED NO_DATA"
                                    attribute1="label REQUIRED STRING"
                                    need_applies_to="type brown_soot"/> <!-- User defined label for the soot mass balance src
                                                                        (as generated by this model) -->
      </src>
    </Sources>
* \endcode
*
*/
namespace Uintah{

class MonoSoot: public SourceTermBase {
public:

  MonoSoot( std::string srcName, ArchesLabel* field_labels,
             std::vector<std::string> reqLabelNames, std::string type );

  ~MonoSoot();
  /** @brief Interface for the inputfile and set constants */
  void problemSetup(const ProblemSpecP& db);
  /** @brief Schedule the calculation of the source term */
  void sched_computeSource( const LevelP& level, SchedulerP& sched,
                            int timeSubStep );
  /** @brief Actually compute the source term */
  void computeSource( const ProcessorGroup* pc,
                      const PatchSubset* patches,
                      const MaterialSubset* matls,
                      DataWarehouse* old_dw,
                      DataWarehouse* new_dw,
                      int timeSubStep );
  /** @brief Schedule initialization */
  void sched_initialize( const LevelP& level, SchedulerP& sched );
  void initialize( const ProcessorGroup* pc,
                   const PatchSubset* patches,
                   const MaterialSubset* matls,
                   DataWarehouse* old_dw,
                   DataWarehouse* new_dw );

  class Builder
    : public SourceTermBase::Builder {

    public:

      Builder( std::string name, std::vector<std::string> required_label_names, ArchesLabel* field_labels )
        : _name(name), _field_labels(field_labels), _required_label_names(required_label_names){
          _type = "MonoSoot";
        };
      ~Builder(){};

      MonoSoot* build()
      { return scinew MonoSoot( _name, _field_labels, _required_label_names, _type ); };

    private:

      std::string _name;
      std::string _type;
      ArchesLabel* _field_labels;
      std::vector<std::string> _required_label_names;

  }; // Builder


private:

  ArchesLabel* _field_labels;

  double m_sys_pressure;

  std::string m_nd_name;
  std::string m_soot_mass_name;
  std::string m_balance_name;

  std::string m_mix_mol_weight_name; ///< string name for the average molecular weight (from table)
  std::string m_tar_name;            ///< string name for tar
  std::string m_tar_src_name;        ///< string name for tar src 
  std::string m_Ysoot_name;          ///< string name for Ysoot
  std::string m_Ns_name;             ///< string name for Ns (#/kg)
  std::string m_O2_name;             ///< string name for o2  (from table)
  std::string m_OH_name;             ///< string name for oh  (from table)
  std::string m_CO2_name;            ///< string name for co2  (from table)
  std::string m_H2O_name;            ///< string name for h2o (from table)
  std::string m_H_name;              ///< string name for h (from table)
  std::string m_H2_name;             ///< string name for h2 (from table)
  std::string m_C2H2_name;             ///< string name for h2 (from table)
  std::string m_rho_name;            ///< string name for rho (from table)
  std::string m_temperature_name;    ///< string name for temperature (from table)

  const VarLabel* m_tar_src_label;
  const VarLabel* m_nd_src_label;
  const VarLabel* m_soot_mass_src_label;
  const VarLabel* m_balance_src_label;
  const VarLabel* m_mix_mol_weight_label;
  const VarLabel* m_tar_label;
  const VarLabel* m_Ysoot_label;
  const VarLabel* m_Ns_label;
  const VarLabel* m_o2_label;
  const VarLabel* m_oh_label;
  const VarLabel* m_co2_label;
  const VarLabel* m_h2o_label;
  const VarLabel* m_h_label;
  const VarLabel* m_h2_label;
  const VarLabel* m_c2h2_label;
  const VarLabel* m_temperature_label;
  const VarLabel* m_rho_label;

}; // end MonoSoot
} // end namespace Uintah

#endif
