#define _CRT_SECURE_NO_WARNINGS
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN 
#undef UNICODE
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

#define SplitLine() \
for (int index = 0; index < 54; index++) {\
    printf("=");\
    }\
    printf("\n");

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

typedef struct ListenParameter {
    SOCKET* listenSocket;
    SOCKET* clientSocket;
}ListenParameter;

int isRunning = 1;

int  __cdecl QueryLocalIP();
void KeyInputHandler(void* arg);

int ClientHandler(void* arg);
void ListenTerminateHandler(void* arg);

int main(int argc,char* argv[])
{
    WSADATA wsaData;
    int iResult;
    INT iRetval;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;
   
    ListenParameter listenParameter;

    // Validate the parameters
    if (argc != 2 && argc != 1) {
        printf("usage: %s server-port\n", argv[0]);
        return 1;
    }

    QueryLocalIP();

    SplitLine();
    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;//AF_INET,AF_INET
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    
    char port[20];
    if (!argv[1])
        strcpy(port, DEFAULT_PORT);
    else
        strcpy(port, argv[1]);

    
    // Resolve the server address and port
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d %u\n", iResult, WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("Server running on port:%s\n", port);
    printf("\n");

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    
    
    listenParameter.listenSocket = (void*)ListenSocket;
    listenParameter.clientSocket = (void*)ClientSocket;
    
    HANDLE keyThread = NULL;
    
    HANDLE hThread = NULL;
    keyThread = CreateThread(NULL, 0, KeyInputHandler, (void*)port, 0, NULL);
    while (isRunning) {

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        if (isRunning != 0) {
            hThread = CreateThread(NULL, 0, ClientHandler, (void*)ClientSocket, 0, NULL);
            if (hThread == NULL) {
                printf("CreateThread failed with error: %d\n", GetLastError());
                closesocket(ClientSocket); // 如果线程创建失败，关闭套接字  
            }
        }
        else {
            shutdown(ClientSocket, SD_BOTH);
            closesocket(ClientSocket);
        }
        
    }
    
    // No longer need server socket
    closesocket(ListenSocket);
    
    
    
    WaitForSingleObject(hThread, INFINITE);
    // Shutting down server
    printf("\rShutting down server...Done\n");
    fflush(stdout);
    
    WSACleanup();

    return 0;
}

int ClientHandler(void* arg) {
    SOCKET ClientSocket = (SOCKET)arg;
    int iResult;
    INT iRetval;
    int iSendResult;
    char recvBuffer[DEFAULT_BUFLEN];
    int recvBufLen = DEFAULT_BUFLEN;

    SplitLine();
    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvBuffer, recvBufLen, 0);
        if (iResult > 0) {
            printf("Bytes received:\t%d\n", iResult);
            printf("Rev msg:\t");
            recvBuffer[iResult] = '\0';
            printf("%s\n", recvBuffer);
            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvBuffer, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();

                return 1;
            }
            printf("Bytes sent:\t%d\n", iSendResult);
        }
        else if (iResult == 0) {
            printf("Connection closing...");
            fflush(stdout);
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();

            return 1;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();

        return 1;
    }
    printf("\rConnection closing...Done\n");
    fflush(stdout);
    closesocket(ClientSocket);
    SplitLine();
    return 0;
}

void KeyInputHandler(void* arg) {
    HANDLE listenTerminateThread = NULL;
    int KeyValue = 0;
    while (isRunning) {
        KeyValue = getch();
        printf("User Input:%c\n", KeyValue);
        switch (KeyValue)
        {
        case 'q':
        case 'Q':
        case 27:
            isRunning = 0;
            CreateThread(NULL, 0, ListenTerminateHandler, arg, 0, NULL);
            
            break;
        default:
            break;
        }
    }
    return 0;
}

void ListenTerminateHandler(void* arg) {
    printf("Shutting down server...");

    char TargetPort[20];
    strcpy(TargetPort, arg);
    SOCKET ConnectSocket;
    struct addrinfo hints;
    struct addrinfo* result = NULL,
        * ptr = NULL;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    iResult = getaddrinfo("127.0.0.3", TargetPort, &hints, &result);
    if (iResult != 0) {
        return 1;
    }


    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    // shutdown the connection
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        return 1;
    }
    closesocket(ConnectSocket);
    return 0;
}

int  __cdecl QueryLocalIP() {
    WSADATA wsaData;
    int iResult;
    INT iRetval;

    DWORD dwRetval;

    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    struct sockaddr_in* sockaddr_ipv4;
    struct sockaddr_in6* sockaddr_ipv6;


    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;

    int index = 1;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    //--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    char host[255] = "";
    if (gethostname(host, sizeof(host)) == SOCKET_ERROR) {
        printf("Failed getting local host name\n");
    }
    else {
        printf("Host name:%s\n", host);
    }

    //--------------------------------
// Call getaddrinfo(). If the call succeeds,
// the result variable will hold a linked list
// of addrinfo structures containing response
// information
    dwRetval = getaddrinfo(host, NULL, &hints, &result);
    if (dwRetval != 0) {
        printf("getaddrinfo failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        printf("getaddrinfo response %d\n", index++);
        //printf("\tFlags: 0x%x\n", ptr->ai_flags);
        printf("\tFamily: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC:
            printf("Unspecified\n");
            break;
        case AF_INET:
            printf("AF_INET (IPv4)\n");
            sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            printf("\tIPv4 address %s\n",
                InetNtop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, 46));

            break;
        case AF_INET6:
            printf("AF_INET6 (IPv6)\n");
            // the InetNtop function is available on Windows Vista and later
            sockaddr_ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            printf("\tIPv6 address %s\n",
                InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46));

        case AF_NETBIOS:
            printf("AF_NETBIOS (NetBIOS)\n");
            break;
        default:
            printf("Other %ld\n", ptr->ai_family);
            break;
        }
        
        printf("\tLength of this sockaddr: %I64d\n", ptr->ai_addrlen);
    }
    freeaddrinfo(result);
    WSACleanup();
    return 0;
}

/*
int ListenHandler(ListenParameter* arg);


listenThread = CreateThread(NULL, 0, ListenHandler, &listenParameter, 0, NULL);
keyThread = CreateThread(NULL, 0, KeyInputHandler, (void*)listenThread, 0, NULL);
if (listenThread == NULL) {
    printf("CreateThread failed with error: %d\n", GetLastError());
}
else {
    WaitForSingleObject(listenThread, INFINITE);
    GetExitCodeThread(listenThread, &listenRet);
    ListenSocket = (SOCKET)listenParameter.listenSocket;
    ClientSocket = (SOCKET)listenParameter.clientSocket;
}
int ListenHandler(ListenParameter* arg) {
    SOCKET ListenSocket = (SOCKET)arg->listenSocket;
    SOCKET ClientSocket = (SOCKET)arg->clientSocket;

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    arg->listenSocket = ListenSocket;
    arg->clientSocket = ClientSocket;

    return 0;
}

void KeyInputHandler(void* arg) {
HANDEL listenThread=(HANDEL)arg;
    int KeyValue = 0;
    while (isRunning) {
        KeyValue = getch();
        printf("User Input:%c\n", KeyValue);
        switch (KeyValue)
        {
        case 'q':
        case 'Q':
        case 27:
            isRunning = 0;
            TerminateThread(listenThread,2)
            break;
        default:
            break;
        }
    }
    return 0;
}

*/