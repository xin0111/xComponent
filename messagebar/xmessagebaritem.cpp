#include "xmessagebaritem.h"
#include "xmessagebar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>

xMessageBarItem::xMessageBarItem( const QString &text, xMessageBar::MessageLevel level, int duration, QWidget *parent )
    : QWidget( parent )
    , mTitle( "" )
    , mText( text )
    , mLevel( level )
    , mDuration( duration )
    , mWidget( nullptr )
    , mUserIcon( QIcon() )
    , mLayout( nullptr )
{
  writeContent();
}

xMessageBarItem::xMessageBarItem( const QString &title, const QString &text, xMessageBar::MessageLevel level, int duration, QWidget *parent )
    : QWidget( parent )
    , mTitle( title )
    , mText( text )
    , mLevel( level )
    , mDuration( duration )
    , mWidget( nullptr )
    , mUserIcon( QIcon() )
    , mLayout( nullptr )
{
  writeContent();
}

xMessageBarItem::xMessageBarItem( const QString &title, const QString &text, QWidget *widget, xMessageBar::MessageLevel level, int duration, QWidget *parent )
    : QWidget( parent )
    , mTitle( title )
    , mText( text )
    , mLevel( level )
    , mDuration( duration )
    , mWidget( widget )
    , mUserIcon( QIcon() )
    , mLayout( nullptr )
{
  writeContent();
}

xMessageBarItem::xMessageBarItem( QWidget *widget, xMessageBar::MessageLevel level, int duration, QWidget *parent )
    : QWidget( parent )
    , mTitle( "" )
    , mText( "" )
    , mLevel( level )
    , mDuration( duration )
    , mWidget( widget )
    , mUserIcon( QIcon() )
    , mLayout( nullptr )
{
  writeContent();
}

xMessageBarItem::~xMessageBarItem()
{
}

void xMessageBarItem::writeContent()
{
  if ( !mLayout )
  {
    mLayout = new QHBoxLayout( this );
    mLayout->setContentsMargins( 0, 0, 0, 0 );
    mTextEdit = nullptr;
    mLblIcon = nullptr;
  }

  // ICON
  if ( !mLblIcon )
  {
    mLblIcon = new QLabel( this );
    mLayout->addWidget( mLblIcon );
  }
  QIcon icon;
  if ( !mUserIcon.isNull() )
  {
    icon = mUserIcon;
  }
  else
  {
	  QString msgIcon(":/images/mIconInfo.png");
	  switch (mLevel)
	  {
	  case xMessageBar::CRITICAL:
		  msgIcon = QString(":/images/mIconCritical.png");
		  break;
	  case xMessageBar::WARNING:
		  msgIcon = QString(":/images/mIconWarning.svg");
		  break;
	  case xMessageBar::SUCCESS:
		  msgIcon = QString(":/images/mIconSuccess.png");
		  break;
	  default:
		  break;
	  }
    icon = QIcon( msgIcon ); 
  }
  mLblIcon->setPixmap( icon.pixmap( 24 ) );

  // TITLE AND TEXT
  if ( mTitle.isEmpty() && mText.isEmpty() )
  {
    if ( mTextEdit )
    {
      delete mTextEdit;
      mTextEdit = nullptr;
    }
  }
  else
  {
    if ( !mTextEdit )
    {
      mTextEdit = new QTextEdit( this );
      mTextEdit->setObjectName( "textEdit" );
      mTextEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Maximum );
      mTextEdit->setReadOnly( true );
      mTextEdit->setFrameShape( QFrame::NoFrame );
      // stylesheet set here so Qt-style substitued scrollbar arrows can show within limited height
      // adjusts to height of font set in app options
      mTextEdit->setStyleSheet( "QTextEdit { background-color: rgba(0,0,0,0); margin-top: 0.25em; max-height: 1.75em; min-height: 1.75em; } "
                                "QScrollBar { background-color: rgba(0,0,0,0); } "
                                "QScrollBar::add-page,QScrollBar::sub-page,QScrollBar::handle { background-color: rgba(0,0,0,0); color: rgba(0,0,0,0); } "
                                "QScrollBar::up-arrow,QScrollBar::down-arrow { color: rgb(0,0,0); } " );
      mLayout->addWidget( mTextEdit );
    }
    QString content = mText;
    if ( !mTitle.isEmpty() )
    {
      // add ':' to end of title
      QString t = mTitle.trimmed();
      if ( !content.isEmpty() && !t.endsWith( ':' ) && !t.endsWith( ": " ) )
        t += ": ";
      content.prepend( QLatin1String( "<b>" ) + t + " </b>" );
    }
    mTextEdit->setText( content );
  }

  // WIDGET
  if ( mWidget )
  {
    QLayoutItem *item = mLayout->itemAt( 2 );
    if ( !item || item->widget() != mWidget )
    {
      mLayout->addWidget( mWidget );
    }
  }

  // STYLESHEET
  if ( mLevel == xMessageBar::SUCCESS )
  {
    mStyleSheet = "xMessageBar { background-color: #dff0d8; border: 1px solid #8e998a; } "
                  "QLabel,QTextEdit { color: black; } ";
  }
  else if ( mLevel == xMessageBar::CRITICAL )
  {
    mStyleSheet = "xMessageBar { background-color: #d65253; border: 1px solid #9b3d3d; } "
                  "QLabel,QTextEdit { color: white; } ";
  }
  else if ( mLevel == xMessageBar::WARNING )
  {
    mStyleSheet = "xMessageBar { background-color: #ffc800; border: 1px solid #e0aa00; } "
                  "QLabel,QTextEdit { color: black; } ";
  }
  else if ( mLevel == xMessageBar::INFO )
  {
    mStyleSheet = "xMessageBar { background-color: #e7f5fe; border: 1px solid #b9cfe4; } "
                  "QLabel,QTextEdit { color: #2554a1; } ";
  }
  mStyleSheet += "QLabel#mItemCount { font-style: italic; }";
}

xMessageBarItem* xMessageBarItem::setText( const QString& text )
{
  mText = text;
  writeContent();
  return this;
}

xMessageBarItem *xMessageBarItem::setTitle( const QString& title )
{
  mTitle = title;
  writeContent();
  return this;
}

xMessageBarItem *xMessageBarItem::setLevel( xMessageBar::MessageLevel level )
{
  mLevel = level;
  writeContent();
  emit styleChanged( mStyleSheet );
  return this;
}

xMessageBarItem *xMessageBarItem::setWidget( QWidget *widget )
{
  if ( mWidget )
  {
    QLayoutItem *item;
    item = mLayout->itemAt( 2 );
    if ( item->widget() == mWidget )
    {
      delete item->widget();
    }
  }
  mWidget = widget;
  writeContent();
  return this;
}

xMessageBarItem* xMessageBarItem::setIcon( const QIcon &icon )
{
  mUserIcon = icon;
  return this;
}


xMessageBarItem* xMessageBarItem::setDuration( int duration )
{
  mDuration = duration;
  return this;
}

