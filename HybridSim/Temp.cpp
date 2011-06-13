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
        for(list<const_trans>::iterator it = const_trans_queue.begin(); it !=const_trans_queue.end(); ++it)
        {
            (*it).delay_counter--;
        
            if((*it).delay_counter == 0)
            {
            
        
                if (((*it).iswrite == true)) {
            
                    DRAMWriteCallback(systemID, (*it).addr, currentClockCycle);
                }
                else {
                    DRAMReadCallback(systemID, (*it).addr, currentClockCycle);
                }
                 
                  const_trans_queue.erase(*it);
                                     
            }
        }
    
        step();
    
    }

    bool HybridSystem::addTransaction(bool isWrite, uint64_t addr)
    {
    
        const_trans t;
    
        t.isWrite = isWrite;
        t.addr = addr;
        t.delay_counter = NUM_CYCLES;
    
        
        
        const_trans_queue.push_back(t);
    
        return true;
    }

    HybridSystem *getMemorySystemInstance(uint id)
    {
        return new HybridSystem(id);
    }
    
    void HybridSystem::RegisterCallbacks( TransactionCompleteCB *readDone, TransactionCompleteCB *writeDone
                                         /*void (*reportPower)(double bgpower, double burstpower, double refreshpower, double actprepower)*/)
	{
		// Save the external callbacks.
		ReadDone = readDone;
		WriteDone = writeDone;
        
	}
 
}    
