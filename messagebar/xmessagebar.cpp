#include "xmessagebar.h"
#include "xmessagebaritem.h"

#include <QWidget>
#include <QPalette>
#include <QStackedWidget>
#include <QProgressBar>
#include <QToolButton>
#include <QTimer>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QLabel>

xMessageBar::xMessageBar( QWidget *parent )
    : QFrame( parent )
    , mCurrentItem( nullptr )
{
  QPalette pal = palette();
  pal.setBrush( backgroundRole(), pal.window() );
  setPalette( pal );
  setAutoFillBackground( true );
  setFrameShape( QFrame::StyledPanel );
  setFrameShadow( QFrame::Plain );

  mLayout = new QGridLayout( this );
  mLayout->setContentsMargins( 9, 1, 9, 1 );
  setLayout( mLayout );

  mCountProgress = new QProgressBar( this );
  mCountStyleSheet = QString( "QProgressBar { border: 1px solid rgba(0, 0, 0, 75%);"
                              " border-radius: 2px; background: rgba(0, 0, 0, 0);"
                              " image: url(:/images/%1) }"
                              "QProgressBar::chunk { background-color: rgba(0, 0, 0, 30%); width: 5px; }" );

  mCountProgress->setStyleSheet( mCountStyleSheet.arg( "mIconTimerPause.png" ) );
  mCountProgress->setObjectName( "mCountdown" );
  mCountProgress->setFixedSize( 25, 14 );
  mCountProgress->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  mCountProgress->setTextVisible( false );
  mCountProgress->setRange( 0, 5 );
  mCountProgress->setHidden( true );
  mLayout->addWidget( mCountProgress, 0, 0, 1, 1 );

  mItemCount = new QLabel( this );
  mItemCount->setObjectName( "mItemCount" );
  mItemCount->setToolTip( tr( "Remaining messages" ) );
  mItemCount->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
  mLayout->addWidget( mItemCount, 0, 2, 1, 1 );

  mCloseMenu = new QMenu( this );
  mCloseMenu->setObjectName( "mCloseMenu" );
  mActionCloseAll = new QAction( tr( "Close all" ), this );
  mCloseMenu->addAction( mActionCloseAll );
  connect( mActionCloseAll, SIGNAL( triggered() ), this, SLOT( clearWidgets() ) );

  mCloseBtn = new QToolButton( this );
  mCloseMenu->setObjectName( "mCloseMenu" );
  mCloseBtn->setToolTip( tr( "Close" ) );
  mCloseBtn->setMinimumWidth( 40 );
  mCloseBtn->setStyleSheet(
    "QToolButton { background-color: rgba(0, 0, 0, 0); }"
    "QToolButton::menu-button { background-color: rgba(0, 0, 0, 0); }" );
  mCloseBtn->setCursor( Qt::PointingHandCursor );
  mCloseBtn->setIcon(QIcon(":/images/mIconClose.svg"));
  mCloseBtn->setIconSize( QSize( 18, 18 ) );
  mCloseBtn->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
  mCloseBtn->setMenu( mCloseMenu );
  mCloseBtn->setPopupMode( QToolButton::MenuButtonPopup );
  connect( mCloseBtn, SIGNAL( clicked() ), this, SLOT( popWidget() ) );
  mLayout->addWidget( mCloseBtn, 0, 3, 1, 1 );

  mCountdownTimer = new QTimer( this );
  mCountdownTimer->setInterval( 1000 );
  connect( mCountdownTimer, SIGNAL( timeout() ), this, SLOT( updateCountdown() ) );

  connect( this, SIGNAL( widgetAdded( xMessageBarItem* ) ), this, SLOT( updateItemCount() ) );
  connect( this, SIGNAL( widgetRemoved( xMessageBarItem* ) ), this, SLOT( updateItemCount() ) );

  // start hidden
  setVisible( false );
}

xMessageBar::~xMessageBar()
{
}

void xMessageBar::mousePressEvent( QMouseEvent * e )
{
  if ( mCountProgress == childAt( e->pos() ) && e->button() == Qt::LeftButton )
  {
    if ( mCountdownTimer->isActive() )
    {
      mCountdownTimer->stop();
      mCountProgress->setStyleSheet( mCountStyleSheet.arg( "mIconTimerContinue.png" ) );
    }
    else
    {
      mCountdownTimer->start();
      mCountProgress->setStyleSheet( mCountStyleSheet.arg( "mIconTimerPause.png" ) );
    }
  }
}

void xMessageBar::popItem( xMessageBarItem *item )
{
  Q_ASSERT( item );

  if ( item != mCurrentItem && !mItems.contains( item ) )
    return;

  if ( item == mCurrentItem )
  {
    if ( mCurrentItem )
    {
      QWidget *widget = mCurrentItem;
      mLayout->removeWidget( widget );
      mCurrentItem->hide();
      disconnect( mCurrentItem, SIGNAL( styleChanged( QString ) ), this, SLOT( setStyleSheet( QString ) ) );
      delete mCurrentItem;
      mCurrentItem = nullptr;
    }

    if ( !mItems.isEmpty() )
    {
      showItem( mItems.at( 0 ) );
    }
    else
    {
      hide();
    }
  }
  else
  {
    mItems.removeOne( item );
  }

  emit widgetRemoved( item );
}

