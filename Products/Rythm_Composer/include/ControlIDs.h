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



// Control IDs

enum EControlID
{
	// TODO: Add control IDs below here.
	giCtrl_Delay_Bypass = 0,

	giCtrl_Shuffle,
	giCtrl_Accent,
	// BD
	giCtrl_BD_Tune,
	giCtrl_BD_Attack,
	giCtrl_BD_Level,
	giCtrl_BD_Decay,
	// SD
	giCtrl_SD_Tune,
	giCtrl_SD_Tone,
	giCtrl_SD_Level,
	giCtrl_SD_Decay,
	// LT
	giCtrl_LT_Tune,
	giCtrl_LT_Dec,
	giCtrl_LT_Level,
	// MT
	giCtrl_MT_Tune,
	giCtrl_MT_Dec,
	giCtrl_MT_Level,
	// HT
	giCtrl_HT_Tune,
	giCtrl_HT_Dec,
	giCtrl_HT_Level,
	// HT
	giCtrl_Rim_Level,
	giCtrl_Clap_Level,
	// CH
	giCtrl_CH_Level,
	giCtrl_CH_Dec,
	// OH
	giCtrl_OH_Level,
	giCtrl_OH_Dec,
	// Chrash
	giCtrl_Chrash_Level,
	giCtrl_Chrash_Tune,
	// Ride
	giCtrl_Ride_Level,
	giCtrl_Ride_Dec,
	// Start
	giCtrl_Start,
	
	
	// Tempo
	giCtrl_Tempo,
	// Sync
	giCtrl_Sync,
	// Song
	giCtrl_Song,
	
	giCtrl_Copy,
	giCtrl_Paste,
	giCtrl_Clear,
	
	giCtrl_SeqStep01,
	giCtrl_Next	=	giCtrl_SeqStep01 +16,
	
	// Volume
	giCtrl_Volume,
	
	// Steps
	giCtrl_Steps,
	
	


	// This enum index *must* be the last!
	giNrOfDelayControls
};
