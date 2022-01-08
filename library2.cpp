#include "library2.h"
#include "coursesManager.h"

void* Init()
{
    coursesManager* DS = nullptr;
    try { DS = new coursesManager; }
    catch (std::bad_alloc&) { return nullptr; }
    if (DS->validateInit()) return((void*)DS); // If everything went smoothly, return the DS.
    delete DS; // Otherwise, something went wrong. Undo.
    return nullptr;
}

StatusType AddCourse(void* DS, int courseID)
{
    if (DS == nullptr || courseID <= 0) return INVALID_INPUT;
    if (((coursesManager*)DS)->findCourse(courseID) != nullptr) return FAILURE;
    if (!((coursesManager*)DS)->insertCourse(courseID)) return ALLOCATION_ERROR;
    return SUCCESS;
}

StatusType RemoveCourse(void* DS, int courseID)
{
    if (DS == nullptr || courseID <= 0) return INVALID_INPUT;
    courseNode* temp = ((coursesManager*)DS)->findCourse(courseID);
    if (temp == nullptr) return FAILURE;
    if (!((coursesManager*)DS)->removeCourseFromTable(courseID)) return ALLOCATION_ERROR;
    int counter = 0;
    for (int i = 0; i < temp->getNext(); ++i)
    {
        Class* node = temp->findClass(i);
        if (node != nullptr)
        {
            ((coursesManager*)DS)->removeClassFromTree(node);
            counter++; // counter++ iff a song has at least 1 watch count
        }
    }
    ((coursesManager*)DS)->updateWatchedCount(-counter); // update watch count after removal
    delete temp;
    return SUCCESS;
}

StatusType AddClass(void* DS, int courseID, int* classID)
{
    if (DS == nullptr || courseID <= 0) return INVALID_INPUT;
    courseNode* temp = ((coursesManager*)DS)->findCourse(courseID);
    if (temp == nullptr) return FAILURE;
    //cout<<"Course ID "<<temp->getId()<<"\n";
    if (!temp->addClass()) return ALLOCATION_ERROR;
    *classID = temp->getNext() - 1;
    return SUCCESS;
}

StatusType WatchClass(void* DS, int courseID, int classID, int time)
{
    if (DS == nullptr || courseID <= 0 || time <= 0 || classID < 0) return INVALID_INPUT;
    courseNode* temp = ((coursesManager*)DS)->findCourse(courseID);
    if (temp == nullptr) return FAILURE;
    if (temp->getNext() <= classID) return INVALID_INPUT;
	Class* content;
    try { content = new Class(); }
    catch (std::bad_alloc&) { return ALLOCATION_ERROR; }
    content->courseID = courseID;
    content->classID = classID;
	
    Class* new_node = temp->findClass(classID);
    if (new_node == nullptr) // class is not in the tree, so create its content struct and insert it(T.D)
    {
        content->time = time;
        new_node = ((coursesManager*)DS)->insertClassToTree(content);
        if (new_node == nullptr) return ALLOCATION_ERROR;
        ((coursesManager*)DS)->updateClassInArr(temp, new_node, classID);
        ((coursesManager*)DS)->updateWatchedCount(1);
    }

    else // otherwise, first insert its updated node, and then remove the previous one
    {
        content->time = new_node->time + time;
        ((coursesManager*)DS)->insertClassToTree(content);
        ((coursesManager*)DS)->removeClassFromTree(new_node);
        ((coursesManager*)DS)->updateClassInArr(temp, ((coursesManager*)DS)->findClassInTree(content), classID);
    }

    return SUCCESS;
}

StatusType TimeViewed(void* DS, int courseID, int classID, int* timeViewed)
{
    if (DS == nullptr || courseID <= 0 || classID < 0) return INVALID_INPUT;
    courseNode* temp = ((coursesManager*)DS)->findCourse(courseID);
    if (temp == nullptr) return FAILURE;
    if (temp->getNext() <= classID) return INVALID_INPUT;
    Class* new_node = temp->findClass(classID);
    if (new_node == nullptr) *timeViewed = 0;
    else *timeViewed = new_node->time;
    return SUCCESS;
}

StatusType GetIthWatchedClass(void* DS, int i, int* courseID, int* classID)
{
    if (DS == nullptr || i <= 0) return INVALID_INPUT;
    if (i > ((coursesManager*)DS)->getWatchedClassesCount()) return FAILURE;
    void* temp = nullptr;
    (Class*)((coursesManager*)DS)->getIthClass(i, &temp);
    *courseID = ((Class*)temp)->courseID;
    *classID = ((Class*)temp)->classID;
    return SUCCESS;
}

void Quit(void** DS)
{
    delete (coursesManager*)(*DS);
    *DS = nullptr;
}
