

#include "KSOS.h"

CUndoManager::CUndoManager()
{
}

CUndoManager::~CUndoManager()
{
}

void CUndoManager::AddUndoState(const SUndoState& State)
{
	mUndoStates.push_back(State);
}

void CUndoManager::Undo()
{
	if (mUndoStates.size() == 0) {
		return;
	}

	SUndoState State = *(mUndoStates.rend());
	mUndoStates.pop_back();

	std::list<CUndoEvent>::const_iterator it = State.Events.begin();
	for (; it != State.Events.end(); it++) {
		CUndoEvent Event = *it;
	}
}

