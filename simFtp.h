#pragma once

#include "simSock.h"
#include "queue.h"
#include "helper.h"
#include "strmanip.h"

#define SYS_UNIX	0
#define SYS_WINDOWS 1

/*
FTP Replies
100 	Series: The requested action is being initiated, expect another reply before proceeding with a new command.
110 	Restart marker replay . In this case, the text is exact and not left to the particular implementation; it must read: MARK yyyy = mmmm where yyyy is User-process data stream marker, and mmmm server's equivalent marker (note the spaces between markers and "=").
120 	Service ready in nnn minutes.
125 	Data connection already open; transfer starting.
150 	File status okay; about to open data connection.
200 	Command okay.
202 	Command not implemented, superfluous at this site.
211 	System status, or system help reply.
212 	Directory status.
213 	File status.
214 	Help message.On how to use the server or the meaning of a particular non-standard command. This reply is useful only to the human user.
215 	NAME system type. Where NAME is an official system name from the registry kept by IANA.
220 	Service ready for new user.
221 	Service closing control connection.
225 	Data connection open; no transfer in progress.
226 	Closing data connection. Requested file action successful (for example, file transfer or file abort).
227 	Entering Passive Mode (h1,h2,h3,h4,p1,p2).
228 	Entering Long Passive Mode (long address, port).
229 	Entering Extended Passive Mode (|||port|).
230 	User logged in, proceed. Logged out if appropriate.
231 	User logged out; service terminated.
232 	Logout command noted, will complete when transfer done.
250 	Requested file action okay, completed.
257 	"PATHNAME" created.
331 	User name okay, need password.
332 	Need account for login.
350 	Requested file action pending further information
421 	Service not available, closing control connection. This may be a reply to any command if the service knows it must shut down.
425 	Can't open data connection.
426 	Connection closed; transfer aborted.
430 	Invalid username or password
434 	Requested host unavailable.
450 	Requested file action not taken.
451 	Requested action aborted. Local error in processing.
452 	Requested action not taken. Insufficient storage space in system.File unavailable (e.g., file busy).
500 	Syntax error, command unrecognized. This may include errors such as command line too long.
501 	Syntax error in parameters or arguments.
502 	Command not implemented.
503 	Bad sequence of commands.
504 	Command not implemented for that parameter.
530 	Not logged in.
532 	Need account for storing files.
550 	Requested action not taken. File unavailable (e.g., file not found, no access).
551 	Requested action aborted. Page type unknown.
552 	Requested file action aborted. Exceeded storage allocation (for current directory or dataset).
553 	Requested action not taken. File name not allowed.
631 	Integrity protected reply.
632 	Confidentiality and integrity protected reply.
633 	Confidentiality protected reply.
*/

//X00
#define TYPE_POSITIVE_PRELIMINARY	1
#define TYPE_POSITIVE_COMPLETION	2
#define TYPE_POSITIVE_INTERMEDIATE	3
#define TYPE_NEGATIVE_TRANSIENT		4
#define TYPE_NEGATIVE_PERMANENT		5
#define TYPE_PROTECTED_REPLY		6

//0X0
#define TYPE_SYNTAX			0
#define TYPE_INFORMATION	1
#define TYPE_CONNECTIONS	2
#define TYPE_AUTHENTICATION 3
#define TYPE_UNUSED			4
#define TYPE_FILESYSTEM		5

//Positive Preliminary Replies
#define REPLY_SERIES						100
#define REPLY_RESTART_MARKER				110
#define REPLY_SERVICE_READY					120
#define REPLY_DATA_CONNECTION_ALREADY_OPEN	125
#define REPLY_FILE_STATUS_OK				150

