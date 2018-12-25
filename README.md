# Artificial_database
This is an OLTP storage engine which is a copy of embedded innodb. Now there is no open source innodb without mysql. One can use this version to construct your own OLTP storage.

$./confugure

$make -j8

$make install

Enter to the tests and run the executable files, just like:

$./ib_test1

, and the result like this:

API: 3.0.0
InnoDB: Mutexes and rw_locks use GCC atomic builtins
InnoDB: The first specified data file ./ibdata1 did not exist:
InnoDB: a new database to be created!
181225  1:48:37  InnoDB: Setting file ./ibdata1 size to 32 MB
InnoDB: Database physically writes the file full: wait...
181225  1:48:37  InnoDB: Log file log/ib_logfile0 did not exist: new to be created
InnoDB: Setting log file log/ib_logfile0 size to 32 MB
InnoDB: Database physically writes the file full: wait...
181225  1:48:37  InnoDB: Log file log/ib_logfile1 did not exist: new to be created
InnoDB: Setting log file log/ib_logfile1 size to 32 MB
InnoDB: Database physically writes the file full: wait...
InnoDB: Doublewrite buffer not found: creating new
InnoDB: Doublewrite buffer created
InnoDB: Creating foreign key constraint system tables
InnoDB: Foreign key constraint system tables created
181225  1:48:37 Embedded InnoDB 1.0.6.6750 started; log sequence number 0
Create table
Begin transaction
Open cursor
Lock table in IX
Insert rows
Query table
1:a|1:i|10|
1:a|1:t|1|
1:b|1:u|2|
1:b|1:z|9|
1:c|1:b|3|
1:c|1:n|8|
1:d|1:f|7|
1:d|1:n|4|
1:e|1:j|6|
1:e|1:s|5|
Update a row
Query table
1:a|1:i|110|
1:a|1:t|101|
1:b|1:u|2|
1:b|1:z|9|
1:c|1:b|3|
1:c|1:n|8|
1:d|1:f|7|
1:d|1:n|4|
1:e|1:j|6|
1:e|1:s|5|
Delete a row
Query table
1:a|1:i|110|
1:a|1:t|101|
1:b|1:u|2|
1:c|1:b|3|
1:c|1:n|8|
1:d|1:f|7|
1:d|1:n|4|
1:e|1:j|6|
1:e|1:s|5|
Close cursor
Commit transaction
Drop table
181225  1:48:37  InnoDB: Starting shutdown...
181225  1:48:43  InnoDB: Shutdown completed; log sequence number 49725
