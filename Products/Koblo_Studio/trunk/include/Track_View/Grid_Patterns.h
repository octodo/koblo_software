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

/*
enum EGridColors{


const int giColor_1					= 0;
const int giColor_2					= 141;
const int giColor_4					= 142;
const int giColor_8					= 192;
const int giColor_16				= 208;
const int giColor_32				= 230;

};
*/
enum EGridColors{

giColor_1	=	0,
giColor_2,
giColor_4,
giColor_8,
giColor_16,
giColor_32,
giNumber_Of_Grid_Colors
};



//------------------------------------------
//	2/4
//------------------------------------------
const int giSignature2_4				= 0;
const int giSignature2_4_Beats			= 2;
const int giSignature2_4_Sub_Beats		= 16;

const int gpiSignature2_4[giSignature2_4_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_2,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

};

//------------------------------------------
//	3/4
//------------------------------------------
const int giSignature3_4				= 1;
const int giSignature3_4_Beats			= 3;
const int giSignature3_4_Sub_Beats		= 24;
const int gpiSignature3_4[giSignature3_4_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_4,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_4,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32
};

//------------------------------------------
//	4/4
//------------------------------------------
const int giSignature4_4				= 2;
const int giSignature4_4_Beats			= 4;
const int giSignature4_4_Sub_Beats		= 32;
const int gpiSignature4_4[giSignature4_4_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_4,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_2,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32

giColor_4,						// 1	-	Beat
giColor_32,						// 2	-	1/32
giColor_16,						// 3	-	1/16
giColor_32,						// 4	-	1/32
giColor_8,						// 5	-	1/16
giColor_32,						// 6	-	1/32
giColor_16,						// 7	-	1/16
giColor_32,						// 8	-	1/32
};

//------------------------------------------
//	6/8
//------------------------------------------
const int giSignature6_8				= 3;
const int giSignature6_8_Beats			= 2;
const int giSignature6_8_Sub_Beats		= 12;
const int gpiSignature6_8[giSignature6_8_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_2,						
giColor_32,
giColor_8,						
giColor_32,
giColor_8,					
giColor_32,



};

//------------------------------------------
//	9/8
//------------------------------------------
const int giSignature9_8				= 4;
const int giSignature9_8_Beats			= 3;
const int giSignature9_8_Sub_Beats		= 18;
const int gpiSignature9_8[giSignature9_8_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_2,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_2,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

};

//------------------------------------------
//	12/8
//------------------------------------------
const int giSignature12_8					= 5;
const int giSignature12_8_Beats				= 4;
const int giSignature12_8_Sub_Beats			= 24;
const int gpiSignature12_8[giSignature12_8_Sub_Beats] = {

giColor_1,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_4,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_2,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,

giColor_4,						// 1	-	Beat
giColor_32,
giColor_8,						
giColor_32,
giColor_8,						
giColor_32,





};

const int giZoom_Levels			=	37;
const int gi_Default_Zoom_Level	=	9;

const double gpiPixels_Pr_Sec[giZoom_Levels] = {
4,
5.03968419958,
6.34960420787,
8,
10.07936839916,
12.69920841575,
16,					// Zoom Level 0
20.15873679832,
25.39841683149,
32,					// Zoom Level 2
40.31747359664,
50.79683366298,
64,					// Zoom Level 4
80.63494719327,
101.59366732596,
128,				// Zoom Level 6
161.26989438654,
203.18733465193,
256,				// Zoom Level 8
322.53978877309,
406.37466930386,
512,				// Zoom Level 10
645.07957754617,
812.74933860772,
1024,				// Zoom Level 12
1290.15915509235,
1625.49867721543,
2048,				// Zoom Level 14
2580.3183101847,
3250.99735443087,
4096,				// Zoom Level 16
5160.6366203694,
6501.99470886174,
8192,				// Zoom Level 18
10321.2732407388,
13003.98941772349,
16384,				// Zoom Level 20


};