//Positive Completion Replies
#define REPLY_COMMAND_OK					200
#define REPLY_COMMAND_NOT_IMPLEMENTED		202
#define REPLY_SYSTEM_STATUS					211
#define REPLY_DIRECTORY_STATUS				212
#define REPLY_FILE_STATUS					213
#define REPLY_HELP_MESSAGE					214
#define REPLY_NAME							215
#define REPLY_SYSTEM_READY					220
#define REPLY_CLOSING_CONNECTION			221
#define REPLY_DATA_CONNECTION_OPEN			225
#define REPLY_FILE_XFER_SUCCESSFUL			226
#define REPLY_ENTERING_PASSIVE_MODE			227
#define REPLY_ENTERING_LONG_PASSIVE_MODE	228
#define REPLY_ENTERING_EXT_PASSIVE_MODE		229
#define REPLY_USER_LOGGED_IN				230
#define REPLY_USER_LOGGED_IN_TERM			231
#define REPLY_LOGOUT_COMMAND_NOTED			232
#define REPLY_FILE_ACTION_OK				250
#define REPLY_FILE_CREATED					257

//Positive Intermediate Replies
#define REPLY_NEED_PASSWORD					331
#define REPLY_NEED_ACCOUNT					332
#define REPLY_FILE_ACTION_PENDING			350

//Negative Transient Replies
#define REPLY_SERVICE_NOT_AVAILABLE			421
#define REPLY_CANNOT_OPEN_DATA_CONN			425
#define REPLY_CONNECTION_CLOSED				426
#define REPLY_INVALID_USERNAME_PASS			430
#define REPLY_HOST_UNAVAILABLE				434
#define REPLY_FILE_ACTION_NOT_TAKEN			450
#define REPLY_ACTION_ABORTED				451
#define REPLY_FILE_BUSY						452

//Negative Permanent Replies
#define REPLY_SYNTAX_ERROR					500
#define REPLY_SYNTAX_ERROR_PARAM			501
#define REPLY_COMMAND_NOT_IMPLEMENTED		502
#define REPLY_BAD_COMMAND_SEQUENCE			503
#define REPLY_COMMAND_NOT_IMPLEMENTED_PARAM 504
#define REPLY_NOT_LOGGED_IN					530
#define REPLY_NEED_ACCOUNT_STOR				532
#define REPLY_FILE_UNAVILABLE				550
#define REPLY_UNKNOWN_PAGE_TYPE				551
#define REPLY_STORAGE_EXCEED				552
#define REPLY_INVALID_FILENAME				553

//Protected Replies
#define REPLY_INTEGRITY_PROTECTED_REPLY		631
#define REPLY_CONF_AND_INT_PROTECTED_REPLY	632
#define REPLY_CONFIDENTIALY_PROTECTED_REPLY	633

struct FTP_Control_Message
{
	char code[3];
	int dCode;
	char message[1024];
	int type1;
	int type2;
	bool error;
	FTP_Control_Message(char* line);
	FTP_Control_Message() {};
};

FTP_Control_Message::FTP_Control_Message(char* line)
{
	char *next;
	char *p;

	p = strtok_s(line, " ", &next);
	
	if(next!=NULL)
		strcpy(message, next);
	else
		strcpy(message, "None");

	if(p!=NULL)
	{
		strcpy(code, p);
		dCode = atoi(p);
	}
	else
	{
		strcpy(code, "-1");
		error = true;
		return;
	}

	if(code[0]=='4' || code[0]=='5')
		error = true;
	else
		error = false;

	switch(code[0]){
	case 1:
		type1 = TYPE_POSITIVE_PRELIMINARY;
		break;
	case 2:
		type1 = TYPE_POSITIVE_COMPLETION;
		break;
	case 3:
		type1 = TYPE_POSITIVE_INTERMEDIATE;
		break;
	case 4:
		type1 = TYPE_NEGATIVE_TRANSIENT;
		break;
	case 5:
		type1 = TYPE_NEGATIVE_PERMANENT;
		break;
	case 6:
		type1 = TYPE_PROTECTED_REPLY;
		break;
	}

	switch(code[1]){
	case 0:
		type2 = TYPE_SYNTAX;
		break;
	case 1:
		type2 = TYPE_INFORMATION;
		break;
	case 2:
		type2 = TYPE_CONNECTIONS;
		break;
	case 3:
		type2 = TYPE_AUTHENTICATION;
		break;
	case 4:
		type2 = TYPE_UNUSED;
		break;
	case 5:
		type2 = TYPE_FILESYSTEM;
		break;
	}
}

