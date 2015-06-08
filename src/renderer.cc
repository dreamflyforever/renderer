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

#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include "cRenderer.h"

#include <sys/stat.h>
#include <sys/types.h>


bool quit;
cRenderer* renderer;

void shutdown() 
{
	delete(renderer);
}

void onSignal(int in)
{
	quit = true;
}

int main(int argc, char *argv[])
{
	// Test, whether directory "$HOME/.renderer" exists
	char* home=getenv("HOME");
	char pfad_to_renderer[100];
	sprintf(pfad_to_renderer,"%s/.renderer",home);
	DIR* dir=opendir(pfad_to_renderer);
	if(dir==NULL)
	{
		mkdir(pfad_to_renderer,0100755);
		cout << "Generating directory: " << pfad_to_renderer << endl; 
	}else
	{
		closedir(dir);
		cout << "Directory(" << pfad_to_renderer << ") exists." << endl;
	}
	
	bool isdaemon=false;
	char* optionName;
	if(argc>1)
	{
		for(int i=1;i<argc;i++)
		{
			optionName=argv[i];
			if(!strcmp(optionName,"-a"))
			{
				i++;
			}else if(!strcmp(optionName,"-f"))
			{
			}else if(!strcmp(optionName,"-d"))
			{
				isdaemon=true;
			}else if(!strcmp(optionName,"-v"))
			{
				cout << "Media Renderer 0.1.1" << endl;
				exit(0);
			}else if(!strcmp(optionName,"-h"))
			{
				// print usage
				cout << "usage: " << endl;
				cout << "\trenderer [options]" << endl;
				cout << endl;
				cout << "options:" << endl;
				cout << "\t-h           : \thelp" << endl;
				cout << "\t-a <ip_addr> : \tset the ip address" << endl;
				cout << "\t-f           : \tenable fullscreen" << endl;
				cout << "\t-d           : \texecute as daemon" << endl;
				cout << "\t-v           : \tshow the version" << endl;
				exit(0);
			}else
			{
				// print usage
				cout << "usage: " << endl;
				cout << "\trenderer [options]" << endl;
				cout << "options:" << endl;
				cout << "\t-h           : \thelp" << endl;
				cout << "\t-a <ip_addr> : \tset the ip address" << endl;
				cout << "\t-f           : \tenable fullscreen" << endl;
				cout << "\t-d           : \texecute as daemon" << endl;
				cout << "\t-v           : \tshow the version" << endl;
				exit(0);
			}
		}
	}

	if(isdaemon)
	{
		signal(SIGCHLD,SIG_IGN);
		pid_t daemonpid;
		if ( (daemonpid = fork()) < 0)
		{
			// FATAL: cannot fork child 
			cout << "cannot fork child" << endl;
		}else if(daemonpid==0)
		{
			int daemonfd;
			char pfad_to_logfile[100];
			sprintf(pfad_to_logfile,"%s/.renderer/log",home);
			FILE* file=fopen(pfad_to_logfile,"w");
			fclose(file);
			daemonfd=open(pfad_to_logfile,O_WRONLY,O_CREAT);
			if(dup2(daemonfd, 1) == -1  &&  close(daemonfd) == -1)
			{
				perror("dup2");
				exit(1);
			}
			signal(SIGTERM, onSignal);
			signal(SIGINT, onSignal);
			signal(SIGKILL, onSignal);
			quit = false;
			renderer = new cRenderer(argc,argv);

			while (!quit) {
				usleep(100000);
			}
			shutdown();
			cout << "normal beendet." << endl;
			return 0;
		}else
		{
			exit(0);
		}
	}else
	{
		signal(SIGTERM, onSignal);
		signal(SIGINT, onSignal);
		signal(SIGKILL, onSignal);
		quit = false;
		renderer = new cRenderer(argc,argv);

		
		//	daemon(0,0);
		while (!quit) {
			usleep(100000);
		}
		shutdown();
		cout << "normal beendet." << endl;
		return 0;
	}
}
