  /// \file MipFragmentMergingAlg.h
/*
 *
 * MipFragmentMergingAlg.h header template automatically generated by a class generator
 * Creation date : lun. juin 27 2016
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


#ifndef MIPFRAGMENTMERGINGALG_H
#define MIPFRAGMENTMERGINGALG_H

#include "Pandora/Algorithm.h"
#include "Pandora/PandoraInputTypes.h"

namespace arbor_content
{

/** 
 * @brief MipFragmentMergingAlg class
 */ 
class MipFragmentMergingAlg : public pandora::Algorithm
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
	pandora::StatusCode Run();
	pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

	typedef std::map<const pandora::Cluster *, const pandora::Cluster *> ClusterToClusterMap;

private:
	/**
	 *  @brief  Find mip cluster fragments and their most probable parent cluster
	 *
	 *  @param  clusterVector the input cluster list
	 *  @param  clusterToClusterMap the map of mip cluster to parent cluster to receive
	 */
	pandora::StatusCode FindMipFragments(const pandora::ClusterVector &clusterVector, ClusterToClusterMap &clusterToClusterMap) const;

	/**
	 *
	 */
	pandora::StatusCode FindChargedMipFragments(const pandora::ClusterVector &clusterVector, ClusterToClusterMap &clusterToClusterMap);

	/**
	 *  @brief  Whether the cluster is a mip fragment
	 *
	 *  @param  pCluster the target cluster address
	 *
	 *  @return bool whether the cluster is a mip cluster fragment
	 */
	bool IsMipFragment(const pandora::Cluster *const pCluster) const;

	/**
	 *  @brief  Whether the cluster is a possible parent cluster to merge a mip in
	 *
	 *  @param  pCluster the target address cluster
	 *
	 *  @return bool whether the cluster is a possible parent cluster
	 */
	bool IsPossibleParent(const pandora::Cluster *const pCluster) const;

	/**
	 *  @brief  Find the best possible parent cluster for the target mip cluster
	 *
	 *  @param  pMipCluster the mip cluster address
	 *  @param  clusterVector the input list of potential parent cluster
	 *  @param  pParentCluster the parent cluster address to receive
	 */
	pandora::StatusCode FindMipParentCluster(const pandora::Cluster *const pMipCluster, const pandora::ClusterVector &clusterVector, const pandora::Cluster *&pParentCluster) const;

	/**
	 *  @brief  Merge mip clusters into their parent
	 *
	 *  @param  clusterToClusterMap the cluster map to process
	 */
	pandora::StatusCode MergeClusters(ClusterToClusterMap &clusterToClusterMap) const;

	/**
	 *  @brief  Tidy the cluster map by replacing occurrences of clusters that will be deleted
	 *
	 *  @param  pReplaceCluster the cluster address to replace
	 *  @param  pRemoveCluster the cluster address to remove from the map
	 *  @param  clusterToClusterMap the target cluster map
	 */
	pandora::StatusCode TidyClusterMap(const pandora::Cluster *const pReplaceCluster, const pandora::Cluster *const pRemoveCluster, ClusterToClusterMap &clusterToClusterMap) const;

private:
	bool                           m_discriminatePhotonPid;            ///< Whether to discriminate photons
	float                          m_maxMipTransverseRatio;            ///< The min transverse ratio to consider a mip
	float                          m_maxNHitsPerLayer;                 ///< The max number of hits per layer to consider a mip
	float                          m_minNHitsPerLayer;                 ///< The min number of hits per layer to consider a mip
	float                          m_minParentClusterEnergy;           ///< The min parent cluster energy for a potential merging
	unsigned int                   m_minDaughterClusterNHits;          ///< The min number of hits to consider a mip
	unsigned int                   m_minParentClusterNHits;            ///< The min number of hits to consider a parent cluster
	unsigned int                   m_minDaughterClusterNPseudoLayers;  ///< The min number of pseudo layer to consider a mip
	unsigned int                   m_minInnerPseudoLayer;              ///< The min inner pseudo layer id to consider a mip
	float                          m_maxMipBackwardAngle;              ///< The max backward angle between a parent cluster and the mip
	float                          m_maxMipBackwardDistance;           ///< The max backward distance between the parent cluster and the start of mip
	float                          m_maxClusterSeparationPseudoLayer;  ///< The max pseudo layer between two clusters
        float                          m_maxClusterSeparationPseudoLayer2; ///< The max pseudo layer between two clusters
        float                          m_maxClusterSeparationPseudoLayer3; ///< The max pseudo layer between two clusters
	unsigned int                   m_minParentClusterBackwardNHits;    ///< The min number of hits within the ROI to consider a parent cluster
	float                          m_minMipNEfficientLayerFraction;    ///< The min numer of efficient pseudo layer to consider a mip
	float                          m_minMipChi2;
	unsigned int                   m_nParentFitPseudoLayers;
	float                          m_maxCosineDirection;
        float                          m_maxCosineDirection2;
        float                          m_maxCosineDirection3;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MipFragmentMergingAlg::Factory::CreateAlgorithm() const
{
    return new MipFragmentMergingAlg();
}

} 

#endif  //  MIPFRAGMENTMERGINGALG_H
