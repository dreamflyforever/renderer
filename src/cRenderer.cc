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

#include <fstream>
#include "cRenderer.h"

bool file_exists(char* filename)
{
	ifstream file;
	file.open(filename,ifstream::in);
	bool return_value=file.is_open();
	if(return_value)
	{
		file.close();
	}
	return return_value;
}

void* pt2obj; 

/*
 * Constructor
 */
cRenderer::cRenderer(int argc,char* argv[]) 
{
	// FunctionPointer Tutorial
	// http://www.newty.de/fpt/callback.html

	pt2obj = (int*) this;
	
	// global variable init
	initVar();

	parseConfigFile();
	
	handleOptions(argc,argv);

	positionToSeek = 0;
	
	services[ConnectionManager] = new cConnectionManager(this);
	services[AVTransport] = new cAVTransport(this);
	services[RenderingControl] = new cRenderingControl(this);

	//mutex init
	ithread_mutex_init(&rendererMutex,NULL);
	ithread_mutex_init(&outputMutex,NULL);
	
	//find the path to web-dir
	if(file_exists("/usr/local/share/renderer/AVTransport.xml")&&
	   file_exists("/usr/local/share/renderer/ConnectionManager.xml")&&
	   file_exists("/usr/local/share/renderer/RendererControl.xml"))
	{
		web_root="/usr/local/share/renderer";
	}else if(file_exists("/usr/share/renderer/AVTransport.xml")&&
	   		 file_exists("/usr/share/renderer/ConnectionManager.xml")&&
	   		 file_exists("/usr/share/renderer/RendererControl.xml"))
	{
		web_root="/usr/share/renderer";
	}else
	{
		cout << "couldn't find description file." << endl;
		cout << "make sure you have run make install." << endl;
		exit(1);
	}
	
	//SDK-Initialisierung
	if(ip_addr==NULL)
	{
		cout << "random ip" << endl;
	}else
	{
		cout << ip_addr << endl;
	}
	int err;
	err = UpnpInit( ip_addr , 0 );
	if ( err != UPNP_E_SUCCESS ) {
		cout << "couldn't init SDK: " << UpnpGetErrorMessage(err) << endl;
		UpnpFinish();
		exit(1);
	}
	if (UpnpSetWebServerRootDir(web_root) != UPNP_E_SUCCESS) {
		cout << "couldn't set web server's root dir." << endl;
		UpnpFinish();
		exit(1);
	}
/*	
	char tmp[256];
	sprintf(tmp, "http://%s:%i/AVRendererDesc.xml", UpnpGetServerIpAddress(), UpnpGetServerPort());
	err = UpnpRegisterRootDevice(tmp, callbackEventHandler, NULL, &renderer_hdl);
	if ( err != UPNP_E_SUCCESS) {
		cout << "couldn't register root device: " << err << endl;
		UpnpFinish();
		exit(1);
	}		
	cout << "SDK intitalized and device registered: (" << tmp << ")." << endl;
*/

	
	IXML_Document* desc = getDescriptionDoc();
	string str = ixmlPrintDocument(desc);
	int lng = str.length();
//	cout << ixmlPrintDocument(desc) << endl;
	err = UpnpRegisterRootDevice2(	UPNPREG_BUF_DESC,
									str.c_str(),
									lng,
									1,
									callbackEventHandler, 
									NULL, 
									&renderer_hdl
								 );
	if ( err != UPNP_E_SUCCESS) {
		cout << "couldn't register root device: " << UpnpGetErrorMessage(err) << endl;
		UpnpFinish();
		exit(1);
	}		
	cout << "SDK intitalized and device registered." << endl;

	// Advertisement
	if (UpnpSendAdvertisement(renderer_hdl, 1000) != UPNP_E_SUCCESS) {
		cout << "couldn't advertise device" << endl;
		UpnpFinish();
		exit(1);
	}		
	cout << "device advertised." << endl;
}


/*
 * Destructor
 */
