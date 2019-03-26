/*
The following example program (in file prog.c) should be compiled with this Unix command:

     gcc -I/usr/include/mysql sms.c -lmysqlclient -lnsl -lm -lz -o sms

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sms.h"
#include "sms_implement.h"

extern "C" sms_base *create_new_sms_object(const char * path)
{
	printf("creating new sms object\n");
	return new sms("localhost", "root", "root", "gammu",path);
}

void reader::readerInit(const char *path)
{
	 fp = fopen(path, "r");
	 if(fp)
	 {
	 	goodConfig = true;
	 	finish = false;
	 }
	 else
	 {
	 	goodConfig = false;
	 	finish = true;
	 }
	 
	 temp = '@';
}

const char* reader::nextHeader()
{

	int i = 0;
	
	while(!feof(fp) && temp != '[')
		if(fgetc(fp) == '[')
			break;
	
	if(feof(fp))
	{
		finish = true;
		goodConfig = false;
		return "@";
	}
	
	while(!feof(fp))
	{
		temp = fgetc(fp);
		if(temp == ']')
		{
			header[i] = '\0';
			return header;
		}
		else
		{
			header[i] = temp;
		}
		i++;
	}
	
	finish = true;
	goodConfig = false;
	
	return "@";
}
const char* reader::nextToken()
{

	int i = 0;
	
	if(temp != '[')
	{	while(!feof(fp) && temp != '[')
		{

			temp = fgetc(fp);
			
			if(temp == '|')
			{
				token[i] = '\0';
				return token;
			}
				
			if(temp != '\n')
			{
				token[i] = temp;
				i++;
			}
		}
		
		if(temp == '[')
			return "@";
		
		if(feof(fp))
		{
			goodConfig = false;
			finish = true;
			return "@";
		}
	
		
	}
	
	return "@";
}

bool reader::finished()
{
	return finish;
}

bool reader::state()
{
	return goodConfig;
}

smsSplit::smsSplit(const char *text)
{
	safe_strcpy(multiMsg,text,sizeof(multiMsg));
	if(strlen(text) > MULTIMSGMAXLEN)
	{
		printf("Mensaje demasiado largo");
		count = 0;
	}
	else
		if(strlen(text) < 1)
			printf("Mensaje demasiado corto");
		else
		{
			actualChar = 0;
			actualMessage = 0;
			count = (short int)ceil((double)strlen(multiMsg)/155);
		}
}

short int smsSplit::getCount()
{
	return count;
}

const char *smsSplit::getNextMsg()
{
	int i;
	for(i = 0; i < 155; i++)
	{
		msg[i] = multiMsg[actualChar];
		if(msg[i] == '\0')
			return msg;
		actualChar++;
	}
	msg[i] = '\0';
	return msg;
}

sms::~sms()
{
	mysql_close(conn);
}


sms::sms(const char *host, const char *user, const char *password, const char *database, const char *path)
{
	char tempPath[300];
	char TruePath[300];
	
	if(strcmp("./",path) == 0)
	{
		char *tpath = NULL;
		tpath=getcwd(tpath,300);
		snprintf(TruePath,sizeof(TruePath),"%s/",tpath);
		//safe_strcpy(TruePath,tpath,sizeof(TruePath));
		//cout<<"\n current Path"<<path;
	}
	else
	{
		safe_strcpy(TruePath,path,sizeof(TruePath));
	}
	snprintf(gammuFile,sizeof(gammuFile),"%sgammusmsdrc.conf",TruePath);
	
/*"gammusmsdrc.conf","receive.conf","send.conf"*/
	char query[300];
	char tempHeader[NAMEMAXLEN];
	char tempNumber[NUMMAXLEN];
	char tempName[NAMEMAXLEN];
	goodConfig = true;
	//sprintf(gammuFile,"%s",smsdConfig);
	
	
	if(0!=system("pkill -0 \"mysqld\""))
	{
		system("mysqld &");
		sleep(3);
		if(0!=system("pkill -0 \"mysqld\""))
			goodConfig = false;
	}
	
	if(0!=system("pkill -0 \"gammu\""))
	{	
		char command [330];
		snprintf(command,sizeof(command),"gammu --smsd MYSQL %s &",gammuFile);
		system(command);
		sleep(1);
		if(0!=system("pkill -0 \"gammu\""))
			goodConfig = false;
	}
	
	conn = mysql_init(NULL);
   	
   	/* Connect to database */
   	if (!mysql_real_connect(conn, host, user, password, database, 0, NULL, 0))
   	{
      		fprintf(stderr, "%s\n", mysql_error(conn));
      		goodConfig = false;
      	}
      	
      	if (mysql_query(conn, "DELETE FROM receive"))
      	{
        	fprintf(stderr, "%s\n", mysql_error(conn));
      		goodConfig = false;
   	}
   	
   	if (mysql_query(conn, "DELETE FROM send"))
      	{
        	fprintf(stderr, "%s\n", mysql_error(conn));
      		goodConfig = false;
   	}
   	
      	reader config;
      	
      	snprintf(tempPath,sizeof(tempPath),"%s/receive.conf",TruePath);
      	
      	config.readerInit(tempPath);
      	
      	while(config.state() && !config.finished())
      	{
      		safe_strcpy(tempHeader,config.nextHeader(),sizeof(tempHeader));

      		while(config.state() && !config.finished())
      		{
      			safe_strcpy(query,"insert into receive (number,name,type) values ('",sizeof(query));
      			safe_strcpy(tempNumber,config.nextToken(),sizeof(tempNumber));
      			safe_strcpy(tempName,config.nextToken(),sizeof(tempName));
      			
      			if(strcmp(tempNumber,"@") == 0)
      				break;

      			safe_strcat(query,tempNumber,sizeof(query));
      			safe_strcat(query,"','",sizeof(query));
      			safe_strcat(query,tempName,sizeof(query));
      			safe_strcat(query,"','",sizeof(query));
      			safe_strcat(query,tempHeader,sizeof(query));
      			safe_strcat(query,"');",sizeof(query));
			
      			if (mysql_query(conn,query))
      			{
      				fprintf(stderr, "%s\n", mysql_error(conn));
      				goodConfig = false;
   			}
      		}
      	}
      	
      	snprintf(tempPath,sizeof(tempPath),"%s/send.conf",TruePath);
      	
      	config.readerInit(tempPath);

      	while(config.state() && !config.finished())
      	{
      		safe_strcpy(tempHeader,config.nextHeader(),sizeof(tempHeader));

      		while(config.state() && !config.finished())
      		{
      			safe_strcpy(query,"insert into send (number,name,type) values ('",sizeof(query));
      			safe_strcpy(tempNumber,config.nextToken(),sizeof(tempNumber));
      			safe_strcpy(tempName,config.nextToken(),sizeof(tempName));
      			if(strcmp(tempNumber,"@") == 0)
      				break;
      			safe_strcat(query,tempNumber,sizeof(query));
      			safe_strcat(query,"','",sizeof(query));
      			safe_strcat(query,tempName,sizeof(query));
      			safe_strcat(query,"','",sizeof(query));
      			safe_strcat(query,tempHeader,sizeof(query));
      			safe_strcat(query,"');",sizeof(query));

      			if (mysql_query(conn,query))
      			{
      				fprintf(stderr, "%s\n", mysql_error(conn));
      				goodConfig = false;
   			}
      		}
      	}
      	printf("SMS properly configured\n");
}

