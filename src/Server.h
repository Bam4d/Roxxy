/*
 * Server.h
 *
 *  Created on: 7 Mar 2016
 *      Author: bam4d
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <proxygen/httpserver/HTTPServer.h>

using namespace proxygen;

/**
 * Small wrapper for starting proxygen server
 */
class Server {
public:
	Server(int threads, int port, std::string ip, int numBrowsers, std::string resourceFilterFilename);
	virtual ~Server();

	void StartServer();

private:
	std::string ip;
	int port;
	int threads;

	HTTPServer* server;
	std::vector<HTTPServer::IPConfig> IPs;

	std::thread serverListenerThread;
};

#endif /* SERVER_H_ */
