// protobuf_test.cpp : 定义控制台应用程序的入口点。
//


#include <google/protobuf/stubs/common.h>

#include "Head.pb.h"
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <streambuf>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>


#pragma comment(lib, "libprotobuf.lib")

static int computeRawVarint32Size(const int value) {  
	if ((value & (0xffffffff <<  7)) == 0) return 1;  
	if ((value & (0xffffffff << 14)) == 0) return 2;  
	if ((value & (0xffffffff << 21)) == 0) return 3;  
	if ((value & (0xffffffff << 28)) == 0) return 4;  
	return 5;
} 


#define PORT 12580  
#define MAXDATASIZE 20  
#define BACKLOG 10  

using namespace std;
using namespace com::bbk::vivoAssistant::server::codec;


// using ::google::protobuf::io::CodedOutputStream;
// using ::google::protobuf::io::CodedInputStream;
// using ::google::protobuf::io::ZeroCopyOutputStream;
// using ::google::protobuf::io::OstreamOutputStream;
// using ::google::protobuf::io::ArrayOutputStream;

using namespace google::protobuf::io;

int _tmain(int argc, _TCHAR* argv[])
{
	Protocal protocal;
	protocal.set_req("ack");
	protocal.set_version("ffffffffff");

	WSADATA wsaData;
	SOCKET ListeningSocket;
	SOCKET NewConnection;
	SOCKADDR_IN ServerAddr;
	SOCKADDR_IN ClientAddr;
	int Port = PORT;

	// 初始化Windows Socket 2.2

	WSADATA t_wsa; // WSADATA structure
	WORD wVers; // version number
	int iError; // error number

	wVers = MAKEWORD(2, 2); // Set the version number to 2.2
	iError = WSAStartup(wVers, &t_wsa); // Start the WSADATA

	if(iError != NO_ERROR || iError == 1){
		MessageBox(NULL, (LPCTSTR)"Error at WSAStartup()", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
		WSACleanup();
		return 0;
	}

	if(LOBYTE(t_wsa.wVersion) != 2 || HIBYTE(t_wsa.wVersion) != 2){
		MessageBox(NULL, (LPCTSTR)"Error at WSAStartup()", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
		WSACleanup();
		return 0;
	}

	SOCKET sClient;
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if(sClient == INVALID_SOCKET || iError == 1){
		MessageBox(NULL, (LPCTSTR)"Invalid Socket!", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN sinClient;
	memset(&sinClient, 0, sizeof(sinClient));

	char cIP[50];
	strcpy(cIP, "127.0.0.1");
	sinClient.sin_family = AF_INET;
	sinClient.sin_addr.s_addr = inet_addr(cIP); // Where to start server?
	sinClient.sin_port = htons(Port); // Port

	if(connect(sClient, (LPSOCKADDR)&sinClient, sizeof(sinClient)) == SOCKET_ERROR){
		/* failed at starting server */
		MessageBox(NULL, (LPCTSTR)"Could not connect to the server!", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
		WSACleanup();
		return 0;
	}
	// Now we can send/recv data!
	int iRet;

	std::string data;
	protocal.SerializeToString(&data);

	int len = data.length();
	
 	int lenbytes = computeRawVarint32Size(len);

// 	char buffer_size[5];
// 	int size = 0;
// 
// 	while (len > 0x7F) {
//		buffer_size[size++] = (static_cast<char>(len) & 0x7F) | 0x80;
// 		len >>= 7;
// 	}
// 	buffer_size[size++] = static_cast<char>(len) & 0x7F;

	ll:	char tmpArr[1000];
	memset(tmpArr,0,sizeof(tmpArr));
	ZeroCopyOutputStream *raw_output = new ArrayOutputStream(tmpArr,protocal.ByteSize()+1);    
	CodedOutputStream* coded_output = new CodedOutputStream(raw_output);  
// 

// 
// 	char *buffer = new char[len + lenbytes];
// 
// 	ofstream os(buffer);

// 	CodedOutputStream output(&bb);
// 
// 	output.WriteVarint32(protocal.ByteSize());
//	protocal.SerializeWithCachedSizes(&output);
// 	int len = 0;
// 	output.GetDirectBufferPointer(&bb, &len);

// 	ZeroCopyOutputStream *raw_output = new OstreamOutputStream(&os);
// 	CodedOutputStream *coded_output = new CodedOutputStream(raw_output);



	coded_output->WriteVarint32(protocal.ByteSize());
	if (!protocal.SerializeToCodedStream(coded_output))
	{
		cerr<<"Fail to serial addressbook data!\n";
		return 0;
	}
	

//	_write(fd,tmpArr,addressBook.ByteSize()+1);

// 	delete coded_output;
// 	delete raw_output;

	//send(sClient, buffer_size, size, 0);//直接写

// 	char *buffer = new char[len];
// 	strcpy(buffer, data.c_str());
	
	iRet = send(sClient, tmpArr, protocal.ByteSize() + lenbytes, 0);
	if(iRet == SOCKET_ERROR){
		MessageBox(NULL, (LPCTSTR)"Could not send data!", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
		WSACleanup();
		return 0;
	}
	//delete buffer;

	delete coded_output;
	delete raw_output;

	int bytes;
	bytes = SOCKET_ERROR;
	char *cServerMessage;
	cServerMessage = new char[2048];

	memset(cServerMessage,0,sizeof(cServerMessage));



	while(bytes = recv(sClient, cServerMessage, 2047, 0)){
		if(bytes == SOCKET_ERROR){
			char cError[500];
			sprintf(cError, "Connection failed, WINSOCK error code: %d", WSAGetLastError());
			MessageBox(NULL, (LPCTSTR)cError, (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
			closesocket(sClient);
			// Shutdown Winsock
			WSACleanup();
			return 0;
		}
		if (bytes == 0 || bytes == WSAECONNRESET) {
			MessageBox(NULL, (LPCTSTR)"Connection Disconnected!", (LPCTSTR)"Client::Error", MB_OK|MB_ICONERROR);
			closesocket(sClient);
			// Shutdown Winsock
			WSACleanup();
			return 0;
		}
		if(bytes < 1){
			Sleep(300);
			continue;
		}
		
		Protocal p;

		OutputDebugString(_T("server response\n"));
		
		ZeroCopyInputStream *raw_input = new ArrayInputStream(cServerMessage,2048);    
		CodedInputStream* coded_input = new CodedInputStream(raw_input); 
	
		unsigned int  size;
		coded_input->ReadVarint32(&size);

		p.ParseFromCodedStream(coded_input);
		
		std::string data = cServerMessage;

		
		
		cout<< p.req() << endl;
		cout<< p.version() << endl;
// 		OutputDebugString(p.req());
// 		OutputDebugString(p.version());
// // 		delete [] cServerMessage;
// 		cServerMessage = new char[2048];
		Sleep(100); // Don't consume too much CPU power.

		goto ll;
	}

	delete [] cServerMessage;
	// Cleanup
	closesocket(sClient);
	// Shutdown Winsock
	WSACleanup();

	while(true)Sleep(2000);
	
	return 0;
}

