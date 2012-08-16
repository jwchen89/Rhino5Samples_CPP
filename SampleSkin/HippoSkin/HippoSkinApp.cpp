/////////////////////////////////////////////////////////////////////////////
// HippoSkinApp.cpp

#include "stdafx.h"
#include "HippoSkinApp.h"

// Rhino plug-in declaration
RHINO_PLUG_IN_DECLARE

// Rhino plug-in id
// Provide a unique uuid for your plug-in
RHINO_PLUG_IN_ID( L"675BD714-C05D-4B14-9066-06CB08DC7880" );

// Rhino plug-in version
// Provide a version number string for this plug-in
RHINO_PLUG_IN_VERSION( __DATE__"  "__TIME__ )

// Rhino plug-in developer declarations
RHINO_PLUG_IN_DEVELOPER_ORGANIZATION( L"Robert McNeel & Associates" );
RHINO_PLUG_IN_DEVELOPER_ADDRESS( L"3670 Woodland Park Avenue North\015\012Seattle WA 98103" );
RHINO_PLUG_IN_DEVELOPER_COUNTRY( L"United States" );
RHINO_PLUG_IN_DEVELOPER_PHONE( L"206-545-6877" );
RHINO_PLUG_IN_DEVELOPER_FAX( L"206-545-7321" );
RHINO_PLUG_IN_DEVELOPER_EMAIL( L"tech@mcneel.com" );
RHINO_PLUG_IN_DEVELOPER_WEBSITE( L"http://www.rhino3d.com" );
RHINO_PLUG_IN_UPDATE_URL( L"https://github.com/mcneel/" );

//
//	Note!
//
//    A Rhino plug-in is an MFC DLL.
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.

/////////////////////////////////////////////////////////////////////////////
// CHippoSkinApp

BEGIN_MESSAGE_MAP(CHippoSkinApp, CWinApp)
END_MESSAGE_MAP()

CHippoSkinApp::CHippoSkinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CHippoSkinApp object
CHippoSkinApp theApp;

BOOL CHippoSkinApp::InitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  //   Only standard MFC DLL instance initialization belongs here. 
  //   All other significant initialization should take place in
  //   CHippoSkinPlugIn::OnLoadPlugIn().
	CWinApp::InitInstance();
	return TRUE;
}

int CHippoSkinApp::ExitInstance()
{
  // CRITICAL: DO NOT CALL RHINO SDK FUNCTIONS HERE!
  //   Only standard MFC DLL instance clean up belongs here. 
  //   All other significant cleanup should take place in either
  //   CHippoSkinPlugIn::OnSaveAllSettings() or CHippoSkinPlugIn::OnUnloadPlugIn().
  return CWinApp::ExitInstance();
}

////////////////////////////////////////////////////////////////////////////
// CSplashWnd

IMPLEMENT_DYNAMIC( CSplashWnd, CWnd )

CSplashWnd::CSplashWnd()
{
}

