#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include "randrconfig.h"
#include "randrdisplay.h"

#include "loaderconfiglogin.h"

LoaderConfigLogin::LoaderConfigLogin()
{
    mDisplay = new RandRDisplay();
}

LoaderConfigLogin::~LoaderConfigLogin()
{
    delete mDisplay;
}

void LoaderConfigLogin::execute()
{
    QSettings config;
    mDisplay->loadDisplay(config, true);
    mDisplay->applyProposed(false);
}
