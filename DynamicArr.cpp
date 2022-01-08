#include <new>
#include "DynamicArr.h"

// --- listsArr class methods implementations and related functions ---
// listsArr hashing function
int get_cell(int id, int m)
{
    const double a = 0.61803;
    int y = (int)(id * a);
    return (int)(m * ((id * a) - y));
}

// searches for a node in the system with given ID and returns it or nullptr otherwise
courseNode* listsArr::find(int id)
{
    int index = get_cell(id, this->cap);
    courseNode* temp = this->array[index];
    while (temp)
    {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void listsArr::swap_arrays(int newSize, int oldSize, courseNode** prevArr, courseNode** newArr)
{
    for (int j = 0; j < oldSize; ++j)
    {
        while (prevArr[j])
        {
            prevArr[j]->prev = nullptr;
            courseNode* temp = prevArr[j];
            prevArr[j] = prevArr[j]->next;
            int index = get_cell(temp->getId(), newSize);
            if (newArr[index]) newArr[index]->prev = temp;
            temp->next = newArr[index];
            newArr[index] = temp;
        }
    }
}

// all below bool functions return false in case there was memory Allocation Error

// diminish our array into quarter of its current size
bool listsArr::diminish()
{
    int oldcap = this->cap;
    if (cap / 4 < 4) this->cap = 4;
    else this->cap /= 4;
    courseNode** newArr;
    try { newArr = new courseNode*[this->cap]; }
    catch (std::bad_alloc&) { return false; }
    for (int i = 0; i < this->cap; ++i)
    {
        newArr[i] = nullptr;
    }
    swap_arrays(cap, oldcap, this->array, newArr);
    if (this->array) delete[] this->array;
    this->array = newArr;
    return true;
}

// enlarge our array into twice of its current size
bool listsArr::expand()
{
    courseNode** newArr = nullptr;
    this->cap *= 2;
    try { newArr = new courseNode*[cap]; }
    catch (std::bad_alloc&) { return false; }
    for (int i = 0; i < cap; ++i)
    {
        newArr[i] = nullptr;
    }
    swap_arrays(cap, cap / 2, this->array, newArr);
    if (this->array) delete[] this->array;
    this->array = newArr;
    return true;
}

// inserts new Node possesses given Id into the system
bool listsArr::insert(int id)
{
    courseNode* temp = nullptr;
    try { temp = new courseNode(id); }
    catch (std::bad_alloc&) { return false; }
    temp->prev = nullptr;
    temp->next = nullptr;
    if (numEl + 1 > 6 * cap && !expand())
    {
        delete temp;
        return false;
    }
    int index = get_cell(id, this->cap);
    if (this->array[index])
    {
        this->array[index]->prev = temp;
        temp->next = this->array[index];
    }
    this->array[index] = temp;
    this->numEl += 1;
    return true;
}

// removes course with given Id from the array, and returns it's node
courseNode* listsArr::remove(int id)
{
    courseNode* course = this->find(id);
    if (course == nullptr) return nullptr;
    if (numEl - 1 < cap / 3) {
        if (!diminish())return nullptr;
    }
    int index = get_cell(course->id, this->cap);
    if (!course->prev) {
        array[index] = course->next;
        if (course->next)course->next->prev = nullptr;
    }
    else
    {
        course->prev->next = course->next;
        if (course->next) course->next->prev = course->prev;
    }
    numEl -= 1;
    return course;
}

listsArr::~listsArr()
{
    for (int i = 0; i < this->cap; ++i)
    {
        while (this->array[i])
        {
            courseNode* tempCourse = this->array[i];
            this->array[i] = this->array[i]->next;
            delete tempCourse;
        }
    }
    delete[] this->array;
}

// --- directArr class methods implementations ---
// returns false in case there was memory Allocation Error
bool directArr::insert(int id)
{
    Class** newArr;
    if (this->numEl + 1 == this->cap)
    {
        try { newArr = new Class*[this->cap * 2]; }
        catch (std::bad_alloc&) { return false; }
        for (int i = 0; i < this->cap; ++i)
        {
            newArr[i] = this->array[i];
        }
        for (int i = this->cap; i < this->cap * 2; ++i)
        {
            newArr[i] = nullptr;
        }
        delete[] this->array;
        this->array = newArr;
        this->cap *= 2;
    }
    this->numEl += 1;
    return true;
}

Class* directArr::find(int id)
{
    if (id < 0 || id >= this->cap) return nullptr;
    return this->array[id];
}
