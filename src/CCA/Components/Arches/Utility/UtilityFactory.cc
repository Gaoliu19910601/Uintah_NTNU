#include <CCA/Components/Arches/Utility/UtilityFactory.h>
#include <CCA/Components/Arches/Utility/GridInfo.h>
#include <CCA/Components/Arches/Utility/TaskAlgebra.h>
#include <CCA/Components/Arches/Utility/SurfaceNormals.h>
#include <CCA/Components/Arches/Utility/SurfaceVolumeFractionCalc.h>
#include <CCA/Components/Arches/Utility/MassFlowRate.h>
#include <CCA/Components/Arches/Task/TaskInterface.h>
#include <CCA/Components/Arches/Transport/PressureEqn.h>
#include <CCA/Components/Arches/Utility/ForcingTurbulence.h>

using namespace Uintah;

UtilityFactory::UtilityFactory( const ApplicationCommon* arches ) :
TaskFactoryBase(arches)
{
  _factory_name = "UtilityFactory";
}

UtilityFactory::~UtilityFactory()
{}

void
UtilityFactory::register_all_tasks( ProblemSpecP& db )
{

  //GRID INFORMATION
  std::string tname = "grid_info";
  TaskInterface::TaskBuilder* tsk = scinew GridInfo::Builder( tname, 0 );
  _base_tasks.push_back(tname);
  register_task( tname, tsk, db );

  if ( db->findBlock("KokkosSolver")){
    tname = "vol_fraction_calc";
    tsk = scinew SurfaceVolumeFractionCalc::Builder( tname, 0 );
    _base_tasks.push_back(tname);
    register_task( tname, tsk, db );
  }
  
  tname = "surface_normals";
  tsk = scinew SurfaceNormals::Builder( tname, 0 );
  _base_tasks.push_back(tname);
  register_task( tname, tsk, db );


  ProblemSpecP db_all_util = db->findBlock("Utilities");

  //<Utilities  >
  if ( db_all_util ){
    for ( ProblemSpecP db_util = db_all_util->findBlock("utility"); db_util != nullptr;
          db_util = db_util->findNextBlock("utility")){

      std::string name;
      std::string type;
      db_util->getAttribute("label", name);
      db_util->getAttribute("type", type);

      if ( type == "variable_math" ){

        std::string grid_type="NA";
        if ( db_util->findBlock("grid") != nullptr ){
          db_util->findBlock("grid")->getAttribute("type", grid_type);
        }
        else {
          throw InvalidValue("Error: variable_math must specify a grid variable type", __FILE__, __LINE__);
        }

        TaskInterface::TaskBuilder* tsk;
        if ( grid_type == "CC" ){
          tsk = scinew TaskAlgebra<CCVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FX" ){
          tsk = scinew TaskAlgebra<SFCXVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FY" ){
          tsk = scinew TaskAlgebra<SFCYVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FZ" ){
          tsk = scinew TaskAlgebra<SFCZVariable<double> >::Builder(name, 0);
        }
        else {
          throw InvalidValue("Error: grid_type not recognized.",__FILE__,__LINE__);
        }
        _base_tasks.push_back(name);
        register_task(name, tsk, db_util);

      } else if ( type == "mass_flow_rate" ){

        tsk = scinew MassFlowRate::Builder( name, 0 );
        _mass_flow_rate.push_back(name);
        register_task( name, tsk, db_util );

      } else if ( type == "poisson"){

        std::string press_name = "[PressureEqn]";
        tsk = scinew PressureEqn::Builder(press_name, 0, _materialManager);
        register_task( press_name, tsk, db_util);

      } else if ( type == "forcing_turbulence" ){

        std::string task_name = "forced_turbulence";
        tsk = scinew ForcingTurbulence::Builder( task_name, 0 );
        register_task( task_name , tsk, db_util );

      } else {

        throw InvalidValue("Error: Utility type not recognized.",__FILE__,__LINE__);
      }

      assign_task_to_type_storage(name, type);
    }
  }
}

void
UtilityFactory::add_task( ProblemSpecP& db ){

  ProblemSpecP db_all_util = db->findBlock("Utilities");

  //<Utilities>
  if ( db_all_util ){
    for ( ProblemSpecP db_util = db_all_util->findBlock("utility"); db_util != nullptr; db_util = db_util->findNextBlock("utility")){

      std::string name;
      std::string type;
      db_util->getAttribute("label", name);
      db_util->getAttribute("type", type);

      if ( type == "variable_math" ){

        std::string grid_type="NA";
        if ( db_util->findBlock("grid") != nullptr ){
          db_util->findBlock("grid")->getAttribute("type", grid_type);
        }
        else {
          throw InvalidValue("Error: variable_math must specify a grid variable type", __FILE__, __LINE__);
        }

        TaskInterface::TaskBuilder* tsk;
        if ( grid_type == "CC" ){
          tsk = scinew TaskAlgebra<CCVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FX" ){
          tsk = scinew TaskAlgebra<SFCXVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FY" ){
          tsk = scinew TaskAlgebra<SFCYVariable<double> >::Builder(name, 0);
        }
        else if ( grid_type == "FZ" ){
          tsk = scinew TaskAlgebra<SFCZVariable<double> >::Builder(name, 0);
        }
        else {
          throw InvalidValue("Error: grid_type not recognized.",__FILE__,__LINE__);
        }
        register_task(name, tsk, db_util);

      }
      else {
        throw InvalidValue("Error: Utility type not recognized.",__FILE__,__LINE__);
      }

      assign_task_to_type_storage(name, type);
    }
  }
}

void
UtilityFactory::build_all_tasks( ProblemSpecP& db )
{
}
