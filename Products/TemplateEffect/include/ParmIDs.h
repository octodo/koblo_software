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



// Parameter IDs
enum EParmID
{
	// This enum index *must* be the first!
	giParmIDTemplateBypass = 0,

	// TODO: Create more parm-IDs below here.
	// NOTE: Parameters will be saved to presets, can be MIDI controlable and can be automated.
	// NOTE: You *must* create parameters for *all* Parm-IDs in CTemplatePlugIn::AddParameters()!
	// NOTE: If you re-arrange the parm-IDs you *must* reorder them in CTemplatePlugIn::AddParameters() too (but remember that this will break existing presets)!



	// This enum index *must* be the last!
	giNrOfTemplateParms
};


