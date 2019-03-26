
1) Install apache!
   sudo apt-get install apache2 apache2-dev apache2-doc

2) compile with:

        apxs2 -c mod_helloworld.c
        apxs2 -c mod_react.c

3) load into Apache with:

        sudo apxs2 -ie mod_helloworld.la
        sudo apxs2 -ie mod_react.la

4) Create file /etc/apache2/mods-available/helloworld.conf (5 lines) to run this module for ALL subdiretories of /helloworld:

<IfModule mod_helloworld.c>
  <Location /helloworld>
    SetHandler helloworld
  </Location>
</IfModule>

  Also: /etc/apache2/mods-available/react.conf :

<IfModule mod_react.c>
  <Location /react>
    SetHandler react 
  </Location>
</IfModule>

5) Create the file /etc/apache2/mods-available/helloworld.load (one line) to load the module:

LoadModule helloworld_module /usr/lib/apache2/modules/mod_helloworld.so

Also: /etc/apache2/mods-available/react.load with content:

LoadModule react_module /usr/lib/apache2/modules/mod_react.so

6) Enable the module (basically creates two symbolic links):

	a2enmod helloworld
	a2enmod react 

6) Restart Apache:

	/etc/init.d/apache2 restart


To test a post:

wget --quiet --output-document=- --post-data='tag=temp&value=true' http://127.0.0.1/react/output/