cRenderer::~cRenderer() 
{
	ithread_mutex_destroy(&rendererMutex);
	ithread_mutex_destroy(&outputMutex);
	if (strcmp("STOPPED",services[AVTransport]->value[cAVTransport::AV_TransportState]))
	{
		cout << "mplayer is quitting" << endl;
		char* test="quit\n";
		write(PARENT_WRITE,test,strlen(test));
	} else {
		cout << "mplayer is already exited" << endl;
	}
	int err;
	err=UpnpUnRegisterRootDevice(renderer_hdl);
	if (err != UPNP_E_SUCCESS) {
		cout << "couldn't unregister root device:" << err << endl;
		UpnpFinish();
		exit(1);
	}		
	if (UpnpFinish() != UPNP_E_SUCCESS) {
		cout << "couldn't finish" << endl;
		exit(1);
	}
}


/*
 * Handles Action Requests
 */
void cRenderer::handleActionRequest(Upnp_Action_Request *Event)
{
	ithread_mutex_lock(&rendererMutex);
	cout << ixmlPrintDocument(Event->ActionRequest) << endl;	
	if( (strcmp(Event->ServiceID,services[RenderingControl]->service_id)==0)&&
		(strcmp(Event->DevUDN,udn)==0)){
		for(int i=0;i<services[RenderingControl]->action_count;i++){
			if(!strcmp(Event->ActionName,
					   services[RenderingControl]->action_name[i])){
				((cRenderingControl*)services[RenderingControl])->callAction(i,Event->ActionRequest,&Event->ActionResult,Event);
				break;
			}						   
		}
	}else if( (strcmp(Event->DevUDN,udn)==0)&&
				(strcmp(Event->ServiceID,services[ConnectionManager]->service_id)==0)){
		for(int i=0;i<services[ConnectionManager]->action_count;i++){
			if(!strcmp(Event->ActionName,
					   services[ConnectionManager]->action_name[i])){
				((cConnectionManager*)services[ConnectionManager])->callAction(i,Event->ActionRequest,&Event->ActionResult,Event);
				break;
			}						   
		}
	}else if( (strcmp(Event->DevUDN,udn)==0)&&
				(strcmp(Event->ServiceID,services[AVTransport]->service_id)==0)){
		for(int i=0;i<services[AVTransport]->action_count;i++){
			if(!strcmp(Event->ActionName,
					   services[AVTransport]->action_name[i])){
				((cAVTransport*)services[AVTransport])->callAction(i,Event->ActionRequest,
						   							&Event->ActionResult,Event);
				break;
			}						   
		}
	}else{
		cout << "service false" << endl;
		return;
	}
	
	ithread_mutex_unlock(&rendererMutex);
}

/*
 * Handles Subscription Requests
 */
void cRenderer::handleSubscriptionRequest(Upnp_Subscription_Request* Event)
{
	ithread_mutex_lock(&rendererMutex);
	int err = 254;
	if (!strcmp("urn:upnp-org:serviceId:RenderingControl1",Event->ServiceId)) {
		cout << "subscription of service RenderingControl1 requested" << endl;
		char* evRC_name[1];
		char* evRC_value[1];
		evRC_name[0]=services[RenderingControl]->name[cRenderingControl::RC_LastChange] ;
		evRC_value[0]=services[RenderingControl]->value[cRenderingControl::RC_LastChange] ;
		
		cout << evRC_name[0] << endl;
		err = UpnpAcceptSubscription(renderer_hdl,
								Event->UDN,
								Event->ServiceId,
								(const char **) evRC_name,
								(const char **) evRC_value,
								1,
								Event->Sid);
		cout << "nach accept" << endl;
		(services[RenderingControl])->setVar(RenderingControl,1,cRenderingControl::RC_Volume,services[RenderingControl]->value[cRenderingControl::RC_Volume]);

	} else if (!strcmp("urn:upnp-org:serviceId:ConnectionManager1",Event->ServiceId)) {
		cout << "subscription of service ConnectionManager1 requested" << endl;
		char* evCM_name[3];
		char* evCM_value[3];
		
		evCM_name[0]=services[ConnectionManager]->name[cConnectionManager::CM_SourceProtocolInfo];
		evCM_name[1]=services[ConnectionManager]->name[cConnectionManager::CM_SinkProtocolInfo];
		evCM_name[2]=services[ConnectionManager]->name[cConnectionManager::CM_CurrentConnectionIDs];
		
		evCM_value[0]=services[ConnectionManager]->value[cConnectionManager::CM_SourceProtocolInfo];
		evCM_value[1]=services[ConnectionManager]->value[cConnectionManager::CM_SinkProtocolInfo];
		evCM_value[2]=services[ConnectionManager]->value[cConnectionManager::CM_CurrentConnectionIDs];
		
		err = UpnpAcceptSubscription(renderer_hdl,
								Event->UDN,
								Event->ServiceId,
								(const char **) evCM_name,
								(const char **) evCM_value,
								3, // muss noch korrigiert werden
								Event->Sid);
	} else if (!strcmp("urn:upnp-org:serviceId:AVTransport1",Event->ServiceId)) {
		cout << "subscription of service AVTransport1 requested" << endl;
		char* evAV_name[1];
		char* evAV_value[1];
		evAV_name[0]=services[AVTransport]->name[cAVTransport::AV_LastChange];
		evAV_value[0]=services[AVTransport]->value[cAVTransport::AV_LastChange];
		err = UpnpAcceptSubscription(renderer_hdl,
								Event->UDN,
								Event->ServiceId,
								(const char **) evAV_name,
								(const char **) evAV_value,
								1,
								Event->Sid);
	} else {		
		cout << "subscription of uncatched service " <<
				Event->ServiceId <<
				" requested" <<
		endl;
	}
	cout << "accepted? " << Event->ServiceId << " " ;	
	cout << " - err: " << err << endl;

	
	ithread_mutex_unlock(&rendererMutex);
}
/*
 * Handles Get Var Requests
 */
