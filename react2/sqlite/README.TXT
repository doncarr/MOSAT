To see what tables exist in sqlite

using a query:

select name from sqlite_master where type='table';

using a dot command:

.tables

To see the database types:

.schema <table> or 

select sql from sqlite_master where name='<table>';
