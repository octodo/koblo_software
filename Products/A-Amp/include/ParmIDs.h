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
	giParmIDAAmpBypass = 0,
	giParam_Gain,
	giParam_Tube,
	giParam_Harm,
	giParam_Limiter,
	giParam_Swap,
 
	
	/*
	 swap 0:
	 tube		=	1,
	 harm		=	2,
	 limiter	=	3, 
	 
	
	 
	 swap 0: 123 / THL
	 swap 1: 132 / TLH
	 swap 2: 213 / HTL
	 swap 3: 213 / HLT
	 swap 4: 312 / LTH
	 swap 5: 321 / LHT
	*/
	
	// This enum index *must* be the last!
	giNrOfAAmpParms
};