bool sms::sms_send(const char *message, const char *number)
{
	if(goodConfig == true)
	{
		char query[500];
		
		//printf("%s",message);
		
		if(strlen(message) <= 160)
		{
			//system("ls");
			sprintf(query,"echo \"%s\" | gammu --sendsmsdsms TEXT %s MYSQL %s",message,number,gammuFile);
			if (system(query) != 0)
			{
      				fprintf(stderr,"Error al enviar mensaje\n");
      				return false;
      			}
		}
		else
		{
			smsSplit spliter(message);
		
			for(int i = 1; i <= spliter.getCount(); i++)
			{
				sprintf(query,"echo \"(%d/%d)%s\" | gammu --sendsmsdsms TEXT %s MYSQL %s",i,spliter.getCount(),spliter.getNextMsg(),number,gammuFile);
				//printf("%s",query);
				if(system(query) != 0)
				{
					fprintf(stderr,"Error al enviar el mensaje\n");
					return false;
				}
			}
		}
 
		return true;
	}
	else
	{
		printf("No se puede enviar mensaje, no se configuro apropiadamente la lista de destinatarios");
		return false;
	}
}

bool sms::sms_send_member(const char *message, const char *name, const char *group)
{
	MYSQL_RES *res;
	bool send = false;
	
	if(goodConfig == true)
	{

		char query[500] = "SELECT number FROM send WHERE name = '";
		safe_strcat(query,name,sizeof(query));
		safe_strcat(query,"' AND type = '",sizeof(query));
		safe_strcat(query,group,sizeof(query));
		safe_strcat(query,"';",sizeof(query));
		
		
		if (mysql_query(conn, query))
		{
      			fprintf(stderr, "%s\n", mysql_error(conn));
      			return send;
      		}
      		
      		res = mysql_store_result(conn);
      		
      		
      		while ((row = mysql_fetch_row(res)) != NULL)
      		{
      			send = sms_send(message,row[0]);
      			
      		}
      		
      		mysql_free_result(res);
      		
      		return send;
	}
	else
		printf("No se puede enviar mensaje, no se configuro apropiadamente la lista de destinatarios");
	return send;
}

