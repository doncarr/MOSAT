/*
Relevant function calls

sms::sms(const char *host, const char *user, const char *password, const char *database, const char *smsdConfig, const char *receiveFile, const char *sendFile);

Its the constructor, initialize the object, creates the connection with mysql, and reads the files receive and send


bool sms::sms_send(const char *message, const char *number);

	Sends a message to an arbitrary number, returns true if success


bool sms::sms_send_member(const char *message, const char *name, const char *group);

	Sends a message to a member of the given group, the group its defined in the file send.conf, returns true if success
	
bool sms::sms_send_group(const char *message, const char *group);
	
	Sends  a message to the entire given group if present in the send.conf file, return true if success
	
bool sms::sms_send_all(const char *message);

	Sends a message to all the numbers in the send.conf file, returns true if success
	
smsMessage sms::next_sms();

	Retrieve the next message of any number, present or not in the receive.conf file, returns a smsMessage object
	
smsMessage sms::next_sms_number(const char *number);
	
	Retrieve the next message from the given number, present or not in the receive.conf file, return a smsMessage object

smsMessage sms::next_sms_member(const char *name, const char *group);

	Retrive the next message from the given member of the given group defined in the receive.conf file, returns a smsMessage object
	
smsMessage sms::next_sms_group(const char *group);

	Retrieve the next message from any member of the given group defined in the receive.conf file, returns a smsMessage object
	
void sms::sms_prueba();

	Metod with a never ending loop that keeps checking for new message every 10 seconds and responds, to the number with the message received, it will be modified soit responds with information about the prototype if receives a well formed message, and the number its a member of the group.

const char *smsMessage::getMessage();
	
	Retrives the message, returns a constant string
	
const char *smsMessage::getNumber();

	Retrieves the number of the sender, returns a constant string

const char *smsMessage::getName();

	Retrieves the name of the sender, the name will be a "@" if no name present, returns a constant string
	
const char *smsMessage::getGroup();

	Retrives the groups of the sender, the group will be a "@" if no group present, returns a constant string
	
bool smsMessage::success();
	
	Returns true if there is a message, false if there is no message
	
*/



#include <mysql.h>
#include <stdio.h>
#include "rtcommon.h"
#include "arg.h"
#include <string.h>
#include <queue>
#include <math.h>

#define MSGMAXLEN 161
#define MULTIMSGMAXLEN 481
#define NUMMAXLEN 19
#define NAMEMAXLEN 31
#define LISTMAXLEN 50

using namespace std;

class smsSplit
{
private:
	short int count;
	short int actualChar;
	short int actualMessage;
	char multiMsg[MULTIMSGMAXLEN];
	char msg[MSGMAXLEN];
public:
	smsSplit(const char *text);
	short int getCount();
	const char *getNextMsg();
};

class sms:public sms_base
{

private:
	MYSQL *conn;
	MYSQL_ROW row;
	MYSQL_RES *resGroups;
	MYSQL_ROW rowGroups;
	bool goodConfig;
	char gammuFile[300];
	smsMessage actualMessage;
	
	int sms_send_multiple(char *message);
	int sms_send_single(char *message);
	
	

public:
	sms(const char *host, const char *user, const char *password, const char *database, const char *path);
	~sms();
	bool sms_send(const char *message, const char *number);
	bool sms_send_member(const char *message, const char *name, const char *group);
	bool sms_send_group(const char *message, const char *group);
	bool sms_send_all(const char *message);
	void sms_prueba();
	smsMessage next_sms();
	smsMessage next_sms_number(const char *number);
	smsMessage next_sms_member(const char *name, const char *group);
	smsMessage next_sms_group(const char *group);
};

class reader
{

private:
	char header[NAMEMAXLEN];
	char token[NAMEMAXLEN];
	char temp;
	FILE *fp;
	bool goodConfig;
	bool finish;

public:
	void readerInit(const char *path);
	const char *nextHeader();
	const char *nextToken();
	bool state();
	bool finished();
};