CSplashWnd::~CSplashWnd()
{
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
  ON_WM_PAINT()
  ON_WM_CREATE()
END_MESSAGE_MAP()

void CSplashWnd::OnPaint()
{
  CPaintDC dc( this ); // device context for painting

  CRect r;
  GetClientRect( r );

  if( (HBITMAP)m_splash_bitmap )
  {
    CDC memDC;
    memDC.CreateCompatibleDC( NULL );
    CBitmap* pBMP = memDC.SelectObject( &m_splash_bitmap );
    dc.BitBlt( 0, 0, r.Width(), r.Height(), &memDC, 0, 0, SRCCOPY );
  }
  else
  {
    dc.FillSolidRect( r, ::GetSysColor(COLOR_WINDOW) );
    COLORREF cr = dc.SetTextColor( ::GetSysColor(COLOR_WINDOWTEXT) );
    int iBkMode = dc.SetBkMode( TRANSPARENT );
    CString s = L"Sample Splash Screen";
    dc.DrawText( s, r, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
    dc.SetTextColor( cr );
    dc.SetBkMode( iBkMode );
  }
}

int CSplashWnd::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if (CWnd::OnCreate(lpCreateStruct) == -1 )
    return -1;

  AFX_MANAGE_STATE( ::AfxGetStaticModuleState() );
  m_splash_bitmap.LoadBitmap( IDB_SPLASH );

  CRect rW, rC;
  GetWindowRect( rW );
  GetClientRect( rC );

  BITMAP bmp;
  m_splash_bitmap.GetBitmap( &bmp );

  SetWindowPos( NULL,
                0, 0,
                bmp.bmWidth + (rW.Width() - rC.Width()),
                bmp.bmHeight + (rW.Height() - rC.Height()),
                SWP_NOMOVE|SWP_NOZORDER
              );

  CenterWindow();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CHippoSkinDLL

class CHippoSkinDLL : public CRhinoSkinDLL
{
public:
  CHippoSkinDLL();
  ~CHippoSkinDLL();

  // Required overrides
  HICON MainRhinoIcon();
  const wchar_t* ApplicationName();
  UUID SkinPlugInID();
  void ShowSplash( bool bShow );

  // Optional overrides
  HMENU MainRhinoMenu();

private:
  HICON m_hIcon; // Used to save the skin icon handle so the destructor can delete it later.
  CMenu m_menu;  // Used to save the menu handle so the destructor can delete it later.
  CSplashWnd m_wndSplash; // Splash window to create and show when ShowSplash(true) is called.
};

// The one and only CHippoSkinDLL object. 
// This must be created for the skin to load.
static CHippoSkinDLL theSkin;

CHippoSkinDLL::CHippoSkinDLL()
: m_hIcon(NULL)
{
}

CHippoSkinDLL::~CHippoSkinDLL()
{
  // Make sure the DLL module instance is active since this is a DLL which is being loaded
  // by the Rhino executable.
  //
  // (Extracted from the Platform SDK help file)
  // For more information on module states and MFC, see "Managing the State Data of MFC Modules" in Creating
  // New Documents, Windows, and Views and Technical Note 58.
  // 
  AFX_MANAGE_STATE( ::AfxGetStaticModuleState() );

  // Destroy the splash window if necessary.
  if( ::IsWindow(m_wndSplash.m_hWnd) )
    m_wndSplash.DestroyWindow();

  // Destroy the applicaion icon if necessary.
  if( m_hIcon )
    ::DestroyIcon( m_hIcon );
  m_hIcon = NULL;
}

const wchar_t* CHippoSkinDLL::ApplicationName()
{
  // Return application name string used to replace the string "Rhino".
  // this must return a non NULL string or the skin DLL will fail to load.
  return L"Hippopotamus";
}

// Return CSkinPlugInSamplePlugIn::PlugInID()
UUID CHippoSkinDLL::SkinPlugInID()
{
  // Returns the UUID of the companion Rhino plug-in that is used
  // to manage this DLL's menu and provided other extensions to 
  // Rhino. If this plug-in is not going to provide a custom menu, 
  // then is must return ON_nil_uuid.

  // TODO: Return a unique identifier for the plug-in.
  // {675BD714-C05D-4B14-9066-06CB08DC7880}
  return ON_UuidFromString( RhinoPlugInId() );
}

void CHippoSkinDLL::ShowSplash( bool bShow )
{
  // This method will be called when Rhino wants to display or hide a splash screen
  // on startup.  If you do not provide a splash screen then none will appear.  
  // This will not be called if Rhino is started with the "/nosplash" option.

  // (Extracted from the Platform SDK help file)
  // For more information on module states and MFC, see 
  // "Managing the State Data of MFC Modules" in Creating
  // New Documents, Windows, and Views and Technical Note 58.
  AFX_MANAGE_STATE( ::AfxGetStaticModuleState() );

  if( bShow && FALSE == ::IsWindow(m_wndSplash.m_hWnd) )
  {
    CSize size( ::GetSystemMetrics(SM_CXFULLSCREEN), ::GetSystemMetrics(SM_CYFULLSCREEN) );
    CRect r( CPoint(0,0), size );
    r.DeflateRect( r.Width() / 3, r.Height() / 3 );
    m_wndSplash.CreateEx( WS_EX_TOPMOST,
                          AfxRegisterWndClass(NULL),
                          ApplicationName(),
                          WS_POPUP | WS_VISIBLE | WS_BORDER,
                          r,
		                      NULL,
                          0,
                          NULL
                          );
  }
  else if( bShow )
  {
    m_wndSplash.ShowWindow( SW_SHOW );
    m_wndSplash.UpdateWindow();
  }
  else if( !bShow && ::IsWindow(m_wndSplash.m_hWnd) )
  {
    m_wndSplash.DestroyWindow();
  }
}

HICON CHippoSkinDLL::MainRhinoIcon()
{
  // Return HICON to be used by Rhino main frame and dialog boxes, 
  // the skin DLL will fail to load if this returns NULL.

  // (Extracted from the Platform SDK help file)
  // For more information on module states and MFC, see 
  // "Managing the State Data of MFC Modules" in Creating
  // New Documents, Windows, and Views and Technical Note 58.
  AFX_MANAGE_STATE( ::AfxGetStaticModuleState());

  if( NULL == m_hIcon )
    m_hIcon = theApp.LoadIcon( IDI_ICON );

  return m_hIcon;
}

HMENU CHippoSkinDLL::MainRhinoMenu()
{
  // Override this and return a valid HMENU if you want to replace
  // the main Rhino menu otherwise Rhino will use its default menu.

  // (Extracted from the Platform SDK help file)
  // For more information on module states and MFC, see 
  // "Managing the State Data of MFC Modules" in Creating
  // New Documents, Windows, and Views and Technical Note 58.
  AFX_MANAGE_STATE( ::AfxGetStaticModuleState());

  // TODO: load your menu resource here. 
  //   For example:
  if( NULL == m_menu.GetSafeHmenu() )
    m_menu.LoadMenu( IDR_MENU1 );

  return m_menu.GetSafeHmenu();
}
