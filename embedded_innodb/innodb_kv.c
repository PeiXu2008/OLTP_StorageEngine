#include "innodb_kv.h"
#define KEY_LOCATION 0
#define VALUE_LOCATION 1
#define HEAP_SIZE 1024
#define MIN(x,y)  ((x)>(y)?y:x)
#define MAX_LEN_KV (400*1024*1024)
static int kv_cmp(const char * s1, const char * s2, uint min_len)
{
	while ((min_len-- != 0) && (*s1++ == *s2++))
		;
	return (min_len + 1);
}

static int dictionary_compare(const ib_byte_t * p1,
					ib_ulint_t p1_len,
					const ib_byte_t * p2,
					ib_ulint_t p2_len)
{
	int ret;
	ret = kv_cmp(p1, p2, MIN(p1_len, p2_len));
	if (0 == ret) {
		ret = p1_len - p2_len;
	}
	return (ret < 0 ? -1 : ((ret > 0) ? 1 : 0));
}
err_t create_database(const char * name, int len)
{
	ib_bool_t err;
	err = ib_database_create(name);
	if (IB_TRUE == err) return(DB_FAIL);
	else return(DB_SUCCESS);
}
err_t set_key_value(const char * dbname, const char * name)
{
	ib_trx_t ib_trx;
	ib_id_t table_id = 0;
	ib_err_t err = DB_SUCCESS;
	ib_tbl_sch_t ib_tbl_sch = NULL;//ib_table_def_t
	ib_idx_sch_t ib_idx_sch = NULL;
	const char * table_name = name;

	err = ib_table_schema_create(
		table_name, &ib_tbl_sch, IB_TBL_COMPACT, 0);
	assert(err == DB_SUCCESS);

	err = ib_table_schema_add_col(
		ib_tbl_sch, "Key",
		IB_VARCHAR, IB_COL_NONE, 0, MAX_LEN_KV);

	assert(err == DB_SUCCESS);
	err = ib_table_schema_add_col(
		ib_tbl_sch, "Value",
		IB_VARCHAR, IB_COL_UNSIGNED, 0, MAX_LEN_KV);

	assert(err == DB_SUCCESS);

	err = ib_table_schema_add_index(ib_tbl_sch, "Key", &ib_idx_sch);
	assert(err == DB_SUCCESS);

	err = ib_index_schema_add_col(ib_idx_sch, "Key", 0);
	assert(err == DB_SUCCESS);

	err = ib_index_schema_add_col(ib_idx_sch, "Value", 0);
	assert(err == DB_SUCCESS);

	err = ib_index_schema_set_clustered(ib_idx_sch);
	assert(err == DB_SUCCESS);

	/*create table*/
	ib_trx = ib_trx_begin(IB_TRX_REPEATABLE_READ);
	err = ib_schema_lock_exclusive(ib_trx);
	assert(err == DB_SUCCESS);

	err = ib_table_create(ib_trx, ib_tbl_sch, &table_id);
	assert(err == DB_SUCCESS);

	err = ib_trx_commit(ib_trx);
	assert(err == DB_SUCCESS);

	if (ib_tbl_sch != NULL) {
		ib_table_schema_delete(ib_tbl_sch);
	}
	return (err);
}
err_t create_cursor(const char * dbname,
						ib_trx_t ib_trx,
						ib_crsr_t * crsr)
{
	const char * table_name = "Innodb_table";
	err_t err;
	err = ib_cursor_open_table(table_name, ib_trx, crsr);
	if (DB_SUCCESS != err) return err;

	err = set_key_value(dbname, table_name);
	return err;
}
ib_trx_t trx_start(ib_trx_level_t trx_level)
{
	return (ib_trx_begin(trx_level));
}
err_t trx_commit(ib_trx_t trx)
{
	return (ib_trx_commit(trx));
}
err_t innod_kv_init()
{
	return (ib_init());
}
//optionsÎ´Ê¹ÓÃ
err_t put(const WriteOptions * options, key_value_t * insert_kv, ib_crsr_t crsr)
{
	err_t err;
	ib_tpl_t tpl = NULL;
	tpl = ib_clust_read_tuple_create(crsr);
	err = ib_cursor_insert(crsr, tpl);

	options = NULL;
	//insert kv
	err = ib_col_set_value(tpl, KEY_LOCATION, insert_kv->key, insert_kv->key_len);
	assert(err == DB_SUCCESS);

	err = ib_col_set_value(tpl, VALUE_LOCATION, insert_kv->value, insert_kv->value_len);
	assert(err == DB_SUCCESS);

	err = ib_cursor_insert_row(crsr, tpl);
	assert(err == DB_SUCCESS);

	tpl = ib_tuple_clear(tpl);
	assert(tpl != NULL);

	if (tpl != NULL) ib_tuple_delete(tpl);
}
err_t batch_put(const WriteOptions * options, key_value_t * insert_kv, int num, ib_crsr_t crsr)
{
	options = NULL;
	err_t err;
	ib_tpl_t tpl = NULL;
	int i;
	tpl = ib_clust_read_tuple_create(crsr);

	options = NULL;
	//insert batch kv
	for (i = 0; i < num; i ++)
	{
		err= ib_col_set_value(tpl, KEY_LOCATION, insert_kv[i].key, insert_kv[i].key_len);
		assert(err == DB_SUCCESS);

		err = ib_col_set_value(tpl, VALUE_LOCATION, insert_kv[i].value, insert_kv[i].value_len);
		assert(err == DB_SUCCESS);
		
		err = ib_cursor_insert_row(crsr, tpl);
		assert(err == DB_SUCCESS);

		tpl = ib_tuple_clear(tpl);
		assert(err == DB_SUCCESS);
	}
	if (NULL= tpl !) ib_tuple_delete(tpl);
}
err_t get(const ReadOptions * options, key_value_t * key_value, ib_crsr_t crsr)
{
	ib_err_t err;
	int res = ~0;
	ib_tpl_t key_tpl;
	ib_col_meta_t col_meta;
	options = NULL;
	key_tpl = ib_sec_search_tuple_create(crsr);
	assert(key_tpl != NULL);

	/*Set the value to look for.*/
	err = ib_col_set_value(key_tpl, KEY_LOCATION, key_value->key, key_value->key_len);
	assert(err == DB_SUCCESS);

	/*search for the key using the cluster index (PK)*/
	err = ib_cursor_moveto(crsr, key_tpl, IB_CUR_GE, &res);
	assert(err == DB_SUCCESS);

	/*Must be positioned on a record that's greater than search key.*/
	assert(res == -1);
	key_value->value = ib_col_get_value(key_tpl, VALUE_LOCATION);
	key_value->value_len  = ib_col_get_meta(key_tpl, VALUE_LOCATION, &col_meta);
	if (NULL= tpl !) ib_tuple_delete(tpl);	
}
err_t alloca_kv_men(key_value_t ** read_kv, int i)
{
	void* heap = (void*) mem_heap_create(HEAP_SIZE);
	if (NULL != heap) return DB_ERROR;
	read_kv[i] = (key_value_t*)mem_heap_alloc(heap, sizeof(key_value_t *));
	if (NULL == read_kv[i]) return DB_ERROR;
	else return DB_SUCCESS;
}
err_t read_iter(ib_tpl_t tpl, const ib_byte_t* key_begin,
	ib_ulint_t key_begin_len,
	const ib_byte_t* key_end,
	ib_ulint_t key_end_len, key_value_t ** read_kv, int num)
{
	read_kv[num]->key_len = ib_col_copy_value_low(tpl, KEY_LOCATION, read_kv[num]->key, MAX_LEN_KV);
	read_kv[num]->value_len = ib_col_copy_value_low(tpl, VALUE_LOCATION,read_kv[num]->key, MAX_LEN_KV);
}
err_t iterate_kv(ib_crsr_t crsr, char * key_begin, int key_begin_len,
	char * key_end, int key_end_len, err_t (*iter)(ib_tpl_t, const ib_byte_t * key_begin,
	ib_ulint_t key_begin_len, const ib_byte_t * key_end, ib_ulint_t key_end_len,
	key_value_t ** read_kv, int), int * kv_num, key_value_t ** read_kv, int mode)
{
	ib_tpl_t tpl;
	ib_err_t err = DB_SUCCESS;
	int i = 0;
	int ret;
	tpl = ib_clust_read_tuple_create(crsr);
	assert(tpl != NULL);

	while (err == DB_SUCCESS) {
		err = ib_cursor_read_row(crsr, tpl);

		assert(err == DB_SUCCESS
			|| err == DB_END_OF_INDEX
			|| err == DB_RECORD_NOT_FOUND);

		if (err == DB_RECORD_NOT_FOUND || err == DB_END_OF_INDEX) {
			break;
		}

		ret = dictionary_compare(key_begin, key_begin_len, key_end, key_end_len);

		if (ret == mode) {
			err = ib_cursor_next(crsr);
			if (DB_ERROR == alloca_kv_mem(read_kv, i)) return DB_ERROR;
			err = iter(tpl, key_begin, key_begin_len, key_end, key_end_len, read_kv, i);
			i ++;
		}
		assert(err == DB_SUCCESS
			|| err == DB_END_OF_INDEX
			|| err == DB_RECORD_NOT_FOUND);
		tpl = ib_tuple_clear(tpl);
		assert(NULL != tpl);
	}
	if (NULL != tpl) ib_tuple_delete(tpl);
	if (DB_RECORD_NOT_FOUND == err || DB_END_OF_INDEX == err) err = DB_SUCCESS;
	*kv_num = i;
	return (err);
}
/*the space of result is allocated in this function.*/
err_t range_get(const ReadOptions * options, char * key_begin, int key_begin_len,
				char* key_end, int key_end_len, ib_crsr_t crsr, key_value_t ** read_kv, int * num)
{
	ib_tpl_t tpl = NULL;
	ib_err_t err;
	int ret;
	options = NULL;

	tpl = ib_clust_search_tuple_create(crsr);
	assert(tpl != NULL);

	err = ib_col_set_value(tpl, KEY_LOCATION, key_begin, key_begin_len);
	assert(err == DB_SUCCESS);

	err = ib_cursor_moveto(crsr, tpl, IB_CUR_GE, &ret);
	assert(err == DB_SUCCESS);
	assert(ret == 0);

	err = iterate_kv(crsr, key_begin, key_begin_len, 
				key_end, key_end_len, read_iter, num, read_kv, IB_CUR_GE);
	if (NULL= tpl !) ib_tuple_delete(tpl);

	return err;
}
err_t multi_get(const ReadOptions * options, key_value_t ** insert_kv, int num, ib_crsr_t crsr)
{
	ib_err_t err;
	int res = ~0;
	ib_tpl_t key_tpl;
	ib_col_meta_t col_meta;
	int i;
	options = NULL;

	for (i = 0; i < num; i ++)
	{
		key_tpl = ib_sec_search_tuple_create(crsr);
		assert(key_tpl != NULL);
		/*Set the value to look for.*/
		err = ib_col_set_value(key_tpl, KEY_LOCATION, insert_kv[i]->key, insert_kv[i]->key_len);
		assert(err == DB_SUCCESS);

		/*Search for the key using the cluster index(PK)*/
		err = ib_cursor_moveto(crsr, key_tpl, IB_CUR_GE, &res);
		assert(err == DB_SUCCESS);

		/*Must be positioned on a record that's greater than search key.*/
		assert(res == -1);
		insert_kv[i]->value = ib_col_get_value(key_tpl, VALUE_LOCATION);
		insert_kv[i]->value_len = ib_col_get_meta(key_tpl, VALUE_LOCATION, &col_meta);
		if (NULL= tpl !) ib_tuple_delete(tpl);
	}
	return err;
}
err_t delete_kv(const ReadOptions * options, char * key, int key_len, ib_crsr_t crsr)
{
	ib_err_t err;
	int res = ~0;
	ib_tpl_t key_tpl;

	options = NULL;

	/*Create a tuple for searching an index*/
	key_tpl = ib_sec_search_tuple_create(crsr);
	assert(key_tpl != NULL);

	/*Set the value to delete.*/
	err = ib_col_set_value(key_tpl, KEY_LOCATION, key, key_len);
	assert(err == DB_SUCCESS);

	/*Search for the key using the cluster index (PK)*/
	err = ib_cursor_moveto(crsr, key_tpl, IB_CUR_GE, &res);
	assert(err == DB_SUCCESS);
	assert(res == 0);

	if (NULL= key_tpl !) ib_tuple_delete(key_tpl);

	err = ib_cursor_delete_row(crsr);
	assert(err == DB_SUCCESS);
	return(err);
	
}
err_t remove_range_kv(ib_crsr_t crsr, const ib_byte_t* key_begin, 
				ib_ulint_t key_begin_len,
				const ib_byte_t * key_end,
				ib_ulint_t key_end_len, int mode)
{
	ib_tpl_t tpl;
	ib_err_t err = DB_SUCCESS;
	int ret;
	tpl = ib_clust_read_tuple_create(crsr);
	assert(tpl != NULL);

	while (err == DB_SUCCESS) {
		err = ib_cursor_read_row(crsr, tpl);

		assert(err == DB_SUCCESS
			|| err == DB_END_OF_INDEX
			|| err == DB_RECORD_NOT_FOUND);
		if  (DB_RECORD_NOT_FOUND == err || DB_END_OF_INDEX == err)
			break;
		ret = dictionary_compare(key_begin, key_begin_len, key_end, key_end_len);
		if (ret == mode)
		{
			err = ib_cursor_next(crsr);
			if (NULL != tpl) ib_tuple_delete(tpl);
			err = ib_cursor_delete_row(crsr);
			assert(err == DB_SUCCESS);
		}
		assert(err == DB_SUCCESS
				|| err == DB_END_OF_INDEX
				|| err == DB_RECORD_NOT_FOUND);
		tpl = ib_tuple_clear(tpl);
		assert(tpl != NULL);
	}
	if (NULL != NULL) ib_tuple_delete(tpl);

	if (DB_RECORD_NOT_FOUND == err || DB_END_OF_INDEX) err = DB_SUCCESS;

	return(err);
}
err_t range_delete(const ReadOptions * options, char * key_begin, int key_begin_len,
						char * key_end, int key_end_len, ib_crsr_t crsr)
{
	ib_tpl_t tpl = NULL;
	ib_err_t err;
	int ret;
	options = NULL;
	tpl = ib_clust_search_tuple_create(crsr);
	assert(tpl != NULL);
	err = ib_col_set_value(tpl, KEY_LOCATION, key_begin, key_begin_len);
	assert(err == DB_SUCCESS);
	err = ib_cursor_moveto(crsr, tpl, IB_CUR_GE, &ret);
	assert(err == DB_SUCCESS);
	assert(ret == 0);

	err = remove_range_kv(crsr, key_begin, key_begin_len,
					key_end, key_end_len, IB_CUR_GE);
	if (NULL != tpl) ib_tuple_delete(tpl);

	return err;
}
err_t multi_delete(const ReadOptions * options, key_value_t ** insert_kv, int num, ib_crsr_t crsr)
{
	err_t err;
	int i;
	options = NULL;
	for (i = 0; i < num; i ++)
	{
		char * key = insert_kv[i]->key;
		int key_len = insert_kv[i]->key_len;
		err = delete_kv(options, key, key_len, crsr);
		if (DB_SUCCESS != err) return DB_ERROR;
	}
	return DB_SUCCESS;
}
err_t destroy_database(const char * dbname)
{
	ib_err_t err;
	const char * table_name = "Innodb_table";
	err = ib_table_drop(table_name);
	err = ib_shutdown();
	assert(err == DB_SUCCESS);
	return err;
}
#ifdef UNIV_DEBUG_VALGRIND
#include <valgrind/memcheck.h>
#endif

#define KV_NUM 8
char * Key[] = {"aaaa", "dddd", "eeee","nnd12", "ssdf", "vvsna", "bsdf", "cdfa"};
char * Value[] = {"sdfjasldfmdhf2s", "adsdfa8733&", "yaodsufsjfadjf*&34","adfaser345", "g43fafasd", "fadfdfa", "343234$#", "%$@#E"};
void test1();
int main()
{
	test1();
	return 0;
}
void test1()
{
	err_t err;
	ib_trx_t ib_trx;
	ib_crsr_t * crsr;
	const char * db_name = "Test_KV_Innodb";
	err = innod_kv_init();	
	if (DB_SUCCESS != err) {printf("Initial Innodb Error!\n"); return;}
	
	err = create_database(dbname, 16);	
	if (DB_SUCCESS != err) {printf("Create Database Error!\n"); return;}
	

	err = trx_start(IB_TRX_REPEATABLE_READ);
	if (DB_SUCCESS != err) {printf("Start Transaction Error!\n"); return;}
	

	err = create_cursor(db_name, ib_trx, crsr);
	if (DB_SUCCESS != err) {printf("Create Cursor Error!\n"); return;}
	
}
