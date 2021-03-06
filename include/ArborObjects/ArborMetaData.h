  /// \file ArborMetaData.h
/*
 *
 * ArborMetaData.h header template automatically generated by a class generator
 * Creation date : sam. mars 21 2015
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


#ifndef ARBOR_META_DATA_H
#define ARBOR_META_DATA_H 1

#include "ArborApi/ArborInputTypes.h"
#include "ArborObjects/Connector.h"

namespace arbor_content
{

class CaloHit;
class Connector;

/**
 *  @brief CaloHitMetaData class
 */
class CaloHitMetaData
{
public:
	/**
	 *  @brief  Constructor
	 *
	 *  @param  pCaloHit the calo hit for this meta data
	 */
	CaloHitMetaData(arbor_content::CaloHit *const pCaloHit);

	/**
	 * @brief  Destructor
	 */
	~CaloHitMetaData();

	/**
	 *  @brief  Get the calo hit of this connector meta data
	 */
	const arbor_content::CaloHit *GetCaloHit() const;

	/**
	 *  @brief  Whether the calo hit is connected with another one
	 *
	 *  @param  pCaloHit an other calo hit to check for connection
	 */
	bool IsConnected(const arbor_content::CaloHit *const pCaloHit) const;

	/**
	 *  @brief  Whether the calo hit is connected with another one in the given direction
	 *
	 *  @param  pCaloHit an other calo hit to check for connection
	 *  @param  direction the connector direction
	 */
	bool IsConnected(const arbor_content::CaloHit *const pCaloHit, ConnectorDirection direction)  const;

	/**
	 *  @brief  Find a connector
	 *
	 *  @param  pCaloHit an other calo hit to check for connection
	 *  @param  pConnector the connector address to receive
	 */
	pandora::StatusCode FindConnector(const arbor_content::CaloHit *const pCaloHit, const Connector *&pConnector) const;

	/**
	 *  @brief  Find a connector in a given direction
	 *
	 *  @param  pCaloHit an other calo hit to check for connection
	 *  @param  direction the connector direction
	 *  @param  pConnector the connector address to receive
	 *
	 */
	pandora::StatusCode FindConnector(const arbor_content::CaloHit *const pCaloHit, ConnectorDirection direction,
			const Connector *&pConnector) const;

	/**
	 *  @brief  Whether the calo hit has at least one connection
	 */
	bool HasAnyConnection() const;

	/**
	 *  @brief  Get the global connector list (backward and forward)
	 */
	const ConnectorList &GetConnectorList() const;

	/**
	 *  @brief  Get the connector list for a given direction
	 *
	 *  @param  direction the connector direction
	 */
	const ConnectorList &GetConnectorList(ConnectorDirection direction) const;

	/**
	 *  @brief  Whether the calo hit is a seed calo hit in the tree.
	 */
	bool IsSeed() const;

	/**
	 *  @brief  Whether the calo hit is a leaf calo hit in the tree
	 */
	bool IsLeaf() const;

	/**
	 *  @brief  Add an available connector to meta data
	 *
	 *  @param  pConnector an unavailable connector to add
	 *  @param  direction the connector direction
	 */
	pandora::StatusCode AddConnector(const Connector *const pConnector, ConnectorDirection direction);

	/**
	 *  @brief  Remove the connection with an other calo hit
	 *
	 *  @param  pConnector the connector to remove (not deleted)
	 */
	pandora::StatusCode RemoveConnector(const Connector *const pConnector);

	/**
	 *  @brief  Remove all connections
	 */
	pandora::StatusCode RemoveAllConnections();

private:

	arbor_content::CaloHit                   *m_pCaloHit;                 ///< The handled arbor calo hit
	ConnectorList                             m_allConnectorList;        ///< All connected calo hits
	ConnectorList                             m_forwardConnectorList;   ///< Forward connected calo hits
	ConnectorList                             m_backwardConnectorList;  ///< Backward connected calo hits
	bool                                      m_isLeaf;                   ///< Whether the calo hit is a leaf
	bool                                      m_isSeed;                   ///< Whether the calo hit is a seed

	friend class CaloHit;
};

} 

#endif  //  ARBOR_META_DATA_H
