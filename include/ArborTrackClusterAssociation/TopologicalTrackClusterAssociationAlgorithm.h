  /// \file TopologicalTrackClusterAssociationAlgorithm.h
/*
 *
 * TopologicalTrackClusterAssociationAlgorithm.h header template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
 *
 * This file is part of ArborContent libraries.
 * 
 * ArborContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#ifndef TOPOLOGICALTRACKCLUSTERASSOCIATIONALGORITHM_H
#define TOPOLOGICALTRACKCLUSTERASSOCIATIONALGORITHM_H

#include "Pandora/Algorithm.h"

namespace arbor_content
{

/** 
 *  @brief  TopologicalTrackClusterAssociationAlgorithm class
 */ 
class TopologicalTrackClusterAssociationAlgorithm : public pandora::Algorithm
{
public:
	/**
	 *  @brief  Factory class for instantiating algorithm
	 */
	class Factory : public pandora::AlgorithmFactory
	{
	public:
		pandora::Algorithm *CreateAlgorithm() const;
	};

private:

	typedef std::map<const pandora::Track *, pandora::ClusterList> TrackClusterAssociationMap;

	/**
	 *  @brief  Get the closest hit position from the track extrapolation at calorimeter
	 *
	 *  @param  pTrack the track to project
	 *  @param  pCaloHitList the list in which to look the closest position
	 *
	 *  @return  the closest hit position to track extrapolation at calorimeter
	 */
	pandora::CartesianVector GetClosestHitPosition(const pandora::Track *const pTrack, const pandora::CaloHitList *pCaloHitList) const;

	/**
	 *  @brief  Perform the track association between a track (first) and a cluster list (second)
	 *
	 *  @param  trackClusterAssociationMap map of track and cluster list to associate
	 */
	pandora::StatusCode PerformTrackClusterAssociations(const TrackClusterAssociationMap &trackClusterAssociationMap);


	pandora::StatusCode Run();
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);


	 unsigned int                         m_trackClusterNLayersCut;            ///< The track to cluster N layers difference cut
	 float                                 m_trackClusterDistanceCut;          ///< The track to cluster innermost position difference cut
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TopologicalTrackClusterAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TopologicalTrackClusterAssociationAlgorithm();
}

} 

#endif  //  TOPOLOGICALTRACKCLUSTERASSOCIATIONALGORITHM_H