Queue<FTP_Control_Message*>* queue;

struct Flags
{
	bool ownerRead;
	bool ownerWrite;
	bool ownerExecute;

	bool groupRead;
	bool groupWrite;
	bool groupExecute;

	bool otherRead;
	bool otherWrite;
	bool otherExecute;

	bool isDirectory;
};

class Directory;
class Directory_Entry;

class Directory
{
private:
	Directory_Entry** entries;
	char data[4096];
	int nEntries;
public:
	Directory() {nEntries = 0; entries = new Directory_Entry*[4096];}
	Directory(char* data, int system);
	void AddEntry(char* line, int system);
	Directory_Entry* GetEntry(int i) {if(i<nEntries) return entries[i]; else return NULL;}
	int numEntries() {return nEntries;}
};


class Directory_Entry
{
private:
	char line[4096];
	char perm[10];
	char owner[16];
	char group[16];
	char unk1[16];
	char size[16];
	char month[16];
	char day[3];
	char yearOrTime[16];
	char filename[32];
	Flags flags;
public:
	Directory_Entry(char* line, int system);
	char* getMonth() {return month;}
	int getMonthN() 
	{
		if(strcmp(month, "January")==0)
			return 1;
		else if(strcmp(month, "Feb")==0)
			return 2;
		else if(strcmp(month, "Mar")==0)
			return 3;
		else if(strcmp(month, "Apr")==0)
			return 4;
		else if(strcmp(month, "May")==0)
			return 5;
		else if(strcmp(month, "Jun")==0)
			return 6;
		else if(strcmp(month, "Jul")==0)
			return 7;
		else if(strcmp(month, "Aug")==0)
			return 8;
		else if(strcmp(month, "Sep")==0)
			return 9;
		else if(strcmp(month, "Oct")==0)
			return 10;
		else if(strcmp(month, "Nov")==0)
			return 11;
		else if(strcmp(month, "Dec")==0)
			return 12;
		else if(strcmp(month, "Smarch")==0)
		{
			printf("Lousy Smarch weather!\n");
			return 13;
		}
		else
			return 0;
	}
	int getDay() {return atoi(day);}
	char* getFileName() {return filename;}
	Flags* getFlags() {return &flags;}
};

Directory::Directory(char* data, int system)
{
	char* line;
	char* p;
	char* next;
	int lines = 0;
	nEntries = 0;

	strcpy(this->data, data);

	p = data;

	while(*p)
	{
		if(*p=='\n')
			lines++;
		p++;
	}

	entries = new Directory_Entry*[lines];

	line = strtok_s(data, "\r\n", &next);

	do {
		entries[nEntries++] = new Directory_Entry(line, system);
	}while((line = strtok_s(NULL, "\r\n", &next))!=NULL);
}

void Directory::AddEntry(char* line, int system)
{
	entries[nEntries++] = new Directory_Entry(line, system);
}

