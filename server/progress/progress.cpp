#include "progress.h"
#include "prgNode.h"
#include "connection.h"
#include <netdb.h>
#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;
extern struct node* prgNode[PRG_HASH_SIZE];
extern string ip;
struct connection* head = NULL;
vector<class process*> procs;

void buildProcessCache ()
{
	buildCache ();
	getTcpCt (&head);
	getUdpCt (&head);
	
	for (struct connection* p = head; p != NULL; p = p->_next) {
		for (struct node * q = prgNode[PRG_HASH_FUNC(p->_inode)]; 
			q !=  NULL;  q = q->_next) {
			if (p->_inode == q->_inode) {
				class process* proc = new process (q->_name
								  , p->_port
								  , p->_remIp
								  , p->_remPort
								  , q->_pid); 
				procs.push_back (proc);
			}			
		}		
	}
}

void delProcCache ()
{
	for (size_t i = 0; i < procs.size (); ++i) {
		if (procs[i] != NULL) {
			delete procs[i];
			procs[i] = NULL;
		}
	}
	clearCache ();
}

class process* getProcByName (const std::string& name)
{
	for (size_t i = 0; i < procs.size (); ++i) {
		if (procs[i] != NULL && procs[i]->getName () == name)
			return procs[i];
	}
	return NULL;
}

class process* getProcByPid (int pid)
{
	for (size_t i = 0; i < procs.size (); ++i) {
		if (procs[i] != NULL && procs[i]->getPid () == pid)
			return procs[i];
	}
	return NULL;
}

class process* getProcByPort (unsigned int port)
{
	for (size_t i = 0; i < procs.size (); ++i) {
		if (procs[i] != NULL && procs[i]->getPort () == port)
			return procs[i];
	}
	return NULL;
}

class process* getProcByConInfo (const std::string& sourceIp, 
				 const std::string& dstIp,
				 unsigned int sourcePort,
				 unsigned int dstPort)
{
	for (size_t i = 0; i < procs.size (); ++i) {
		if (procs[i] == NULL)
			continue;
		if (procs[i]->getPort () == sourcePort && 
		    procs[i]->getRemPort () == dstPort &&
		    procs[i]->getRemIp () == dstIp)
			return procs[i];
		else if (procs[i]->getPort () == dstPort && 
		    	 procs[i]->getRemPort () == sourcePort &&
		     	 procs[i]->getRemIp () == sourceIp &&
			 dstIp == ip)
			return procs[i];
	}
	return NULL;

}