void cRenderer::handleGetVarRequest(Upnp_State_Var_Request* Event)
{
	ithread_mutex_lock(&rendererMutex);
	
	cout << "get var requested:" << 
			Event->DevUDN <<
			Event->ServiceID <<
			Event->StateVarName <<
//			Event->CtrlPtIPAddr <<
			Event->CurrentVal <<
	endl;
	
	ithread_mutex_unlock(&rendererMutex);
}



/*
 * Handles sdk's callbacks
 */
int cRenderer::callbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
	cRenderer *cp = (cRenderer*) pt2obj;
	cout << "cp->UDN " << cp->udn << endl;

	switch (EventType) {
		case UPNP_CONTROL_ACTION_REQUEST: { /* Received by a device when a control point issues a control request. */
			cp->handleActionRequest((struct Upnp_Action_Request *) Event);
			break; 
		}
		case UPNP_CONTROL_GET_VAR_REQUEST: { /* Received by a device when a query for a single service variable arrives. */
			// deprecated
			cp->handleGetVarRequest((struct Upnp_State_Var_Request *) Event);
			break; 
		}
		case UPNP_EVENT_SUBSCRIPTION_REQUEST: { /* Received by a device when a query for a single service variable arrives. */
			cp->handleSubscriptionRequest((struct Upnp_Subscription_Request *) Event);
			break; 
		}
		default: {
			cout << "uncatched Event: " << EventType << endl;
			break;
		}
	}
	return UPNP_E_SUCCESS;
}

/*
 * Sends a command to mplayer, if mplayer is running
 */
char* cRenderer::mplayer_cmd(char command[],bool output)
{
	//ithread_mutex_lock(&outputMutex);
	cout << services[AVTransport]->value[cAVTransport::AV_TransportState] << endl;
	if (strcmp(services[AVTransport]->value[cAVTransport::AV_TransportState],"STOPPED")!=0) {
		char cmd[100];
		sprintf(cmd,"%s\n",command);
		write(PARENT_WRITE,cmd,strlen(cmd));
		if(output)
		{
			string out = this->showOutput();
			cout << "answer from mplayer: " <<
				out  << endl << "===============================================" <<
				endl;
			return (char*)out.c_str();
		}
		return "";
	} else {
		cout << "mplayer not running!" << endl;
		return "";
	}		
}