Directory_Entry::Directory_Entry(char* line, int system)
{
	char* field;
	char* fields[32];
	char* next;
	int i = 0;

	flags.ownerRead = false;
	flags.ownerWrite = false;
	flags.ownerExecute = false;
	flags.groupRead = false;
	flags.groupWrite = false;
	flags.groupExecute = false;
	flags.otherRead = false;
	flags.otherWrite = false;
	flags.otherExecute = false;
	flags.isDirectory = false;

	Chop(line);

	//printf("%s\n", line);

	field = strtok_s(line, " \t", &next);
	do {
		//printf("[%s]  ", field);
		fields[i++] = field;
	}while((field = strtok_s(NULL, " \t", &next))!=NULL);
	//printf("\n");

	if(i>=8 && system==SYS_UNIX)
	{
		strcpy(perm, fields[0]);
		if(strlen(perm)>=10)
		{
			if(perm[0]!='-')
				flags.isDirectory = true;
			else if(perm[1]!='-')
				flags.ownerRead = true;
			else if(perm[2]!='-')
				flags.ownerWrite = true;
			else if(perm[3]!='-')
				flags.ownerExecute = true;
			else if(perm[4]!='-')
				flags.groupRead = true;
			else if(perm[5]!='-')
				flags.groupWrite = true;
			else if(perm[6]!='-')
				flags.groupExecute = true;
			else if(perm[7]!='-')
				flags.otherRead = true;
			else if(perm[8]!='-')
				flags.otherWrite = true;
			else if(perm[9]!='-')
				flags.otherExecute = true;
		}
		
		strcpy(unk1, fields[1]);
		strcpy(owner, fields[2]);
		strcpy(group, fields[3]);
		strcpy(size, fields[4]);
		strcpy(month, fields[5]);
		strcpy(day, fields[6]);
		strcpy(yearOrTime, fields[7]);
		strcpy(filename, fields[8]);

		//if(flags.isDirectory)
			//printf("Directory:\n");
		//else
			//printf("File:\n");

		//printf("\tPerm: %s\n\tOwner: %s\n\tGroup: %s\n\tSize: %s bytes\n\tUnk1: %s\n\tMonth: %s\n\tDay: %s\n\tYearOrTime: %s\n\tFilename: %s\n\n", perm, owner, group, size, unk1, month, day, yearOrTime, filename);
	}
}

DWORD WINAPI ControlThread(LPVOID lpargs)
{
	int n;
	char buffer[1024];
	FTP_Control_Message* ftp;
	SOCKET control = (SOCKET)lpargs;

	while((n = ReadLineSocket(control, buffer, 1024))>0)
	{
		buffer[n] = 0;
		printf("%s", buffer);
		ftp = new FTP_Control_Message(buffer);
		queue->enqueue(ftp);
	}

	CloseSocket(control);

	ExitThread(0);
}

class FTP_Connection {
private:
	SOCKET control;
	char ip[32];
	char username[32];
	char password[32];
	char mode;
	int system;
	bool OpenDataConnection(SOCKET* data);
	bool Decode(char* buffer, FTP_Control_Message* ftp_msg);
	FTP_Control_Message* WaitForReply(int code);
public:
	FTP_Connection(char* ip, char* username, char* password, char mode = 'b');
	void connect();
	void disconnect();
	Directory* ls(int *nLines = NULL);
	void cd(char* remoteDirectory);
	void get(char* remoteFilePath, char* localFilePath);
	void put(char* localFilePath, char* remoteFilePath);
	void binary();
	void ascii();
	char* syst();
	int size(char* filename);
	char getMode() {return mode;}
};

FTP_Connection::FTP_Connection(char* ip, char* username, char* password, char mode)
{
	strcpy(this->ip, ip);
	strcpy(this->username, username);
	strcpy(this->password, password);
	this->mode = mode;
	queue = new Queue<FTP_Control_Message*>();
}

FTP_Control_Message* FTP_Connection::WaitForReply(int code)
{
	FTP_Control_Message* ftp_msg;
	while(true)
	{
		if(!queue->isEmpty())
		{
			ftp_msg = queue->dequeue();
			if(ftp_msg->dCode==code)
				return ftp_msg;
			else if(ftp_msg->dCode==431)
			{
				delete ftp_msg;
				return NULL;
			}
			else if(ftp_msg->type1==TYPE_NEGATIVE_TRANSIENT)
			{
				if(code/300 && ftp_msg->dCode==code+100)
				{
					delete ftp_msg;
					return NULL;
				}
				else if(code/200 && ftp_msg->dCode==code+200)
				{
					delete ftp_msg;
					return NULL;
				}
				else if(code/100 && ftp_msg->dCode==code+300)
				{
					delete ftp_msg;
					return NULL;
				}
			}
			else if(ftp_msg->type1==TYPE_NEGATIVE_PERMANENT)
			{
				if(code/300 && ftp_msg->dCode==code+200)
				{
					delete ftp_msg;
					return NULL;
				}
				else if(code/200 && ftp_msg->dCode==code+300)
				{
					delete ftp_msg;
					return NULL;
				}
				else if(code/100 && ftp_msg->dCode==code+400)
				{
					delete ftp_msg;
					return NULL;
				}
			}
			else
				delete ftp_msg;
		}
		else
			Sleep(0);
	}
}

