/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2016, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include "maps-precomp.h" // Precomp header
#include <mrpt/maps/CHeightGridMap2D_MRF.h>
#include <mrpt/poses/CPose2D.h>
#include <mrpt/poses/CPose3D.h>

using namespace mrpt;
using namespace mrpt::maps;
using namespace mrpt::obs;
using namespace mrpt::utils;
using namespace mrpt::poses;
using namespace std;
using namespace mrpt::math;

//  =========== Begin of Map definition ============
MAP_DEFINITION_REGISTER("CHeightGridMap2D_MRF,dem_mrf", mrpt::maps::CHeightGridMap2D_MRF)

CHeightGridMap2D_MRF::TMapDefinition::TMapDefinition() :
	run_map_estimation_at_ctor(true),
	min_x(-2),
	max_x(2),
	min_y(-2),
	max_y(2),
	resolution(0.10f),
	mapType(mrGMRF_SD)
{
}

void CHeightGridMap2D_MRF::TMapDefinition::loadFromConfigFile_map_specific(const mrpt::utils::CConfigFileBase  &source, const std::string &sectionNamePrefix)
{
	// [<sectionNamePrefix>+"_creationOpts"]
	const std::string sSectCreation = sectionNamePrefix+string("_creationOpts");
	MRPT_LOAD_CONFIG_VAR(run_map_estimation_at_ctor, bool,   source,sSectCreation);
	MRPT_LOAD_CONFIG_VAR(min_x, double,   source,sSectCreation);
	MRPT_LOAD_CONFIG_VAR(max_x, double,   source,sSectCreation);
	MRPT_LOAD_CONFIG_VAR(min_y, double,   source,sSectCreation);
	MRPT_LOAD_CONFIG_VAR(max_y, double,   source,sSectCreation);
	MRPT_LOAD_CONFIG_VAR(resolution, double,   source,sSectCreation);
	mapType = source.read_enum<CHeightGridMap2D_MRF::TMapRepresentation>(sSectCreation,"mapType",mapType);

	insertionOpts.loadFromConfigFile(source, sectionNamePrefix+string("_insertOpts") );
}

void CHeightGridMap2D_MRF::TMapDefinition::dumpToTextStream_map_specific(mrpt::utils::CStream &out) const
{
	out.printf("MAP TYPE                                  = %s\n", mrpt::utils::TEnumType<CHeightGridMap2D_MRF::TMapRepresentation>::value2name(mapType).c_str() );
	LOADABLEOPTS_DUMP_VAR(run_map_estimation_at_ctor , bool);
	LOADABLEOPTS_DUMP_VAR(min_x         , double);
	LOADABLEOPTS_DUMP_VAR(max_x         , double);
	LOADABLEOPTS_DUMP_VAR(min_y         , double);
	LOADABLEOPTS_DUMP_VAR(max_y         , double);
	LOADABLEOPTS_DUMP_VAR(resolution         , double);

	this->insertionOpts.dumpToTextStream(out);
}

mrpt::maps::CMetricMap* CHeightGridMap2D_MRF::internal_CreateFromMapDefinition(const mrpt::maps::TMetricMapInitializer &_def)
{
	const CHeightGridMap2D_MRF::TMapDefinition &def = *dynamic_cast<const CHeightGridMap2D_MRF::TMapDefinition*>(&_def);
	CHeightGridMap2D_MRF *obj = new CHeightGridMap2D_MRF(def.mapType,def.min_x,def.max_x,def.min_y,def.max_y,def.resolution,def.run_map_estimation_at_ctor);
	obj->insertionOptions  = def.insertionOpts;
	return obj;
}
//  =========== End of Map definition Block =========

IMPLEMENTS_SERIALIZABLE(CHeightGridMap2D_MRF, CRandomFieldGridMap2D,mrpt::maps)

