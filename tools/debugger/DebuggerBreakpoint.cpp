/*****************************************************************************
                    The Dark Mod GPL Source Code
 
 This file is part of the The Dark Mod Source Code, originally based 
 on the Doom 3 GPL Source Code as published in 2011.
 
 The Dark Mod Source Code is free software: you can redistribute it 
 and/or modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. For details, see LICENSE.TXT.
 
 Project: The Dark Mod (http://www.thedarkmod.com/)
 
 $Revision$ (Revision of last commit) 
 $Date$ (Date of last commit)
 $Author$ (Author of last commit)
 
******************************************************************************/

#include "precompiled.h"
#pragma hdrstop

#include "DebuggerApp.h"
#include "DebuggerBreakpoint.h"

int rvDebuggerBreakpoint::mNextID = 1;

rvDebuggerBreakpoint::rvDebuggerBreakpoint ( const char* filename, int linenumber, int id )
{
	mFilename = filename;
	mLineNumber = linenumber;
	mEnabled = true;
	
	if ( id == -1 )
	{	
		mID = mNextID++;
	}	
	else 
	{
		mID = id;
	}
}

rvDebuggerBreakpoint::rvDebuggerBreakpoint ( rvDebuggerBreakpoint& bp )
{
	mFilename = bp.mFilename;
	mEnabled = bp.mEnabled;
	mLineNumber = bp.mLineNumber;
}

rvDebuggerBreakpoint::~rvDebuggerBreakpoint ( void )
{
}