bool sms::sms_send_group(const char *message, const char *group)
{
	MYSQL_RES *res;
	bool send = false;
	if(goodConfig == true)
	{

		char query[500] = "SELECT number FROM send WHERE type = '";
		safe_strcat(query,group,sizeof(query));
		safe_strcat(query,"';",sizeof(query));
		
		
		if (mysql_query(conn, query))
		{
      			fprintf(stderr, "%s\n", mysql_error(conn));
      			return send;
      		}
      		
      		res = mysql_store_result(conn);
      		
      		
      		while ((row = mysql_fetch_row(res)) != NULL)
      		{
      			send = sms_send(message,row[0]);
      		}
      		
      		mysql_free_result(res);
      		
      		return send;
	}
	else
		printf("No se puede enviar mensaje, no se configuro apropiadamente la lista de destinatarios");
	return send;
}

bool sms::sms_send_all(const char *message)
{
	MYSQL_RES *res;
	bool send = false;
	if(goodConfig == true)
	{

		char query[500] = "SELECT number FROM send;";
		
		if (mysql_query(conn, query))
		{
      			fprintf(stderr, "%s\n", mysql_error(conn));
      			return send;
      		}
      		
      		res = mysql_store_result(conn);
      		
      		
      		while ((row = mysql_fetch_row(res)) != NULL)
      		{
      			send = sms_send(message,row[0]);
      		}
      		
      		mysql_free_result(res);
      		
      		return send;
	}
	else
		printf("No se puede enviar mensaje, no se configuro apropiadamente la lista de destinatarios");
	return send;
}

smsMessage sms::next_sms()
{
	MYSQL_RES *res, *res2;
	MYSQL_ROW row2;
	char query[100] = "SELECT TextDecoded, SenderNumber, ID FROM inbox WHERE Processed = 'false';";
	
	if(mysql_query(conn,query))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		actualMessage.setSuccess(false);
	}
	
	res = mysql_store_result(conn);
	
	if((row = mysql_fetch_row(res)) != NULL)
	{
		//actualMessage.setData(row[0],row[1],"","");
		//actualMessage.setSuccess(true);
		
		sprintf(query,"UPDATE inbox SET processed = 'true' WHERE ID = '%s';",row[2]);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
		
		sprintf(query,"SELECT * FROM receive WHERE number = '%s';",row[1]);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
		
		res2 = mysql_store_result(conn);
		
		if((row2 = mysql_fetch_row(res2)) != NULL)
			actualMessage.setData(row[0],row[1],row2[1],row2[2]);
		else
			actualMessage.setData(row[0],row[1],"@","@");
		actualMessage.setSuccess(true);
	}
	else
		actualMessage.setSuccess(false);
	
	mysql_free_result(res);
	
	return actualMessage;
}

smsMessage sms::next_sms_number(const char *number)
{
	MYSQL_RES *res, *res2;
	MYSQL_ROW row2;
	char query[100];
	
	sprintf(query,"SELECT TextDecoded, SenderNumber, ID FROM inbox WHERE SenderNumber = '%s';",number);
	
	if(mysql_query(conn,query))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		actualMessage.setSuccess(false);
	}
	
	res = mysql_store_result(conn);
	
	if((row = mysql_fetch_row(res)) != NULL)
	{
		//actualMessage.setData(row[0],row[1],"","");
		actualMessage.setSuccess(true);
		
		sprintf(query,"UPDATE inbox SET processed = 'true' WHERE ID = '%s';",row[2]);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
		
		sprintf(query,"SELECT * FROM receive WHERE number = '%s';",number);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
		
		res2 = mysql_store_result(conn);
		
		if((row2 = mysql_fetch_row(res2)) != NULL)
			actualMessage.setData(row[0],row[1],row2[1],row2[2]);
		else
			actualMessage.setData(row[0],row[1],"@","@");
		actualMessage.setSuccess(true);
	}
	else
		actualMessage.setSuccess(false);
	
	mysql_free_result(res);
	
	return actualMessage;
}

