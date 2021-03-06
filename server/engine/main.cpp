#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <vector>

#include "../progress/progress.h"
#include "../capturer/filter/filter.h"
#include "../capturer/capturer.h"
#include "../util/utilFunc.h"
#include "../util/sysDefine.h"

struct sysConfig config;
extern std::vector<class process*> procs;
extern std::string ip;

static void help ()
{
	std::cerr << "useage: netpop [-V] [-d seconds] [-n processname] [-p pid] [-k speedlimit(kb/s)]\n";
	std::cerr << "        -V : print version info\n";
	std::cerr << "        -d : delay for update refresh rate in seconds, default is 2\n";
	std::cerr << "        -n : monitor specific process by it's full name (not recommend)\n";
	std::cerr << "        -p : monitor specific process by it's pid(recommend)\n";
	std::cerr << "        -k : set the limit of bandwith (kb/s). the process will be\n"; 		std::cerr << "             killed when it's speed is bigger than the limit\n";
}

static void getConfig (char** argv)
{
	for (++argv; *argv; ++argv) {
		if (**argv == '-') {
			switch (*(++*argv)) {
				case 'V':
					std::cout << "version 1.1.1 , programed by richard , in 2013.10\n";
					exit (0);
				case 'd':
					config._refreshDelay = Atoi (*(++argv));
					break;
				case 'n':
					config._processName = *(++argv);
					break;
				case 'p':
					config._processPid = Atoi (*(++argv));
					break;
				case 'k':
					config._speedLimit = Atoi (*(++argv));
					break;
				default :
					help ();
					exit (0);
			}			
		}
		else {
			help ();
			exit (0);
		}
	}		
}

static std::string getNetDevInfo ()
{
	 char name[20], ipAddress[20];
         if (getNetInfo (name, ipAddress) == -1) {
         	std::cerr << "get interface info failed\n";
		exit (-1);
         }
	 
	 ip = ipAddress;
	 return std::string(name);
}

int main (int argc, char** argv)
{
	try {
		getConfig (argv);
		if (signal (SIGALRM, sigAlrm) == SIG_ERR) {
			std::cout << "set signal alarm error" << "\n";	
			exit (1);	
		}
		if (signal (SIGINT, sigInt) == SIG_ERR) {
			std::cout << "set signal int error" << "\n";
			exit (1);
		}

		if (config._processPid != -1 || config._processName != NULL) {
			buildProcessCache ();			
			process* proc = NULL;			
			if (config._processPid != -1) {
				proc=getProcByPid(config._processPid);			
			}		
			else if (config._processName != NULL) {
				proc = getProcByName (config._processName);	
			}	
			if (proc == NULL) {
				std::cerr << "get process failed\n";
				exit (0);			
			}			
			
			std::string devName = getNetDevInfo ();
			capturer ct (devName.c_str ());
			filter fr;
			char temp[180];
			sprintf (temp, "(src host %s and src port %d) or \
			(dst host %s and dst port %d)", ip.c_str (),
			proc->getPort (), ip.c_str (), proc->getPort ());

			fr.setRule (temp);
			ct.capturePackage (&fr, packageHandle, config._refreshDelay);
		}
		else {	
			buildProcessCache ();

			filter fr;
			char temp[] = "udp or tcp";
			fr.setRule (temp);
			
			std::string devName = getNetDevInfo ();
			std::cout << "---" << devName;
			capturer ct (devName.c_str ());
			ct.capturePackage (&fr, packageHandle, config._refreshDelay);
		}	
	}
	catch (myException& e) {
		std::cout << "Exception: "<< e.what () << "\n";
	}


	return 0;
}
