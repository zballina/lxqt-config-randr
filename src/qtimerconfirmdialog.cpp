#include <QtCore/QDebug>

#include "qtimerconfirmdialog.h"
#include "ui_qtimerconfirmdialog.h"

QTimerConfirmDialog::QTimerConfirmDialog(int milisec, const QString &caption,
                                         const QString &title,
                                         bool modal, TimerStyle style,
                                         const char *name, QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::QTimerConfirmDialog)
{
    mUi->setupUi(this);
    setObjectName(name);
    setWindowTitle(title);
    setModal(modal);
    mUi->captionLabel->setText(caption);
    mTStyle = style;

    mTotalTimer = new QTimer(this);
    mTotalTimer->setSingleShot(false);
    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setSingleShot(false);

    mSecTotal = mSecRemaining = milisec;
    mUpdateInterval = 1000;

    mUi->timerProgressBar->setRange(0, mSecTotal);

    connect(mTotalTimer, SIGNAL(timeout()), this, SLOT(slotInternalTimeout()) );
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateTime()) );

    slotUpdateTime( false );
    qDebug() << "Terminated constructor confirm dialog";

}

QTimerConfirmDialog::~QTimerConfirmDialog()
{
    delete mTotalTimer;
    delete mUpdateTimer;
    delete mUi;
}


int QTimerConfirmDialog::exec()
{
    qDebug() << "Mostrando e iniciando los temporizadores";
    mTotalTimer->start(mSecTotal);
    mUpdateTimer->start(mUpdateInterval);
    return QDialog::exec();
}

void QTimerConfirmDialog::setRefreshInterval( int milisec )
{
    mUpdateInterval = milisec;
    if ( mUpdateTimer->isActive() )
        mUpdateTimer->start( mUpdateInterval );
}

QTimerConfirmDialog::TimerStyle QTimerConfirmDialog::timerStyle() const
{
    return mTStyle;
}

void QTimerConfirmDialog::setTimerStyle( const TimerStyle newStyle )
{
    mTStyle = newStyle;
}

void QTimerConfirmDialog::slotUpdateTime( bool update )
{
    qDebug() << "slotUpdateTime" << update;
    if ( update )
    {
        switch (mTStyle)
        {
            case CountDown:
                mSecRemaining -= mUpdateInterval;
                break;
            case CountUp:
                mSecRemaining += mUpdateInterval;
                break;
            case Manual:
                break;
        }
    }

    mUi->timerProgressBar->setValue( mSecRemaining );

    mUi->timerLabel->setText( QObject::tr("%1 seconds remaining:").arg(mSecRemaining/1000));
}

void QTimerConfirmDialog::slotInternalTimeout()
{
    qDebug() << "slotInternalTimeout";

    emit timerTimeout();
    reject();
}


void QTimerConfirmDialog::on_buttonBox_accepted()
{
    emit timerTimeout();
    accept();
}

void QTimerConfirmDialog::on_buttonBox_rejected()
{
    emit timerTimeout();
    reject();
}
