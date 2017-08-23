#include <string>
#include <cstdio>
#include <iostream>
#include "rocksdb/db.h"

#include "rocksdb/options.h"

using namespace std;
using namespace rocksdb;

int main()
{
	DB* db;
	Options options;
	options.create_if_missing = true;
	Status status = DB::open(options, "/tmp/testdb/", &db);
	if (status.ok()) printf("Open is success!\n");
	else printf("Open is not success.\n");

	//assert(status.ok());
	printf("It works!\n");
	status = db->Put(WriteOptions(), "key", "value1");
	if (status.ok()) printf("Put operation is success!\n");
	else printf("Put operation is not success!\n");
	status = db->Put(WriteOptions(), "key", "value2");
	if (status.ok()) printf("Put operation is success!\n");
	else printf("Put operation is not success!\n");
	std::string value;

	//get value
	status = db->Get(ReadOptions(), "key", &value);
	//assert(status.ok());
	//assert(value == "value");
	cout<<"value ="<<value<<endl;
	//#else
	rocksdb::WriteBatch batch;
	batch.Put("key", "value2");
	batch.Put("key", "value1");
	status = db->Write(WriteOptions(), &batch);
	if (status.ok()) printf("Put operation is success!\n");
	else printf("Put operation is not succees!\n");
	//std::string value;
	//get single value
	//status = db->Get(ReadOptions(), "key", &value);
	//cout<<"single value = "<< value<<endl;
	//get multivalues
	std::vector<Slice> vkey(2);
	vkey[0] = Slice("key");
	vkey[1] = Slice("key");
	std::vector<std::string> values(2);
	std::vector<Status> status1;
	printf("Before multiget. \n");
	status1 = db->Multiget(ReadOptions(), (const std::vector<Slice>)vkey, &values);
	printf("After multiget.\n");
	std::vector<Status>::iterator its;
	for (its = status.begin(); its != status1.end(); its ++)
		if ((*its).ok()) printf("Status is error.\n");
	//assert(status.ok());
	//assert(value == "value");
	std::vector<std::string>::iterator it;
	for (it = values.begin(); it != values.end(); it ++)
		cout<<"Multiget value = "<<*it<<endl;

	if (status.ok()) printf("Get operation is success!\n");
	else printf("Get operation is not success!\n");
	delete db;
	printf("\n Test over!\n");
	return 0;
}