// Constructor
CHeightGridMap2D_MRF::CHeightGridMap2D_MRF(
	TMapRepresentation mapType,
	double x_min, double x_max,
	double y_min, double y_max, double resolution,
	bool  run_first_map_estimation_now
	) :
	CRandomFieldGridMap2D(mapType, x_min,x_max,y_min,y_max,resolution ),
	insertionOptions()
{
	m_rfgm_run_update_upon_clear = run_first_map_estimation_now;
	// Set the grid to initial values (and adjusts the KF covariance matrix!)
	//  Also, calling clear() is mandatory to end initialization of our base class (read note in CRandomFieldGridMap2D::CRandomFieldGridMap2D)
	CMetricMap::clear();
}

CHeightGridMap2D_MRF::~CHeightGridMap2D_MRF()
{
}

/*---------------------------------------------------------------
						clear
  ---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::internal_clear()
{
	// Just do the generic clear:
	CRandomFieldGridMap2D::internal_clear();

	// Anything else special for this derived class?
}


/*---------------------------------------------------------------
						insertObservation
  ---------------------------------------------------------------*/
bool  CHeightGridMap2D_MRF::internal_insertObservation(
	const CObservation	*obs,
	const CPose3D		*robotPose )
{
	MRPT_START
#if 0
	CPose2D robotPose2D;
	CPose3D robotPose3D;

	if (robotPose) {
		robotPose2D = CPose2D(*robotPose);
		robotPose3D = (*robotPose);
	}
	else {
		// Default values are (0,0,0)
	}

	if ( IS_CLASS(obs, CObservation2DRangeScan ))
	{
		/********************************************************************
					OBSERVATION TYPE: CObservation2DRangeScan
		********************************************************************/
		const CObservation2DRangeScan *o = static_cast<const CObservation2DRangeScan*>( obs );
		// Finally, do the actual map update with that value:
		this->insertIndividualReading(sensorReading, mrpt::math::TPoint2D(sensorPose.x(),sensorPose.y()) );
		return true;	// Done!
	}
#endif

	return false;
	MRPT_END
}

/*---------------------------------------------------------------
						computeObservationLikelihood
  ---------------------------------------------------------------*/
double	 CHeightGridMap2D_MRF::internal_computeObservationLikelihood(
	const CObservation		*obs,
	const CPose3D			&takenFrom )
{
	MRPT_UNUSED_PARAM(obs);MRPT_UNUSED_PARAM(takenFrom);
	THROW_EXCEPTION("Not implemented yet!");
}

