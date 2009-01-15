/*!	\file CContextOSX.cpp
	\author Michael Olsen
	\date 10/Mar/2005
	\date 10/Mar/2005
*/

static OSStatus AppEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);

static OSStatus WindowEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);

DEFINE_ONE_SHOT_HANDLER_GETTER(WindowEventHandler)

static const EventTypeSpec    kWindowEvents[] =
{
	{ kEventClassWindow, kEventWindowActivated },
	{ kEventClassWindow, kEventWindowDeactivated },
	{ kEventClassWindow, kEventWindowHandleActivate },
	{ kEventClassWindow, kEventWindowHandleDeactivate },
	{ kEventClassWindow, kEventWindowGetClickActivation },
	{ kEventClassWindow, kEventWindowGetClickModality },
	{ kEventClassWindow, kEventWindowClose },
	{ kEventClassWindow, kEventWindowResizeStarted },
	{ kEventClassWindow, kEventWindowResizeCompleted },
	{ kEventClassWindow, kEventWindowBoundsChanging},
	{ kEventClassWindow, kEventWindowBoundsChanged},
	{ kEventClassWindow, kEventWindowCursorChange}
};


CContextOSX::CContextOSX()
{
	// Get the max number of allowed open files
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	if ((rl.rlim_cur != RLIM_INFINITY) && (rl.rlim_cur != rl.rlim_max)) {
		// Not set to inifinity or max
		rl.rlim_cur = rl.rlim_max;
		setrlimit(RLIMIT_NOFILE, &rl);
	}
}

CContextOSX::~CContextOSX()
{
}

IContext* IContext::Create()
{
	return dynamic_cast<IContext*>(new CContextOSX());
}

void CContextOSX::Destroy()
{
	delete dynamic_cast<CContextOSX*>(this);
}

void CContextOSX::SetInstanceData(void* /*p*/)
{
}

void* CContextOSX::GetInstanceData()
{
	return NULL;
}

const void* CContextOSX::GetInstanceData() const
{
	return NULL;
}

void CContextOSX::GetPathName(tchar* pszPathName)
{
	IFile::GetSystemDirectory(IFile::SystemDirApplications, pszPathName);

	std::string sPathName((const char*)pszPathName);

	tchar pszName[64];
	mpCallback->GetName(pszName);
	std::string sName((const char*)pszName);
	
	sPathName += sName;
	sPathName += ".kspi";

    strcpy((char*)pszPathName, sPathName.c_str());
}

void CContextOSX::Init()
{
}

CContextOSX* gpMainContext = NULL;

void* CContextOSX::CreateMainWindow(SSize /*Size*/, tint32 /*iWindowsOnly_MenuResourceID = -1*/, tint32 /*iWindowsOnly_IconResourceID = -1*/)
{
	gpMainContext = this;

	IBNibRef sNibRef;
    OSStatus                    err;
    static const EventTypeSpec    kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };

    // Create a Nib reference, passing the name of the nib file (without the .nib extension).
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference( CFSTR("main"), &sNibRef );
//    require_noerr( err, CantGetNibRef );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
    err = SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
//    require_noerr( err, CantSetMenuBar );
    
    // Install our handler for common commands on the application target
//    InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
//                                    GetEventTypeCount( kAppEvents ), kAppEvents,
//                                    0, NULL );

	WindowRef              gpNexsynWindow;
    err = CreateWindowFromNib( sNibRef, CFSTR("MainWindow"), &gpNexsynWindow );

    // Position new windows in a staggered arrangement on the main screen
    RepositionWindow( gpNexsynWindow, NULL, kWindowCascadeOnMainScreen );
    
    // The window was created hidden, so show it
//    ShowWindow( gpNexsynWindow );

	::InvalMenuBar();
	::DrawMenuBar();

	// Install application event handler
    InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
                                    GetEventTypeCount( kAppEvents ), kAppEvents,
                                    this, NULL );

	InstallWindowEventHandler(gpNexsynWindow, GetWindowEventHandlerUPP(),
		GetEventTypeCount(kWindowEvents), kWindowEvents,
		gpNexsynWindow, NULL);

	return (void*)gpNexsynWindow;
}

void CContextOSX::ShowWindow(void* pWnd, tbool bShow)
{
	if (bShow) {
		::ShowWindow((WindowRef)pWnd);
	}
	else {
		::HideWindow((WindowRef)pWnd);
	}
}

tbool CContextOSX::IsWindowVisible(void* pWnd)
{
	return ::IsWindowVisible((WindowRef)pWnd);
}

void CContextOSX::SelectWindow(void* pWnd)
{
	::SelectWindow((WindowRef)pWnd);
}

