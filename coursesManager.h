#ifndef WET2_COURSESMANAGER_H
#define WET2_COURSESMANAGER_H

#include "DynamicArr.h"
#include <iostream>

int compareClass(void* id1, void* id2);
void destroyClass(void* lesson);
// stores courses and their classes
class coursesManager
{
    rankedavl* watchedClasses;
    int watchedClassCount;
    listsArr* coursesTable;

public:
    coursesManager() : watchedClassCount(0)
    {
        watchedClasses = new rankedavl(rankedavl::genericFind, destroyClass, compareClass);//
        coursesTable = new listsArr();
    }

    ~coursesManager();

    bool validateInit();
    courseNode* findCourse(int id);
    bool insertCourse(int id);
    void updateClassInArr(courseNode* course, Class* c, int id);
    void removeClassFromTree(void* content);
    Class* findClassInTree(void* content);
    Class* insertClassToTree(void* content);
    bool removeCourseFromTable(int id);
    void updateWatchedCount(int n) { this->watchedClassCount += n; }
    int getWatchedClassesCount() { return this->watchedClassCount; }
    void* getIthClass(int i, void** content);
};
#endif //WET2_COURSESMANAGER_H
