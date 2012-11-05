#ifndef QTIMERCONFIRMDIALOG_H
#define QTIMERCONFIRMDIALOG_H

#include <QtCore/QTimer>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>

namespace Ui {
class QTimerConfirmDialog;
}

class QTimerConfirmDialog : public QDialog
{
    Q_OBJECT
public:
    enum TimerStyle
    {
        CountDown,
        CountUp,
        Manual
    };

public:
    QTimerConfirmDialog(int milisec, const QString &caption=QString(),
                        const QString &title=QString(),
                        bool modal=true,
                        TimerStyle style=CountDown,
                        const char *name=0,
                        QWidget *parent = 0);
    ~QTimerConfirmDialog();

    /**
     * Execute the dialog modelessly - see @see QDialog .
     */
//   virtual void setVisible( bool visible );
    /**
     * Set the refresh interval for the timer progress. Defaults to one second.
     */
    void setRefreshInterval( int msec );
    /**
     * Retrieves the current @ref TimerStyle. @see setTimerStyle
     */
    QTimerConfirmDialog::TimerStyle timerStyle() const;
    /**
     * Sets the @ref TimerStyle. @see timerStyle
     */
    void setTimerStyle( TimerStyle newStyle );

Q_SIGNALS:
  /**
   * Signal which is emitted once the timer has timed out.
   */
  void timerTimeout();

public Q_SLOTS:
  /**
   * Execute the dialog modally - see @see QDialog .
   */
  int exec();

private Q_SLOTS:
  /**
   * Updates the dialog with the current progress levels.
   */
  void slotUpdateTime( bool update = true );

  /**
   * The internal
   */
  void slotInternalTimeout();

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
    Ui::QTimerConfirmDialog *mUi;
    QTimer *mTotalTimer;
    QTimer *mUpdateTimer;
    TimerStyle mTStyle;
    int mSecRemaining;
    int mUpdateInterval;
    int mSecTotal;
};

#endif // QTIMERCONFIRMDIALOG_H
