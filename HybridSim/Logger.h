#ifndef HYBRIDSIM_LOGGER_H
#define HYBRIDSIM_LOGGER_H

#include <iostream>
#include <fstream>

#include "config.h"

//test

namespace HybridSim
{

	class Logger: public SimulatorObject
	{
		public:
		Logger();
		~Logger();

		// Overall state
		uint64_t num_accesses;
		uint64_t num_reads;
		uint64_t num_writes;

		uint64_t num_misses;
		uint64_t num_hits;

		uint64_t num_read_misses;
		uint64_t num_read_hits;
		uint64_t num_write_misses;
		uint64_t num_write_hits;
		
		uint64_t sum_latency;
		uint64_t sum_read_latency;
		uint64_t sum_write_latency;
		uint64_t sum_queue_latency;

		uint64_t sum_hit_latency;
		uint64_t sum_miss_latency;

		uint64_t sum_read_hit_latency;
		uint64_t sum_read_miss_latency;

		uint64_t sum_write_hit_latency;
		uint64_t sum_write_miss_latency;

		uint64_t max_queue_length;
		uint64_t sum_queue_length;

		uint64_t idle_counter;
		uint64_t flash_idle_counter;
		uint64_t dram_idle_counter;

		unordered_map<uint64_t, uint64_t> pages_used; // maps page_addr to num_accesses

		// Epoch state (reset at the beginning of each epoch)
		uint64_t epoch_count;

		uint64_t cur_num_accesses;
		uint64_t cur_num_reads;
		uint64_t cur_num_writes;

		uint64_t cur_num_misses;
		uint64_t cur_num_hits;

		uint64_t cur_num_read_misses;
		uint64_t cur_num_read_hits;
		uint64_t cur_num_write_misses;
		uint64_t cur_num_write_hits;
		
		uint64_t cur_sum_latency;
		uint64_t cur_sum_read_latency;
		uint64_t cur_sum_write_latency;
		uint64_t cur_sum_queue_latency;

		uint64_t cur_sum_hit_latency;
		uint64_t cur_sum_miss_latency;

		uint64_t cur_sum_read_hit_latency;
		uint64_t cur_sum_read_miss_latency;

		uint64_t cur_sum_write_hit_latency;
		uint64_t cur_sum_write_miss_latency;

		uint64_t cur_max_queue_length;
		uint64_t cur_sum_queue_length;

		uint64_t cur_idle_counter;
		uint64_t cur_flash_idle_counter;
		uint64_t cur_dram_idle_counter;

		unordered_map<uint64_t, uint64_t> cur_pages_used; // maps page_addr to num_accesses


		// Epoch lists (used to store state from previous epochs)
		list<uint64_t> num_accesses_list;
		list<uint64_t> num_reads_list;
		list<uint64_t> num_writes_list;

		list<uint64_t> num_misses_list;
		list<uint64_t> num_hits_list;

		list<uint64_t> num_read_misses_list;
		list<uint64_t> num_read_hits_list;
		list<uint64_t> num_write_misses_list;
		list<uint64_t> num_write_hits_list;
		
		list<uint64_t> sum_latency_list;
		list<uint64_t> sum_read_latency_list;
		list<uint64_t> sum_write_latency_list;
		list<uint64_t> sum_queue_latency_list;

		list<uint64_t> sum_hit_latency_list;
		list<uint64_t> sum_miss_latency_list;

		list<uint64_t> sum_read_hit_latency_list;
		list<uint64_t> sum_read_miss_latency_list;

		list<uint64_t> sum_write_hit_latency_list;
		list<uint64_t> sum_write_miss_latency_list;

		list<uint64_t> max_queue_length_list;
		list<uint64_t> sum_queue_length_list;

		list<uint64_t> idle_counter_list;
		list<uint64_t> flash_idle_counter_list;
		list<uint64_t> dram_idle_counter_list;

		list<uint64_t> access_queue_length_list;

		list<unordered_map<uint64_t, uint64_t>> pages_used_list; // maps page_addr to num_accesses


		// -----------------------------------------------------------
		// Missed Page Record

		class MissedPageEntry
		{
			public:
			uint64_t cycle;
			uint64_t missed_page;
			uint64_t victim_page;
			uint64_t cache_set;
			uint64_t cache_page;
			bool dirty;
			bool valid;

			MissedPageEntry(uint64_t c, uint64_t missed, uint64_t victim, uint64_t set, uint64_t cache_line, bool d, bool v)
			{
				cycle = c;
				missed_page = missed;
				victim_page = victim;
				cache_set = set;
				cache_page = cache_line;
				dirty = d;
				valid = v;
			}
		};

		list<MissedPageEntry> missed_page_list;


		unordered_map<uint64_t, uint64_t> latency_histogram; 
		unordered_map<uint64_t, uint64_t> set_conflicts; 

		// -----------------------------------------------------------
		// Processing state (used to keep track of current transactions, but not part of logging state)


		class AccessMapEntry
		{
			public:
			uint64_t start; // Starting cycle of access
			uint64_t process; // Cycle when processing starts
			uint64_t stop; // Stopping cycle of access
			bool read_op; // Is this a read_op?
			bool hit; // Is this a hit?
			AccessMapEntry()
			{
				start = 0;
				process = 0;
				stop = 0;
				read_op = false;
				hit = false;
			}
		};


		// Store access info while the access is being processed.
		unordered_map<uint64_t, AccessMapEntry> access_map;

		// Store the address and arrival time while access is waiting to be processed.
		// Must do this because duplicate addresses may arrive close together.
		list<pair <uint64_t, uint64_t>> access_queue;

		ofstream debug;

		// -----------------------------------------------------------
		// API Methods
		void update();

		// External logging methods.
		void access_start(uint64_t addr);
		void access_process(uint64_t addr, bool read_op, bool hit);
		void access_stop(uint64_t addr);

		void access_update(uint64_t queue_length, bool idle, bool flash_idle, bool dram_idle);

		//void access_cache(uint64_t addr, bool hit);

		void access_page(uint64_t page_addr);

		void access_set_conflict(uint64_t cache_set);

		void access_miss(uint64_t missed_page, uint64_t victim_page, uint64_t cache_set, uint64_t cache_page, bool dirty, bool valid);


		void print();

		// -----------------------------------------------------------
		// Internal helper methods.

		void read();
		void write();

		void hit();
		void miss();

		void read_hit();
		void read_miss();
		void write_hit();
		void write_miss();

		double compute_running_average(double old_average, double num_values, double new_value);
		void latency(uint64_t cycles);
		void read_latency(uint64_t cycles);
		void write_latency(uint64_t cycles);
		void read_hit_latency(uint64_t cycles);
		void read_miss_latency(uint64_t cycles);
		void write_hit_latency(uint64_t cycles);
		void write_miss_latency(uint64_t cycles);
		void queue_latency(uint64_t cycles);

		void hit_latency(uint64_t cycles);
		void miss_latency(uint64_t cycles);

		double divide(uint64_t a, uint64_t b);

		double miss_rate();
		double read_miss_rate();
		double write_miss_rate();
		double compute_throughput(uint64_t cycles, uint64_t accesses);
		double latency_cycles(uint64_t sum, uint64_t accesses);
		double latency_us(uint64_t sum, uint64_t accesses);

		void epoch_reset(bool init);
	};

}

#endif