void* CContextOSX::CreateExtraWindow(void* pszResName, SSize /*Size*/, tbool)
{
	IBNibRef sNibRef;
    OSStatus err;

    // Create a Nib reference, passing the name of the nib file (without the .nib extension).
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference( CFSTR("main"), &sNibRef );
//    require_noerr( err, CantGetNibRef );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
//    err = SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
//    require_noerr( err, CantSetMenuBar );
    
    // Install our handler for common commands on the application target
//    InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
//                                    GetEventTypeCount( kAppEvents ), kAppEvents,
//                                    0, NULL );

	WindowRef              pWindow;
    err = CreateWindowFromNib(sNibRef, (CFStringRef)pszResName, &pWindow );

    // Position new windows in a staggered arrangement on the main screen
//    RepositionWindow( pWindow, NULL, kWindowCascadeOnMainScreen );
    
    // The window was created hidden, so show it
//    ShowWindow( pWindow );
//	  HideWindow(pWindow);

	::InvalMenuBar();
	::DrawMenuBar();

	InstallWindowEventHandler(pWindow, GetWindowEventHandlerUPP(),
		GetEventTypeCount(kWindowEvents), kWindowEvents,
		pWindow, NULL);

	return (void*)pWindow;
}

void CContextOSX::LimitedHandleEventQueue(tbool /*bAllowMouseAndKeyboard*/, tbool* /*pbBreak*/)
{
} // LimitedHandleEventQueue

void CContextOSX::RunMainLoop()
{
//	::RunApplicationEventLoop();

	EventRef theEvent;
	EventTargetRef theTarget;

	theTarget = ::GetEventDispatcherTarget();

//	while (::ReceiveNextEvent(0, NULL, kEventDurationForever, true, &theEvent) == noErr) {
//		::SendEventToEventTarget(theEvent, theTarget);
//		::ReleaseEvent(theEvent);
//	}

	tint32 iRunIndex = 0;
	tbool bStop = false;
	while (bStop == false) {
		if (::ReceiveNextEvent(0, NULL, 0.001, true, &theEvent) == noErr) {
			// We have an event
			::SendEventToEventTarget(theEvent, theTarget);
			::ReleaseEvent(theEvent);
		}
		else {
			if (mpAppCallback) {
				bStop = mpAppCallback->OnInit(iRunIndex++);
			}
			else {
				bStop = true;
			}
		}
	}

	// We're done initializing, so enter the regular loop
	::RunApplicationEventLoop();
}

void CContextOSX::OnMenuEventHandler(tint32 iData1, tint32 iData2)
{
	std::list<SMenuInfo>::const_iterator it = mMenuInfos.begin();
	for (; it != mMenuInfos.end(); it++) {
		const SMenuInfo& Info = *it;

		if (Info.Ref == (MenuRef)iData2) {
			if (Info.iIndex == iData1) {
				if (mpAppCallback) {
					mpAppCallback->OnMenuEvent(Info.sName.c_str());
				}
			}
		}
	}
}

static OSStatus AppEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    OSStatus    result = eventNotHandledErr;
    
    switch ( GetEventClass( inEvent ) )
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
            
            switch ( GetEventKind( inEvent ) )
            {
                case kEventCommandProcess:
					CContextOSX* pContext = (CContextOSX*)inRefcon;
					pContext->OnMenuEventHandler(cmd.source.menu.menuItemIndex, (tint32)cmd.source.menu.menuRef);
 /*                   switch ( cmd.commandID )
                    {
                        case kHICommandNew:
                            result = HandleNew();
                            break;

                        default:
						{
							int iIndex = cmd.source.menu.menuItemIndex;
							if (cmd.source.menu.menuRef == refAudioDevice) {
								OnMenuAudioDevice(iIndex);
							}
							else if (cmd.source.menu.menuRef == refAudioSampleRate) {
								OnMenuAudioSampleRate(iIndex);
							}
							else if (cmd.source.menu.menuRef == refAudioBufferSize) {
								OnMenuAudioBufferSize(iIndex);
							}
							else if (cmd.source.menu.menuRef == refMIDI) {
								OnMenuMIDIDevice(iIndex);
							}
							else if (cmd.source.menu.menuRef == refWindows) {
								OnMenuWindows(iIndex);
							}
							else if (cmd.source.menu.menuRef == refFile) {
								OnMenuFile(iIndex);
							}
						}
                        break;
                    }
                    break;*/
					
            }
            break;
        }
            
        default:
            break;
    }
    
    return result;
}

void CContextOSX::DoCreateMenuItem(const MenuRef& Ref, const SMenuItemList* pList, tint32& riMenuID, const std::string& sName)
{
	tuint32 uiMenuIndex;
	for (uiMenuIndex = 0; uiMenuIndex < pList->uiItemCount; uiMenuIndex++) {
		char psPascal[256];
		psPascal[0] = strlen(pList->pItems[uiMenuIndex].pszName);
		strcpy((char*)(psPascal + 1), pList->pItems[uiMenuIndex].pszName);

		std::string sNameCur(sName);
		sNameCur += "@";
		sNameCur += std::string(pList->pItems[uiMenuIndex].pszName);

		if (pList->pItems[uiMenuIndex].bHasSubMenu) {
			MenuRef RefSubMenu;
			OSErr err = ::CreateNewMenu(riMenuID++, 0, &RefSubMenu);
			err = ::SetMenuTitle(RefSubMenu, (const unsigned char*)psPascal);
			::InsertMenuItem(Ref, (const unsigned char*)psPascal, 0);

			err = ::SetMenuItemHierarchicalMenu(Ref, 1, RefSubMenu);

			const SMenuItemList* pSubList = pList->pItems[uiMenuIndex].pSubMenu;
			DoCreateMenuItem(RefSubMenu, pSubList, riMenuID, sNameCur);
		}
		else {
		//	static int iID = 1000;
			
			::InsertMenuItem(Ref, (const unsigned char*)psPascal, 1000);
			
			char cmd = pList->pItems[uiMenuIndex].cmd;
			
			if(cmd)
				::SetItemCmd( Ref, uiMenuIndex+1, cmd );
		
			
	//		SetItemCmd( Ref, riMenuID, 'N'+riMenuID );
			
			SMenuInfo Info;
			Info.Ref = Ref;
			Info.iIndex = (tint32)uiMenuIndex + 1;
			Info.sName = sNameCur;
			mMenuInfos.push_back(Info);
			
		}
	}
}

