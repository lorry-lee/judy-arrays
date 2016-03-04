#ifndef JUDY64NB_H
#define JUDY64NB_H

typedef unsigned char uchar;
typedef unsigned int uint;

#if defined(__LP64__) || \
	defined(__x86_64__) || \
	defined(__amd64__) || \
	defined(_WIN64) || \
	defined(__sparc64__) || \
	defined(__arch64__) || \
	defined(__powerpc64__) || \
	defined (__s390x__)
	//	defines for 64 bit

	typedef unsigned long long judyvalue;
	typedef unsigned long long JudySlot;
	#define JUDY_key_mask (0x07)
	#define JUDY_key_size 8
	#define JUDY_slot_size 8
	#define JUDY_span_bytes (3 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_2
	#define JUDY_radix_equiv JUDY_8

	#define PRIjudyvalue	"llu"

#else
	//	defines for 32 bit

	typedef uint judyvalue;
	typedef uint JudySlot;
	#define JUDY_key_mask (0x03)
	#define JUDY_key_size 4
	#define JUDY_slot_size 4
	#define JUDY_span_bytes (7 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_4
	#define JUDY_radix_equiv JUDY_8

	#define PRIjudyvalue	"u"

#endif

typedef struct {
	void *seg;			// next used allocator
	uint next;			// next available offset
} JudySeg;

typedef struct {
	JudySlot next;		// judy object
	uint off;			// offset within key
	int slot;			// slot within object
} JudyStack;

typedef struct {
	JudySlot root[1];	// root of judy array
	void **reuse[8];	// reuse judy blocks
	JudySeg *seg;		// current judy allocator
	uint level;			// current height of stack
	uint max;			// max height of stack
	uint depth;			// number of Integers in a key, or zero for string keys
	JudyStack stack[1];	// current cursor
} Judy;

//	functions:
//	judy_open:	open a new judy array returning a judy object.
Judy *judy_open (uint max, uint depth);
//	judy_close:	close an open judy array, freeing all memory.
void judy_close (Judy *judy);
//	judy_clone:	clone an open judy array, duplicating the stack.
void *judy_clone (Judy *judy);
//	judy_data:	allocate data memory within judy array for external use.
void *judy_data (Judy *judy, uint amt);
//	judy_cell:	insert a string into the judy array, return cell pointer.
JudySlot *judy_cell (Judy *judy, uchar *buff, uint max);
//	judy_strt:	retrieve the cell pointer greater than or equal to given key
JudySlot *judy_strt (Judy *judy, uchar *buff, uint max);
//	judy_slot:	retrieve the cell pointer, or return NULL for a given key.
JudySlot *judy_slot (Judy *judy, uchar *buff, uint max);
//	judy_key:	retrieve the string value for the most recent judy query.
uint judy_key (Judy *judy, uchar *buff, uint max);
//	judy_end:	retrieve the cell pointer for the last string in the array.
JudySlot *judy_end (Judy *judy);
//	judy_nxt:	retrieve the cell pointer for the next string in the array.
JudySlot *judy_nxt (Judy *judy);
//	judy_prv:	retrieve the cell pointer for the prev string in the array.
JudySlot *judy_prv (Judy *judy);
//	judy_del:	delete the key and cell for the current stack entry.
JudySlot *judy_del (Judy *judy);

#endif /* JUDY64NB_H */
