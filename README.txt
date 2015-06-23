Description:
You will find here two applications: server and client parts.
Server is written in nodejs and runs on localhost and listens port 9000.
By receiving POST request, you will see some logs about received data.

The client dragon.exe runs a little window with "send" button.
By pressing this button application injects awl.dll to chrome.exe
and copies locked file \Current Tabs to new destination. After the 
copy operation application ejects the dll.
Tthen it sends file content to the server.

Why copying? For some reasons HttpSendRequest doesn't want to send
request from address space of the chrome.exe process.

Building:

	Deps: You will need VS2005 or leater and build application.
		As oouput you will get dragon.exe and awl.dll
		
Runing:
Web server:
	Open command line and run form the project directory:
	>node server.js

Dragon:
	Just run dragon.exe