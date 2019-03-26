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

#include "rtcommon.h"

#define MSGMAXLEN 161
#define MULTIMSGMAXLEN 481
#define NUMMAXLEN 19
#define NAMEMAXLEN 31
#define LISTMAXLEN 50

using namespace std;

class smsMessage
{
private:
	char message[MULTIMSGMAXLEN];
	char number[NUMMAXLEN];
	char name[NAMEMAXLEN];
	char group[NAMEMAXLEN];
	bool status;

public:
	void setData(const char *a_message, const char *a_number, 
                     const char *a_name, const char *a_group) {
          safe_strcpy(this->message,a_message,sizeof(this->message));
	  safe_strcpy(this->number,a_number,sizeof(this->number));
	  safe_strcpy(this->name,a_name,sizeof(this->name));
	  safe_strcpy(this->group,a_group,sizeof(this->group));
	};
	const char *getMessage(){return message;};
	const char *getNumber(){return number;};
	const char *getName(){return name;};
	const char *getGroup(){return group;};
	void setSuccess(bool x){status = x;};
	bool success(){return status;};
};


class sms_base
{	

public:
	virtual ~sms_base(){};
	virtual bool sms_send(const char *message, const char *number) = 0;
	virtual bool sms_send_member(const char *message, const char *name, const char *group) = 0;
	virtual bool sms_send_group(const char *message, const char *group) = 0;
	virtual bool sms_send_all(const char *message) = 0;
	virtual void sms_prueba() = 0;
	virtual smsMessage next_sms() = 0;
	virtual smsMessage next_sms_number(const char *number) = 0;
	virtual smsMessage next_sms_member(const char *name, const char *group) = 0;
	virtual smsMessage next_sms_group(const char *group) = 0;
};

typedef sms_base *(*new_sms_t)(const char *path);


