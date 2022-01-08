#ifndef WET2_DYNAMICARR_H
#define WET2_DYNAMICARR_H

#include <iostream>
#include "rankedavl.h"

struct Class
{
    int courseID;
    int classID;
    int time;
};

class listsArr;
class directArr;

//Dynamic Array
template <class T> class DynamicArr
{
protected:
    int numEl; // number of elements currently stored in the system
    int cap; // current array size
    T* array; // a pointer to first cell of array
public:
    DynamicArr() : numEl(0), cap(4) {
        this->array = new T[4];
        for (int i = 0; i < 4; ++i) array[i] = nullptr;
    }
    virtual ~DynamicArr() { }
    virtual bool insert(int id) = 0;
};

class directArr : public DynamicArr<Class*>
{
public:
    directArr() : DynamicArr() { }
    ~directArr() override
    {
      /*  for (int i = 0; i < this->cap; ++i)
        {
            if (this->array[i]) delete this->array[i];
        }*/
        delete[] this->array;
    }
    bool insert(int id) override;

    //storing node in this->array[id]
    void updateCell(int id, Class* node)
    {
        this->array[id] = node;
    }
    Class* find(int id);
    friend class courseNode;
};

class courseNode
{
    int id;
    int nextClass;
    courseNode* next;
    courseNode* prev;
    directArr coursesArray;

public:
    courseNode(int id) : id(id), nextClass(0), next(nullptr), prev(nullptr), coursesArray() { }
    int getId() { return this->id; }
    int getNext() { return this->nextClass; }
    bool addClass()
    {
        if (this->coursesArray.insert(nextClass) != true) return false;
        this->nextClass++;
        return true;
    }
    void updateClassNode(int id, Class* c)
    {
        coursesArray.updateCell(id, c);
    }
    Class* findClass(int id) { return this->coursesArray.find(id); }
    friend class listsArr;
};

class listsArr : public DynamicArr<courseNode*>
{
public:
    listsArr() : DynamicArr() {}
    void swap_arrays(int newSize, int oldSize, courseNode** artists, courseNode** temp);
    ~listsArr() override;
    bool insert(int id) override;
    courseNode* find(int id);
    bool expand();
    courseNode* remove(int id);
    bool diminish();
    friend class CoursesManager;
};

#endif //WET2_DYNAMICARR_H
