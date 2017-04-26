#pragma once
#include "Pool.hpp"
#include <vector>

//! The Dynamic Pool class that is a templete class.
    /*! Allocates memory in pools dynamically allocates new pools when one is full.
    */
template <class T>
class DynamicPool{
    private:
        size_t objectSize = sizeof(T);  /*!< A size_t value: objectSize. The size of the object. */
        std::vector<Pool<T>*> pools;
        std::vector<bool> poolsFull;

        void createPool(){
            pools.resize(pools.size());
            Pool<T>* pool = new Pool<T>(poolCapacity);
            pools.push_back(pool);
            poolsFull.push_back(false);
            capacity += poolCapacity;
        }

    public:

        int count = 0; /*!< An int value: count. The number of objects in the dynamic pool. */
        int poolCapacity;/*!< An int value: poolCapacity. The capacity of the pool. */
        int capacity; /*!< An int value: capacity. The capacity of the  dynamicpool. */

        DynamicPool(){}

        DynamicPool(int poolCapacity, int numPoolsReserved = 1){
            this->poolCapacity = poolCapacity;
            for(int i = 0; i < numPoolsReserved; i++){
                createPool();
            }
        }
 

        T* operator [] (std::size_t index){
            int poolIndex     = index / poolCapacity;
            int internalIndex = index % poolCapacity;

            return pools[poolIndex]->at(internalIndex);
        }

        T* create(){
            int poolIndex = 0;
            if(count < capacity){
                for(;poolIndex < pools.size();poolIndex++){
                    if(!poolsFull[poolIndex]){
                        count++;
                        T* obj = pools[poolIndex]->create();
                        poolsFull[poolIndex] = pools[poolIndex]->isFull();
                        return obj;
                    }
                }
            }

            //all pools are full
            count++;
            createPool();
            return pools[pools.size() - 1]->create();
        }

        void remove(T* ptr){
            for(int i = 0;i<pools.size();i++){
                if(pools[i]->remove(ptr)){
                    poolsFull[i] = pools[i]->isFull();
                    count--;
                    return;
                }
            }
            std::cerr << "DynamicPool remove: pointer not found" << std::endl;
        }

        void remove(int index){
            if(index < 0 || index > capacity){
                std::cerr << "DynamicPool remove: index out of range" << std::endl;
                return;
            }
            int poolIndex     = index / poolCapacity;
            int internalIndex = index % poolCapacity;

            pools[poolIndex]->remove(internalIndex);
            poolsFull[poolIndex] = pools[poolIndex]->isFull();
            count--;
 
        }

};
