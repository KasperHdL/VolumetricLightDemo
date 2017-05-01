#pragma once 
#include <stdlib.h>
#include <iostream>

//! The Pool class that is a templete class.
    /*! Allocates memory in pool.
    */
template <class T>
class Pool{

    size_t objectSize = sizeof(T);  /*!< A size_t value: objectSize. The size of the object. */
    T* objects; /*!< A T pointer: objects. The objects. */
    bool* occupied; /*!< A bool pointer: occupied. If the place in the pool is occupied or not. */

public:

    //! A begin method.
        /*! Fetch the first object in  memory.
         * \return T pointer. The pointer to the first adress in memory.
        */
    T* begin() { return &objects[0]; }

    //! A end method.
        /*! Fetch the last object in  memory.
         * \return T pointer. The pointer to the last adress in memory.
        */
    T* end() { return &objects[capacity]; }

    int count = 0; /*!< An int value: count. The number of objects in the pool. */
    int capacity; /*!< An int value: capacity. The capacity of the pool. */

    //! The Pool contructor.
        /*! Does nothing, but does something anyway!
         * \overload Pool(capacity)
        */
    Pool(){}

    //! The overloaded Pool contructor, taking 1 argument.
        /*! Calls the initializeser of the Pool class.
         * \param capacity an int. The capacity the pool should have.
         * \overload Pool()
        */
    Pool(int capacity){
        initialize(capacity);
    }

    //! The Pool destructor.
        /*! Frees all the objects and occupied objects.
        */
    ~Pool(){
        if(capacity == 0)return;

        free(objects);
        free(occupied);
    }

    Pool( const Pool& other ) = delete; // non construction-copyable
    Pool& operator=( const Pool& ) = delete; // non copyable

    //! A initialize method, taking 1 argument.
        /*! Initializes the pool, with the capacity inputted.
         * \param capacity an int. The capacity the pool should have.
        */
    void initialize(int capacity){
        this->capacity = capacity;

        objects = (T*) malloc(objectSize * capacity);
        occupied = (bool*) calloc(capacity, sizeof(bool));

        //objects = new (std::nothrow) T[capacity];
        //occupied = new (std::nothrow) bool[capacity];

        if(objects == nullptr || occupied == nullptr){
            //could not allocate memory
            std::cout << "Could not allocate memory" << std::endl;
        }
    }

    //! An operator overload of [], taking 1 argument.
    /*! Overload the [] so that the pool can be indexed.
     * \param index a size_t value. The index used.
     * \return T pointer. The object at that index, nullptr if there is no object with that index.
     * \overload at()
     */
    T* operator [] (std::size_t index){
        return at(index);
    }


    //! An getter of the pool, taking 1 argument.
    /*! a function version of the operator overload []
     * \param index a size_t value. The index used.
     * \return T pointer. The object at that index, nullptr if there is no object with that index.
     * \overload []
     */
    T* at(std::size_t index){
        if(index < 0 || index >= capacity){
            std::cout << "Index out of range, returning nullptr" << std::endl;
            return nullptr;
        }

        if(occupied[index] == false){
            //std::cout << "Index is not occupied, returning nullptr" << std::endl;
            return nullptr;
        }

        return &objects[index];
    }

    //! A create method.
        /*! Assigns a new object to the pool, and returns it
         * \return T pointer. The object created (nullptr if there is no more space).
        */
    T* create(){
        int index = 0;
        for(;index <= capacity;index++){
            if(index == capacity){
                std::cout << "Pool is completely occupied, returning nullptr" << std::endl;
                return nullptr;

            }
            if(occupied[index] == false){
                occupied[index] = true;
                break;
            }
        }

        count++;

        return &objects[index];
    }

    //! A remove method, taking 1 argument.
        /*! Removes an object from the pool, note that this function is O(n), use remove(int index) if possible 
         * \param ptr a T pointer. The object that should be removed.
         * \overload remove()
        */
    bool remove(T* ptr){
        for(int i = 0;i < capacity;i++){
            if(occupied[i] && ptr == &objects[i]){
                remove(i);
                return true;
            }
        }
        return false;
    }

    //! A remove overload method, taking 1 argument.
        /*! Removes an object from the pool, note that his function is O(1)
         * \param index an int. The index of the object that should be removed.
         * \overload remove()
        */
    void remove(int index){
        if(index < 0 || index > capacity){
            std::cerr << "Pool remove: index out of range" << std::endl;
            return;
        }
        occupied[index] = false;
        count--;
    }

    bool inline isFull(){
        return count == capacity;

    }
};