void cRenderer::wakeup(int in)
{
	cRenderer* cp = (cRenderer*) pt2obj;
	if (strcmp(cp->services[AVTransport]->value[cAVTransport::AV_TransportState],"STOPPED")!=0) {
		cp->mplayer_is_running = false;
		cp->services[AVTransport]->setVar(AVTransport,2,cAVTransport::AV_TransportState,"STOPPED");
		// notification
		IXML_Document* LCdoc = NULL;
		IXML_Node* node = NULL;
		IXML_Element* Ele = NULL;
		LCdoc = ixmlParseBuffer("<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"></Event>");
		node = ixmlNode_getFirstChild( (IXML_Node*) LCdoc );
		Ele = ixmlDocument_createElement( LCdoc, "InstanceID" );
		ixmlElement_setAttribute(Ele, "val","0");
		ixmlNode_appendChild(  node,(IXML_Node*) Ele);
		IXML_Element* Ele2 = NULL;
		Ele2 = ixmlDocument_createElement(LCdoc, 
									cp->services[AVTransport]->name[cAVTransport::AV_TransportState]);
		ixmlElement_setAttribute(Ele2, "val", cp->services[AVTransport]->value[cAVTransport::AV_TransportState]);
		ixmlNode_appendChild((IXML_Node*)Ele,(IXML_Node*) Ele2);
		IXML_Document* myDoc2 = UpnpCreatePropertySet(1,
									"LastChange",
									ixmlPrintDocument(LCdoc)
		); 
		UpnpNotifyExt(cp->renderer_hdl,cp->udn,cp->services[AVTransport]->service_id,myDoc2);
		cout << "SIGCHLD" << endl;
	}
}


/*
 * CREATE THE PAIR OF PIPES
 *
 * Pipes have two ends but just one direction: to get a two-way
 * conversation you need two pipes. It's an error if we cannot make
 * them both, and we define these macros for easy reference.
 */
int cRenderer::mypopen(char* uri)
{
	if ( pipe(readpipe) < 0  ||  pipe(writepipe) < 0 || pipe(errpipe) < 0 )
	{
		/* FATAL: cannot create pipe */
		cout << "cannot create pipe" << endl;
	}
	
	if ( (childpid = fork()) < 0)
	{
		/* FATAL: cannot fork child */
		cout << "cannot fork child" << endl;
	}
	else if ( childpid == 0 )	/* in the child */
	{
		close(PARENT_WRITE);
		close(PARENT_READ);
		close(errpipe[0]);
	
		//Hier siehe auch http://www.unixwiz.net/techtips/remap-pipe-fds.htm
		//Ich habe einfach alles kopiert.
		if ( CHILD_READ == 0  &&  CHILD_WRITE == 1 )
		{
			//perror("case 1");
			DUP2CLOSE(CHILD_READ, 0);
			DUP2CLOSE(CHILD_WRITE, 1);
		}
		if ( CHILD_READ >= 1  &&  CHILD_WRITE > 1 )
		{
		 	DUP2CLOSE(CHILD_READ, 0);
		 	DUP2CLOSE(CHILD_WRITE, 1);
		}
		if ( CHILD_READ == 0  &&  CHILD_WRITE >= 1 )
		 	DUP2CLOSE(CHILD_WRITE, 1);

		if ( CHILD_READ  >= 1  &&  CHILD_WRITE == 1 )
		 	DUP2CLOSE(CHILD_READ, 0);
		if ( CHILD_READ >= 1   &&  CHILD_WRITE == 0 )
		{
		 	DUP2CLOSE(CHILD_WRITE, 1);
		 	DUP2CLOSE(CHILD_READ, 0);
		}
		if ( CHILD_READ == 1  &&  CHILD_WRITE == 0 )
		{
			const int tmp = dup(CHILD_WRITE);		 //NOTE! this is not dup2() ! 

			//tmp > 1
			close(CHILD_WRITE);
			DUP2CLOSE(CHILD_READ, 0);
			DUP2CLOSE(tmp,1);
		}
		DUP2CLOSE(errpipe[1],2);
		//DUP2CLOSE(CHILD_READ, 0);
		//DUP2CLOSE(CHILD_WRITE, 1);
	
		/* do child stuff */
		char* ar[5];
		ar[0]=this->mplayer_bin;
		ar[1]="-slave";
		ar[2]="-quiet";
		ar[3]=uri;
		ar[4]=fullscreen;
		//mplayer_is_running = true;
		execvp(ar[0],ar);
    		perror("mplayer(failed)"); /* we only arrive here on error! */
	}
	else				/* in the parent */
	{
		struct sigaction sigact;
		sigset_t  sigset;
		
		/* set up handler for SIGCHLD, but get the sig from parent */
		sigemptyset(&sigset); /* create an empty signal mask set */
		sigact.sa_handler = cRenderer::wakeup; /* use the wakeup() handler */
		sigact.sa_mask    = sigset; /* use the empty sigset */
		sigact.sa_flags   = 0;      /* no flags needed */
		sigaction(SIGCHLD, &sigact, NULL);
	
		struct sigaction sigact2;
		sigset_t  sigset2;
		sigemptyset(&sigset2);  //create an empty signal mask set 
		sigact2.sa_handler = cRenderer::wakeup;  //use the wakeup() handler 
		sigact2.sa_mask    = sigset;  //use the empty sigset 
		sigact2.sa_flags   = 0;       //no flags needed 
		sigaction(SIGPIPE, &sigact2, NULL);
		
		/* do parent stuff */
		close(CHILD_READ);
		close(CHILD_WRITE);
		close(errpipe[1]);
	
		//make read pipe noblocking
		int flags;
		flags = fcntl(PARENT_READ, F_GETFL, 0);
		//flags |= O_NONBLOCK;
		fcntl(PARENT_READ, F_SETFL, flags);
		readfp=fdopen(PARENT_READ,"r");
		
		//output
		//sleep(1);
		//showOutput();
		char * line = NULL;
    		size_t len = 0;
		getline(&line,&len,readfp);
		cout << line << endl;
		while(strcmp(line,"Starting playback...\n")!=0)
		{
			getline(&line,&len,readfp);
			cout << line << endl;
			if(strstr(line,"Exiting... (End of file)")!=NULL)
			{
				wait(0);
				return 0;
			}
		}
		mplayer_is_running = true;
		(services[AVTransport])->setVar(AVTransport,12,
				cAVTransport::AV_TransportState,"PLAYING",
				cAVTransport::AV_CurrentTrack,"1",
				cAVTransport::AV_NumberOfTracks,"1",
				cAVTransport::AV_CurrentTrackURI,(services[AVTransport])->value[cAVTransport::AV_AVTransportURI],
				cAVTransport::AV_CurrentTrackMetaData,"",
				cAVTransport::AV_AbsoluteTimePosition,(services[AVTransport])->value[cAVTransport::AV_AbsoluteTimePosition]);
		cout << "raus" << endl;
		if(line)
		{
			free(line);
		}
		
		//int speed = 1;
		bool first=true;
		while(mplayer_is_running) {
			// get_time_pos nur wenn PLAYING (TODO)
			if (strcmp(services[AVTransport]->value[cAVTransport::AV_TransportState],"PLAYING")==0) {
				if(first)
				{					
					char tstamp[20];
					string tmp = mplayer_cmd("get_time_length",true);
					tmp = tmp.substr(tmp.find_first_of('=')+1);
					seconds2tstamp(tstamp,(char* )tmp.c_str());
					cout << tstamp << "###############################\n########" << endl;
					if (strcmp(tmp.c_str(),"00.0")!=0) {
		//				setVar(AV_id,2,cAVTransport::AV_CurrentTrackDuration,"99:59:59");
						(services[AVTransport])->setVar(AVTransport,2,cAVTransport::AV_CurrentTrackDuration,"NOT_IMPLEMENTED");
					} else {
						(services[AVTransport])->setVar(AVTransport,2,cAVTransport::AV_CurrentTrackDuration,tstamp);
					}
					first=false;
				}
				
				string length = mplayer_cmd("get_time_pos",true);
				if(strcmp(length.c_str(),"")!=0) {
					length=length.substr(length.find_first_of('=')+1);
					char returnvalue[100];
					strcpy(returnvalue,length.c_str());
					char tstamp[20];
					seconds2tstamp(tstamp,returnvalue);
					cout <<  tstamp  << endl;
					(services[AVTransport])->setVar(AVTransport,2,cAVTransport::AV_RelativeTimePosition,tstamp);
				}
			}
			sleep(1);
		}
		//wait for child process
		wait(0);
		//wakeup(0);
		//wait(0);
	}
	return 0;
}

/*
 * mplayer thread start
 */
void* cRenderer::threadCreate( void *ptr )
{
	cRenderer *cp = (cRenderer*) pt2obj;
	char *message;
    message = (char *) ptr;
	cp->mypopen(message);
	return (void*)NULL;
}


/*	
 * read output from mplayer and show it
 */
string cRenderer::showOutput()
{
	char * line = NULL;
    	size_t len = 0;
	string returnvalue="";
	/*char buffer[256];
	int i;
	usleep(100000);
	while((i=read(PARENT_READ,buffer,256))>0)
	{
		buffer[i-1]='\0';
		returnvalue+=buffer;
	}*/
	if(readfp==NULL)
	{
		cout << "readfp is NULL (error)" << endl;
		exit(1);
	}
	getline(&line,&len,readfp);
	cout << line << endl;
	while(strstr(line,"ANS_LENGTH=")==NULL && strstr(line,"ANS_TIME_POSITION=")==NULL)
	{
		getline(&line,&len,readfp);
		cout << line << endl;
	}
	returnvalue=line;
	if(line)
	{
		free(line);
	}
	return returnvalue;
}

void cRenderer::mplayer_start()
{
	iret = pthread_create( &thread, NULL, threadCreate, (void*) services[AVTransport]->value[cAVTransport::AV_AVTransportURI]);
}

void cRenderer::seconds2tstamp(char* out, char* in)
{
	int ihours = 0;
	int iminutes = 0;
	int iseconds = atoi(in);
	
	iminutes = iseconds/60;
	iseconds = iseconds - iminutes*60;
	ihours = iminutes/60;
	iminutes = iminutes - ihours*60;
	
	char chours[3];
	char cminutes[3];
	char cseconds[3];
	
	if (ihours<10) sprintf(chours,"0%i",ihours);
	else sprintf(chours,"%i",ihours);
	
	if (iminutes<10) sprintf(cminutes,"0%i",iminutes);
	else sprintf(cminutes,"%i",iminutes);
	
	if (iseconds<10) sprintf(cseconds,"0%i",iseconds);
	else sprintf(cseconds,"%i",iseconds);
	
	sprintf(out,"%s:%s:%s",chours,cminutes,cseconds);
}

int cRenderer::time2seconds(char* in)
{
	string length = in;

	string cHours = length.substr(0,length.find_first_of(':'));

	length = length.substr(length.find_first_of(':')+1);
	string cMinutes = length.substr(0,length.find_first_of(':'));

	length = length.substr(length.find_first_of(':')+1);
	string cSeconds = length.substr(0,length.find_first_of(':'));
	
	int iHours = atoi(cHours.c_str());
	int iMinutes = atoi(cMinutes.c_str());
	int iSeconds = atoi(cSeconds.c_str());
	
	iSeconds = iSeconds + (60*iMinutes) + (3600*iHours);
	return iSeconds;
}

void cRenderer::parseConfigFile()
{
	
	char* home=getenv("HOME");
	ifstream myfile;
	char home_conf[50];
	sprintf(home_conf,"%s/.renderer/renderer.conf",home);
	myfile.open(home_conf,ifstream::in);
	if (myfile.is_open()) {
		cout << "using config " << home_conf << endl;
	} else {
		cout << "config not found" << endl;
		cout << "generating config" << endl;
		FILE* file=fopen(home_conf,"w");
		fputs("#This file is automatically created\n\n",file);
		fputs("#Set a friendly name for your renderer\n",file);
		fputs("#FRIENDLYNAME=Linux-Renderer(%h)\n\n",file);
		fputs("#Set a ip address.\n",file);
		fputs("#IP_ADDRESS=192.168.1.1\n\n",file);
		fputs("#Set the path to mplayer\n",file);
		fputs("#MPLAYER_BIN=/usr/bin/mplayer\n\n",file);
		fclose(file);
		myfile.open(home_conf,ifstream::in);
		//exit(1);	
	}
	string line;
	string str_xmlIcon = "";
	string str_friendlyName = "";
	string varName,value;
	while (! myfile.eof() )	{
		// Zeile holen
		getline (myfile,line);

		// Kommentare entfernen
		if (line.find_first_of('#') == 0)
			line="";
		if (strcmp(line.c_str(),"")) {

			// Zeile trimmen
			trim(line);

			// Name und Wert holen
			getVarName(varName, line);
			getValue(value, line);
	
			//Hostname einkleben
			char hostname[50];
			gethostname(hostname,50);
			str_replace("%h",hostname,value);
			
			// zuweisen
			if (!strcmp(varName.c_str(),"FRIENDLYNAME")) {
				sprintf(friendlyName,"%s",value.c_str());
			} else if (!strcmp(varName.c_str(),"IP_ADDRESS")) {
				sprintf(ip_addr,"%s",value.c_str());
			} else if (!strcmp(varName.c_str(),"MPLAYER_BIN")) {
				sprintf(mplayer_bin,"%s",value.c_str());
			}
		}
	}
	myfile.close();
	this->udn = strdup(this->get_uuid());
//	strcpy(udn,str_udn.c_str());
	
}

IXML_Document* cRenderer::getDescriptionDoc()
{
	IXML_Document* descDoc;
	IXML_Node* node;
	IXML_Node* node2;
	IXML_Node* node3;
	IXML_Element* Ele;
	IXML_Element* Ele2;
	IXML_Element* Ele3;
	IXML_Element* Ele4;
	descDoc = ixmlParseBuffer("<root></root>");
	node = ixmlNode_getFirstChild( (IXML_Node*) descDoc );
	
	Ele = ixmlDocument_createElement( descDoc, "specVersion" );
	Ele2 = ixmlDocument_createElement( descDoc, "major" );
	node2 = ixmlDocument_createTextNode( descDoc, "1" );
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);
	Ele2 = ixmlDocument_createElement( descDoc, "minor" );
	node2 = ixmlDocument_createTextNode( descDoc, "0" );
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);
	ixmlNode_appendChild(node, (IXML_Node*) Ele);

	// Beginn <device>
	Ele = ixmlDocument_createElement( descDoc, "device" );

	Ele2 = ixmlDocument_createElement( descDoc, "deviceType" );
	node2 = ixmlDocument_createTextNode(descDoc, "urn:schemas-upnp-org:device:MediaRenderer:1");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);
	
	Ele2 = ixmlDocument_createElement( descDoc, "friendlyName" );
	node2 = ixmlDocument_createTextNode(descDoc, this->friendlyName);
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);
	
	Ele2 = ixmlDocument_createElement( descDoc, "manufacturer" );
	node2 = ixmlDocument_createTextNode(descDoc, "PG483");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "manufacturerURL" );
	node2 = ixmlDocument_createTextNode(descDoc, "http://www.ubiquitous-media.de");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "modelDescription" );
	node2 = ixmlDocument_createTextNode(descDoc, "Linux-Renderer(C++,PG483) 1.0");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "modelName" );
	node2 = ixmlDocument_createTextNode(descDoc, "AV Renderer");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "modelNumber" );
	node2 = ixmlDocument_createTextNode(descDoc, "1.0");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "modelURL" );
	node2 = ixmlDocument_createTextNode(descDoc, "http://www.ubiquitous-media.de");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "serialNumber" );
	node2 = ixmlDocument_createTextNode(descDoc, "000000000001");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "UDN" );
	node2 = ixmlDocument_createTextNode(descDoc, this->udn);
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "UPC" );
	node2 = ixmlDocument_createTextNode(descDoc, "000000000001");
	ixmlNode_appendChild((IXML_Node*) Ele2, node2);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele2 = ixmlDocument_createElement( descDoc, "iconList" );
	Ele3 = ixmlDocument_createElement(descDoc, "icon");
	Ele4 = ixmlDocument_createElement(descDoc, "mimeType");
	node3 = ixmlDocument_createTextNode(descDoc, "image/gif");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "width");
	node3 = ixmlDocument_createTextNode(descDoc, "32");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "height");
	node3 = ixmlDocument_createTextNode(descDoc, "32");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "depth");
	node3 = ixmlDocument_createTextNode(descDoc, "8");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "url");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnpmplayer.gif");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) Ele3);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	
	Ele2 = ixmlDocument_createElement( descDoc, "serviceList" );
	
	Ele3 = ixmlDocument_createElement(descDoc, "service");
	Ele4 = ixmlDocument_createElement(descDoc, "serviceType");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:schemas-upnp-org:service:RenderingControl:1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "serviceId");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:upnp-org:serviceId:RenderingControl1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "controlURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/control/RenderingControl1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "eventSubURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/event/RenderingControl1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "SCPDURL");
	node3 = ixmlDocument_createTextNode(descDoc, "RendererControl.xml");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) Ele3);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele3 = ixmlDocument_createElement(descDoc, "service");
	Ele4 = ixmlDocument_createElement(descDoc, "serviceType");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:schemas-upnp-org:service:ConnectionManager:1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "serviceId");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:upnp-org:serviceId:ConnectionManager1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "controlURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/control/ConnectionManager1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "eventSubURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/event/ConnectionManager1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "SCPDURL");
	node3 = ixmlDocument_createTextNode(descDoc, "ConnectionManager.xml");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) Ele3);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	Ele3 = ixmlDocument_createElement(descDoc, "service");
	Ele4 = ixmlDocument_createElement(descDoc, "serviceType");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:schemas-upnp-org:service:AVTransport:1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "serviceId");
	node3 = ixmlDocument_createTextNode(descDoc, "urn:upnp-org:serviceId:AVTransport1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "controlURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/control/AVTransport1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "eventSubURL");
	node3 = ixmlDocument_createTextNode(descDoc, "/upnp/event/AVTransport1");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	Ele4 = ixmlDocument_createElement(descDoc, "SCPDURL");
	node3 = ixmlDocument_createTextNode(descDoc, "AVTransport.xml");
	ixmlNode_appendChild((IXML_Node*) Ele4, node3);
	ixmlNode_appendChild((IXML_Node*) Ele3, (IXML_Node*) Ele4);
	ixmlNode_appendChild((IXML_Node*) Ele2, (IXML_Node*) Ele3);
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	
	ixmlNode_appendChild((IXML_Node*) Ele, (IXML_Node*) Ele2);

	ixmlNode_appendChild(node, (IXML_Node*) Ele);

	return descDoc;
}

