#ifndef POSTJOBACTION_H
#define POSTJOBACTION_H

#include <QString>
#include <QUuid>

class PostJobAction
{
public:
    virtual ~PostJobAction() {}
    virtual void doAction() = 0;
};

#endif // POSTJOBACTION_H
