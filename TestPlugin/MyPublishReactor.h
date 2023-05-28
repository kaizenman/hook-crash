#pragma once

#include "pch.h"

class MyPublishReactor : public AcPublishReactor
{
public:
    virtual void OnAboutToBeginBackgroundPublishing(AcPublishBeforeJobInfo *pInfo);
    virtual void OnEndPublish(AcPublishReactorInfo *pInfo);
};