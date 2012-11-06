#include <QtGui/QMessageBox>

#include "razorrandrconfiguration.h"
#include "ui_razorrandrconfiguration.h"

RazorRandrConfiguration::RazorRandrConfiguration(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::RazorRandrConfiguration)
{
    mUi->setupUi(this);
    updateButtons(false);
    mRandrDisplay = new RandRDisplay();
    mRandrConfig = new RandRConfig(this, mRandrDisplay);

    mUi->verticalLayout->addWidget(mRandrConfig);
    connect(mRandrConfig, SIGNAL(changed(bool)), this, SLOT(updateButtons(bool)));
}

RazorRandrConfiguration::~RazorRandrConfiguration()
{
    delete mRandrDisplay;
    delete mRandrConfig;
    delete mUi;
}

void RazorRandrConfiguration::updateButtons(bool status)
{
    mUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(status);
    mUi->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(status);
    mUi->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(status);
}

void RazorRandrConfiguration::on_buttonBox_clicked(QAbstractButton *button)
{
    if(mUi->buttonBox->button(QDialogButtonBox::Ok) == button)
    {
        mRandrConfig->apply();
        QApplication::quit();
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Apply) == button)
    {
        mRandrConfig->apply();
        updateButtons(false);
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Cancel) == button)
    {
        QApplication::quit();
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Reset) == button)
    {
        mRandrConfig->load();
        updateButtons(false);
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Help) == button)
    {
        about();
    }
}

void RazorRandrConfiguration::about()
{
    QMessageBox about;
    about.aboutQt(this);
}
