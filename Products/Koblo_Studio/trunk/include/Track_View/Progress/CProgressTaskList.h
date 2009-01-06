// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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

class CProgressTaskList
{
protected:
	std::list<CProgressTask*> listCProgressTask;

	volatile tbool mbAbort;
	volatile tbool mbAbort_DontTell;

public:
	CProgressTaskList()
	{
		mbAbort = false;
		
	} // constructor

	virtual ~CProgressTaskList();

	void Add(CProgressTask* pTask)
	{
		CAutoLock Lock(mMutex);

		listCProgressTask.insert(listCProgressTask.end(), pTask);
	} // Add

	tbool IsInProgress()
	{
		CAutoLock Lock(mMutex);

		return IsInProgress_NoLock();
	} // IsInProgress

	void DoAbort(tbool bNoDialogBox = false)
	{
		mbAbort = true;
		mbAbort_DontTell = bNoDialogBox;
	} // DoAbort

	tbool DoWork();
	tbool IsDone();

	const tchar* GetError()
	{
		CAutoLock Lock(mMutex);

		if (!IsInProgress_NoLock())
			return "No task";

		CProgressTask* pInfo = *(listCProgressTask.begin());
		return pInfo->GetError();
	} // GetError

	virtual const tchar* GetProgress(tuint64* piIx, tuint64* piAll)
	{
		CAutoLock Lock(mMutex);

		if (!IsInProgress_NoLock()) {
			*piIx = *piAll = 0;
			return "";
		}

		CProgressTask* pInfo = *(listCProgressTask.begin());
		return pInfo->GetProgress(piIx, piAll);
	}

protected:


	CMutex mMutex;

	void DeleteList();

	tbool IsInProgress_NoLock()
	{
		return (listCProgressTask.size() > 0);
	} // IsInProgress

	tbool IsDone_NoLock();

}; // CProgressTaskList

