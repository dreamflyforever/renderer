/* 
 *  Copyright (C) 2006 by:
 *  Da He <da.he@uni-dortmund.de>
 *  Julian Flake <julian.flake@uni-dortmund.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "cConnectionManager.h"
#include "cRenderer.h"

cConnectionManager::cConnectionManager(void* lnkcRenderer) :cService(lnkcRenderer)
{
	service_id = "urn:upnp-org:serviceId:ConnectionManager1";
	var_count = 10;
	action_count = 5;
	
	//name[CM_A_ARG_TYPE_ProtocolInfo] = "ProtocolInfo";
	name[CM_A_ARG_TYPE_ProtocolInfo] = "A_ARG_TYPE_ProtocolInfo";
	value[CM_A_ARG_TYPE_ProtocolInfo] = "*:*:*:*";

	//name[CM_A_ARG_TYPE_ConnectionStatus] = "ConnectionStatus";
	name[CM_A_ARG_TYPE_ConnectionStatus] = "A_ARG_TYPE_ConnectionStatus";
	value[CM_A_ARG_TYPE_ConnectionStatus] = "Unknown";

	//name[CM_A_ARG_TYPE_AVTransportID] = "AVTransportID";
	name[CM_A_ARG_TYPE_AVTransportID] = "A_ARG_TYPE_AVTransportID";
	value[CM_A_ARG_TYPE_AVTransportID] = "0";

	//name[CM_A_ARG_TYPE_RcsID] = "RcsID";
	name[CM_A_ARG_TYPE_RcsID] = "A_ARG_TYPE_RcsID";
	value[CM_A_ARG_TYPE_RcsID] = "0";

	//name[CM_A_ARG_TYPE_ConnectionID] = "ConnectionID";
	name[CM_A_ARG_TYPE_ConnectionID] = "A_ARG_TYPE_ConnectionID";
	value[CM_A_ARG_TYPE_ConnectionID] = "-1";

	//name[CM_A_ARG_TYPE_ConnectionManager] = "ConnectionManager";
	name[CM_A_ARG_TYPE_ConnectionManager] = "A_ARG_TYPE_ConnectionManager";
	value[CM_A_ARG_TYPE_ConnectionManager] = "";
	
	name[CM_SourceProtocolInfo] = "SourceProtocolInfo";
	value[CM_SourceProtocolInfo] = "";

	name[CM_SinkProtocolInfo] = "SinkProtocolInfo";
	value[CM_SinkProtocolInfo] = "http-get:*:image/gif:*,http-get:*:image/jpeg:*,http-get:*:audio/mpeg:*,http-get:*:video/mpeg:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/avi:*,http-get:*:video/wmv:*,http-get:*:audio/x-ms-wma:*";

	name[CM_A_ARG_TYPE_Direction] = "A_ARG_TYPE_Direction";
	value[CM_A_ARG_TYPE_Direction] = "Input";
	
	name[CM_CurrentConnectionIDs] = "CurrentConnectionIDs";
	value[CM_CurrentConnectionIDs] = "0";
	
	action_name[0]="GetProtocolInfo";
	action_name[1]="PrepareForConnection";
	action_name[2]="ConnectionComplete";
	action_name[3]="GetCurrentConnectionIDs";
	action_name[4]="GetCurrentConnectionInfo";

	actions[0] = &cConnectionManager::GetProtocolInfo;
	actions[1] = &cConnectionManager::PrepareForConnection;
	actions[2] = &cConnectionManager::ConnectionComplete;
	actions[3] = &cConnectionManager::GetCurrentConnectionIDs;
	actions[4] = &cConnectionManager::GetCurrentConnectionInfo;
}


cConnectionManager::~cConnectionManager()
{
	// TODO: put destructor code here
}

int cConnectionManager::callAction(int i, IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	return (this->*actions[i])(in,out,Event);
}

/*
 * Actions
 */
int cConnectionManager::GetProtocolInfo(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	cout << "GetProtocolInfo: " << in << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"Source",value[CM_SourceProtocolInfo]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"Sink",value[CM_SinkProtocolInfo]);
	return 0;
}
int cConnectionManager::PrepareForConnection(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	cout << "PrepareForConnection: " << in << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"ConnectionID",value[CM_A_ARG_TYPE_ConnectionID]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"AVTransportID",value[CM_A_ARG_TYPE_AVTransportID]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"RcsID",value[CM_A_ARG_TYPE_RcsID]);
	return 0;
}
int cConnectionManager::ConnectionComplete(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	cout << "ConnectionComplete: " << in << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,NULL,NULL);
	return 0;
}
int cConnectionManager::GetCurrentConnectionIDs(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	cout << "GetCurrentConnectionIDs: " << in << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"ConnectionIDs",value[CM_CurrentConnectionIDs]);
	return 0;
}
int cConnectionManager::GetCurrentConnectionInfo(IXML_Document* in, IXML_Document** out, Upnp_Action_Request* Event)
{
	cout << "GetCurrentConnectionInfo: " << in << endl;
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"ProtocolInfo",value[CM_A_ARG_TYPE_ProtocolInfo]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"RcsID",value[CM_A_ARG_TYPE_RcsID]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"AVTransportID",value[CM_A_ARG_TYPE_AVTransportID]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"Direction",value[CM_A_ARG_TYPE_Direction]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"Status",value[CM_A_ARG_TYPE_ConnectionStatus]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"PeerConnectionID",value[CM_A_ARG_TYPE_ConnectionID]);
	UpnpAddToActionResponse(&Event->ActionResult,Event->ActionName,service_id,"PeerConnectionManager",value[CM_A_ARG_TYPE_ConnectionManager]);
	return 0;
}