/*---------------------------------------------------------------
  Implements the writing to a CStream capability of CSerializable objects
 ---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::writeToStream(mrpt::utils::CStream &out, int *version) const
{
	if (version)
		*version = 0;
	else
	{
		dyngridcommon_writeToStream(out);

		// To assure compatibility: The size of each cell:
		uint32_t n = static_cast<uint32_t>(sizeof( TRandomFieldCell ));
		out << n;

		// Save the map contents:
		n = static_cast<uint32_t>(m_map.size());
		out << n;

		// Save the "m_map": This requires special handling for big endian systems:
#if MRPT_IS_BIG_ENDIAN
		for (uint32_t i=0;i<n;i++)
		{
			out << m_map[i].kf_mean << m_map[i].dm_mean << m_map[i].dmv_var_mean;
		}
#else
		// Little endian: just write all at once:
		out.WriteBuffer( &m_map[0], sizeof(m_map[0])*m_map.size() );  // TODO: Do this endianness safe!!
#endif


		// Save the insertion options:
		out << uint8_t(m_mapType)
			<< m_cov
			<< m_stackedCov;

		out << insertionOptions.sigma
			<< insertionOptions.cutoffRadius
			<< insertionOptions.R_min
			<< insertionOptions.R_max
			<< insertionOptions.KF_covSigma
			<< insertionOptions.KF_initialCellStd
			<< insertionOptions.KF_observationModelNoise
			<< insertionOptions.KF_defaultCellMeanValue
			<< insertionOptions.KF_W_size;

		out << m_average_normreadings_mean << m_average_normreadings_var << uint64_t(m_average_normreadings_count);

		out << genericMapParams;
	}
}

/*---------------------------------------------------------------
  Implements the reading from a CStream capability of CSerializable objects
 ---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::readFromStream(mrpt::utils::CStream &in, int version)
{
	switch(version)
	{
	case 0:
		{
			dyngridcommon_readFromStream(in);

			// To assure compatibility: The size of each cell:
			uint32_t n;
			in >> n;

			ASSERT_EQUAL_( n , static_cast<uint32_t>( sizeof( TRandomFieldCell ) ));
			// Load the map contents:
			in >> n;
			m_map.resize(n);

			// Read the note in writeToStream()
#if MRPT_IS_BIG_ENDIAN
			for (uint32_t i=0;i<n;i++)
				in >> m_map[i].kf_mean >> m_map[i].dm_mean >> m_map[i].dmv_var_mean;
#else
			// Little endian: just read all at once:
			in.ReadBuffer( &m_map[0], sizeof(m_map[0])*m_map.size() );
#endif

			{
				uint8_t	i;
				in  >> i;
				m_mapType = TMapRepresentation(i);

				in	>> m_cov
					>> m_stackedCov;

				in  >> insertionOptions.sigma
					>> insertionOptions.cutoffRadius
					>> insertionOptions.R_min
					>> insertionOptions.R_max
					>> insertionOptions.KF_covSigma
					>> insertionOptions.KF_initialCellStd
					>> insertionOptions.KF_observationModelNoise
					>> insertionOptions.KF_defaultCellMeanValue
					>> insertionOptions.KF_W_size;
			}

			{
				uint64_t N;
				in >> m_average_normreadings_mean >> m_average_normreadings_var >> N;
				m_average_normreadings_count = N;
			}

			in >> genericMapParams;

			m_hasToRecoverMeanAndCov = true;
		} break;
	default:
		MRPT_THROW_UNKNOWN_SERIALIZATION_VERSION(version)

	};

}

/*---------------------------------------------------------------
					TInsertionOptions
 ---------------------------------------------------------------*/
CHeightGridMap2D_MRF::TInsertionOptions::TInsertionOptions()
{
}

/*---------------------------------------------------------------
					dumpToTextStream
  ---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::TInsertionOptions::dumpToTextStream(mrpt::utils::CStream	&out) const
{
	out.printf("\n----------- [CHeightGridMap2D_MRF::TInsertionOptions] ------------ \n\n");
	out.printf("[TInsertionOptions.Common] ------------ \n\n");
	internal_dumpToTextStream_common(out);  // Common params to all random fields maps:

//	out.printf("[TInsertionOptions.CHeightGridMap2D_MRF] ------------ \n\n");
//	out.printf("std_windNoise_phi						= %f\n", std_windNoise_phi);

	out.printf("\n");
}

/*---------------------------------------------------------------
					loadFromConfigFile
  ---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::TInsertionOptions::loadFromConfigFile(
	const mrpt::utils::CConfigFileBase  &iniFile,
	const std::string &section)
{
	// Common data fields for all random fields maps:
	internal_loadFromConfigFile_common(iniFile,section);

	// Specific data fields for this class:
	// ...
}

/*---------------------------------------------------------------
						getAs3DObject
---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::getAs3DObject( mrpt::opengl::CSetOfObjectsPtr	&outObj) const
{
	MRPT_START
	if (!genericMapParams.enableSaveAs3DObject) return;
	CRandomFieldGridMap2D::getAs3DObject(outObj);
	MRPT_END
}

/*---------------------------------------------------------------
						getAs3DObject
---------------------------------------------------------------*/
void  CHeightGridMap2D_MRF::getAs3DObject(
	mrpt::opengl::CSetOfObjectsPtr	&meanObj,
	mrpt::opengl::CSetOfObjectsPtr	&varObj ) const
{
	MRPT_START
	if (!genericMapParams.enableSaveAs3DObject) return;
	CRandomFieldGridMap2D::getAs3DObject(meanObj,varObj);
	MRPT_END
}