bool FTP_Connection::OpenDataConnection(SOCKET* data)
{
	char buffer[4096];
	char data_ip[32];
	char data_port[32];
	char** octets;
	int nOctets;
	int port;
	char* p1;
	char* p2;
	int n;
	FTP_Control_Message* ftp_msg;

	//Set passive mode
	WriteSocket(control, "PASV\r\n", strlen("PASV\r\n"));
	//printf("PASV\n");
	if((ftp_msg = WaitForReply(227))==NULL)
		return false;

	//Derive ip and port from response
	p1 = strstr(ftp_msg->message, "(");
	if(!p1)
		return false;
	p1++;
	p2 = strstr(p1, ")");
	if(!p2)
		return false;
	*p2 = 0;
	octets = SplitString(p1, &nOctets);

	if(nOctets==6)
	{
		//Construct ip address and port for data connection
		sprintf(data_ip, "%s.%s.%s.%s", octets[0], octets[1], octets[2], octets[3]);
		port = 256 * atoi(octets[4]);
		port += atoi(octets[5]);
		sprintf(data_port, "%d", port);
		//printf("Opening connection back to %s:%s\n", data_ip, data_port);

		//Open data connection
		if(OpenClientSocket(data, data_ip, data_port)==-1)
		{
			printf("Unable to connect to ftp data port!\n");
			return false;
		}
		else
			return true;
	}
	else
	{
		printf("Unrecognized response!\n");
		return false;
	}
}

void FTP_Connection::connect()
{
	char userString[1024];
	char passString[1024];
	char buffer[4096];
	char data_ip[32];
	char data_port[8];
	char* system;
	int port;
	int n;

	//Connect to FTP server
	printf("Connecting to %s:21...",ip);
	if(OpenClientSocket(&control, ip, "21")==-1)
	{
		printf("failed!\n");
		return;
	}
	printf("connected!\n");

	CreateThread(NULL, 0, ControlThread, (LPVOID)control, 0, NULL);

	WaitForReply(220);

	printf("Logging in as %s...", username);
	//Send USER command
	sprintf(userString, "USER %s\r\n", username);
	if(WriteSocket(control, userString, strlen(userString), MSG_OOB)==-1)
		printf("ERROR: %d\n", WSAGetLastError());
	//printf("%s", userString);
	if(WaitForReply(331)==NULL)
	{
		printf("failed!\n");
		return;
	}

	//Send PASS command
	sprintf(passString, "PASS %s\r\n", password);
	if(WriteSocket(control, passString, strlen(passString), MSG_OOB)==-1)
		printf("ERROR: %d\n", WSAGetLastError());
	//printf("%s", passString);
	if(WaitForReply(230)==NULL)
	{
		printf("failed!\n");
		return;
	}

	printf("success!\n");

	if(mode=='b')
	{
		WriteSocket(control, "TYPE I\r\n", strlen("TYPE I\r\n"));
		//printf("TYPE I\n");
		if(WaitForReply(200)==NULL)
			return;
	}

	system = syst();

	if(system==NULL)
	{
		this->system = -1;
		printf("SYST failed!\n");
	}
	else if(strstr(system, "UNIX")!=NULL)
	{
		this->system = SYS_UNIX;
		printf("FTP server is UNIX based\n");
	}
	else
	{
		this->system = SYS_WINDOWS;
		printf("FTP server is WINDOWS based\n");
	}
}

void FTP_Connection::disconnect()
{
	WriteSocket(control, "QUIT\r\n", strlen("QUIT\r\n"));
	CloseSocket(control);
}