char* cRenderer::get_uuid() 
{
	char* home=getenv("HOME");
	char pfad_to_uuid[100];
	sprintf(pfad_to_uuid,"%s/.renderer/uuid",home);
	// uuid holen, bzw generieren
	char uuid[42];
	ifstream uuid_in;
	uuid_in.open(pfad_to_uuid,ifstream::in) ;
	if (!uuid_in.is_open()) {
		return generate_uuid();
	}
	
	uuid_in.read(uuid,45);
	if(strlen(uuid)!=41) {
		return generate_uuid();
	}
	
	uuid_in.close();
	cout << "alt: " << uuid << endl;
	return strdup(uuid);

}

char* cRenderer::generate_uuid() 
{
	char* home=getenv("HOME");
	char pfad_to_uuid[100];
	sprintf(pfad_to_uuid,"%s/.renderer/uuid",home);
	//uuid generieren
	char uuid[42];
	uuid_t uu; 
	uuid_generate(uu);
	sprintf(uuid,"uuid:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			uu[0],uu[1],uu[2],uu[3],uu[4],uu[5],uu[6],uu[7],uu[8],uu[9],uu[10],uu[11],uu[12],uu[13],uu[14],uu[15]);
	ofstream uuid_out;
	uuid_out.open(pfad_to_uuid,ofstream::out);
	if (!uuid_out.is_open())
		cout << "couldn't open uuidfile" << endl;
	uuid_out.write(uuid,strlen(uuid));
	uuid_out.close();
	cout << "neu: " << uuid << endl;
	return strdup(uuid);
}

void cRenderer::trim(string& str)
{
	string::size_type start = str.find_first_not_of(' ');
	string::size_type end = str.find_last_not_of(' ');
	str = str.substr(	start == string::npos ? 0 : start, 
						end == string::npos ? str.length()-1 : end-start+1);
}

void cRenderer::str_replace(string needle, string replacement, string& haystack)
{
	string::size_type start = haystack.find(needle);
	if (start!=string::npos && start>0)
		haystack = haystack.replace( start, needle.length(), replacement);
}

void cRenderer::getVarName(string& varName, string& haystack)
{
	int end = haystack.find_first_of("=");
	if (end>0) {
		varName = haystack.substr(0,end);
	}
}

void cRenderer::getValue(string& value, string& haystack)
{
	int begin = haystack.find_first_of("=");
	if (begin>0) {
		value = haystack.substr(begin+1,haystack.length());
	}
}

void cRenderer::handleOptions(int argc,char* argv[])
{
	char* optionName;
	char* optionValue;
	if(argc!=0)
	{
		for(int i=0;i<argc;i++)
		{
			optionName=argv[i];
			if(!strcmp(optionName,"-a"))
			{
				i++;
				optionValue=argv[i];
				this->ip_addr=strdup(optionValue);
			}else if(!strcmp(optionName,"-f"))
			{
				this->fullscreen="-fs";
			}
		}
	}
}

void cRenderer::initVar()
{
	ip_addr=NULL;
	fullscreen=NULL;
	mplayer_bin="/usr/bin/mplayer";
	char hostname[50];
	gethostname(hostname,50);
	sprintf(friendlyName,"Linux-Renderer(%s)",hostname);
}