smsMessage sms::next_sms_group(const char *group)
{
	MYSQL_RES *res;
	char query[250];
	
	sprintf(query,"SELECT inbox.TextDecoded, receive.number, receive.name, receive.type, inbox.ID FROM inbox, receive WHERE inbox.Processed = 'false' AND inbox.SenderNumber = receive.number AND receive.type = '%s';",group);
	
	if(mysql_query(conn,query))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		actualMessage.setSuccess(false);
	}
	
	res = mysql_store_result(conn);
	
	if((row = mysql_fetch_row(res)) != NULL)
	{
		actualMessage.setData(row[0],row[1],row[2],row[3]);
		actualMessage.setSuccess(true);
		
		sprintf(query,"UPDATE inbox SET processed = 'true' WHERE ID = '%s';",row[4]);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
	}
	else
		actualMessage.setSuccess(false);
	
	mysql_free_result(res);
	
	
	return actualMessage;
}

smsMessage sms::next_sms_member(const char *name, const char *group)
{
	MYSQL_RES *res;
	char query[300];
	
	sprintf(query,"SELECT inbox.TextDecoded, receive.number, receive.name, receive.type, inbox.ID FROM inbox, receive WHERE inbox.Processed = 'false' AND inbox.SenderNumber = receive.number AND receive.name = '%s'AND receive.number = '%s';",name,group);
	
	if(mysql_query(conn,query))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
		actualMessage.setSuccess(false);
	}
	
	res = mysql_store_result(conn);
	
	if((row = mysql_fetch_row(res)) != NULL)
	{
		actualMessage.setData(row[0],row[1],row[2],row[3]);
		actualMessage.setSuccess(true);
		
		sprintf(query,"UPDATE inbox SET processed = 'true' WHERE ID = '%s';",row[4]);
	
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
	}
	else
		actualMessage.setSuccess(false);
	
	mysql_free_result(res);
	
	return actualMessage;

}

/*void sms::sms_prueba()
{
	MYSQL_RES *res;
	char query[1000];
	
	while(true)
	{
	

	//SELECT SenderNumber, TextDecoded, ID FROM inbox WHERE processed = 'false' ORDER BY ReceivingDateTime
	sprintf(query,"SELECT SenderNumber, TextDecoded, ID FROM inbox WHERE processed = 'false' ORDER BY ReceivingDateTime;");
	if(mysql_query(conn,query))
	{
		fprintf(stderr,"%s\n",mysql_error(conn));
	}
	
	res = mysql_store_result(conn);
	
	printf("%s\n",query);
	
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		printf("%s\n",row[0]);
		sprintf(query,"Se recibio el siguiente mensaje: %s",row[1]);
		printf("%s\n",query);
		printf("Antes del send");
		sms_send(query,row[0]);
		sprintf(query,"UPDATE inbox SET processed = 'true' WHERE ID = '%s';",row[2]);
		printf("%s\n",query);
		if(mysql_query(conn,query))
		{
			fprintf(stderr,"%s\n",mysql_error(conn));
		}
	}
		mysql_free_result(res);
		sleep(10);
	}
	
}*/

void sms::sms_prueba()
{
	smsMessage mensaje;
	char temp[191];
	int number;
	
	while(true)
	{
		mensaje = next_sms();
		if(mensaje.success())
		{
			printf("Mensaje recibido:%s\nDel individuo:%s\nDel grupo:%s\n",mensaje.getMessage(),mensaje.getName(),mensaje.getGroup());
			strcpy(temp,mensaje.getMessage());
					//printf("Mensaje recibido%s\n",temp);
			if(strncasecmp("estatus.",temp,8)==0)
			{
				number = atoi(temp+8);
				if(number >=1 && number <= 12)
					sms_send(mensaje.getMessage(),mensaje.getNumber());
				else
					sms_send("Numero de unidad fuera de rango",mensaje.getNumber());
			}
			else
			{
				sprintf(temp,"Mensaje invalido recibido:\n%s",mensaje.getMessage());
				sms_send(temp,mensaje.getNumber());
			}
			//sms_send(mensaje.getMessage(),mensaje.getNumber());
			printf("Se respondio un mensaje\n");
		}
		else
			printf("No se han recibido mensajes nuevos\n");
		sleep(2);
	}
	
}
