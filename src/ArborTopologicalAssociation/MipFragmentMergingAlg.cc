  /// \file MipFragmentMergingAlg.cc
/*
 *
 * MipFragmentMergingAlg.cc source template automatically generated by a class generator
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


#include "ArborTopologicalAssociation/MipFragmentMergingAlg.h"

#include "Pandora/AlgorithmHeaders.h"
#include "Helpers/ClusterFitHelper.h"

#include "ArborHelpers/SortingHelper.h"
#include "ArborHelpers/CaloHitHelper.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"

namespace arbor_content
{

pandora::StatusCode MipFragmentMergingAlg::Run()
{
	const pandora::ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

	if(pClusterList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	pandora::ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());

	// sort them by inner pseudo layer
	std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

	ClusterToClusterMap mipToParentClusterMap;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FindMipFragments(clusterVector, mipToParentClusterMap));
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->MergeClusters(mipToParentClusterMap));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipFragmentMergingAlg::FindMipFragments(const pandora::ClusterVector &clusterVector, ClusterToClusterMap &clusterToClusterMap) const
{
	for(pandora::ClusterVector::const_reverse_iterator iIter = clusterVector.rbegin(), iEndIter = clusterVector.rend() ;
			iEndIter != iIter ; ++iIter)
	{
		const pandora::Cluster *const pDaughterCluster(*iIter);

		// mip fragment ?
		if(!this->IsMipFragment(pDaughterCluster))
			continue;

		const pandora::Cluster *pBestCluster(NULL);

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->FindMipParentCluster(pDaughterCluster, clusterVector, pBestCluster));

		if(NULL != pBestCluster)
			clusterToClusterMap[pDaughterCluster] = pBestCluster;
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipFragmentMergingAlg::FindMipParentCluster(const pandora::Cluster *const pMipCluster, const pandora::ClusterVector &clusterVector, const pandora::Cluster *&pParentCluster) const
{
	pParentCluster = NULL;

	const pandora::CartesianVector mipInnerCentroid(pMipCluster->GetCentroid(pMipCluster->GetInnerPseudoLayer()));
	const pandora::CartesianVector mipBackwardDirection(pMipCluster->GetFitToAllHitsResult().GetDirection() * -1.f);
	const unsigned int maxBackwardPseudoLayer(pMipCluster->GetInnerPseudoLayer() >= m_maxMipBackwardPseudoLayer ? 0 : pMipCluster->GetInnerPseudoLayer()-m_maxMipBackwardPseudoLayer);

	float bestDistanceToCluster(std::numeric_limits<float>::max());

	for(pandora::ClusterVector::const_reverse_iterator jIter = clusterVector.rbegin(), jEndIter = clusterVector.rend() ;
			jEndIter != jIter ; ++jIter)
	{
		const pandora::Cluster *const pCluster(*jIter);

		if(!this->IsPossibleParent(pCluster))
			continue;

		const pandora::OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
		unsigned int nHitsInRoi(0);
		pandora::CartesianVector centroidInRoi(0.f, 0.f, 0.f);

		for(pandora::OrderedCaloHitList::const_reverse_iterator layerIter = orderedCaloHitList.rbegin(), layerEndIter = orderedCaloHitList.rend() ;
				layerEndIter != layerIter ; ++layerIter)
		{
			if(layerIter->first <= maxBackwardPseudoLayer)
				break;

			for(pandora::CaloHitList::const_iterator iterI = layerIter->second->begin(), endIterI = layerIter->second->end() ;
					endIterI != iterI ; ++iterI)
			{
				const pandora::CaloHit *const pCaloHit(*iterI);

				const pandora::CartesianVector differenceVector(pCaloHit->GetPositionVector()-mipInnerCentroid);
				const float distanceToHit(differenceVector.GetMagnitude());
				const float angleWithMip(differenceVector.GetOpeningAngle(mipBackwardDirection));

				if(distanceToHit > m_maxMipBackwardDistance || angleWithMip > m_maxMipBackwardAngle)
					continue;

				++nHitsInRoi;
				centroidInRoi += pCaloHit->GetPositionVector();
			}
		}

		if(nHitsInRoi < m_minParentClusterBackwardNHits)
			continue;

		centroidInRoi = centroidInRoi * (1.f/nHitsInRoi);

		const float distanceToCluster((mipInnerCentroid - centroidInRoi).GetMagnitude());

		if(distanceToCluster < bestDistanceToCluster)
		{
			bestDistanceToCluster = distanceToCluster;
			pParentCluster = pCluster;
		}
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipFragmentMergingAlg::MergeClusters(ClusterToClusterMap &clusterToClusterMap) const
{
	for(ClusterToClusterMap::iterator iter = clusterToClusterMap.begin(), endIter = clusterToClusterMap.end() ;
			endIter != iter ; ++iter)
	{
		const pandora::Cluster *const pDaughterCluster(iter->first);
		const pandora::Cluster *const pParentCluster(iter->second);

		if((NULL == pDaughterCluster) || (NULL == pParentCluster))
			continue;

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->TidyClusterMap(pParentCluster, pDaughterCluster, clusterToClusterMap));
		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pDaughterCluster));

		(iter->second) = NULL;
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipFragmentMergingAlg::TidyClusterMap(const pandora::Cluster *const pReplaceCluster, const pandora::Cluster *const pRemoveCluster, ClusterToClusterMap &clusterToClusterMap) const
{
	if((NULL == pReplaceCluster) || (NULL == pRemoveCluster))
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	if(pReplaceCluster == pRemoveCluster)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	for(ClusterToClusterMap::iterator iter = clusterToClusterMap.begin(), endIter = clusterToClusterMap.end() ;
			endIter != iter ; ++iter)
	{
		if(iter->first == pRemoveCluster)
			continue;

		if(iter->second == pRemoveCluster)
			iter->second = pReplaceCluster;
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MipFragmentMergingAlg::IsMipFragment(const pandora::Cluster *const pCluster) const
{
	if(NULL == pCluster)
		return false;

	if(pCluster->GetNCaloHits() < m_minDaughterClusterNHits)
		return false;

	if(pCluster->GetInnerPseudoLayer() < m_minInnerPseudoLayer)
		return false;

	if(m_discriminatePhotonPid && pCluster->GetParticleIdFlag() == pandora::PHOTON)
		return false;

	if(!pCluster->GetAssociatedTrackList().empty())
		return false;

	try
	{
		ClusterPca clusterPca(pCluster);

		if(clusterPca.GetTransverseRatio() > m_maxMipTransverseRatio)
			return false;
	}
	catch(pandora::StatusCodeException &exception)
	{
		return false;
	}

	const unsigned int nPseudoLayers(pCluster->GetOuterPseudoLayer() - pCluster->GetInnerPseudoLayer() + 1);
	const unsigned int nTouchedPseudoLayers(pCluster->GetOrderedCaloHitList().size());
	const unsigned int nCaloHits(pCluster->GetNCaloHits());
	const float nHitsPerLayer(static_cast<float>(nCaloHits) / static_cast<float>(nTouchedPseudoLayers));
	const float efficientLayers(static_cast<float>(nTouchedPseudoLayers)/static_cast<float>(nPseudoLayers));

	if(nPseudoLayers < m_minDaughterClusterNPseudoLayers)
		return false;

	if(nHitsPerLayer > m_maxNHitsPerLayer || nHitsPerLayer < m_minNHitsPerLayer)
		return false;

	if(efficientLayers < m_minMipNEfficientLayerFraction)
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MipFragmentMergingAlg::IsPossibleParent(const pandora::Cluster *const pCluster) const
{
	if(pCluster->GetHadronicEnergy() < m_minParentClusterEnergy || pCluster->GetNCaloHits() < m_minParentClusterNHits)
		return false;

	if(m_discriminatePhotonPid && pCluster->GetParticleIdFlag() == pandora::PHOTON)
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode MipFragmentMergingAlg::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
	m_discriminatePhotonPid = true;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "DiscriminatePhotonPid", m_discriminatePhotonPid));

	m_maxMipTransverseRatio = 0.01f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxMipTransverseRatio", m_maxMipTransverseRatio));

	m_minDaughterClusterNHits = 5;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinDaughterClusterNHits", m_minDaughterClusterNHits));

	m_maxNHitsPerLayer = 1.5f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxNHitsPerLayer", m_maxNHitsPerLayer));

	m_minNHitsPerLayer = 0.8;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinNHitsPerLayer", m_minNHitsPerLayer));

	m_minParentClusterEnergy = 0.5f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinParentClusterEnergy", m_minParentClusterEnergy));

	m_minParentClusterNHits = 10;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinParentClusterNHits", m_minParentClusterNHits));

	m_minDaughterClusterNPseudoLayers = 4;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinDaughterClusterNPseudoLayers", m_minDaughterClusterNPseudoLayers));

	m_minInnerPseudoLayer = 5;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinInnerPseudoLayer", m_minInnerPseudoLayer));

	m_maxMipBackwardAngle = M_PI/6.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxMipBackwardAngle", m_maxMipBackwardAngle));

	m_maxMipBackwardDistance = 600.f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxMipBackwardDistance", m_maxMipBackwardDistance));

	m_maxMipBackwardPseudoLayer = 15;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MaxMipBackwardPseudoLayer", m_maxMipBackwardPseudoLayer));

	m_minParentClusterBackwardNHits = 5;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinParentClusterBackwardNHits", m_minParentClusterBackwardNHits));

	m_minMipNEfficientLayerFraction = 0.8f;
	PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
	     "MinMipNEfficientLayerFraction", m_minMipNEfficientLayerFraction));

	return pandora::STATUS_CODE_SUCCESS;
}

} 