Directory* FTP_Connection::ls(int *nLines)
{
	char cmdString[1024];
	char buffer[4096];
	int n;
	SOCKET data;
	Directory* dir;

	dir = new Directory();

	if(!OpenDataConnection(&data))
		return NULL;

	//Send LIST command
	WriteSocket(control, "LIST\r\n", strlen("LIST\r\n"));
	//printf("LIST\n");
	if(WaitForReply(150)==NULL)
		return NULL;

	while((n = ReadLineSocket(data, buffer, 4096))>0) {
		buffer[n] = 0;
		dir->AddEntry(buffer, system);
	}
	
	WaitForReply(226);

	CloseSocket(data);

	return dir;
}

void FTP_Connection::cd(char* remoteDirectory)
{
	char buffer[4096];
	int n;

	//Send CWD command
	WriteSocket(control, "CWD\r\n", strlen("CWD\r\n"));
	//printf("CWD\n");
	if(WaitForReply(250)==NULL)
		return;
}

void FTP_Connection::get(char* remoteFile, char* localFile)
{
	FILE* fp;
	SOCKET data;
	int n;
	int bytesRead = 0;
	int totalBytes = 0;
	int bars = 0;
	float fComplete = 0;
	char completion[1024];
	char retrString[1024];
	char buffer[4096];

	if(!OpenDataConnection(&data))
		return;

	if(mode == 'b')
		fp = fopen(localFile, "wb");
	else
		fp = fopen(localFile, "w");

	if(fp==NULL)
	{
		printf("Invalid local path %s!\n", localFile);
		return;
	}

	totalBytes = size(remoteFile);

	//Send RETR command
	sprintf(retrString, "RETR %s\r\n", remoteFile);
	WriteSocket(control, retrString, strlen(retrString));
	//printf("%s", retrString);
	if(WaitForReply(150)==NULL)
		return;
	
	//Download file
	printf("Downloading %s...\n", remoteFile);
	while(n = ReadSocket(data, buffer, 4096))
	{
		sprintf(completion, "\t%3.2f%% complete", (fComplete*100));
		for(int i=0; i<strlen(completion) + 53; i++)
			printf("\r");
		printf("%s /", completion);
		for(int i=0; i<bars; i++)
			printf("=");
		printf(">");
		for(int i=bars+1; i<50; i++)
			printf(" ");
		printf("/");

		bytesRead += n;
		fComplete = ((float)bytesRead/(float)totalBytes);
		bars = fComplete*50;

		fwrite(buffer, 1, n, fp);
	}
	printf("\n");

	WaitForReply(226);

	CloseSocket(data);

	fclose(fp);
}

void FTP_Connection::put(char* localFile, char* remoteFile)
{
	printf("Not yet implemented...\n");
}

void FTP_Connection::binary()
{
	int n;
	char buffer[4096];
	mode = 'b';

	WriteSocket(control, "TYPE I\r\n", strlen("TYPE I\r\n"));
	//printf("TYPE I\n");
	if(WaitForReply(200)==NULL)
		return;
}

void FTP_Connection::ascii()
{
	int n;
	char buffer[4096];
	mode = 'a';

	WriteSocket(control, "TYPE A\r\n", strlen("TYPE A\r\n"));
	//printf("TYPE A\n");
	if(WaitForReply(200)==NULL)
		return;
}

char* FTP_Connection::syst()
{
	int n;
	char* buffer;
	FTP_Control_Message* ftp_msg;

	buffer = new char[1024];

	WriteSocket(control, "SYST\r\n", strlen("SYST\r\n"));
	//printf("SYST\n");
	if((ftp_msg = WaitForReply(215))==NULL)
		return NULL;
	
	return ftp_msg->message;
}

int FTP_Connection::size(char* filename)
{
	char* next;
	int code;
	char sizeString[1024];
	char buffer[4096];
	int n;
	FTP_Control_Message* ftp_msg;

	//Send SIZE command
	sprintf(sizeString, "SIZE %s\r\n", filename);
	WriteSocket(control, sizeString, strlen(sizeString));
	//printf("%s\n", sizeString);
	if((ftp_msg = WaitForReply(213))==NULL)
		return -1;

	return atoi(ftp_msg->message);
}