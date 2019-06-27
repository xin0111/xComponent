#ifndef XMESSAGEBARITEM_H__
#define XMESSAGEBARITEM_H__

#include "xmessagebar.h"

#include <QWidget>
#include <QIcon>
#include <QTextEdit>
#include <QHBoxLayout>

/** \ingroup gui
 * \class QgsMessageBarItem
 */
class  xMessageBarItem : public QWidget
{
    Q_OBJECT
  public:
    //! make out a widget containing a message to be displayed on the bar
    xMessageBarItem( const QString &text, xMessageBar::MessageLevel level = xMessageBar::INFO, int duration = 0, QWidget *parent = nullptr );

    //! make out a widget containing title and message to be displayed on the bar
    xMessageBarItem( const QString &title, const QString &text, xMessageBar::MessageLevel level = xMessageBar::INFO, int duration = 0, QWidget *parent = nullptr );

    //! make out a widget containing title, message and widget to be displayed on the bar
    xMessageBarItem( const QString &title, const QString &text, QWidget *widget, xMessageBar::MessageLevel level = xMessageBar::INFO, int duration = 0, QWidget *parent = nullptr );

    //! make out a widget containing a widget to be displayed on the bar
    xMessageBarItem( QWidget *widget, xMessageBar::MessageLevel level = xMessageBar::INFO, int duration = 0, QWidget *parent = nullptr );

    ~xMessageBarItem();

    xMessageBarItem *setText( const QString& text );

    xMessageBarItem *setTitle( const QString& title );

    xMessageBarItem *setLevel( xMessageBar::MessageLevel level );

    xMessageBarItem *setWidget( QWidget *widget );

    xMessageBarItem *setIcon( const QIcon &icon );

    xMessageBarItem *setDuration( int duration );

    //! returns the duration in second of the message
    int duration() const { return mDuration; }

    //! returns the level
    xMessageBar::MessageLevel level() { return mLevel; }

    //! returns the styleSheet
    QString getStyleSheet() { return mStyleSheet; }

  signals:
    //! emitted when the message level has changed
    void styleChanged( const QString& styleSheet );


  private:
    void writeContent();

    QString mTitle;
    QString mText;
    xMessageBar::MessageLevel mLevel;
    int mDuration;
    QWidget *mWidget;
    QIcon mUserIcon;
    QHBoxLayout *mLayout;
    QLabel *mLblIcon;
    QString mStyleSheet;
    QTextEdit *mTextEdit;
};

#endif // XMESSAGEBARITEM_H__
