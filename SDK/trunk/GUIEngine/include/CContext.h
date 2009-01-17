// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


#ifndef _CONTEXT_
#define _CONTEXT_


/*! \class
 * \brief Implementation of IContext context that is cross-platform
*/
class CContext : public virtual IContext
{
public:
	//! Constructor
	CContext();
	//! Destructor
	virtual ~CContext();

	//! IContext override
	virtual void SetCallback(IContextCallback* pCallback);
	//! IContext override
	virtual void SetApplicationCallback(IApplicationCallback* pCallback);

	//! IContext override - dummy content
	virtual void SetClosingBehaviour(void* pWnd, EClosingBehaviour eClosingBehaviour) { };
	//! IContext override - dummy content
	virtual const EClosingBehaviour GetClosingBehaviour(void* pWnd) { return ge::IContext::giClosingReallyCloses; };

	//! IContext override - dummy content
	virtual void SetMinWindowSize(IWindow* pWnd, SSize sizeMin) { };
	
	//! IContext override - dummy content
	virtual void SetMaxWindowSize(IWindow* pWnd, SSize sizeMax) { };


	// Internal
	virtual IContextCallback* GetCallback();
	virtual const IContextCallback* GetCallback() const;

protected:
	IContextCallback* mpCallback;

	IApplicationCallback* mpAppCallback;

};

#endif	// _CONTEXT_
