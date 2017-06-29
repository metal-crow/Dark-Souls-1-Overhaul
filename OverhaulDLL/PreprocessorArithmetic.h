/*
	Original author unknown; source:
		https://stackoverflow.com/questions/43513352/macro-perform-integer-arithmetic/43523637
*/

#pragma once

#define MUL(A,B) NUM(GLUE(P,A)(GLUE_I(P,B)()))
#define ADD(A,B) NUM(GLUE(P,A)(),GLUE_I(P,B)())
#define GLUE(X,Y) GLUE_I(X,Y)
#define GLUE_I(X,Y) X##Y
#define NUM(...) NUM_I(__VA_ARGS__,\
 /*255,254,253,252,251,250,\
 249,248,247,246,245,244,243,242,241,240,\
 239,238,237,236,235,234,233,232,231,230,\
 229,228,227,226,225,224,223,222,221,220,\
 219,218,217,216,215,214,213,212,211,210,\
 209,208,207,206,205,204,203,202,201,200,\
 199,198,197,196,195,194,193,192,191,190,\
 189,188,187,186,185,184,183,182,181,180,\
 179,178,177,176,175,174,173,172,171,170,\
 169,168,167,166,165,164,163,162,161,160,\
 159,158,157,156,155,154,153,152,151,150,\
 149,148,147,146,145,144,143,142,141,140,\
 139,138,137,136,135,134,133,132,131,130,\
 129,128,127,126,*/125,124,123,122,121,120,\
 119,118,117,116,115,114,113,112,111,110,\
 109,108,107,106,105,104,103,102,101,100,\
  99, 98, 97, 96, 95, 94, 93, 92, 91, 90,\
  89, 88, 87, 86, 85, 84, 83, 82, 81, 80,\
  79, 78, 77, 76, 75, 74, 73, 72, 71, 70,\
  69, 68, 67, 66, 65, 64, 63, 62, 61, 60,\
  59, 58, 57, 56, 55, 54, 53, 52, 51, 50,\
  49, 48, 47, 46, 45, 44, 43, 42, 41, 40,\
  39, 38, 37, 36, 35, 34, 33, 32, 31, 30,\
  29, 28, 27, 26, 25, 24, 23, 22, 21, 20,\
  19, 18, 17, 16, 15, 14, 13, 12, 11, 10,\
   9,  8,  7,  6,  5,  4,  3,  2,  1,  )

#define NUM_I(\
  /*_255,_254,_253,_252,_251,_250,\
  _249,_248,_247,_246,_245,_244,_243,_242,_241,_240,\
  _239,_238,_237,_236,_235,_234,_233,_232,_231,_230,\
  _229,_228,_227,_226,_225,_224,_223,_222,_221,_220,\
  _219,_218,_217,_216,_215,_214,_213,_212,_211,_210,\
  _209,_208,_207,_206,_205,_204,_203,_202,_201,_200,\
  _199,_198,_197,_196,_195,_194,_193,_192,_191,_190,\
  _189,_188,_187,_186,_185,_184,_183,_182,_181,_180,\
  _179,_178,_177,_176,_175,_174,_173,_172,_171,_170,\
  _169,_168,_167,_166,_165,_164,_163,_162,_161,_160,\
  _159,_158,_157,_156,_155,_154,_153,_152,_151,_150,\
  _149,_148,_147,_146,_145,_144,_143,_142,_141,_140,\
  _139,_138,_137,_136,_135,_134,_133,_132,_131,_130,\
  _129,_128,_127,_126,*/_125,_124,_123,_122,_121,_120,\
  _119,_118,_117,_116,_115,_114,_113,_112,_111,_110,\
  _109,_108,_107,_106,_105,_104,_103,_102,_101,_100,\
   _99, _98, _97, _96, _95, _94, _93, _92, _91, _90,\
   _89, _88, _87, _86, _85, _84, _83, _82, _81, _80,\
   _79, _78, _77, _76, _75, _74, _73, _72, _71, _70,\
   _69, _68, _67, _66, _65, _64, _63, _62, _61, _60,\
   _59, _58, _57, _56, _55, _54, _53, _52, _51, _50,\
   _49, _48, _47, _46, _45, _44, _43, _42, _41, _40,\
   _39, _38, _37, _36, _35, _34, _33, _32, _31, _30,\
   _29, _28, _27, _26, _25, _24, _23, _22, _21, _20,\
   _19, _18, _17, _16, _15, _14, _13, _12, _11, _10,\
   _9, _8, _7, _6, _5, _4, _3, _2, _1,X,...) X