void CContextOSX::CreateMenu(const SMenuItemList* pList)
{
	tint32 iMenuID = 10;


	tuint32 uiMenuIndex;
	for (uiMenuIndex = 0; uiMenuIndex < pList->uiItemCount; uiMenuIndex++) {
		char psPascal[256];
		psPascal[0] = strlen(pList->pItems[uiMenuIndex].pszName);
		strcpy((char*)(psPascal + 1), pList->pItems[uiMenuIndex].pszName);

		MenuRef Ref;
		OSErr err = ::CreateNewMenu(iMenuID++, 0, &Ref);
		err = ::SetMenuTitle(Ref, (const unsigned char*)psPascal);
		::InsertMenu(Ref, 0);

		// All root menu items are lists
		const SMenuItemList* pSubList = pList->pItems[uiMenuIndex].pSubMenu;
		std::string sName(pList->pItems[uiMenuIndex].pszName);
		DoCreateMenuItem(Ref, pSubList, iMenuID, sName);
	}
}

void CContextOSX::SizeChanged(void* pWnd, SSize SizeNew)
{
	SSize SizeNewOsX = SizeNew;
	
	SizeNewOsX.iCX	+= 1;
	SizeNewOsX.iCY  += 1;
	
	if (mpAppCallback) {
		mpAppCallback->SizeChanged(pWnd, SizeNewOsX);
	}
}

void CContextOSX::WindowClosed(void* pWnd)
{
	if (mpAppCallback) {
		mpAppCallback->WindowClosed(pWnd);
	}
}

void CContextOSX::CloseWindow(void* pWnd)
{
	WindowClosed(pWnd);

	::DisposeWindow((WindowPtr)pWnd);
}

OSStatus WindowEventHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)
{
    OSStatus    err = eventNotHandledErr;
	WindowRef Ref;
    
    switch ( GetEventClass( inEvent ) )
    {
        case kEventClassCommand:
        {
            HICommandExtended cmd;
            verify_noerr( GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof( cmd ), NULL, &cmd ) );
            
            switch ( GetEventKind( inEvent ) )
            {
                case kEventCommandProcess:
                    switch ( cmd.commandID )
                    {
                        // Add your own command-handling cases here
                        
                        default:
                            break;
                    }
                    break;
            }
            break;
        }

		case kEventClassWindow:
			switch (GetEventKind(inEvent)) {
				case kEventWindowClose:{
//					kspi::IGUI* pGUI = (kspi::IGUI*)inRefcon;
//					pGUI->Destroy();
					// Hide window, but don't close it
					Ref = (WindowRef)inRefcon;

					::HideWindow(Ref);

					gpMainContext->WindowClosed((void*)Ref);

					return noErr;
				}
				case kEventWindowActivated:{
					Ref = (WindowRef)inRefcon;

					return noErr;
				}
				/*
				case kEventWindowDeactivated:{
					WindowRef Ref = (WindowRef)inRefcon;
					
					if(Ref == gWndZoom){
							::HideWindow(gWndZoom);
							//gbShowZoom = false;
						}
					break;
				
				}
				*/

				case kEventWindowBoundsChanged:
				{
					UInt32 Attributes;
					OSStatus err;

					err = GetEventParameter (inEvent, kEventParamAttributes, typeUInt32,
						NULL, sizeof(UInt32), NULL, &Attributes);

					if (!err) {
						if (Attributes & kWindowBoundsChangeSizeChanged) {
							// Window is being resized
							WindowRef Ref = (WindowRef)inRefcon;
							Rect rct;
							::GetWindowBounds(Ref, kWindowContentRgn, &rct);
							gpMainContext->SizeChanged(Ref, SSize(rct.right - rct.left+1, rct.bottom - rct.top+1));
							return noErr;
						}
						else if (Attributes & kWindowBoundsChangeOriginChanged) {
							// Window is being moved
						}
					}

					return noErr;
				}

				case kEventWindowCursorChange:
				{
					((CWindowOSX*)inRefcon)->SetCurrentMouseCursor();
				}
				return noErr;

		}
		break;

        default:
            break;
    }
    
    return err;
}




