/*
 *
 * PerformanceMonitoringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : jeu. mai 19 2016
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

#include "Pandora/AlgorithmHeaders.h"

#include "ArborMonitoring/PerformanceMonitoringAlgorithm.h"
#include "ArborHelpers/ReclusterHelper.h"
#include "ArborHelpers/ClusterHelper.h"
#include "ArborApi/ArborContentApi.h"

namespace arbor_content
{

pandora::StatusCode PerformanceMonitoringAlgorithm::Run()
{
	bool runVisualMonitoring(false);
	float pfoTotalEnergy(0.f);
	float pfoTotalNeutralEnergy(0.f);
	float pfoTotalChargedEnergy(0.f);
	float pfoTotalChi(0.f);

	const pandora::PfoList *pPfoList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pPfoList));

	if(pPfoList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	for(pandora::PfoList::const_iterator pfoIter = pPfoList->begin(), pfoEndIter = pPfoList->end() ;
			pfoEndIter != pfoIter ; ++pfoIter)
	{
		const pandora::Pfo *const pPfo = *pfoIter;

		pfoTotalEnergy += pPfo->GetEnergy();

		if(pPfo->GetTrackList().empty())
		{
			pfoTotalNeutralEnergy += pPfo->GetEnergy();
		}
		else
		{
			pfoTotalChargedEnergy += pPfo->GetEnergy();

			const pandora::TrackList &trackList(pPfo->GetTrackList());
			const pandora::ClusterList &clusterList(pPfo->GetClusterList());
			float trackEnergySum(0.f);
			float clusterEnergySum(0.f);

			for(pandora::TrackList::const_iterator iter = trackList.begin(), endIter = trackList.end() ;
					endIter != iter ; ++iter)
				trackEnergySum += (*iter)->GetEnergyAtDca();

			for(pandora::ClusterList::const_iterator iter = clusterList.begin(), endIter = clusterList.end() ;
					endIter != iter ; ++iter)
			{
				const float clusterEnergy((*iter)->GetCorrectedHadronicEnergy(this->GetPandora()));

				clusterEnergySum += (*iter)->GetCorrectedHadronicEnergy(this->GetPandora());

				ARBOR_LOG( "Track p = " << trackEnergySum << " GeV , cluster energy = " << clusterEnergy << std::endl );
			}

			pfoTotalChi += ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), clusterEnergySum, trackEnergySum);
		}
	}

	if(fabs(pfoTotalEnergy - m_expectedPfoTotalEnergy) > m_pfoTotalEnergyDeviation)
	{
		ARBOR_LOG( "** Energy exceeded ! E = " << pfoTotalEnergy << " GeV, excepted = " << m_expectedPfoTotalEnergy << " GeV **" << std::endl );
		runVisualMonitoring = true;
	}

	ARBOR_LOG( "Total pfo energy = " << pfoTotalEnergy << " GeV" << std::endl );
	ARBOR_LOG( "Neutral pfo energy = " << pfoTotalNeutralEnergy << " GeV" << std::endl );
	ARBOR_LOG( "Charged pfo energy = " << pfoTotalChargedEnergy << " GeV" << std::endl );
	ARBOR_LOG( "Total chi = " << pfoTotalChi << std::endl );

	if(runVisualMonitoring)
	{
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_visualMonitoringAlgorithmName));
	}

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PerformanceMonitoringAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ExpectedPfoTotalEnergy", m_expectedPfoTotalEnergy));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "PfoTotalEnergyDeviation", m_pfoTotalEnergyDeviation));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithm(*this, xmlHandle,
		 "VisualMonitoring", m_visualMonitoringAlgorithmName));

    return pandora::STATUS_CODE_SUCCESS;
}

} 