bool xMessageBar::popWidget( xMessageBarItem *item )
{
  if ( !item || !mCurrentItem )
    return false;

  if ( item == mCurrentItem )
  {
    popItem( mCurrentItem );
    return true;
  }

  Q_FOREACH ( xMessageBarItem *existingItem, mItems )
  {
    if ( existingItem == item )
    {
      mItems.removeOne( existingItem );
      delete existingItem;
      return true;
    }
  }

  return false;
}

bool xMessageBar::popWidget()
{
  if ( !mCurrentItem )
    return false;

  resetCountdown();

  xMessageBarItem *item = mCurrentItem;
  popItem( item );

  return true;
}

bool xMessageBar::clearWidgets()
{
  if ( !mCurrentItem && mItems.empty() )
    return true;

  while ( !mItems.isEmpty() )
  {
    popWidget();
  }
  popWidget();

  return !mCurrentItem && mItems.empty();
}

void xMessageBar::pushSuccess( const QString& title, const QString& message )
{
  pushMessage( title, message, SUCCESS );
}

void xMessageBar::pushInfo( const QString& title, const QString& message )
{
  pushMessage( title, message, INFO );
}

void xMessageBar::pushWarning( const QString& title, const QString& message )
{
  pushMessage( title, message, WARNING );
}

void xMessageBar::pushCritical( const QString& title, const QString& message )
{
  pushMessage( title, message, CRITICAL );
}

void xMessageBar::showItem( xMessageBarItem *item )
{
  Q_ASSERT( item );

  if ( mCurrentItem )
    disconnect( mCurrentItem, SIGNAL( styleChanged( QString ) ), this, SLOT( setStyleSheet( QString ) ) );

  if ( item == mCurrentItem )
    return;

  if ( mItems.contains( item ) )
    mItems.removeOne( item );

  if ( mCurrentItem )
  {
    mItems.prepend( mCurrentItem );
    mLayout->removeWidget( mCurrentItem );
    mCurrentItem->hide();
  }

  mCurrentItem = item;
  mLayout->addWidget( item, 0, 1, 1, 1 );
  mCurrentItem->show();

  if ( item->duration() > 0 )
  {
    mCountProgress->setRange( 0, item->duration() );
    mCountProgress->setValue( item->duration() );
    mCountProgress->setVisible( true );
    mCountdownTimer->start();
  }

  connect( mCurrentItem, SIGNAL( styleChanged( QString ) ), this, SLOT( setStyleSheet( QString ) ) );
  setStyleSheet( item->getStyleSheet() );
  show();

  emit widgetAdded( item );
}

void xMessageBar::pushItem( xMessageBarItem *item )
{
  resetCountdown();
  // avoid duplicated widget
  popWidget( item );
  showItem( item );
}

xMessageBarItem* xMessageBar::pushWidget( QWidget *widget, xMessageBar::MessageLevel level, int duration )
{
  xMessageBarItem *item;
  item = dynamic_cast<xMessageBarItem*>( widget );
  if ( item )
  {
    item->setLevel( level )->setDuration( duration );
  }
  else
  {
    item = new xMessageBarItem( widget, level, duration );
  }
  pushItem( item );
  return item;
}

void xMessageBar::pushMessage( const QString &title, const QString &text, xMessageBar::MessageLevel level, int duration )
{
  xMessageBarItem *item = new xMessageBarItem( title, text, level, duration );
  pushItem( item );
}

xMessageBarItem* xMessageBar::createMessage( const QString &text, QWidget *parent )
{
  xMessageBarItem* item = new xMessageBarItem( text, INFO, 0, parent );
  return item;
}

xMessageBarItem* xMessageBar::createMessage( const QString &title, const QString &text, QWidget *parent )
{
  return new xMessageBarItem( title, text, xMessageBar::INFO, 0, parent );
}

xMessageBarItem* xMessageBar::createMessage( QWidget *widget, QWidget *parent )
{
  return new xMessageBarItem( widget, INFO, 0, parent );
}

void xMessageBar::updateCountdown()
{
  if ( !mCountdownTimer->isActive() )
  {
    resetCountdown();
    return;
  }
  if ( mCountProgress->value() < 2 )
  {
    popWidget();
  }
  else
  {
    mCountProgress->setValue( mCountProgress->value() - 1 );
  }
}

void xMessageBar::resetCountdown()
{
  if ( mCountdownTimer->isActive() )
    mCountdownTimer->stop();

  mCountProgress->setStyleSheet( mCountStyleSheet.arg( "mIconTimerPause.png" ) );
  mCountProgress->setVisible( false );
}

void xMessageBar::updateItemCount()
{
  mItemCount->setText( !mItems.isEmpty() ? tr( "%n more", "unread messages", mItems.count() ) : QString() );

  // do not show the down arrow for opening menu with "close all" if there is just one message
  mCloseBtn->setMenu( !mItems.isEmpty() ? mCloseMenu : nullptr );
  mCloseBtn->setPopupMode( !mItems.isEmpty() ? QToolButton::MenuButtonPopup : QToolButton::DelayedPopup );
}
