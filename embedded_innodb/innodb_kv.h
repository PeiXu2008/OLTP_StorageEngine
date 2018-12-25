#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "test0aux.h"
typedef long long int Snapshot;
// Options that control read operations
typedef struct tag_ReadOptions {
  // If true, all data read from underlying storage will be
  // verified against corresponding checksums.
  // Default: true
  bool verify_checksums;

  // Should the "data block"/"index block"/"filter block" read for this
  // iteration be cached in memory?
  // Callers may wish to set this field to false for bulk scans.
  // Default: true
  bool fill_cache;

  // If this option is set and memtable implementation allows, Seek
  // might only return keys with the same prefix as the seek-key
  //
  // ! DEPRECATED: prefix_seek is on by default when prefix_extractor
  // is configured
  // bool prefix_seek;

  // If "snapshot" is non-nullptr, read as of the supplied snapshot
  // (which must belong to the DB that is being read and which must
  // not have been released).  If "snapshot" is nullptr, use an impliicit
  // snapshot of the state at the beginning of this read operation.
  // Default: nullptr
  const Snapshot* snapshot;

  // If "prefix" is non-nullptr, and ReadOptions is being passed to
  // db.NewIterator, only return results when the key begins with this
  // prefix.  This field is ignored by other calls (e.g., Get).
  // Options.prefix_extractor must also be set, and
  // prefix_extractor.InRange(prefix) must be true.  The iterator
  // returned by NewIterator when this option is set will behave just
  // as if the underlying store did not contain any non-matching keys,
  // with two exceptions.  Seek() only accepts keys starting with the
  // prefix, and SeekToLast() is not supported.  prefix filter with this
  // option will sometimes reduce the number of read IOPs.
  // Default: nullptr
  //
  // ! DEPRECATED
  // const Slice* prefix;

  // "iterate_upper_bound" defines the extent upto which the forward iterator
  // can returns entries. Once the bound is reached, Valid() will be false.
  // "iterate_upper_bound" is exclusive ie the bound value is
  // not a valid entry.  If iterator_extractor is not null, the Seek target
  // and iterator_upper_bound need to have the same prefix.
  // This is because ordering is not guaranteed outside of prefix domain.
  // There is no lower bound on the iterator. If needed, that can be easily
  // implemented
  //
  // Default: nullptr
  const char* iterate_upper_bound;


  // Specify to create a tailing iterator -- a special iterator that has a
  // view of the complete database (i.e. it can also be used to read newly
  // added data) and is optimized for sequential reads. It will return records
  // that were inserted into the database after the creation of the iterator.
  // Default: false
  // Not supported in ROCKSDB_LITE mode!
  bool tailing;

  // Specify to create a managed iterator -- a special iterator that
  // uses less resources by having the ability to free its underlying
  // resources on request.
  // Default: false
  // Not supported in ROCKSDB_LITE mode!
  bool managed;

  // Enable a total order seek regardless of index format (e.g. hash index)
  // used in the table. Some table format (e.g. plain table) may not support
  // this option.
  // If true when calling Get(), we also skip prefix bloom when reading from
  // block based table. It provides a way to read exisiting data after
  // changing implementation of prefix extractor.
  bool total_order_seek;

  // Enforce that the iterator only iterates over the same prefix as the seek.
  // This option is effective only for prefix seeks, i.e. prefix_extractor is
  // non-null for the column family and total_order_seek is false.  Unlike
  // iterate_upper_bound, prefix_same_as_start only works within a prefix
  // but in both directions.
  // Default: false
  bool prefix_same_as_start;

  // Keep the blocks loaded by the iterator pinned in memory as long as the
  // iterator is not deleted, If used when reading from tables created with
  // BlockBasedTableOptions::use_delta_encoding = false,
  // Iterator's property "rocksdb.iterator.is-key-pinned" is guaranteed to
  // return 1.
  // Default: false
  bool pin_data;

  // If non-zero, NewIterator will create a new table reader which
  // performs reads of the given size. Using a large size (> 2MB) can
  // improve the performance of forward iteration on spinning disks.
  // Default: 0
  size_t readahead_size;

}ReadOptions;


// Options that control write operations
typedef struct tag_WriteOptions {
  // If true, the write will be flushed from the operating system
  // buffer cache (by calling WritableFile::Sync()) before the write
  // is considered complete.  If this flag is true, writes will be
  // slower.
  //
  // If this flag is false, and the machine crashes, some recent
  // writes may be lost.  Note that if it is just the process that
  // crashes (i.e., the machine does not reboot), no writes will be
  // lost even if sync==false.
  //
  // In other words, a DB write with sync==false has similar
  // crash semantics as the "write()" system call.  A DB write
  // with sync==true has similar crash semantics to a "write()"
  // system call followed by "fdatasync()".
  //
  // Default: false
  bool sync;

  // If true, writes will not first go to the write ahead log,
  // and the write may got lost after a crash.
  bool disableWAL;

  // The option is deprecated. It's not used anymore.
  uint64_t timeout_hint_us;

  // If true and if user is trying to write to column families that don't exist
  // (they were dropped),  ignore the write (don't return an error). If there
  // are multiple writes in a WriteBatch, other writes will succeed.
  // Default: false
  bool ignore_missing_column_families;

}WriteOptions;

typedef int err_t;
typedef struct tag_key_value_t
{
	char * key;
	int key_len;
	char * value;
	int value_len;
}key_value_t;


