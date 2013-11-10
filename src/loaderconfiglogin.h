#ifndef LOADERCONFIGLOGIN_H
#define LOADERCONFIGLOGIN_H

class RandRDisplay;

class LoaderConfigLogin
{
public:
    explicit LoaderConfigLogin();
    ~LoaderConfigLogin();
    void execute();

private:
    RandRDisplay *mDisplay;
};

#endif // LOADERCONFIGLOGIN_H
