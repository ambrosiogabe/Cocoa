#include "cocoa/commands/CommandHistory.h"

namespace Cocoa
{
	ICommand* CommandHistory::mCommands[1000] = {};
	int CommandHistory::mCommandSize = 0;
	int CommandHistory::mCommandPtr = 0;

	void CommandHistory::addCommand(ICommand* cmd)
	{
		cmd->execute();

		if (mCommandPtr < mCommandSize - 1)
		{
			for (int i = mCommandSize - 1; i > mCommandPtr; i--)
			{
				delete mCommands[i];
			}
			mCommandSize = mCommandPtr + 1;
		}

		mCommands[mCommandSize] = cmd;
		mCommandSize++;

		if (mCommandSize > 1 && mCommands[mCommandSize - 1]->canMerge() && mCommands[mCommandSize - 2]->canMerge())
		{
			if (mCommands[mCommandSize - 1]->mergeWith(mCommands[mCommandSize - 2]))
			{
				delete mCommands[mCommandSize - 1];
				mCommandSize--;
			}
		}

		mCommandPtr = mCommandSize - 1;
	}

	void CommandHistory::setNoMergeMostRecent()
	{
		if (mCommandSize - 1 >= 0)
		{
			mCommands[mCommandSize - 1]->setNoMerge();
		}
	}

	void CommandHistory::undo()
	{
		if (mCommandPtr >= 0)
		{
			mCommands[mCommandPtr]->undo();
			mCommandPtr--;
		}
	}

	void CommandHistory::redo()
	{
		int redoCommand = mCommandPtr + 1;
		if (redoCommand < mCommandSize && redoCommand >= 0)
		{
			mCommands[redoCommand]->execute();
			mCommandPtr++;
		}
	}
}