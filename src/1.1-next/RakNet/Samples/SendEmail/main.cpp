#include "EmailSender.h"
#include "FileList.h"
#include <stdio.h>

void main(void)
{
    FileList fileList;
	EmailSender emailSender;
	char *sendResult;
	const char *quote = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
//	const char base64Map[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//	char output[1024];
//	emailSender.Base64Encoding(quote, strlen(quote), output, base64Map);
//	printf("%s", output);

	char mailServer[128], senderUsername[128], receiver[128];
	printf("Tests sending email.\n");
	printf("Enter mail server: ");
	gets(mailServer);
	printf("Enter email account username: ");
	gets(senderUsername);
	printf("Enter receiver email address: ");
	gets(receiver);

	fileList.AddFile("quote.txt", quote, (const unsigned int) strlen(quote), 0, 0);
	sendResult=emailSender.Send(mailServer,
		25,
		senderUsername,
		receiver,
		senderUsername,
		receiver,
		"Nigerian businessman seeks partner to smuggle 80 million Viagra pills to pay for home refinancing.",
		"Fuck it, just give me your bank account info and save us both time :).\n.\n..\n.\n(Should be .,.,..,.)\r\n.\r\n.\r\n..\r\n.\r\n(Should be .,.,..,.)12345\r\n.\r\n",
		&fileList,
		true);
	if (sendResult!=0)
		printf("Send Failed! %s", sendResult);
	else
		printf("Success (probably).\n");
}
