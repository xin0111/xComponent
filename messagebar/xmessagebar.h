#ifndef XMESSAGEBAR_H__
#define XMESSAGEBAR_H__

#include <QString>
#include <QFrame>
#include <QIcon>
#include <QColor>
#include <QList>

class QWidget;
class QGridLayout;
class QMenu;
class QProgressBar;
class QToolButton;
class QLabel;
class QAction;
class QTimer;

class xMessageBarItem;

/** \ingroup gui
 * A bar for displaying non-blocking messages to the user.
 */
class  xMessageBar : public QFrame
{
    Q_OBJECT

  public:
    enum MessageLevel
    {
      INFO = 0,
      WARNING = 1,
      CRITICAL = 2,
      SUCCESS = 3
    };

    xMessageBar( QWidget *parent = nullptr );
    ~xMessageBar();

    /**
     * Display a message item on the bar after hiding the currently visible one
     * and putting it in a stack.
     * The message bar will take ownership of the item.
     *
     * @param item Item to display
     */
    void pushItem( xMessageBarItem *item );

    /** Display a widget as a message on the bar after hiding the currently visible one
     *  and putting it in a stack.
     * @param widget message widget to display
     * @param level is QgsMessageBar::INFO, WARNING, CRITICAL or SUCCESS
     * @param duration timeout duration of message in seconds, 0 value indicates no timeout
     */
    xMessageBarItem *pushWidget( QWidget *widget, MessageLevel level = INFO, int duration = 0 );

    /** Remove the passed widget from the bar (if previously added),
     *  then display the next one in the stack if any or hide the bar
     *  @param item item to remove
     *  @return true if the widget was removed, false otherwise
     */
    bool popWidget( xMessageBarItem *item );

    //! make out a widget containing a message to be displayed on the bar
    static xMessageBarItem* createMessage( const QString &text, QWidget *parent = nullptr );
    //! make out a widget containing title and message to be displayed on the bar
    static xMessageBarItem* createMessage( const QString &title, const QString &text, QWidget *parent = nullptr );
    //! make out a widget containing title and message to be displayed on the bar
    static xMessageBarItem* createMessage( QWidget *widget, QWidget *parent = nullptr );

    //! convenience method for pushing a message to the bar
    void pushMessage( const QString &text, MessageLevel level = INFO, int duration = 5 ) { return pushMessage( QString::null, text, level, duration ); }
    //! convenience method for pushing a message with title to the bar
    void pushMessage( const QString &title, const QString &text, MessageLevel level = INFO, int duration = 5 );

    xMessageBarItem *currentItem() { return mCurrentItem; }

  signals:
    //! emitted when a message widget is added to the bar
    void widgetAdded( xMessageBarItem *item );

    //! emitted when a message widget was removed from the bar
    void widgetRemoved( xMessageBarItem *item );

  public slots:
    /** Remove the currently displayed widget from the bar and
     *  display the next in the stack if any or hide the bar.
     *  @return true if the widget was removed, false otherwise
     */
    bool popWidget();

    /** Remove all items from the bar's widget list
     *  @return true if all items were removed, false otherwise
     */
    bool clearWidgets();

    /**
     * Pushes a success message with default timeout to the message bar
     * @param title title string for message
     * @param message The message to be displayed
     */
    void pushSuccess( const QString& title, const QString& message );

    /**
     * Pushes a information message with default timeout to the message bar
     * @param title title string for message
     * @param message The message to be displayed
     */
    void pushInfo( const QString& title, const QString& message );

    /**
     * Pushes a warning with default timeout to the message bar
     * @param title title string for message
     */
    void pushWarning( const QString& title, const QString& message );

    /**
     * Pushes a critical warning with default timeout to the message bar
     * @param title title string for message
     */
    void pushCritical( const QString& title, const QString& message );

  protected:
    void mousePressEvent( QMouseEvent * e ) override;

  private:
    void popItem( xMessageBarItem *item );
    void showItem( xMessageBarItem *item );
    xMessageBarItem *mCurrentItem;
    QList<xMessageBarItem *> mItems;
    QMenu *mCloseMenu;
    QToolButton *mCloseBtn;
    QGridLayout *mLayout;
    QLabel *mItemCount;
    QAction *mActionCloseAll;
    QTimer *mCountdownTimer;
    QProgressBar *mCountProgress;
    QString mCountStyleSheet;

  private slots:
    //! updates count of items in widget list
    void updateItemCount();

    //! updates the countdown for widgets that have a timeout duration
    void updateCountdown();
    void resetCountdown();
};

#endif // XMESSAGEBAR_H__
