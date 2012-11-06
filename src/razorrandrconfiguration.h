#ifndef RAZORRANDRCONFIGURATION_H
#define RAZORRANDRCONFIGURATION_H

#include <QtGui/QDialog>
#include "randrconfig.h"
#include "randrdisplay.h"

namespace Ui {
class RazorRandrConfiguration;
}

class RazorRandrConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit RazorRandrConfiguration(QWidget *parent = 0);
    ~RazorRandrConfiguration();
    void about();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void updateButtons(bool status);

private:

    Ui::RazorRandrConfiguration *mUi;
    RandRConfig *mRandrConfig;
    RandRDisplay *mRandrDisplay;
};

#endif // RAZORRANDRCONFIGURATION_H