#define P1(...)  __VA_ARGS__
#define P2(...)  P1(__VA_ARGS__),__VA_ARGS__
#define P3(...)  P2(__VA_ARGS__),__VA_ARGS__
#define P4(...)  P3(__VA_ARGS__),__VA_ARGS__
#define P5(...)  P4(__VA_ARGS__),__VA_ARGS__
#define P6(...)  P5(__VA_ARGS__),__VA_ARGS__
#define P7(...)  P6(__VA_ARGS__),__VA_ARGS__
#define P8(...)  P7(__VA_ARGS__),__VA_ARGS__
#define P9(...)  P8(__VA_ARGS__),__VA_ARGS__
#define P10(...) P9(__VA_ARGS__),__VA_ARGS__
#define P11(...) P10(__VA_ARGS__),__VA_ARGS__
#define P12(...) P11(__VA_ARGS__),__VA_ARGS__
#define P13(...) P12(__VA_ARGS__),__VA_ARGS__
#define P14(...) P13(__VA_ARGS__),__VA_ARGS__
#define P15(...) P14(__VA_ARGS__),__VA_ARGS__
#define P16(...) P15(__VA_ARGS__),__VA_ARGS__
#define P17(...) P16(__VA_ARGS__),__VA_ARGS__
#define P18(...) P17(__VA_ARGS__),__VA_ARGS__
#define P19(...) P18(__VA_ARGS__),__VA_ARGS__
#define P20(...) P19(__VA_ARGS__),__VA_ARGS__
#define P21(...) P20(__VA_ARGS__),__VA_ARGS__
#define P22(...) P21(__VA_ARGS__),__VA_ARGS__
#define P23(...) P22(__VA_ARGS__),__VA_ARGS__
#define P24(...) P23(__VA_ARGS__),__VA_ARGS__
#define P25(...) P24(__VA_ARGS__),__VA_ARGS__
#define P26(...) P25(__VA_ARGS__),__VA_ARGS__
#define P27(...) P26(__VA_ARGS__),__VA_ARGS__
#define P28(...) P27(__VA_ARGS__),__VA_ARGS__
#define P29(...) P28(__VA_ARGS__),__VA_ARGS__
#define P30(...) P29(__VA_ARGS__),__VA_ARGS__
#define P31(...) P30(__VA_ARGS__),__VA_ARGS__
#define P32(...) P31(__VA_ARGS__),__VA_ARGS__
#define P33(...) P32(__VA_ARGS__),__VA_ARGS__
#define P34(...) P33(__VA_ARGS__),__VA_ARGS__
#define P35(...) P34(__VA_ARGS__),__VA_ARGS__
#define P36(...) P35(__VA_ARGS__),__VA_ARGS__
#define P37(...) P36(__VA_ARGS__),__VA_ARGS__
#define P38(...) P37(__VA_ARGS__),__VA_ARGS__
#define P39(...) P38(__VA_ARGS__),__VA_ARGS__
#define P40(...) P39(__VA_ARGS__),__VA_ARGS__
#define P41(...) P40(__VA_ARGS__),__VA_ARGS__
#define P42(...) P41(__VA_ARGS__),__VA_ARGS__
#define P43(...) P42(__VA_ARGS__),__VA_ARGS__
#define P44(...) P43(__VA_ARGS__),__VA_ARGS__
#define P45(...) P44(__VA_ARGS__),__VA_ARGS__
#define P46(...) P45(__VA_ARGS__),__VA_ARGS__
#define P47(...) P46(__VA_ARGS__),__VA_ARGS__
#define P48(...) P47(__VA_ARGS__),__VA_ARGS__
#define P49(...) P48(__VA_ARGS__),__VA_ARGS__
#define P50(...) P49(__VA_ARGS__),__VA_ARGS__
#define P51(...) P50(__VA_ARGS__),__VA_ARGS__
#define P52(...) P51(__VA_ARGS__),__VA_ARGS__
#define P53(...) P52(__VA_ARGS__),__VA_ARGS__
#define P54(...) P53(__VA_ARGS__),__VA_ARGS__
#define P55(...) P54(__VA_ARGS__),__VA_ARGS__
#define P56(...) P55(__VA_ARGS__),__VA_ARGS__
#define P57(...) P56(__VA_ARGS__),__VA_ARGS__
#define P58(...) P57(__VA_ARGS__),__VA_ARGS__
#define P59(...) P58(__VA_ARGS__),__VA_ARGS__
#define P60(...) P59(__VA_ARGS__),__VA_ARGS__
#define P61(...) P60(__VA_ARGS__),__VA_ARGS__
#define P62(...) P61(__VA_ARGS__),__VA_ARGS__
#define P63(...) P62(__VA_ARGS__),__VA_ARGS__
#define P64(...) P63(__VA_ARGS__),__VA_ARGS__
#define P65(...) P64(__VA_ARGS__),__VA_ARGS__
#define P66(...) P65(__VA_ARGS__),__VA_ARGS__
#define P67(...) P66(__VA_ARGS__),__VA_ARGS__
#define P68(...) P67(__VA_ARGS__),__VA_ARGS__
#define P69(...) P68(__VA_ARGS__),__VA_ARGS__
#define P70(...) P69(__VA_ARGS__),__VA_ARGS__
#define P71(...) P70(__VA_ARGS__),__VA_ARGS__
#define P72(...) P71(__VA_ARGS__),__VA_ARGS__
#define P73(...) P72(__VA_ARGS__),__VA_ARGS__
#define P74(...) P73(__VA_ARGS__),__VA_ARGS__
#define P75(...) P74(__VA_ARGS__),__VA_ARGS__
#define P76(...) P75(__VA_ARGS__),__VA_ARGS__
#define P77(...) P76(__VA_ARGS__),__VA_ARGS__
#define P78(...) P77(__VA_ARGS__),__VA_ARGS__
#define P79(...) P78(__VA_ARGS__),__VA_ARGS__
#define P80(...) P79(__VA_ARGS__),__VA_ARGS__
#define P81(...) P80(__VA_ARGS__),__VA_ARGS__
#define P82(...) P81(__VA_ARGS__),__VA_ARGS__
#define P83(...) P82(__VA_ARGS__),__VA_ARGS__
#define P84(...) P83(__VA_ARGS__),__VA_ARGS__
#define P85(...) P84(__VA_ARGS__),__VA_ARGS__
#define P86(...) P85(__VA_ARGS__),__VA_ARGS__
#define P87(...) P86(__VA_ARGS__),__VA_ARGS__
#define P88(...) P87(__VA_ARGS__),__VA_ARGS__
#define P89(...) P88(__VA_ARGS__),__VA_ARGS__
#define P90(...) P89(__VA_ARGS__),__VA_ARGS__
#define P91(...) P90(__VA_ARGS__),__VA_ARGS__
#define P92(...) P91(__VA_ARGS__),__VA_ARGS__
#define P93(...) P92(__VA_ARGS__),__VA_ARGS__
#define P94(...) P93(__VA_ARGS__),__VA_ARGS__
#define P95(...) P94(__VA_ARGS__),__VA_ARGS__
#define P96(...) P95(__VA_ARGS__),__VA_ARGS__
#define P97(...) P96(__VA_ARGS__),__VA_ARGS__
#define P98(...) P97(__VA_ARGS__),__VA_ARGS__
#define P99(...) P98(__VA_ARGS__),__VA_ARGS__
#define P100(...) P99(__VA_ARGS__),__VA_ARGS__
#define P101(...) P100(__VA_ARGS__),__VA_ARGS__
#define P102(...) P101(__VA_ARGS__),__VA_ARGS__
#define P103(...) P102(__VA_ARGS__),__VA_ARGS__
#define P104(...) P103(__VA_ARGS__),__VA_ARGS__
#define P105(...) P104(__VA_ARGS__),__VA_ARGS__
#define P106(...) P105(__VA_ARGS__),__VA_ARGS__
#define P107(...) P106(__VA_ARGS__),__VA_ARGS__
#define P108(...) P107(__VA_ARGS__),__VA_ARGS__
#define P109(...) P108(__VA_ARGS__),__VA_ARGS__
#define P110(...) P109(__VA_ARGS__),__VA_ARGS__
#define P111(...) P110(__VA_ARGS__),__VA_ARGS__
#define P112(...) P111(__VA_ARGS__),__VA_ARGS__
#define P113(...) P112(__VA_ARGS__),__VA_ARGS__
#define P114(...) P113(__VA_ARGS__),__VA_ARGS__
#define P115(...) P114(__VA_ARGS__),__VA_ARGS__
#define P116(...) P115(__VA_ARGS__),__VA_ARGS__
#define P117(...) P116(__VA_ARGS__),__VA_ARGS__
#define P118(...) P117(__VA_ARGS__),__VA_ARGS__
#define P119(...) P118(__VA_ARGS__),__VA_ARGS__
#define P120(...) P119(__VA_ARGS__),__VA_ARGS__
#define P121(...) P120(__VA_ARGS__),__VA_ARGS__
#define P122(...) P121(__VA_ARGS__),__VA_ARGS__
#define P123(...) P122(__VA_ARGS__),__VA_ARGS__
#define P124(...) P123(__VA_ARGS__),__VA_ARGS__
#define P125(...) P124(__VA_ARGS__),__VA_ARGS__
#define P126(...) P125(__VA_ARGS__),__VA_ARGS__
#define P127(...) P126(__VA_ARGS__),__VA_ARGS__
#define P128(...) P127(__VA_ARGS__),__VA_ARGS__
#define P129(...) P128(__VA_ARGS__),__VA_ARGS__
#define P130(...) P129(__VA_ARGS__),__VA_ARGS__
#define P131(...) P130(__VA_ARGS__),__VA_ARGS__
#define P132(...) P131(__VA_ARGS__),__VA_ARGS__
#define P133(...) P132(__VA_ARGS__),__VA_ARGS__
#define P134(...) P133(__VA_ARGS__),__VA_ARGS__
#define P135(...) P134(__VA_ARGS__),__VA_ARGS__
#define P136(...) P135(__VA_ARGS__),__VA_ARGS__
#define P137(...) P136(__VA_ARGS__),__VA_ARGS__
#define P138(...) P137(__VA_ARGS__),__VA_ARGS__
#define P139(...) P138(__VA_ARGS__),__VA_ARGS__
#define P140(...) P139(__VA_ARGS__),__VA_ARGS__
#define P141(...) P140(__VA_ARGS__),__VA_ARGS__
#define P142(...) P141(__VA_ARGS__),__VA_ARGS__
#define P143(...) P142(__VA_ARGS__),__VA_ARGS__
#define P144(...) P143(__VA_ARGS__),__VA_ARGS__
#define P145(...) P144(__VA_ARGS__),__VA_ARGS__
#define P146(...) P145(__VA_ARGS__),__VA_ARGS__
#define P147(...) P146(__VA_ARGS__),__VA_ARGS__
#define P148(...) P147(__VA_ARGS__),__VA_ARGS__
#define P149(...) P148(__VA_ARGS__),__VA_ARGS__
#define P150(...) P149(__VA_ARGS__),__VA_ARGS__
#define P151(...) P150(__VA_ARGS__),__VA_ARGS__
#define P152(...) P151(__VA_ARGS__),__VA_ARGS__
#define P153(...) P152(__VA_ARGS__),__VA_ARGS__
#define P154(...) P153(__VA_ARGS__),__VA_ARGS__
#define P155(...) P154(__VA_ARGS__),__VA_ARGS__
#define P156(...) P155(__VA_ARGS__),__VA_ARGS__
#define P157(...) P156(__VA_ARGS__),__VA_ARGS__
#define P158(...) P157(__VA_ARGS__),__VA_ARGS__
#define P159(...) P158(__VA_ARGS__),__VA_ARGS__
#define P160(...) P159(__VA_ARGS__),__VA_ARGS__
#define P161(...) P160(__VA_ARGS__),__VA_ARGS__
#define P162(...) P161(__VA_ARGS__),__VA_ARGS__
#define P163(...) P162(__VA_ARGS__),__VA_ARGS__
#define P164(...) P163(__VA_ARGS__),__VA_ARGS__
#define P165(...) P164(__VA_ARGS__),__VA_ARGS__
#define P166(...) P165(__VA_ARGS__),__VA_ARGS__
#define P167(...) P166(__VA_ARGS__),__VA_ARGS__
#define P168(...) P167(__VA_ARGS__),__VA_ARGS__
#define P169(...) P168(__VA_ARGS__),__VA_ARGS__
#define P170(...) P169(__VA_ARGS__),__VA_ARGS__
#define P171(...) P170(__VA_ARGS__),__VA_ARGS__
#define P172(...) P171(__VA_ARGS__),__VA_ARGS__
#define P173(...) P172(__VA_ARGS__),__VA_ARGS__
#define P174(...) P173(__VA_ARGS__),__VA_ARGS__
#define P175(...) P174(__VA_ARGS__),__VA_ARGS__
#define P176(...) P175(__VA_ARGS__),__VA_ARGS__
#define P177(...) P176(__VA_ARGS__),__VA_ARGS__
#define P178(...) P177(__VA_ARGS__),__VA_ARGS__
#define P179(...) P178(__VA_ARGS__),__VA_ARGS__
#define P180(...) P179(__VA_ARGS__),__VA_ARGS__
#define P181(...) P180(__VA_ARGS__),__VA_ARGS__
#define P182(...) P181(__VA_ARGS__),__VA_ARGS__
#define P183(...) P182(__VA_ARGS__),__VA_ARGS__
#define P184(...) P183(__VA_ARGS__),__VA_ARGS__
#define P185(...) P184(__VA_ARGS__),__VA_ARGS__
#define P186(...) P185(__VA_ARGS__),__VA_ARGS__
#define P187(...) P186(__VA_ARGS__),__VA_ARGS__
#define P188(...) P187(__VA_ARGS__),__VA_ARGS__
#define P189(...) P188(__VA_ARGS__),__VA_ARGS__
#define P190(...) P189(__VA_ARGS__),__VA_ARGS__
#define P191(...) P190(__VA_ARGS__),__VA_ARGS__
#define P192(...) P191(__VA_ARGS__),__VA_ARGS__
#define P193(...) P192(__VA_ARGS__),__VA_ARGS__
#define P194(...) P193(__VA_ARGS__),__VA_ARGS__
#define P195(...) P194(__VA_ARGS__),__VA_ARGS__
#define P196(...) P195(__VA_ARGS__),__VA_ARGS__
#define P197(...) P196(__VA_ARGS__),__VA_ARGS__
#define P198(...) P197(__VA_ARGS__),__VA_ARGS__
#define P199(...) P198(__VA_ARGS__),__VA_ARGS__
#define P200(...) P199(__VA_ARGS__),__VA_ARGS__
#define P201(...) P200(__VA_ARGS__),__VA_ARGS__
#define P202(...) P201(__VA_ARGS__),__VA_ARGS__
#define P203(...) P202(__VA_ARGS__),__VA_ARGS__
#define P204(...) P203(__VA_ARGS__),__VA_ARGS__
#define P205(...) P204(__VA_ARGS__),__VA_ARGS__
#define P206(...) P205(__VA_ARGS__),__VA_ARGS__
#define P207(...) P206(__VA_ARGS__),__VA_ARGS__
#define P208(...) P207(__VA_ARGS__),__VA_ARGS__
#define P209(...) P208(__VA_ARGS__),__VA_ARGS__
#define P210(...) P209(__VA_ARGS__),__VA_ARGS__
#define P211(...) P210(__VA_ARGS__),__VA_ARGS__
#define P212(...) P211(__VA_ARGS__),__VA_ARGS__
#define P213(...) P212(__VA_ARGS__),__VA_ARGS__
#define P214(...) P213(__VA_ARGS__),__VA_ARGS__
#define P215(...) P214(__VA_ARGS__),__VA_ARGS__
#define P216(...) P215(__VA_ARGS__),__VA_ARGS__
#define P217(...) P216(__VA_ARGS__),__VA_ARGS__
#define P218(...) P217(__VA_ARGS__),__VA_ARGS__
#define P219(...) P218(__VA_ARGS__),__VA_ARGS__
#define P220(...) P219(__VA_ARGS__),__VA_ARGS__
#define P221(...) P220(__VA_ARGS__),__VA_ARGS__
#define P222(...) P221(__VA_ARGS__),__VA_ARGS__
#define P223(...) P222(__VA_ARGS__),__VA_ARGS__
#define P224(...) P223(__VA_ARGS__),__VA_ARGS__
#define P225(...) P224(__VA_ARGS__),__VA_ARGS__
#define P226(...) P225(__VA_ARGS__),__VA_ARGS__
#define P227(...) P226(__VA_ARGS__),__VA_ARGS__
#define P228(...) P227(__VA_ARGS__),__VA_ARGS__
#define P229(...) P228(__VA_ARGS__),__VA_ARGS__
#define P230(...) P229(__VA_ARGS__),__VA_ARGS__
#define P231(...) P230(__VA_ARGS__),__VA_ARGS__
#define P232(...) P231(__VA_ARGS__),__VA_ARGS__
#define P233(...) P232(__VA_ARGS__),__VA_ARGS__
#define P234(...) P233(__VA_ARGS__),__VA_ARGS__
#define P235(...) P234(__VA_ARGS__),__VA_ARGS__
#define P236(...) P235(__VA_ARGS__),__VA_ARGS__
#define P237(...) P236(__VA_ARGS__),__VA_ARGS__
#define P238(...) P237(__VA_ARGS__),__VA_ARGS__
#define P239(...) P238(__VA_ARGS__),__VA_ARGS__
#define P240(...) P239(__VA_ARGS__),__VA_ARGS__
#define P241(...) P240(__VA_ARGS__),__VA_ARGS__
#define P242(...) P241(__VA_ARGS__),__VA_ARGS__
#define P243(...) P242(__VA_ARGS__),__VA_ARGS__
#define P244(...) P243(__VA_ARGS__),__VA_ARGS__
#define P245(...) P244(__VA_ARGS__),__VA_ARGS__
#define P246(...) P245(__VA_ARGS__),__VA_ARGS__
#define P247(...) P246(__VA_ARGS__),__VA_ARGS__
#define P248(...) P247(__VA_ARGS__),__VA_ARGS__
#define P249(...) P248(__VA_ARGS__),__VA_ARGS__
#define P250(...) P249(__VA_ARGS__),__VA_ARGS__
#define P251(...) P250(__VA_ARGS__),__VA_ARGS__
#define P252(...) P251(__VA_ARGS__),__VA_ARGS__
#define P253(...) P252(__VA_ARGS__),__VA_ARGS__
#define P254(...) P253(__VA_ARGS__),__VA_ARGS__
#define P255(...) P254(__VA_ARGS__),__VA_ARGS__

// MUL(32, ADD(3, 4))