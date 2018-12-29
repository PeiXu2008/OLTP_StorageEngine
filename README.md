# Artificial_database
This is an OLTP storage engine which is a copy of embedded innodb. Now there is no open source innodb without mysql. One can use this version to construct your own OLTP storage.

$cmake .

$make -j4
Then, you can find the libinnodb.a in directory bin/. One can use this library to call the innodb api.
