#include "coursesManager.h"

int compareClass(void* id1, void* id2)
{
    Class* first = (Class*)id1;
    Class* second = (Class*)id2;
    int result = second->time - first->time;
    if (result == 0) result = first->courseID - second->courseID;
    if (result == 0) result = first->classID - second->classID;
    return result;
}

void destroyClass(void* lesson)
{
    if(lesson) delete (Class*) lesson;
}

coursesManager::~coursesManager()
{
    if (watchedClasses != nullptr) delete watchedClasses;
    if (coursesTable != nullptr) delete coursesTable;
}

bool coursesManager::validateInit()
{
    return watchedClasses != nullptr && coursesTable != nullptr;
}

courseNode* coursesManager::findCourse(int id)
{
    return coursesTable->find(id);
}

// all below functions return false in case of memory allocation error
bool coursesManager::insertCourse(int id)
{
    return coursesTable->insert(id);
}

void coursesManager::removeClassFromTree(void* content)
{
    this->watchedClasses->remove(destroyClass, content);
}

bool coursesManager::removeCourseFromTable(int id)
{
    return coursesTable->remove(id) != nullptr;
}

Class* coursesManager::insertClassToTree(void* content)
{
    RankedNode* result = watchedClasses->insert(content);
    if (result == nullptr) return nullptr;
    return (Class*)result->getContent();
}

void* coursesManager::getIthClass(int i,void** content)
{
    watchedClasses->select(i, content);
    return content;
}

Class* coursesManager::findClassInTree(void* content)
{
    RankedNode* result = watchedClasses->find(content);
    if (result == nullptr) return nullptr;
    return (Class*)result->getContent();
}

void coursesManager::updateClassInArr(courseNode* course, Class* c, int id)
{
    course->updateClassNode(id, c);
}
