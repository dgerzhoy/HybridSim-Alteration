//
//  Temp.cpp
//  
//
//  Created by Daniel Gerzhoy on 6/9/11.
//  
//

#include "Temp.h"

namespace HybridSim {

	void HybridSystem::update()
	{
		// Process the transaction queue.
		// This will fill the dram_queue and flash_queue.
        
		if (dram_pending.size() > max_dram_pending)
			max_dram_pending = dram_pending.size();
		if (pending_sets.size() > pending_sets_max)
			pending_sets_max = pending_sets.size();
		if (pending_pages.size() > pending_pages_max)
			pending_pages_max = pending_pages.size();
		if (trans_queue.size() > trans_queue_max)
			trans_queue_max = trans_queue.size();
        
		// Log the queue length.
		bool idle = (trans_queue.size() == 0) && (pending_sets.size() == 0);
		bool flash_idle = (flash_queue.size() == 0) && (flash_pending.size() == 0);
		bool dram_idle = (dram_queue.size() == 0) && (dram_pending.size() == 0);
		log.access_update(trans_queue.size(), idle, flash_idle, dram_idle);
        
        
		// See if there are any transactions ready to be processed.
		if ((active_transaction_flag) && (delay_counter == 0))
		{
            ProcessTransaction(active_transaction);
            active_transaction_flag = false;
		}
		
        
        
		// Used to see if any work is done on this cycle.
		bool sent_transaction = false;
        
        
		list<DRAMSim::Transaction>::iterator it = trans_queue.begin();
		while((it != trans_queue.end()) && (pending_sets.size() < NUM_SETS) && (check_queue) && (delay_counter == 0))
		{
			// Compute the page address.
			uint64_t page_addr = PAGE_ADDRESS(ALIGN((*it).address));
            
            
			// Check to see if this pending set is open.
			if (pending_sets.count(SET_INDEX(page_addr)) == 0)
			{
				// Add to the pending 
				pending_pages.insert(page_addr);
				pending_sets.insert(SET_INDEX(page_addr));
                
				// Log the page access.
				log.access_page(page_addr);
                
				// Set this transaction as active and start the delay counter, which
				// simulates the SRAM cache tag lookup time.
				active_transaction = *it;
				active_transaction_flag = true;
				delay_counter = CONTROLLER_DELAY;
				sent_transaction = true;
                
				// Delete this item and skip to the next.
				it = trans_queue.erase(it);
                
				break;
			}
			else
			{
				// Log the set conflict.
				log.access_set_conflict(SET_INDEX(page_addr));
                
				// Skip to the next and do nothing else.
				// cout << "PAGE IN PENDING" << page_addr << "\n";
				++it;
				//it = trans_queue.end();
			}
		}
        
		// If there is nothing to do, wait until a new transaction arrives or a pending set is released.
		// Only set check_queue to false if the delay counter is 0. Otherwise, a transaction that arrives
		// while delay_counter is running might get missed and stuck in the queue.
		if ((sent_transaction == false) && (delay_counter == 0))
		{
			this->check_queue = false;
		}
        
        
		// Process DRAM transaction queue until it is empty or addTransaction returns false.
		// Note: This used to be a while, but was changed ot an if to only allow one
		// transaction to be sent to the DRAM per cycle.
		bool not_full = true;
		if (not_full && !dram_queue.empty())
		{
			DRAMSim::Transaction tmp = dram_queue.front();
			bool isWrite;
			if (tmp.transactionType == DATA_WRITE)
				isWrite = true;
			else
				isWrite = false;
			not_full = dram->addTransaction(isWrite, tmp.address);
			if (not_full)
			{
				dram_queue.pop_front();
				dram_pending_set.insert(tmp.address);
			}
		}
        
		// Process Flash transaction queue until it is empty or addTransaction returns false.
		// Note: This used to be a while, but was changed ot an if to only allow one
		// transaction to be sent to the flash per cycle.
		not_full = true;
		if (not_full && !flash_queue.empty())
		{
#if FDSIM
			// put some code to deal with FDSim interactions here
			DRAMSim::Transaction t = flash_queue.front();
			FDSim::FlashTransaction ft = FDSim::FlashTransaction(static_cast<FDSim::TransactionType>(t.transactionType), t.address, t.data);
			not_full = flash->add(ft);
#elif NVDSIM
			// put some code to deal with NVDSim interactions here
			//cout << "adding a flash transaction" << endl;
			DRAMSim::Transaction t = flash_queue.front();
			NVDSim::FlashTransaction ft = NVDSim::FlashTransaction(static_cast<NVDSim::TransactionType>(t.transactionType), t.address, t.data);
			//cout << "the address sent to flash was " << t.address << endl;
			not_full = flash->add(ft);
#else
			//not_full = flash->addTransaction(flash_queue.front());
			DRAMSim::Transaction tmp = flash_queue.front();
			bool isWrite;
			if (tmp.transactionType == DATA_WRITE)
				isWrite = true;
			else
				isWrite = false;
			not_full = flash->addTransaction(isWrite, tmp.address);
#endif
			if (not_full){
				flash_queue.pop_front();
				//cout << "popping front of flash queue" << endl;
			}
		}
        
		// Decrement the delay counter.
		if (delay_counter > 0)
		{
			delay_counter--;
		}
        
        
		// Update the logger.
		log.update();
        
		// Update the memories.
		dram->update();
		flash->update();
        
		// Increment the cycle count.
		step();
	}
    
	bool HybridSystem::addTransaction(bool isWrite, uint64_t addr)
	{
		DRAMSim::TransactionType type;
		if (isWrite)
		{
			type = DATA_WRITE;
		}
		else
		{
			type = DATA_READ;
		}
		DRAMSim::Transaction t = DRAMSim::Transaction(type, addr, NULL);
		return addTransaction(t);
	}
    
	bool HybridSystem::addTransaction(DRAMSim::Transaction &trans)
	{
        
		pending_count += 1;
        
		//cout << "enter HybridSystem::addTransaction\n";
		trans_queue.push_back(trans);
		//cout << "pushed\n";
        
		// Start the logging for this access.
		log.access_start(trans.address);
        
		// Restart queue checking.
		this->check_queue = true;
        
		return true; // TODO: Figure out when this could be false.
	}
