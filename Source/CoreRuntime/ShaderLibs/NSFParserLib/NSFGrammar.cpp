/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse NSFParserparse
#define yylex   NSFParserlex
#define yyerror NSFParsererror
#define yylval  NSFParserlval
#define yychar  NSFParserchar
#define yydebug NSFParserdebug
#define yynerrs NSFParsernerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EOLN = 258,
     PATH = 259,
     L_ACCOLADE = 260,
     R_ACCOLADE = 261,
     L_PARENTHESE = 262,
     R_PARENTHESE = 263,
     L_BRACKET = 264,
     R_BRACKET = 265,
     L_ANGLEBRACKET = 266,
     R_ANGLEBRACKET = 267,
     OR = 268,
     ASSIGN = 269,
     COMMA = 270,
     NSF_AT_SYMBOL = 271,
     NSF_COLON = 272,
     NSF_SEMICOLON = 273,
     UNDERSCORE = 274,
     ASTERIK = 275,
     FORWARDSLASH = 276,
     PLUS = 277,
     MINUS = 278,
     N_HEX = 279,
     N_FLOAT = 280,
     N_INT = 281,
     N_STRING = 282,
     N_QUOTE = 283,
     N_BOOL = 284,
     N_VERSION = 285,
     NSFSHADER = 286,
     ARTIST = 287,
     HIDDEN = 288,
     SAVE = 289,
     ATTRIBUTES = 290,
     GLOBALATTRIBUTES = 291,
     ATTRIB = 292,
     ATTRIB_BOOL = 293,
     ATTRIB_STRING = 294,
     ATTRIB_UINT = 295,
     ATTRIB_FLOAT = 296,
     ATTRIB_POINT2 = 297,
     ATTRIB_POINT3 = 298,
     ATTRIB_POINT4 = 299,
     ATTRIB_MATRIX3 = 300,
     ATTRIB_TRANSFORM = 301,
     ATTRIB_COLOR = 302,
     ATTRIB_TEXTURE = 303,
     PACKINGDEF = 304,
     PD_STREAM = 305,
     PD_FIXEDFUNCTION = 306,
     SEMANTICADAPTERTABLE = 307,
     PDP_POSITION = 308,
     PDP_POSITION0 = 309,
     PDP_POSITION1 = 310,
     PDP_POSITION2 = 311,
     PDP_POSITION3 = 312,
     PDP_POSITION4 = 313,
     PDP_POSITION5 = 314,
     PDP_POSITION6 = 315,
     PDP_POSITION7 = 316,
     PDP_BLENDWEIGHTS = 317,
     PDP_BLENDINDICES = 318,
     PDP_NORMAL = 319,
     PDP_POINTSIZE = 320,
     PDP_COLOR = 321,
     PDP_COLOR2 = 322,
     PDP_TEXCOORD0 = 323,
     PDP_TEXCOORD1 = 324,
     PDP_TEXCOORD2 = 325,
     PDP_TEXCOORD3 = 326,
     PDP_TEXCOORD4 = 327,
     PDP_TEXCOORD5 = 328,
     PDP_TEXCOORD6 = 329,
     PDP_TEXCOORD7 = 330,
     PDP_NORMAL2 = 331,
     PDP_TANGENT = 332,
     PDP_BINORMAL = 333,
     PDP_EXTRADATA = 334,
     PDT_FLOAT1 = 335,
     PDT_FLOAT2 = 336,
     PDT_FLOAT3 = 337,
     PDT_FLOAT4 = 338,
     PDT_UBYTECOLOR = 339,
     PDT_SHORT1 = 340,
     PDT_SHORT2 = 341,
     PDT_SHORT3 = 342,
     PDT_SHORT4 = 343,
     PDT_UBYTE4 = 344,
     PDT_NORMSHORT1 = 345,
     PDT_NORMSHORT2 = 346,
     PDT_NORMSHORT3 = 347,
     PDT_NORMSHORT4 = 348,
     PDT_NORMPACKED3 = 349,
     PDT_PBYTE1 = 350,
     PDT_PBYTE2 = 351,
     PDT_PBYTE3 = 352,
     PDT_PBYTE4 = 353,
     PDT_FLOAT2H = 354,
     PDT_NORMUBYTE4 = 355,
     PDT_NORMUSHORT2 = 356,
     PDT_NORMUSHORT4 = 357,
     PDT_UDEC3 = 358,
     PDT_NORMDEC3 = 359,
     PDT_FLOAT16_2 = 360,
     PDT_FLOAT16_4 = 361,
     PDTESS_DEFAULT = 362,
     PDTESS_PARTIALU = 363,
     PDTESS_PARTIALV = 364,
     PDTESS_CROSSUV = 365,
     PDTESS_UV = 366,
     PDTESS_LOOKUP = 367,
     PDTESS_LOOKUPPRESAMPLED = 368,
     PDU_POSITION = 369,
     PDU_BLENDWEIGHT = 370,
     PDU_BLENDINDICES = 371,
     PDU_NORMAL = 372,
     PDU_PSIZE = 373,
     PDU_TEXCOORD = 374,
     PDU_TANGENT = 375,
     PDU_BINORMAL = 376,
     PDU_TESSFACTOR = 377,
     PDU_POSITIONT = 378,
     PDU_COLOR = 379,
     PDU_FOG = 380,
     PDU_DEPTH = 381,
     PDU_SAMPLE = 382,
     RENDERSTATES = 383,
     CMDEFINED = 384,
     CMATTRIBUTE = 385,
     CMCONSTANT = 386,
     CMGLOBAL = 387,
     CMOPERATOR = 388,
     VSCONSTANTMAP = 389,
     VSPROGRAM = 390,
     GSCONSTANTMAP = 391,
     GSPROGRAM = 392,
     PSCONSTANTMAP = 393,
     PSPROGRAM = 394,
     CONSTANTMAP = 395,
     PROGRAM = 396,
     ENTRYPOINT = 397,
     SHADERTARGET = 398,
     SOFTWAREVP = 399,
     THREADGROUPCOUNTS = 400,
     SHADERPROGRAM = 401,
     SHADERTYPE = 402,
     SKINBONEMATRIX3 = 403,
     REQUIREMENTS = 404,
     FEATURELEVEL = 405,
     VSVERSION = 406,
     GSVERSION = 407,
     PSVERSION = 408,
     CSVERSION = 409,
     USERVERSION = 410,
     PLATFORM = 411,
     BONESPERPARTITION = 412,
     BINORMALTANGENTMETHOD = 413,
     BINORMALTANGENTUVSOURCE = 414,
     NBTMETHOD_NONE = 415,
     NBTMETHOD_NI = 416,
     NBTMETHOD_MAX = 417,
     NBTMETHOD_ATI = 418,
     USERDEFINEDDATA = 419,
     IMPLEMENTATION = 420,
     OUTPUTSTREAM = 421,
     STREAMOUTPUT = 422,
     STREAMOUTTARGETS = 423,
     STREAMOUTAPPEND = 424,
     MAXVERTEXCOUNT = 425,
     OUTPUTPRIMTYPE = 426,
     _POINT = 427,
     _LINE = 428,
     _TRIANGLE = 429,
     VERTEXFORMAT = 430,
     FMT_FLOAT = 431,
     FMT_INT = 432,
     FMT_UINT = 433,
     CLASSNAME = 434,
     PASS = 435,
     STAGE = 436,
     TSS_TEXTURE = 437,
     TSS_COLOROP = 438,
     TSS_COLORARG0 = 439,
     TSS_COLORARG1 = 440,
     TSS_COLORARG2 = 441,
     TSS_ALPHAOP = 442,
     TSS_ALPHAARG0 = 443,
     TSS_ALPHAARG1 = 444,
     TSS_ALPHAARG2 = 445,
     TSS_RESULTARG = 446,
     TSS_CONSTANT_DEPRECATED = 447,
     TSS_BUMPENVMAT00 = 448,
     TSS_BUMPENVMAT01 = 449,
     TSS_BUMPENVMAT10 = 450,
     TSS_BUMPENVMAT11 = 451,
     TSS_BUMPENVLSCALE = 452,
     TSS_BUMPENVLOFFSET = 453,
     TSS_TEXCOORDINDEX = 454,
     TSS_TEXTURETRANSFORMFLAGS = 455,
     TSS_TEXTRANSMATRIX = 456,
     TTFF_DISABLE = 457,
     TTFF_COUNT1 = 458,
     TTFF_COUNT2 = 459,
     TTFF_COUNT3 = 460,
     TTFF_COUNT4 = 461,
     TTFF_PROJECTED = 462,
     PROJECTED = 463,
     USEMAPINDEX = 464,
     INVERSE = 465,
     TRANSPOSE = 466,
     TTSRC_GLOBAL = 467,
     TTSRC_CONSTANT = 468,
     TT_WORLD_PARALLEL = 469,
     TT_WORLD_PERSPECTIVE = 470,
     TT_WORLD_SPHERE_MAP = 471,
     TT_CAMERA_SPHERE_MAP = 472,
     TT_SPECULAR_CUBE_MAP = 473,
     TT_DIFFUSE_CUBE_MAP = 474,
     TCI_PASSTHRU = 475,
     TCI_CAMERASPACENORMAL = 476,
     TCI_CAMERASPACEPOSITION = 477,
     TCI_CAMERASPACEREFLECT = 478,
     TCI_SPHEREMAP = 479,
     TOP_DISABLE = 480,
     TOP_SELECTARG1 = 481,
     TOP_SELECTARG2 = 482,
     TOP_MODULATE = 483,
     TOP_MODULATE2X = 484,
     TOP_MODULATE4X = 485,
     TOP_ADD = 486,
     TOP_ADDSIGNED = 487,
     TOP_ADDSIGNED2X = 488,
     TOP_SUBTRACT = 489,
     TOP_ADDSMOOTH = 490,
     TOP_BLENDDIFFUSEALPHA = 491,
     TOP_BLENDTEXTUREALPHA = 492,
     TOP_BLENDFACTORALPHA = 493,
     TOP_BLENDTEXTUREALPHAPM = 494,
     TOP_BLENDCURRENTALPHA = 495,
     TOP_PREMODULATE = 496,
     TOP_MODULATEALPHA_ADDCOLOR = 497,
     TOP_MODULATECOLOR_ADDALPHA = 498,
     TOP_MODULATEINVALPHA_ADDCOLOR = 499,
     TOP_MODULATEINVCOLOR_ADDALPHA = 500,
     TOP_BUMPENVMAP = 501,
     TOP_BUMPENVMAPLUMINANCE = 502,
     TOP_DOTPRODUCT3 = 503,
     TOP_MULTIPLYADD = 504,
     TOP_LERP = 505,
     TA_CURRENT = 506,
     TA_DIFFUSE = 507,
     TA_SELECTMASK = 508,
     TA_SPECULAR = 509,
     TA_TEMP = 510,
     TA_TEXTURE = 511,
     TA_TFACTOR = 512,
     TA_ALPHAREPLICATE = 513,
     TA_COMPLEMENT = 514,
     NTM_BASE = 515,
     NTM_DARK = 516,
     NTM_DETAIL = 517,
     NTM_GLOSS = 518,
     NTM_GLOW = 519,
     NTM_BUMP = 520,
     NTM_NORMAL = 521,
     NTM_PARALLAX = 522,
     NTM_DECAL = 523,
     SAMPLER = 524,
     TSAMP_ADDRESSU = 525,
     TSAMP_ADDRESSV = 526,
     TSAMP_ADDRESSW = 527,
     TSAMP_BORDERCOLOR = 528,
     TSAMP_MAGFILTER = 529,
     TSAMP_MINFILTER = 530,
     TSAMP_MIPFILTER = 531,
     TSAMP_MIPMAPLODBIAS = 532,
     TSAMP_MAXMIPLEVEL = 533,
     TSAMP_MAXANISOTROPY = 534,
     TSAMP_SRGBTEXTURE = 535,
     TSAMP_ELEMENTINDEX = 536,
     TSAMP_DMAPOFFSET = 537,
     TSAMP_ALPHAKILL_DEPRECATED = 538,
     TSAMP_COLORKEYOP_DEPRECATED = 539,
     TSAMP_COLORSIGN_DEPRECATED = 540,
     TSAMP_COLORKEYCOLOR_DEPRECATED = 541,
     TADDR_WRAP = 542,
     TADDR_MIRROR = 543,
     TADDR_CLAMP = 544,
     TADDR_BORDER = 545,
     TADDR_MIRRORONCE = 546,
     TADDR_CLAMPTOEDGE_DEPRECATED = 547,
     TEXF_NONE = 548,
     TEXF_POINT = 549,
     TEXF_LINEAR = 550,
     TEXF_ANISOTROPIC = 551,
     TEXF_PYRAMIDALQUAD = 552,
     TEXF_GAUSSIANQUAD = 553,
     TEXF_FLATCUBIC_DEPRECATED = 554,
     TEXF_GAUSSIANCUBIC_DEPRECATED = 555,
     TEXF_QUINCUNX_DEPRECATED = 556,
     TEXF_MAX_DEPRECATED = 557,
     TAK_DISABLE_DEPRECATED = 558,
     TAK_ENABLE_DEPRECATED = 559,
     TCKOP_DISABLE_DEPRECATED = 560,
     TCKOP_ALPHA_DEPRECATED = 561,
     TCKOP_RGBA_DEPRECATED = 562,
     TCKOP_KILL_DEPRECATED = 563,
     TOKEN_TEXTURE = 564,
     TEXTURE_SOURCE = 565,
     OBJECTS = 566,
     EFFECT_GENERALLIGHT = 567,
     EFFECT_POINTLIGHT = 568,
     EFFECT_DIRECTIONALLIGHT = 569,
     EFFECT_SPOTLIGHT = 570,
     EFFECT_SHADOWPOINTLIGHT = 571,
     EFFECT_SHADOWDIRECTIONALLIGHT = 572,
     EFFECT_SHADOWSPOTLIGHT = 573,
     EFFECT = 574,
     EFFECT_ENVIRONMENTMAP = 575,
     EFFECT_PROJECTEDSHADOWMAP = 576,
     EFFECT_PROJECTEDLIGHTMAP = 577,
     EFFECT_FOGMAP = 578,
     USEMAPVALUE = 579,
     CMOBJECT = 580
   };
#endif
/* Tokens.  */
#define EOLN 258
#define PATH 259
#define L_ACCOLADE 260
#define R_ACCOLADE 261
#define L_PARENTHESE 262
#define R_PARENTHESE 263
#define L_BRACKET 264
#define R_BRACKET 265
#define L_ANGLEBRACKET 266
#define R_ANGLEBRACKET 267
#define OR 268
#define ASSIGN 269
#define COMMA 270
#define NSF_AT_SYMBOL 271
#define NSF_COLON 272
#define NSF_SEMICOLON 273
#define UNDERSCORE 274
#define ASTERIK 275
#define FORWARDSLASH 276
#define PLUS 277
#define MINUS 278
#define N_HEX 279
#define N_FLOAT 280
#define N_INT 281
#define N_STRING 282
#define N_QUOTE 283
#define N_BOOL 284
#define N_VERSION 285
#define NSFSHADER 286
#define ARTIST 287
#define HIDDEN 288
#define SAVE 289
#define ATTRIBUTES 290
#define GLOBALATTRIBUTES 291
#define ATTRIB 292
#define ATTRIB_BOOL 293
#define ATTRIB_STRING 294
#define ATTRIB_UINT 295
#define ATTRIB_FLOAT 296
#define ATTRIB_POINT2 297
#define ATTRIB_POINT3 298
#define ATTRIB_POINT4 299
#define ATTRIB_MATRIX3 300
#define ATTRIB_TRANSFORM 301
#define ATTRIB_COLOR 302
#define ATTRIB_TEXTURE 303
#define PACKINGDEF 304
#define PD_STREAM 305
#define PD_FIXEDFUNCTION 306
#define SEMANTICADAPTERTABLE 307
#define PDP_POSITION 308
#define PDP_POSITION0 309
#define PDP_POSITION1 310
#define PDP_POSITION2 311
#define PDP_POSITION3 312
#define PDP_POSITION4 313
#define PDP_POSITION5 314
#define PDP_POSITION6 315
#define PDP_POSITION7 316
#define PDP_BLENDWEIGHTS 317
#define PDP_BLENDINDICES 318
#define PDP_NORMAL 319
#define PDP_POINTSIZE 320
#define PDP_COLOR 321
#define PDP_COLOR2 322
#define PDP_TEXCOORD0 323
#define PDP_TEXCOORD1 324
#define PDP_TEXCOORD2 325
#define PDP_TEXCOORD3 326
#define PDP_TEXCOORD4 327
#define PDP_TEXCOORD5 328
#define PDP_TEXCOORD6 329
#define PDP_TEXCOORD7 330
#define PDP_NORMAL2 331
#define PDP_TANGENT 332
#define PDP_BINORMAL 333
#define PDP_EXTRADATA 334
#define PDT_FLOAT1 335
#define PDT_FLOAT2 336
#define PDT_FLOAT3 337
#define PDT_FLOAT4 338
#define PDT_UBYTECOLOR 339
#define PDT_SHORT1 340
#define PDT_SHORT2 341
#define PDT_SHORT3 342
#define PDT_SHORT4 343
#define PDT_UBYTE4 344
#define PDT_NORMSHORT1 345
#define PDT_NORMSHORT2 346
#define PDT_NORMSHORT3 347
#define PDT_NORMSHORT4 348
#define PDT_NORMPACKED3 349
#define PDT_PBYTE1 350
#define PDT_PBYTE2 351
#define PDT_PBYTE3 352
#define PDT_PBYTE4 353
#define PDT_FLOAT2H 354
#define PDT_NORMUBYTE4 355
#define PDT_NORMUSHORT2 356
#define PDT_NORMUSHORT4 357
#define PDT_UDEC3 358
#define PDT_NORMDEC3 359
#define PDT_FLOAT16_2 360
#define PDT_FLOAT16_4 361
#define PDTESS_DEFAULT 362
#define PDTESS_PARTIALU 363
#define PDTESS_PARTIALV 364
#define PDTESS_CROSSUV 365
#define PDTESS_UV 366
#define PDTESS_LOOKUP 367
#define PDTESS_LOOKUPPRESAMPLED 368
#define PDU_POSITION 369
#define PDU_BLENDWEIGHT 370
#define PDU_BLENDINDICES 371
#define PDU_NORMAL 372
#define PDU_PSIZE 373
#define PDU_TEXCOORD 374
#define PDU_TANGENT 375
#define PDU_BINORMAL 376
#define PDU_TESSFACTOR 377
#define PDU_POSITIONT 378
#define PDU_COLOR 379
#define PDU_FOG 380
#define PDU_DEPTH 381
#define PDU_SAMPLE 382
#define RENDERSTATES 383
#define CMDEFINED 384
#define CMATTRIBUTE 385
#define CMCONSTANT 386
#define CMGLOBAL 387
#define CMOPERATOR 388
#define VSCONSTANTMAP 389
#define VSPROGRAM 390
#define GSCONSTANTMAP 391
#define GSPROGRAM 392
#define PSCONSTANTMAP 393
#define PSPROGRAM 394
#define CONSTANTMAP 395
#define PROGRAM 396
#define ENTRYPOINT 397
#define SHADERTARGET 398
#define SOFTWAREVP 399
#define THREADGROUPCOUNTS 400
#define SHADERPROGRAM 401
#define SHADERTYPE 402
#define SKINBONEMATRIX3 403
#define REQUIREMENTS 404
#define FEATURELEVEL 405
#define VSVERSION 406
#define GSVERSION 407
#define PSVERSION 408
#define CSVERSION 409
#define USERVERSION 410
#define PLATFORM 411
#define BONESPERPARTITION 412
#define BINORMALTANGENTMETHOD 413
#define BINORMALTANGENTUVSOURCE 414
#define NBTMETHOD_NONE 415
#define NBTMETHOD_NI 416
#define NBTMETHOD_MAX 417
#define NBTMETHOD_ATI 418
#define USERDEFINEDDATA 419
#define IMPLEMENTATION 420
#define OUTPUTSTREAM 421
#define STREAMOUTPUT 422
#define STREAMOUTTARGETS 423
#define STREAMOUTAPPEND 424
#define MAXVERTEXCOUNT 425
#define OUTPUTPRIMTYPE 426
#define _POINT 427
#define _LINE 428
#define _TRIANGLE 429
#define VERTEXFORMAT 430
#define FMT_FLOAT 431
#define FMT_INT 432
#define FMT_UINT 433
#define CLASSNAME 434
#define PASS 435
#define STAGE 436
#define TSS_TEXTURE 437
#define TSS_COLOROP 438
#define TSS_COLORARG0 439
#define TSS_COLORARG1 440
#define TSS_COLORARG2 441
#define TSS_ALPHAOP 442
#define TSS_ALPHAARG0 443
#define TSS_ALPHAARG1 444
#define TSS_ALPHAARG2 445
#define TSS_RESULTARG 446
#define TSS_CONSTANT_DEPRECATED 447
#define TSS_BUMPENVMAT00 448
#define TSS_BUMPENVMAT01 449
#define TSS_BUMPENVMAT10 450
#define TSS_BUMPENVMAT11 451
#define TSS_BUMPENVLSCALE 452
#define TSS_BUMPENVLOFFSET 453
#define TSS_TEXCOORDINDEX 454
#define TSS_TEXTURETRANSFORMFLAGS 455
#define TSS_TEXTRANSMATRIX 456
#define TTFF_DISABLE 457
#define TTFF_COUNT1 458
#define TTFF_COUNT2 459
#define TTFF_COUNT3 460
#define TTFF_COUNT4 461
#define TTFF_PROJECTED 462
#define PROJECTED 463
#define USEMAPINDEX 464
#define INVERSE 465
#define TRANSPOSE 466
#define TTSRC_GLOBAL 467
#define TTSRC_CONSTANT 468
#define TT_WORLD_PARALLEL 469
#define TT_WORLD_PERSPECTIVE 470
#define TT_WORLD_SPHERE_MAP 471
#define TT_CAMERA_SPHERE_MAP 472
#define TT_SPECULAR_CUBE_MAP 473
#define TT_DIFFUSE_CUBE_MAP 474
#define TCI_PASSTHRU 475
#define TCI_CAMERASPACENORMAL 476
#define TCI_CAMERASPACEPOSITION 477
#define TCI_CAMERASPACEREFLECT 478
#define TCI_SPHEREMAP 479
#define TOP_DISABLE 480
#define TOP_SELECTARG1 481
#define TOP_SELECTARG2 482
#define TOP_MODULATE 483
#define TOP_MODULATE2X 484
#define TOP_MODULATE4X 485
#define TOP_ADD 486
#define TOP_ADDSIGNED 487
#define TOP_ADDSIGNED2X 488
#define TOP_SUBTRACT 489
#define TOP_ADDSMOOTH 490
#define TOP_BLENDDIFFUSEALPHA 491
#define TOP_BLENDTEXTUREALPHA 492
#define TOP_BLENDFACTORALPHA 493
#define TOP_BLENDTEXTUREALPHAPM 494
#define TOP_BLENDCURRENTALPHA 495
#define TOP_PREMODULATE 496
#define TOP_MODULATEALPHA_ADDCOLOR 497
#define TOP_MODULATECOLOR_ADDALPHA 498
#define TOP_MODULATEINVALPHA_ADDCOLOR 499
#define TOP_MODULATEINVCOLOR_ADDALPHA 500
#define TOP_BUMPENVMAP 501
#define TOP_BUMPENVMAPLUMINANCE 502
#define TOP_DOTPRODUCT3 503
#define TOP_MULTIPLYADD 504
#define TOP_LERP 505
#define TA_CURRENT 506
#define TA_DIFFUSE 507
#define TA_SELECTMASK 508
#define TA_SPECULAR 509
#define TA_TEMP 510
#define TA_TEXTURE 511
#define TA_TFACTOR 512
#define TA_ALPHAREPLICATE 513
#define TA_COMPLEMENT 514
#define NTM_BASE 515
#define NTM_DARK 516
#define NTM_DETAIL 517
#define NTM_GLOSS 518
#define NTM_GLOW 519
#define NTM_BUMP 520
#define NTM_NORMAL 521
#define NTM_PARALLAX 522
#define NTM_DECAL 523
#define SAMPLER 524
#define TSAMP_ADDRESSU 525
#define TSAMP_ADDRESSV 526
#define TSAMP_ADDRESSW 527
#define TSAMP_BORDERCOLOR 528
#define TSAMP_MAGFILTER 529
#define TSAMP_MINFILTER 530
#define TSAMP_MIPFILTER 531
#define TSAMP_MIPMAPLODBIAS 532
#define TSAMP_MAXMIPLEVEL 533
#define TSAMP_MAXANISOTROPY 534
#define TSAMP_SRGBTEXTURE 535
#define TSAMP_ELEMENTINDEX 536
#define TSAMP_DMAPOFFSET 537
#define TSAMP_ALPHAKILL_DEPRECATED 538
#define TSAMP_COLORKEYOP_DEPRECATED 539
#define TSAMP_COLORSIGN_DEPRECATED 540
#define TSAMP_COLORKEYCOLOR_DEPRECATED 541
#define TADDR_WRAP 542
#define TADDR_MIRROR 543
#define TADDR_CLAMP 544
#define TADDR_BORDER 545
#define TADDR_MIRRORONCE 546
#define TADDR_CLAMPTOEDGE_DEPRECATED 547
#define TEXF_NONE 548
#define TEXF_POINT 549
#define TEXF_LINEAR 550
#define TEXF_ANISOTROPIC 551
#define TEXF_PYRAMIDALQUAD 552
#define TEXF_GAUSSIANQUAD 553
#define TEXF_FLATCUBIC_DEPRECATED 554
#define TEXF_GAUSSIANCUBIC_DEPRECATED 555
#define TEXF_QUINCUNX_DEPRECATED 556
#define TEXF_MAX_DEPRECATED 557
#define TAK_DISABLE_DEPRECATED 558
#define TAK_ENABLE_DEPRECATED 559
#define TCKOP_DISABLE_DEPRECATED 560
#define TCKOP_ALPHA_DEPRECATED 561
#define TCKOP_RGBA_DEPRECATED 562
#define TCKOP_KILL_DEPRECATED 563
#define TOKEN_TEXTURE 564
#define TEXTURE_SOURCE 565
#define OBJECTS 566
#define EFFECT_GENERALLIGHT 567
#define EFFECT_POINTLIGHT 568
#define EFFECT_DIRECTIONALLIGHT 569
#define EFFECT_SPOTLIGHT 570
#define EFFECT_SHADOWPOINTLIGHT 571
#define EFFECT_SHADOWDIRECTIONALLIGHT 572
#define EFFECT_SHADOWSPOTLIGHT 573
#define EFFECT 574
#define EFFECT_ENVIRONMENTMAP 575
#define EFFECT_PROJECTEDSHADOWMAP 576
#define EFFECT_PROJECTEDLIGHTMAP 577
#define EFFECT_FOGMAP 578
#define USEMAPVALUE 579
#define CMOBJECT 580




/* Copy the first part of user declarations.  */



// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
// 
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
// 
// Copyright (c) 1996-2008 Emergent Game Technologies.
// All Rights Reserved.
// 
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Turn off warning in automatically generated grammar:
//  warning C4065: switch statement contains 'default' but no 'case' labels
#if defined(WIN32) || defined(_XENON)
#pragma warning( disable : 4065 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4245 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4706 )
#pragma warning( disable : 4702 )
#endif

#if defined(_XENON)
    #include <xtl.h>
    #include <malloc.h>
#elif defined (WIN32)
    #include <NiSystem.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include <malloc.h>
#elif defined (_PS3)
    #include <ctype.h>    
#endif  //#if defined(_XENON)

    #include "NSFTextFile.h"
    #include "NSFParsedShader.h"

    #include <NiTArray.h>
    #include <NiShaderFactory.h>

    #include <NiTextureStage.h>
    #include <NiSemanticAdapterTable.h>
    
    #include <NSBStateGroup.h>
    #include <NSBRenderStates.h>
    #include <NSBStageAndSamplerStates.h>

    #include <NSBShader.h>
    #include <NSBAttributeDesc.h>
    #include <NSBAttributeTable.h>
    #include <NSBObjectTable.h>
    #include <NSBPackingDef.h>
    #include <NSBImplementation.h>
    #include <NSBStateGroup.h>
    #include <NSBConstantMap.h>
    #include <NSBPass.h>
    #include <NSBTextureStage.h>
    #include <NSBTexture.h>
    #include <NiOutputStreamDescriptor.h>
    #include <NiStreamOutSettings.h>
    
    unsigned int F2DW(float fValue)
    {
        union FloatIntRep
        {
            float f;
            unsigned int ui;
        } kValue;
        
        kValue.f = fValue;
        
        return kValue.ui;
    }
    
    #include <NSBUserDefinedDataSet.h>
    
    
    void NSFParsererror2(const char *s);
    void NSFParsererror(const char *s);
    int  yylex    (void);

    NSFTextFile* g_pkFile = 0;
    
    extern int NSFParserGetLineNumber();

//    #define _ENABLE_DEBUG_STRING_OUT_
    char g_acDSO[1024];
    bool g_bFirstDSOFileAccess = true;
    int g_iDSOIndent = 0;
    FILE* g_pfDSOFile = 0;
    void DebugStringOut(const char* pcOut, bool bIndent = true);

    #define YYDEBUG                 1
    #define NSFParsererror_VERBOSE  1
    #define YYERROR_VERBOSE         1
    #define YYMALLOC NiExternalMalloc
    #define YYFREE NiExternalFree

    #define ERR_INVALID_ENUM    "Invalid enumerant"
    #define ERR_INVALID_COMMAND "Invalid command in block"

    // Gamebryo
        
    #define FLOAT_ARRAY_SIZE        64
    #define MAX_QUOTE_LENGTH    6 * 1024

    NiTPrimitiveArray<float>* g_afValues;
    
    unsigned int ResetFloatValueArray(void);
    unsigned int AddFloatToValueArray(float fValue);
    
    void ResetFloatRangeArrays(void);
    unsigned int AddFloatToLowArray(float fValue);
    unsigned int AddFloatToHighArray(float fValue);
    
    // Ranges
    bool g_bRanged;
    unsigned int g_uiLow, g_uiHigh;
    unsigned int g_uiLowFloatValues;
    float g_afLowValues[FLOAT_ARRAY_SIZE];
    unsigned int g_uiHighFloatValues;
    float g_afHighValues[FLOAT_ARRAY_SIZE];
    
    unsigned int g_uiCurrentPlatforms;
    bool g_bConstantMapPlatformBlock = false;
    
    void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
        unsigned int uiIndex, const char* pcName, const char* pcDebugString);
    unsigned int DecodeAttribTypeString(char* pcAttribType);
    unsigned int DecodePlatformString(char* pcPlatform);
    NiGPUProgram::ProgramType DecodeShaderTypeString(char* pcShaderType);
    unsigned int DecodeFeatureLevelString(char* pcShaderType);
    bool AddAttributeToConstantMap(char* pcName, 
        unsigned int uiRegisterStart, unsigned int uiRegisterCount,
        unsigned int uiExtraNumber, bool bIsGlobal);
    bool SetupOperatorEntry(char* pcName, int iRegStart, int iRegCount, 
        char* pcEntry1, int iOperation, char* pcEntry2, bool bInverse, 
        bool bTranspose);
    NiShaderAttributeDesc::AttributeType DetermineOperatorResult(
        int iOperation, NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultDivide(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultAdd(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);
    NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
        NiShaderAttributeDesc::AttributeType eType1, 
        NiShaderAttributeDesc::AttributeType eType2);

    NSBConstantMap* ObtainConstantMap(NiGPUProgram::ProgramType eProgramType);
    void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
        unsigned int uiPlatforms, NiGPUProgram::ProgramType eType);
    void SetShaderProgramEntryPoint(NSBPass* pkPass,
        const char* pcEntryPoint,unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);
    void SetShaderProgramShaderTarget(NSBPass* pkPass,
        const char* pcShaderTarget, unsigned int uiPlatforms,
        NiGPUProgram::ProgramType eType);

    NiTPointerList<NSFParsedShader*> g_kParsedShaderList;
    NSFParsedShader* g_pkCurrShader = 0;

    // Binary Shader
    NSBShader* g_pkCurrNSBShader = 0;

    // Attribute Table
    bool g_bGlobalAttributes = false;
    NSBAttributeTable* g_pkCurrAttribTable = 0;

    // Object Table
    NSBObjectTable* g_pkCurrObjectTable = 0;

    // Packing Definition
    unsigned int g_uiCurrPDStream = 0;    
    bool g_bCurrPDFixedFunction = false;
    NSBPackingDef* g_pkCurrPackingDef = 0;

    // Requirements
    NSBRequirements* g_pkCurrRequirements = 0;

    // Implementation
    unsigned int g_uiCurrImplementation = 0;
    NSBImplementation* g_pkCurrImplementation = 0;

    // OutputStream
    NiOutputStreamDescriptor* g_pkCurrentOutputStreamDescriptor = 0;
    NiOutputStreamDescriptor::DataType g_eDataType = 
        NiOutputStreamDescriptor::DATATYPE_MAX;

    // RenderState Group
    NSBStateGroup* g_pkCurrRSGroup = 0;

    // ConstantMap
    unsigned int g_auiCurrImplemConstantMap[NSBConstantMap::NSB_SHADER_TYPE_COUNT];
    unsigned int g_auiCurrPassConstantMap[NSBConstantMap::NSB_SHADER_TYPE_COUNT];
    NSBConstantMap* g_pkCurrConstantMap = 0;

    // Pass
    unsigned int g_uiCurrPassIndex = 0;
    NSBPass* g_pkCurrPass = 0;

    // TextureStage
    NSBTextureStage* g_pkCurrTextureStage = 0;
    unsigned int g_uiCurrTextureSlot = 0;
    NSBTexture* g_pkCurrTexture = 0;
    
    // ShaderProgram
    NiGPUProgram::ProgramType g_eCurrentShaderType = 
        (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT;
    
    bool g_bCurrStateValid = false;
    unsigned int g_uiCurrStateState = 0;
    unsigned int g_uiCurrStateValue = 0;
    bool g_bUseMapValue = false;

    NSBUserDefinedDataSet* g_pkCurrUDDataSet = 0;
    NSBUserDefinedDataBlock* g_pkCurrUDDataBlock = 0;
    


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
    float fval;
    int   ival;
    char* sval;
    unsigned int uival;
    unsigned long  dword;
    unsigned short word;
    unsigned char  byte;
    bool bval;
    unsigned int vers;
} YYSTYPE;
/* Line 196 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1211

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  328
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  272
/* YYNRULES -- Number of rules. */
#define YYNRULES  667
/* YYNRULES -- Number of states. */
#define YYNSTATES  1159

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   580

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned short int yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     326,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   327,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    10,    19,    22,    23,
      25,    28,    30,    32,    34,    36,    38,    40,    42,    44,
      45,    47,    49,    52,    53,    55,    56,    58,    60,    62,
      64,    68,    69,    90,   107,   108,   129,   130,   147,   148,
     161,   162,   167,   168,   173,   178,   180,   184,   186,   218,
     219,   225,   226,   232,   233,   237,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   260,   262,   264,   266,
     271,   272,   279,   280,   288,   289,   297,   298,   306,   307,
     315,   316,   323,   324,   331,   332,   339,   345,   350,   351,
     357,   360,   362,   364,   366,   368,   370,   372,   374,   376,
     378,   380,   382,   384,   388,   392,   396,   400,   404,   408,
     412,   416,   420,   424,   428,   429,   436,   439,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   478,   480,   482,
     484,   486,   488,   490,   492,   494,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   517,   519,   521,   523,
     525,   527,   529,   531,   533,   535,   537,   539,   541,   543,
     545,   547,   549,   551,   553,   555,   557,   559,   561,   563,
     565,   567,   569,   571,   573,   575,   577,   579,   581,   583,
     585,   587,   589,   591,   594,   596,   599,   602,   605,   613,
     618,   625,   629,   630,   636,   639,   641,   652,   653,   659,
     662,   664,   666,   668,   671,   673,   675,   677,   679,   681,
     684,   688,   692,   696,   700,   704,   708,   711,   713,   715,
     717,   719,   721,   723,   725,   727,   728,   729,   738,   744,
     750,   755,   760,   765,   770,   771,   778,   779,   786,   787,
     793,   803,   805,   807,   809,   811,   814,   815,   817,   818,
     820,   824,   825,   833,   834,   841,   842,   849,   850,   857,
     861,   865,   869,   873,   875,   878,   880,   884,   888,   892,
     896,   901,   907,   908,   909,   920,   922,   924,   926,   928,
     930,   932,   934,   937,   940,   943,   947,   950,   952,   953,
     954,   964,   967,   970,   973,   977,   980,   982,   983,   984,
     994,   997,  1000,  1003,  1007,  1010,  1012,  1013,  1014,  1024,
    1025,  1031,  1034,  1036,  1038,  1040,  1042,  1044,  1046,  1048,
    1050,  1052,  1054,  1056,  1058,  1061,  1065,  1069,  1073,  1077,
    1081,  1085,  1089,  1093,  1095,  1097,  1101,  1105,  1109,  1113,
    1115,  1117,  1119,  1121,  1124,  1126,  1128,  1130,  1131,  1139,
    1140,  1142,  1145,  1147,  1149,  1151,  1153,  1155,  1157,  1160,
    1162,  1164,  1166,  1168,  1170,  1172,  1174,  1176,  1178,  1180,
    1182,  1184,  1186,  1188,  1190,  1192,  1194,  1196,  1199,  1203,
    1207,  1211,  1215,  1217,  1219,  1221,  1223,  1225,  1227,  1229,
    1231,  1234,  1238,  1242,  1246,  1250,  1254,  1258,  1262,  1266,
    1270,  1274,  1278,  1282,  1286,  1290,  1294,  1298,  1302,  1306,
    1310,  1314,  1318,  1322,  1327,  1332,  1337,  1341,  1345,  1347,
    1349,  1351,  1353,  1354,  1356,  1358,  1360,  1362,  1364,  1366,
    1368,  1370,  1372,  1374,  1376,  1378,  1380,  1382,  1384,  1386,
    1388,  1390,  1392,  1394,  1396,  1398,  1400,  1402,  1404,  1406,
    1408,  1411,  1414,  1417,  1420,  1423,  1426,  1429,  1432,  1433,
    1435,  1438,  1441,  1443,  1445,  1446,  1448,  1450,  1452,  1454,
    1456,  1459,  1463,  1467,  1468,  1473,  1474,  1476,  1478,  1480,
    1482,  1484,  1486,  1489,  1490,  1498,  1499,  1506,  1507,  1509,
    1512,  1514,  1516,  1518,  1520,  1523,  1525,  1527,  1529,  1531,
    1533,  1535,  1537,  1539,  1541,  1543,  1545,  1547,  1549,  1551,
    1553,  1555,  1557,  1560,  1564,  1568,  1572,  1576,  1580,  1584,
    1588,  1592,  1596,  1600,  1604,  1608,  1612,  1616,  1620,  1624,
    1628,  1632,  1636,  1640,  1644,  1648,  1652,  1656,  1660,  1664,
    1668,  1670,  1672,  1674,  1676,  1678,  1680,  1682,  1685,  1687,
    1689,  1691,  1693,  1695,  1697,  1699,  1701,  1703,  1705,  1707,
    1710,  1712,  1714,  1717,  1719,  1721,  1723,  1725,  1728,  1729,
    1737,  1738,  1745,  1746,  1748,  1751,  1753,  1755,  1758,  1762,
    1766,  1770,  1774,  1777,  1779,  1781,  1783,  1785,  1787,  1789,
    1791,  1793,  1795,  1797,  1798,  1805,  1806,  1808,  1811,  1813,
    1815,  1817,  1819,  1821,  1823,  1825,  1827,  1829,  1831,  1833,
    1835,  1836,  1837,  1846,  1847,  1853,  1856,  1858,  1860,  1862,
    1866,  1871,  1874,  1876,  1878,  1879,  1886,  1889,  1891,  1893,
    1895,  1897,  1901,  1902,  1908,  1911,  1913,  1920,  1922,  1924,
    1926,  1928,  1930,  1932,  1936,  1938,  1940,  1942,  1944,  1946,
    1948,  1952,  1956,  1960,  1964,  1966,  1969,  1970
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     329,     0,    -1,   329,   330,    -1,   330,    -1,    -1,    -1,
      31,    27,     5,   331,   338,   332,   333,     6,    -1,     1,
     326,    -1,    -1,   334,    -1,   334,   335,    -1,   335,    -1,
     351,    -1,   377,    -1,   353,    -1,   392,    -1,   567,    -1,
     578,    -1,   598,    -1,    -1,    27,    -1,    28,    -1,   337,
      28,    -1,    -1,   337,    -1,    -1,    28,    -1,     4,    -1,
      27,    -1,    28,    -1,    11,    27,    12,    -1,    -1,     9,
       7,    25,    15,    25,    15,    25,    15,    25,     8,     7,
      25,    15,    25,    15,    25,    15,    25,     8,    10,    -1,
       9,     7,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,     8,    10,    -1,    -1,     9,     7,
      25,    15,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,    15,    25,     8,    10,    -1,    -1,
       9,     7,    25,    15,    25,    15,    25,     8,     7,    25,
      15,    25,    15,    25,     8,    10,    -1,    -1,     9,     7,
      25,    15,    25,     8,     7,    25,    15,    25,     8,    10,
      -1,    -1,     9,    25,    25,    10,    -1,    -1,     9,    26,
      26,    10,    -1,     9,    25,    25,    10,    -1,   349,    -1,
     349,    15,   348,    -1,    25,    -1,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    -1,    -1,    35,
       5,   352,   356,     6,    -1,    -1,    36,     5,   354,   356,
       6,    -1,    -1,     9,    26,    10,    -1,   356,   357,    -1,
     357,    -1,   359,    -1,   360,    -1,   362,    -1,   364,    -1,
     366,    -1,   368,    -1,   370,    -1,   372,    -1,   374,    -1,
     376,    -1,    32,    -1,    33,    -1,    38,    27,   358,    29,
      -1,    -1,    40,   361,    27,   358,    26,   347,    -1,    -1,
      41,   363,    27,   355,   358,   348,   346,    -1,    -1,    42,
     365,    27,   355,   358,   348,   345,    -1,    -1,    43,   367,
      27,   355,   358,   348,   344,    -1,    -1,    44,   369,    27,
     355,   358,   348,   343,    -1,    -1,    45,   371,    27,   355,
     358,   348,    -1,    -1,    46,   373,    27,   355,   358,   348,
      -1,    -1,    47,   375,    27,   358,   348,   342,    -1,    48,
      27,   358,    26,   339,    -1,    48,    27,   358,   339,    -1,
      -1,   311,     5,   378,   379,     6,    -1,   379,   380,    -1,
     380,    -1,   381,    -1,   382,    -1,   383,    -1,   384,    -1,
     385,    -1,   386,    -1,   387,    -1,   388,    -1,   389,    -1,
     390,    -1,   391,    -1,   312,    26,    27,    -1,   313,    26,
      27,    -1,   314,    26,    27,    -1,   315,    26,    27,    -1,
     316,    26,    27,    -1,   317,    26,    27,    -1,   318,    26,
      27,    -1,   320,    26,    27,    -1,   321,    26,    27,    -1,
     322,    26,    27,    -1,   323,    26,    27,    -1,    -1,    49,
      27,     5,   393,   399,     6,    -1,    49,    27,    -1,    -1,
      80,    -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,
      85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,
      90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,    -1,
      95,    -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,
     100,    -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,
     105,    -1,   106,    -1,     1,   326,    -1,    53,    -1,    54,
      -1,    55,    -1,    56,    -1,    57,    -1,    58,    -1,    59,
      -1,    60,    -1,    61,    -1,    62,    -1,    63,    -1,    64,
      -1,    65,    -1,    66,    -1,    67,    -1,    68,    -1,    69,
      -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,
      -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,   107,
      -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,   112,
      -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,   117,
      -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,   122,
      -1,   123,    -1,   124,    -1,   125,    -1,   126,    -1,   127,
      -1,   399,   400,    -1,   400,    -1,     1,   326,    -1,    50,
      26,    -1,    51,    29,    -1,    79,    26,    26,   395,   397,
     398,    26,    -1,    79,    26,    26,   395,    -1,   396,    26,
     395,   397,   398,    26,    -1,   396,    26,   395,    -1,    -1,
      52,     5,   402,   403,     6,    -1,   403,   404,    -1,   404,
      -1,    26,   340,    17,    26,   340,    17,    26,    16,    26,
      18,    -1,    -1,   128,     5,   406,   407,     6,    -1,   407,
     408,    -1,   408,    -1,   409,    -1,   410,    -1,   410,    34,
      -1,   413,    -1,   415,    -1,   414,    -1,   411,    -1,   412,
      -1,     1,   326,    -1,    27,    14,   341,    -1,    27,    14,
      27,    -1,    27,    14,    29,    -1,    27,    14,    25,    -1,
      27,    14,    24,    -1,    27,    14,    26,    -1,   416,   417,
      -1,   417,    -1,   421,    -1,   422,    -1,   423,    -1,   425,
      -1,   427,    -1,   429,    -1,   418,    -1,    -1,    -1,     5,
     419,   156,    14,   476,   420,   416,     6,    -1,   129,   148,
      26,    26,    26,    -1,   129,   148,   340,    26,    26,    -1,
     129,   340,    26,    26,    -1,   129,   340,   340,    26,    -1,
     325,    27,    27,    26,    -1,   325,    27,    27,    27,    -1,
      -1,   130,   424,   340,    26,    26,    26,    -1,    -1,   131,
     426,   340,    26,    26,   348,    -1,    -1,   132,   428,   340,
      26,    26,    -1,   133,    27,    26,    26,    27,   430,    27,
     432,   431,    -1,    20,    -1,    21,    -1,    22,    -1,    23,
      -1,     1,   326,    -1,    -1,   211,    -1,    -1,   210,    -1,
     147,    14,    27,    -1,    -1,   140,   336,     5,   433,   435,
     416,     6,    -1,    -1,   134,   336,     5,   437,   416,     6,
      -1,    -1,   136,   336,     5,   439,   416,     6,    -1,    -1,
     138,   336,     5,   441,   416,     6,    -1,   179,    14,   340,
      -1,   141,    14,     4,    -1,   141,    14,    27,    -1,   141,
      14,    28,    -1,   445,    -1,   445,   444,    -1,   443,    -1,
     142,    14,    27,    -1,   143,    14,    27,    -1,   144,    14,
      29,    -1,   145,    14,    26,    -1,   145,    14,    26,    26,
      -1,   145,    14,    26,    26,    26,    -1,    -1,    -1,   146,
       5,   433,   447,   156,    14,   476,   448,   444,     6,    -1,
     446,    -1,   452,    -1,   451,    -1,   457,    -1,   456,    -1,
     462,    -1,   461,    -1,   135,     4,    -1,   135,    27,    -1,
     135,    28,    -1,   450,    27,    27,    -1,   450,    27,    -1,
     450,    -1,    -1,    -1,   135,     5,   453,   156,    14,   476,
     454,   444,     6,    -1,   137,     4,    -1,   137,    27,    -1,
     137,    28,    -1,   455,    27,    27,    -1,   455,    27,    -1,
     455,    -1,    -1,    -1,   137,     5,   458,   156,    14,   476,
     459,   444,     6,    -1,   139,     4,    -1,   139,    27,    -1,
     139,    28,    -1,   460,    27,    27,    -1,   460,    27,    -1,
     460,    -1,    -1,    -1,   139,     5,   463,   156,    14,   476,
     464,   444,     6,    -1,    -1,   149,     5,   466,   467,     6,
      -1,   467,   468,    -1,   468,    -1,   469,    -1,   470,    -1,
     471,    -1,   472,    -1,   473,    -1,   474,    -1,   475,    -1,
     479,    -1,   481,    -1,   480,    -1,   478,    -1,     1,   326,
      -1,   151,    14,    30,    -1,   152,    14,    30,    -1,   153,
      14,    30,    -1,   154,    14,    30,    -1,   150,    14,    27,
      -1,   155,    14,    30,    -1,   156,    14,   476,    -1,   476,
      13,   477,    -1,   477,    -1,    27,    -1,    27,    14,    29,
      -1,   157,    14,    26,    -1,   159,    14,    26,    -1,   158,
      14,   482,    -1,   160,    -1,   161,    -1,   163,    -1,   162,
      -1,     1,   326,    -1,   484,    -1,   524,    -1,   553,    -1,
      -1,   181,    26,   336,     5,   485,   486,     6,    -1,    -1,
     487,    -1,   487,   488,    -1,   488,    -1,   492,    -1,   520,
      -1,   489,    -1,   490,    -1,   491,    -1,   491,    34,    -1,
     495,    -1,   496,    -1,   497,    -1,   498,    -1,   499,    -1,
     500,    -1,   501,    -1,   502,    -1,   503,    -1,   504,    -1,
     505,    -1,   506,    -1,   507,    -1,   508,    -1,   509,    -1,
     510,    -1,   511,    -1,   512,    -1,     1,   326,    -1,   182,
      14,   493,    -1,   182,    14,   494,    -1,   182,    14,   341,
      -1,   182,    14,    27,    -1,   260,    -1,   261,    -1,   262,
      -1,   263,    -1,   264,    -1,   265,    -1,   266,    -1,   267,
      -1,   268,    26,    -1,   183,    14,   515,    -1,   184,    14,
     516,    -1,   185,    14,   516,    -1,   186,    14,   516,    -1,
     187,    14,   515,    -1,   188,    14,   516,    -1,   189,    14,
     516,    -1,   190,    14,   516,    -1,   191,    14,   516,    -1,
     192,    14,    24,    -1,   193,    14,    25,    -1,   193,    14,
     341,    -1,   194,    14,    25,    -1,   194,    14,   341,    -1,
     195,    14,    25,    -1,   195,    14,   341,    -1,   196,    14,
      25,    -1,   196,    14,   341,    -1,   197,    14,    25,    -1,
     197,    14,   341,    -1,   198,    14,    25,    -1,   198,    14,
     341,    -1,   199,    14,   519,    26,    -1,   199,    14,   519,
     209,    -1,   200,    14,   513,   514,    -1,   200,    14,   207,
      -1,   200,    14,   202,    -1,   203,    -1,   204,    -1,   205,
      -1,   206,    -1,    -1,   207,    -1,   225,    -1,   226,    -1,
     227,    -1,   228,    -1,   229,    -1,   230,    -1,   231,    -1,
     232,    -1,   233,    -1,   234,    -1,   235,    -1,   236,    -1,
     237,    -1,   238,    -1,   239,    -1,   240,    -1,   241,    -1,
     242,    -1,   243,    -1,   244,    -1,   245,    -1,   246,    -1,
     247,    -1,   248,    -1,   249,    -1,   250,    -1,     1,   326,
      -1,   251,   517,    -1,   252,   517,    -1,   253,   517,    -1,
     254,   517,    -1,   255,   517,    -1,   256,   517,    -1,   257,
     517,    -1,    -1,   518,    -1,   258,   259,    -1,   259,   258,
      -1,   258,    -1,   259,    -1,    -1,   220,    -1,   221,    -1,
     222,    -1,   223,    -1,   224,    -1,     1,   326,    -1,   201,
      14,   521,    -1,   212,   523,    27,    -1,    -1,   213,   522,
     523,   350,    -1,    -1,   214,    -1,   215,    -1,   216,    -1,
     217,    -1,   218,    -1,   219,    -1,     1,   326,    -1,    -1,
     269,    26,   336,     5,   525,   527,     6,    -1,    -1,   269,
     336,     5,   526,   527,     6,    -1,    -1,   528,    -1,   528,
     529,    -1,   529,    -1,   530,    -1,   531,    -1,   492,    -1,
     531,    34,    -1,   532,    -1,   533,    -1,   534,    -1,   535,
      -1,   536,    -1,   537,    -1,   538,    -1,   539,    -1,   540,
      -1,   541,    -1,   542,    -1,   543,    -1,   544,    -1,   545,
      -1,   546,    -1,   547,    -1,   548,    -1,     1,   326,    -1,
     270,    14,   549,    -1,   271,    14,   549,    -1,   272,    14,
     549,    -1,   273,    14,    24,    -1,   273,    14,   341,    -1,
     274,    14,   550,    -1,   275,    14,   550,    -1,   276,    14,
     550,    -1,   277,    14,    26,    -1,   277,    14,   341,    -1,
     278,    14,    26,    -1,   278,    14,   341,    -1,   279,    14,
      26,    -1,   279,    14,   341,    -1,   280,    14,    26,    -1,
     280,    14,   341,    -1,   281,    14,    26,    -1,   281,    14,
     341,    -1,   282,    14,    26,    -1,   282,    14,   341,    -1,
     283,    14,   551,    -1,   283,    14,   341,    -1,   284,    14,
     552,    -1,   285,    14,    27,    -1,   285,    14,   327,    -1,
     286,    14,    24,    -1,   286,    14,   341,    -1,   287,    -1,
     288,    -1,   289,    -1,   290,    -1,   291,    -1,   292,    -1,
     324,    -1,     1,   326,    -1,   293,    -1,   294,    -1,   295,
      -1,   296,    -1,   297,    -1,   298,    -1,   301,    -1,   299,
      -1,   300,    -1,   302,    -1,   324,    -1,     1,   326,    -1,
     303,    -1,   304,    -1,     1,   326,    -1,   305,    -1,   306,
      -1,   307,    -1,   308,    -1,     1,   326,    -1,    -1,   309,
      26,   336,     5,   554,   556,     6,    -1,    -1,   309,   336,
       5,   555,   556,     6,    -1,    -1,   557,    -1,   557,   558,
      -1,   558,    -1,   559,    -1,     1,   326,    -1,   310,    14,
     493,    -1,   310,    14,   494,    -1,   310,    14,   341,    -1,
     310,    14,    27,    -1,   560,   561,    -1,   561,    -1,   405,
      -1,   449,    -1,   434,    -1,   436,    -1,   438,    -1,   440,
      -1,   483,    -1,   598,    -1,   570,    -1,    -1,   180,   336,
       5,   563,   560,     6,    -1,    -1,   565,    -1,   565,   566,
      -1,   566,    -1,   405,    -1,   394,    -1,   562,    -1,   465,
      -1,   434,    -1,   436,    -1,   438,    -1,   440,    -1,   442,
      -1,   598,    -1,   401,    -1,    -1,    -1,   165,    27,     5,
     568,   338,   569,   564,     6,    -1,    -1,   167,     5,   571,
     572,     6,    -1,   572,   573,    -1,   573,    -1,   575,    -1,
     574,    -1,   169,    14,    29,    -1,   168,     5,   576,     6,
      -1,   576,   577,    -1,   577,    -1,    27,    -1,    -1,   166,
      27,     5,   579,   580,     6,    -1,   580,   581,    -1,   581,
      -1,   582,    -1,   591,    -1,   583,    -1,   170,    14,    26,
      -1,    -1,   175,     5,   584,   585,     6,    -1,   585,   586,
      -1,   586,    -1,   587,    26,   340,    17,    26,    18,    -1,
     588,    -1,   589,    -1,   590,    -1,   176,    -1,   177,    -1,
     178,    -1,   171,    14,   592,    -1,   593,    -1,   594,    -1,
     595,    -1,   172,    -1,   173,    -1,   174,    -1,   340,    14,
      26,    -1,   340,    14,    29,    -1,   340,    14,   348,    -1,
     340,    14,   340,    -1,   596,    -1,   596,   597,    -1,    -1,
     164,    27,     5,   599,   597,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   551,   551,   552,   559,   606,   556,   627,   634,   636,
     640,   641,   645,   646,   647,   648,   649,   650,   651,   658,
     659,   663,   667,   680,   681,   685,   686,   687,   691,   692,
     696,   704,   707,   726,   747,   750,   773,   776,   797,   800,
     815,   818,   828,   831,   837,   851,   852,   856,   863,   895,
     893,   924,   922,   953,   957,   965,   966,   971,   972,   973,
     974,   975,   976,   977,   978,   979,   980,   986,   987,   992,
    1023,  1022,  1087,  1086,  1177,  1176,  1268,  1267,  1361,  1360,
    1456,  1455,  1547,  1546,  1635,  1634,  1789,  1813,  1848,  1846,
    1874,  1875,  1880,  1881,  1882,  1883,  1884,  1885,  1886,  1887,
    1888,  1889,  1890,  1895,  1909,  1923,  1937,  1951,  1965,  1979,
    1993,  2007,  2021,  2035,  2054,  2051,  2086,  2123,  2124,  2125,
    2126,  2127,  2128,  2129,  2130,  2131,  2132,  2133,  2134,  2135,
    2136,  2137,  2138,  2139,  2140,  2141,  2142,  2143,  2144,  2145,
    2146,  2147,  2148,  2149,  2150,  2151,  2159,  2161,  2163,  2165,
    2167,  2169,  2171,  2173,  2175,  2177,  2179,  2181,  2183,  2185,
    2187,  2189,  2191,  2193,  2195,  2197,  2199,  2201,  2203,  2205,
    2207,  2209,  2214,  2216,  2218,  2220,  2222,  2224,  2226,  2231,
    2233,  2235,  2237,  2239,  2241,  2243,  2245,  2247,  2249,  2251,
    2253,  2255,  2257,  2262,  2263,  2264,  2272,  2278,  2284,  2337,
    2372,  2417,  2458,  2456,  2476,  2477,  2484,  2517,  2515,  2550,
    2551,  2555,  2565,  2578,  2582,  2583,  2584,  2585,  2586,  2587,
    2595,  2628,  2684,  2726,  2768,  2806,  2850,  2851,  2855,  2856,
    2857,  2858,  2859,  2860,  2861,  2866,  2878,  2865,  2891,  2931,
    2971,  3011,  3054,  3132,  3215,  3214,  3241,  3240,  3302,  3301,
    3327,  3359,  3361,  3363,  3365,  3367,  3375,  3376,  3380,  3381,
    3386,  3409,  3406,  3437,  3435,  3462,  3460,  3487,  3485,  3511,
    3526,  3527,  3528,  3532,  3533,  3538,  3545,  3552,  3559,  3566,
    3573,  3580,  3593,  3600,  3590,  3615,  3616,  3617,  3618,  3619,
    3620,  3621,  3628,  3635,  3642,  3652,  3669,  3681,  3692,  3699,
    3690,  3716,  3723,  3730,  3740,  3757,  3769,  3780,  3787,  3778,
    3803,  3810,  3817,  3827,  3844,  3856,  3867,  3874,  3865,  3892,
    3890,  3933,  3934,  3938,  3939,  3940,  3941,  3942,  3943,  3944,
    3945,  3946,  3947,  3948,  3949,  3957,  3968,  3985,  4002,  4019,
    4047,  4058,  4069,  4073,  4080,  4096,  4134,  4145,  4158,  4173,
    4174,  4175,  4176,  4187,  4197,  4198,  4199,  4208,  4206,  4239,
    4241,  4245,  4246,  4250,  4251,  4252,  4256,  4274,  4295,  4299,
    4300,  4301,  4302,  4303,  4304,  4305,  4306,  4307,  4308,  4309,
    4310,  4311,  4312,  4313,  4314,  4315,  4316,  4317,  4326,  4331,
    4336,  4425,  4458,  4459,  4460,  4461,  4462,  4463,  4464,  4465,
    4470,  4477,  4490,  4503,  4516,  4529,  4542,  4555,  4568,  4581,
    4594,  4611,  4621,  4631,  4641,  4651,  4661,  4671,  4681,  4691,
    4701,  4711,  4721,  4731,  4742,  4759,  4777,  4791,  4808,  4809,
    4810,  4811,  4816,  4817,  4822,  4826,  4830,  4834,  4838,  4842,
    4846,  4850,  4854,  4858,  4862,  4866,  4870,  4874,  4878,  4882,
    4886,  4890,  4894,  4898,  4902,  4906,  4910,  4914,  4918,  4922,
    4926,  4934,  4937,  4940,  4943,  4946,  4949,  4952,  4958,  4959,
    4963,  4969,  4974,  4976,  4982,  4985,  4989,  4993,  4997,  5001,
    5005,  5013,  5020,  5037,  5036,  5080,  5081,  5083,  5085,  5087,
    5089,  5091,  5093,  5106,  5104,  5133,  5131,  5158,  5160,  5164,
    5165,  5169,  5188,  5207,  5215,  5219,  5220,  5221,  5222,  5223,
    5224,  5225,  5226,  5227,  5228,  5229,  5230,  5231,  5232,  5233,
    5234,  5235,  5236,  5244,  5273,  5302,  5342,  5353,  5364,  5393,
    5422,  5451,  5462,  5473,  5484,  5495,  5506,  5517,  5528,  5539,
    5550,  5561,  5572,  5583,  5593,  5608,  5621,  5633,  5646,  5656,
    5671,  5673,  5675,  5677,  5679,  5681,  5683,  5688,  5696,  5698,
    5700,  5702,  5704,  5706,  5708,  5710,  5712,  5714,  5716,  5721,
    5729,  5730,  5731,  5739,  5740,  5741,  5742,  5743,  5756,  5754,
    5782,  5780,  5807,  5809,  5813,  5814,  5818,  5819,  5827,  5832,
    5837,  5926,  5961,  5962,  5966,  5967,  5968,  5969,  5970,  5971,
    5972,  5973,  5974,  5980,  5978,  6012,  6014,  6018,  6019,  6023,
    6024,  6025,  6026,  6027,  6028,  6029,  6030,  6031,  6032,  6033,
    6039,  6058,  6037,  6089,  6087,  6096,  6097,  6100,  6101,  6104,
    6115,  6121,  6122,  6125,  6143,  6140,  6163,  6164,  6167,  6168,
    6169,  6172,  6181,  6180,  6191,  6192,  6197,  6221,  6222,  6223,
    6226,  6232,  6238,  6245,  6248,  6249,  6250,  6253,  6260,  6267,
    6279,  6310,  6341,  6433,  6468,  6469,  6475,  6473
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EOLN", "PATH", "L_ACCOLADE",
  "R_ACCOLADE", "L_PARENTHESE", "R_PARENTHESE", "L_BRACKET", "R_BRACKET",
  "L_ANGLEBRACKET", "R_ANGLEBRACKET", "OR", "ASSIGN", "COMMA",
  "NSF_AT_SYMBOL", "NSF_COLON", "NSF_SEMICOLON", "UNDERSCORE", "ASTERIK",
  "FORWARDSLASH", "PLUS", "MINUS", "N_HEX", "N_FLOAT", "N_INT", "N_STRING",
  "N_QUOTE", "N_BOOL", "N_VERSION", "NSFSHADER", "ARTIST", "HIDDEN",
  "SAVE", "ATTRIBUTES", "GLOBALATTRIBUTES", "ATTRIB", "ATTRIB_BOOL",
  "ATTRIB_STRING", "ATTRIB_UINT", "ATTRIB_FLOAT", "ATTRIB_POINT2",
  "ATTRIB_POINT3", "ATTRIB_POINT4", "ATTRIB_MATRIX3", "ATTRIB_TRANSFORM",
  "ATTRIB_COLOR", "ATTRIB_TEXTURE", "PACKINGDEF", "PD_STREAM",
  "PD_FIXEDFUNCTION", "SEMANTICADAPTERTABLE", "PDP_POSITION",
  "PDP_POSITION0", "PDP_POSITION1", "PDP_POSITION2", "PDP_POSITION3",
  "PDP_POSITION4", "PDP_POSITION5", "PDP_POSITION6", "PDP_POSITION7",
  "PDP_BLENDWEIGHTS", "PDP_BLENDINDICES", "PDP_NORMAL", "PDP_POINTSIZE",
  "PDP_COLOR", "PDP_COLOR2", "PDP_TEXCOORD0", "PDP_TEXCOORD1",
  "PDP_TEXCOORD2", "PDP_TEXCOORD3", "PDP_TEXCOORD4", "PDP_TEXCOORD5",
  "PDP_TEXCOORD6", "PDP_TEXCOORD7", "PDP_NORMAL2", "PDP_TANGENT",
  "PDP_BINORMAL", "PDP_EXTRADATA", "PDT_FLOAT1", "PDT_FLOAT2",
  "PDT_FLOAT3", "PDT_FLOAT4", "PDT_UBYTECOLOR", "PDT_SHORT1", "PDT_SHORT2",
  "PDT_SHORT3", "PDT_SHORT4", "PDT_UBYTE4", "PDT_NORMSHORT1",
  "PDT_NORMSHORT2", "PDT_NORMSHORT3", "PDT_NORMSHORT4", "PDT_NORMPACKED3",
  "PDT_PBYTE1", "PDT_PBYTE2", "PDT_PBYTE3", "PDT_PBYTE4", "PDT_FLOAT2H",
  "PDT_NORMUBYTE4", "PDT_NORMUSHORT2", "PDT_NORMUSHORT4", "PDT_UDEC3",
  "PDT_NORMDEC3", "PDT_FLOAT16_2", "PDT_FLOAT16_4", "PDTESS_DEFAULT",
  "PDTESS_PARTIALU", "PDTESS_PARTIALV", "PDTESS_CROSSUV", "PDTESS_UV",
  "PDTESS_LOOKUP", "PDTESS_LOOKUPPRESAMPLED", "PDU_POSITION",
  "PDU_BLENDWEIGHT", "PDU_BLENDINDICES", "PDU_NORMAL", "PDU_PSIZE",
  "PDU_TEXCOORD", "PDU_TANGENT", "PDU_BINORMAL", "PDU_TESSFACTOR",
  "PDU_POSITIONT", "PDU_COLOR", "PDU_FOG", "PDU_DEPTH", "PDU_SAMPLE",
  "RENDERSTATES", "CMDEFINED", "CMATTRIBUTE", "CMCONSTANT", "CMGLOBAL",
  "CMOPERATOR", "VSCONSTANTMAP", "VSPROGRAM", "GSCONSTANTMAP", "GSPROGRAM",
  "PSCONSTANTMAP", "PSPROGRAM", "CONSTANTMAP", "PROGRAM", "ENTRYPOINT",
  "SHADERTARGET", "SOFTWAREVP", "THREADGROUPCOUNTS", "SHADERPROGRAM",
  "SHADERTYPE", "SKINBONEMATRIX3", "REQUIREMENTS", "FEATURELEVEL",
  "VSVERSION", "GSVERSION", "PSVERSION", "CSVERSION", "USERVERSION",
  "PLATFORM", "BONESPERPARTITION", "BINORMALTANGENTMETHOD",
  "BINORMALTANGENTUVSOURCE", "NBTMETHOD_NONE", "NBTMETHOD_NI",
  "NBTMETHOD_MAX", "NBTMETHOD_ATI", "USERDEFINEDDATA", "IMPLEMENTATION",
  "OUTPUTSTREAM", "STREAMOUTPUT", "STREAMOUTTARGETS", "STREAMOUTAPPEND",
  "MAXVERTEXCOUNT", "OUTPUTPRIMTYPE", "_POINT", "_LINE", "_TRIANGLE",
  "VERTEXFORMAT", "FMT_FLOAT", "FMT_INT", "FMT_UINT", "CLASSNAME", "PASS",
  "STAGE", "TSS_TEXTURE", "TSS_COLOROP", "TSS_COLORARG0", "TSS_COLORARG1",
  "TSS_COLORARG2", "TSS_ALPHAOP", "TSS_ALPHAARG0", "TSS_ALPHAARG1",
  "TSS_ALPHAARG2", "TSS_RESULTARG", "TSS_CONSTANT_DEPRECATED",
  "TSS_BUMPENVMAT00", "TSS_BUMPENVMAT01", "TSS_BUMPENVMAT10",
  "TSS_BUMPENVMAT11", "TSS_BUMPENVLSCALE", "TSS_BUMPENVLOFFSET",
  "TSS_TEXCOORDINDEX", "TSS_TEXTURETRANSFORMFLAGS", "TSS_TEXTRANSMATRIX",
  "TTFF_DISABLE", "TTFF_COUNT1", "TTFF_COUNT2", "TTFF_COUNT3",
  "TTFF_COUNT4", "TTFF_PROJECTED", "PROJECTED", "USEMAPINDEX", "INVERSE",
  "TRANSPOSE", "TTSRC_GLOBAL", "TTSRC_CONSTANT", "TT_WORLD_PARALLEL",
  "TT_WORLD_PERSPECTIVE", "TT_WORLD_SPHERE_MAP", "TT_CAMERA_SPHERE_MAP",
  "TT_SPECULAR_CUBE_MAP", "TT_DIFFUSE_CUBE_MAP", "TCI_PASSTHRU",
  "TCI_CAMERASPACENORMAL", "TCI_CAMERASPACEPOSITION",
  "TCI_CAMERASPACEREFLECT", "TCI_SPHEREMAP", "TOP_DISABLE",
  "TOP_SELECTARG1", "TOP_SELECTARG2", "TOP_MODULATE", "TOP_MODULATE2X",
  "TOP_MODULATE4X", "TOP_ADD", "TOP_ADDSIGNED", "TOP_ADDSIGNED2X",
  "TOP_SUBTRACT", "TOP_ADDSMOOTH", "TOP_BLENDDIFFUSEALPHA",
  "TOP_BLENDTEXTUREALPHA", "TOP_BLENDFACTORALPHA",
  "TOP_BLENDTEXTUREALPHAPM", "TOP_BLENDCURRENTALPHA", "TOP_PREMODULATE",
  "TOP_MODULATEALPHA_ADDCOLOR", "TOP_MODULATECOLOR_ADDALPHA",
  "TOP_MODULATEINVALPHA_ADDCOLOR", "TOP_MODULATEINVCOLOR_ADDALPHA",
  "TOP_BUMPENVMAP", "TOP_BUMPENVMAPLUMINANCE", "TOP_DOTPRODUCT3",
  "TOP_MULTIPLYADD", "TOP_LERP", "TA_CURRENT", "TA_DIFFUSE",
  "TA_SELECTMASK", "TA_SPECULAR", "TA_TEMP", "TA_TEXTURE", "TA_TFACTOR",
  "TA_ALPHAREPLICATE", "TA_COMPLEMENT", "NTM_BASE", "NTM_DARK",
  "NTM_DETAIL", "NTM_GLOSS", "NTM_GLOW", "NTM_BUMP", "NTM_NORMAL",
  "NTM_PARALLAX", "NTM_DECAL", "SAMPLER", "TSAMP_ADDRESSU",
  "TSAMP_ADDRESSV", "TSAMP_ADDRESSW", "TSAMP_BORDERCOLOR",
  "TSAMP_MAGFILTER", "TSAMP_MINFILTER", "TSAMP_MIPFILTER",
  "TSAMP_MIPMAPLODBIAS", "TSAMP_MAXMIPLEVEL", "TSAMP_MAXANISOTROPY",
  "TSAMP_SRGBTEXTURE", "TSAMP_ELEMENTINDEX", "TSAMP_DMAPOFFSET",
  "TSAMP_ALPHAKILL_DEPRECATED", "TSAMP_COLORKEYOP_DEPRECATED",
  "TSAMP_COLORSIGN_DEPRECATED", "TSAMP_COLORKEYCOLOR_DEPRECATED",
  "TADDR_WRAP", "TADDR_MIRROR", "TADDR_CLAMP", "TADDR_BORDER",
  "TADDR_MIRRORONCE", "TADDR_CLAMPTOEDGE_DEPRECATED", "TEXF_NONE",
  "TEXF_POINT", "TEXF_LINEAR", "TEXF_ANISOTROPIC", "TEXF_PYRAMIDALQUAD",
  "TEXF_GAUSSIANQUAD", "TEXF_FLATCUBIC_DEPRECATED",
  "TEXF_GAUSSIANCUBIC_DEPRECATED", "TEXF_QUINCUNX_DEPRECATED",
  "TEXF_MAX_DEPRECATED", "TAK_DISABLE_DEPRECATED", "TAK_ENABLE_DEPRECATED",
  "TCKOP_DISABLE_DEPRECATED", "TCKOP_ALPHA_DEPRECATED",
  "TCKOP_RGBA_DEPRECATED", "TCKOP_KILL_DEPRECATED", "TOKEN_TEXTURE",
  "TEXTURE_SOURCE", "OBJECTS", "EFFECT_GENERALLIGHT", "EFFECT_POINTLIGHT",
  "EFFECT_DIRECTIONALLIGHT", "EFFECT_SPOTLIGHT", "EFFECT_SHADOWPOINTLIGHT",
  "EFFECT_SHADOWDIRECTIONALLIGHT", "EFFECT_SHADOWSPOTLIGHT", "EFFECT",
  "EFFECT_ENVIRONMENTMAP", "EFFECT_PROJECTEDSHADOWMAP",
  "EFFECT_PROJECTEDLIGHTMAP", "EFFECT_FOGMAP", "USEMAPVALUE", "CMOBJECT",
  "'\\n'", "'0'", "$accept", "shader_file", "shader", "@1", "@2",
  "nsfshader_components_optional", "nsfshader_components_list",
  "nsfshader_component", "optional_string", "optional_multi_string",
  "optional_description", "optional_filename", "string_or_quote",
  "attribute_name", "range_color_optional", "range_point4_optional",
  "range_point3_optional", "range_point2_optional", "range_float_optional",
  "range_int_optional", "float_values_arbitrary_list", "float_values_1",
  "float_values_16", "attribute_list_with_brackets", "@3",
  "global_attribute_list_with_brackets", "@4", "optional_multiplicity",
  "attribute_list", "attribute_value", "artist_conditional",
  "attribute_bool", "attribute_uint", "@5", "attribute_float", "@6",
  "attribute_point2", "@7", "attribute_point3", "@8", "attribute_point4",
  "@9", "attribute_matrix3", "@10", "attribute_transform", "@11",
  "attribute_color", "@12", "attribute_texture",
  "object_list_with_brackets", "@13", "object_list", "object_value",
  "object_effect_general_light", "object_effect_point_light",
  "object_effect_directional_light", "object_effect_spot_light",
  "object_effect_shadow_point_light",
  "object_effect_shadow_directional_light",
  "object_effect_shadow_spot_light", "object_effect_environment_map",
  "object_effect_projected_shadow_map",
  "object_effect_projected_light_map", "object_effect_fog_map",
  "packing_definition_definition", "@14", "packing_definition_declaration",
  "packing_definition_type", "packing_definition_parameter",
  "packing_definition_tesselator", "packing_definition_usage",
  "packing_definition_entries", "packing_definition_entry",
  "semantic_adapter_table_declaration", "@15", "semantic_adapter_list",
  "semantic_adapter_entry", "renderstate_list_with_brackets", "@16",
  "renderstate_list", "renderstate_entry_save_optional",
  "renderstate_entry_save", "renderstate_entry", "renderstate_attribute",
  "renderstate_string", "renderstate_bool", "renderstate_float",
  "renderstate_hex", "constantmap_list", "constantmap_entry",
  "constantmap_platform_block", "@17", "@18", "constantmap_entry_defined",
  "constantmap_entry_object", "constantmap_entry_attribute", "@19",
  "constantmap_entry_constant", "@20", "constantmap_entry_global", "@21",
  "constantmap_entry_operator", "operator_type",
  "operator_optional_transpose", "operator_optional_inverse",
  "shader_identification", "constantmap_with_brackets", "@22",
  "vs_constantmap_with_brackets", "@23", "gs_constantmap_with_brackets",
  "@24", "ps_constantmap_with_brackets", "@25", "userdefined_classname",
  "shader_program_name_only", "shader_program_bracket_contents_list",
  "shader_program_bracket_content", "shader_program_bracket", "@26", "@27",
  "shader_program_entry", "vertexshader_program_name",
  "vertexshader_program", "vertexshader_program_bracket", "@28", "@29",
  "geometryshader_program_name", "geometryshader_program",
  "geometryshader_program_bracket", "@30", "@31",
  "pixelshader_program_name", "pixelshader_program",
  "pixelshader_program_bracket", "@32", "@33",
  "requirement_list_with_brackets", "@34", "requirement_list",
  "requirement_entry", "requirement_vsversion", "requirement_gsversion",
  "requirement_psversion", "requirement_csversion",
  "requirement_featurelevel", "requirement_userdefined",
  "requirement_platform", "requirement_platform_list",
  "requirement_platform_list_entry", "requirement_remaining",
  "requirement_bonesperpartition",
  "requirement_usesbinormaltangentuvsource",
  "requirement_usesbinormaltangent", "binormaltanget_method",
  "stage_or_sampler_or_texture", "stage", "@35",
  "stage_entry_list_optional", "stage_entry_list",
  "stage_entry_or_texture", "stage_entry_save_optional",
  "stage_entry_save", "stage_entry", "stage_texture",
  "stage_texture_map_ndl", "stage_texture_map_ndl_decal", "stage_color_op",
  "stage_color_arg0", "stage_color_arg1", "stage_color_arg2",
  "stage_alpha_op", "stage_alpha_arg0", "stage_alpha_arg1",
  "stage_alpha_arg2", "stage_result_arg", "stage_constant_deprecated",
  "stage_bumpenvmat00", "stage_bumpenvmat01", "stage_bumpenvmat10",
  "stage_bumpenvmat11", "stage_bumpenvlscale", "stage_bumpenvloffset",
  "stage_texcoordindex", "stage_textransflags",
  "stage_texturetransformflags_count",
  "stage_texturetransformflags_optional_projection",
  "stage_texture_operation", "stage_texture_argument",
  "stage_texture_argument_modifiers_optional",
  "stage_texture_argument_modifier", "stage_texcoordindex_flags",
  "stage_textransmatrix", "stage_textransmatrix_assignment", "@36",
  "stage_textransmatrix_option", "sampler", "@37", "@38",
  "sampler_entry_list_optional", "sampler_entry_list",
  "sampler_entry_save_optional", "sampler_entry_save", "sampler_entry",
  "sampler_addressu", "sampler_addressv", "sampler_addressw",
  "sampler_bordercolor", "sampler_magfilter", "sampler_minfilter",
  "sampler_mipfilter", "sampler_mipmaplodbias", "sampler_maxmiplevel",
  "sampler_maxanisotropy", "sampler_srgbtexture", "sampler_elementindex",
  "sampler_dmapoffset", "sampler_alphakill_deprecated",
  "sampler_colorkeyop_deprecated", "sampler_colorsign_deprecated",
  "sampler_colorkeycolor_deprecated", "sampler_texture_address",
  "sampler_texture_filter", "sampler_texture_alphakill",
  "sampler_texture_colorkeyop", "texture", "@39", "@40",
  "texture_entry_list_optional", "texture_entry_list", "texture_entry",
  "texture_source", "pass_component_list", "pass_component", "pass", "@41",
  "implementation_component_list_optional",
  "implementation_component_list", "implementation_component",
  "implementation", "@42", "@43", "streamoutput", "@44",
  "streamoutput_component_list", "streamoutput_component",
  "streamoutappend", "streamouttargets", "streamoutputtarget_list",
  "streamoutputtarget", "outputstream", "@45",
  "outputstream_component_list", "outputstream_component",
  "maxvertexcount", "vertexformat", "@46", "vertexformat_entry_list",
  "vertexformat_entry", "vertexformat_entry_datatype",
  "vertexformat_entry_datatype_float", "vertexformat_entry_datatype_int",
  "vertexformat_entry_datatype_uint", "outputprimtype", "outputprimtype2",
  "outputprimtype2_POINT", "outputprimtype2_LINE",
  "outputprimtype2_TRIANGLE", "userdefineddata", "userdefineddata_list",
  "userdefineddata_block", "@47", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,    10,    48
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   328,   329,   329,   331,   332,   330,   330,   333,   333,
     334,   334,   335,   335,   335,   335,   335,   335,   335,   336,
     336,   337,   337,   338,   338,   339,   339,   339,   340,   340,
     341,   342,   342,   342,   343,   343,   344,   344,   345,   345,
     346,   346,   347,   347,   347,   348,   348,   349,   350,   352,
     351,   354,   353,   355,   355,   356,   356,   357,   357,   357,
     357,   357,   357,   357,   357,   357,   357,   358,   358,   359,
     361,   360,   363,   362,   365,   364,   367,   366,   369,   368,
     371,   370,   373,   372,   375,   374,   376,   376,   378,   377,
     379,   379,   380,   380,   380,   380,   380,   380,   380,   380,
     380,   380,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   393,   392,   394,   395,   395,   395,
     395,   395,   395,   395,   395,   395,   395,   395,   395,   395,
     395,   395,   395,   395,   395,   395,   395,   395,   395,   395,
     395,   395,   395,   395,   395,   395,   396,   396,   396,   396,
     396,   396,   396,   396,   396,   396,   396,   396,   396,   396,
     396,   396,   396,   396,   396,   396,   396,   396,   396,   396,
     396,   396,   397,   397,   397,   397,   397,   397,   397,   398,
     398,   398,   398,   398,   398,   398,   398,   398,   398,   398,
     398,   398,   398,   399,   399,   399,   400,   400,   400,   400,
     400,   400,   402,   401,   403,   403,   404,   406,   405,   407,
     407,   408,   408,   409,   410,   410,   410,   410,   410,   410,
     411,   412,   413,   414,   415,   415,   416,   416,   417,   417,
     417,   417,   417,   417,   417,   419,   420,   418,   421,   421,
     421,   421,   422,   422,   424,   423,   426,   425,   428,   427,
     429,   430,   430,   430,   430,   430,   431,   431,   432,   432,
     433,   435,   434,   437,   436,   439,   438,   441,   440,   442,
     443,   443,   443,   444,   444,   445,   445,   445,   445,   445,
     445,   445,   447,   448,   446,   449,   449,   449,   449,   449,
     449,   449,   450,   450,   450,   451,   451,   451,   453,   454,
     452,   455,   455,   455,   456,   456,   456,   458,   459,   457,
     460,   460,   460,   461,   461,   461,   463,   464,   462,   466,
     465,   467,   467,   468,   468,   468,   468,   468,   468,   468,
     468,   468,   468,   468,   468,   469,   470,   471,   472,   473,
     474,   475,   476,   476,   477,   478,   479,   480,   481,   482,
     482,   482,   482,   482,   483,   483,   483,   485,   484,   486,
     486,   487,   487,   488,   488,   488,   489,   489,   490,   491,
     491,   491,   491,   491,   491,   491,   491,   491,   491,   491,
     491,   491,   491,   491,   491,   491,   491,   491,   492,   492,
     492,   492,   493,   493,   493,   493,   493,   493,   493,   493,
     494,   495,   496,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   505,   506,   506,   507,   507,   508,   508,   509,
     509,   510,   510,   511,   511,   512,   512,   512,   513,   513,
     513,   513,   514,   514,   515,   515,   515,   515,   515,   515,
     515,   515,   515,   515,   515,   515,   515,   515,   515,   515,
     515,   515,   515,   515,   515,   515,   515,   515,   515,   515,
     515,   516,   516,   516,   516,   516,   516,   516,   517,   517,
     518,   518,   518,   518,   519,   519,   519,   519,   519,   519,
     519,   520,   521,   522,   521,   523,   523,   523,   523,   523,
     523,   523,   523,   525,   524,   526,   524,   527,   527,   528,
     528,   529,   529,   529,   530,   531,   531,   531,   531,   531,
     531,   531,   531,   531,   531,   531,   531,   531,   531,   531,
     531,   531,   531,   532,   533,   534,   535,   535,   536,   537,
     538,   539,   539,   540,   540,   541,   541,   542,   542,   543,
     543,   544,   544,   545,   545,   546,   547,   547,   548,   548,
     549,   549,   549,   549,   549,   549,   549,   549,   550,   550,
     550,   550,   550,   550,   550,   550,   550,   550,   550,   550,
     551,   551,   551,   552,   552,   552,   552,   552,   554,   553,
     555,   553,   556,   556,   557,   557,   558,   558,   559,   559,
     559,   559,   560,   560,   561,   561,   561,   561,   561,   561,
     561,   561,   561,   563,   562,   564,   564,   565,   565,   566,
     566,   566,   566,   566,   566,   566,   566,   566,   566,   566,
     568,   569,   567,   571,   570,   572,   572,   573,   573,   574,
     575,   576,   576,   577,   579,   578,   580,   580,   581,   581,
     581,   582,   584,   583,   585,   585,   586,   587,   587,   587,
     588,   589,   590,   591,   592,   592,   592,   593,   594,   595,
     596,   596,   596,   596,   597,   597,   599,   598
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     0,     0,     8,     2,     0,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     2,     0,     1,     0,     1,     1,     1,     1,
       3,     0,    20,    16,     0,    20,     0,    16,     0,    12,
       0,     4,     0,     4,     4,     1,     3,     1,    31,     0,
       5,     0,     5,     0,     3,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       0,     6,     0,     7,     0,     7,     0,     7,     0,     7,
       0,     6,     0,     6,     0,     6,     5,     4,     0,     5,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     0,     6,     2,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     2,     2,     7,     4,
       6,     3,     0,     5,     2,     1,    10,     0,     5,     2,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     2,
       3,     3,     3,     3,     3,     3,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     0,     8,     5,     5,
       4,     4,     4,     4,     0,     6,     0,     6,     0,     5,
       9,     1,     1,     1,     1,     2,     0,     1,     0,     1,
       3,     0,     7,     0,     6,     0,     6,     0,     6,     3,
       3,     3,     3,     1,     2,     1,     3,     3,     3,     3,
       4,     5,     0,     0,    10,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     3,     2,     1,     0,     0,
       9,     2,     2,     2,     3,     2,     1,     0,     0,     9,
       2,     2,     2,     3,     2,     1,     0,     0,     9,     0,
       5,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     3,     3,     3,     3,     1,
       1,     1,     1,     2,     1,     1,     1,     0,     7,     0,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     4,     4,     4,     3,     3,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     0,     1,
       2,     2,     1,     1,     0,     1,     1,     1,     1,     1,
       2,     3,     3,     0,     4,     0,     1,     1,     1,     1,
       1,     1,     2,     0,     7,     0,     6,     0,     1,     2,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     2,     1,     1,     1,     1,     2,     0,     7,
       0,     6,     0,     1,     2,     1,     1,     2,     3,     3,
       3,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     6,     0,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     0,     8,     0,     5,     2,     1,     1,     1,     3,
       4,     2,     1,     1,     0,     6,     2,     1,     1,     1,
       1,     3,     0,     5,     2,     1,     6,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     1,     2,     0,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       0,     0,     0,     0,     3,     7,     0,     1,     2,     4,
      23,    21,    24,     5,    22,     8,     0,     0,     0,     0,
       0,     0,     0,     0,     9,    11,    12,    14,    13,    15,
      16,    17,    18,    49,    51,     0,     0,     0,     0,    88,
       6,    10,     0,     0,   114,   666,   620,   634,     0,     0,
      70,    72,    74,    76,    78,    80,    82,    84,     0,     0,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,     0,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    55,    52,     0,     0,     0,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,     0,     0,     0,   194,    28,    29,     0,   664,
       0,   621,     0,     0,     0,     0,   637,   638,   640,   639,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    89,    90,    67,    68,     0,     0,    53,    53,    53,
      53,    53,    53,     0,    25,   195,   196,   197,     0,     0,
     115,   193,     0,   665,   667,   605,     0,     0,   642,   635,
     636,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    69,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    27,    25,    26,    87,     0,     0,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   201,    47,   660,   661,   663,
     662,    45,     0,     0,     0,    19,    19,    19,    19,     0,
       0,    19,   610,   619,   609,   613,   614,   615,   616,   617,
     612,   611,     0,   606,   608,   618,   641,   657,   658,   659,
     653,   654,   655,   656,     0,    42,     0,     0,     0,     0,
       0,     0,     0,    31,    86,   199,   145,   172,   173,   174,
     175,   176,   177,   178,     0,     0,   116,   202,   207,    20,
       0,     0,     0,     0,   319,     0,     0,   622,   607,   650,
     651,   652,     0,   645,     0,   647,   648,   649,     0,    71,
      54,    40,    38,    36,    34,    81,    83,     0,    85,     0,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,     0,    46,     0,     0,   263,   265,
     267,     0,     0,   269,   603,   643,   644,     0,     0,     0,
       0,    73,     0,    75,     0,    77,     0,    79,     0,     0,
     200,     0,     0,   205,     0,     0,     0,   210,   211,   212,
     217,   218,   214,   216,   215,     0,     0,     0,     0,   261,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   322,   323,   324,   325,   326,   327,   328,
     329,   333,   330,   332,   331,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   198,     0,   203,   204,   219,     0,
     208,   209,   213,   235,     0,   244,   246,   248,     0,     0,
       0,   227,   234,   228,   229,   230,   231,   232,   233,     0,
       0,     0,     0,   334,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   320,   321,     0,     0,     0,
       0,     0,     0,    19,    19,   594,   596,   597,   598,   599,
     285,   595,   297,   287,   286,   306,   289,   288,   315,   291,
     290,   600,   354,   355,   356,     0,   593,   602,   601,     0,
      44,    43,     0,     0,     0,     0,     0,     0,     0,   224,
     223,   225,   221,   222,   220,     0,     0,     0,     0,     0,
       0,     0,     0,   264,   226,   266,   268,   260,     0,   345,
     339,   335,   336,   337,   338,   340,   344,   341,   343,   346,
       0,   349,   350,   352,   351,   348,   347,   292,   298,   293,
     294,   301,   307,   302,   303,   310,   316,   311,   312,     0,
     623,    19,    19,     0,    19,     0,   296,   305,   314,   604,
     592,     0,    41,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   262,
       0,   353,     0,     0,     0,   282,     0,     0,     0,   495,
       0,   580,   295,   304,   313,   646,     0,     0,     0,     0,
       0,    30,     0,     0,     0,   240,   241,     0,     0,     0,
       0,   242,   243,   342,     0,     0,     0,     0,     0,     0,
       0,   626,   628,   627,   357,   493,     0,   578,     0,     0,
       0,     0,     0,     0,   236,   238,   239,     0,     0,   249,
       0,     0,     0,     0,     0,     0,     0,   624,   625,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     503,     0,     0,   500,   501,   502,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     519,   520,   521,     0,     0,     0,     0,     0,   585,   586,
       0,     0,     0,     0,     0,     0,     0,   245,   247,     0,
     251,   252,   253,   254,     0,   299,   308,   317,     0,   633,
       0,   632,   629,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   362,   365,   366,   367,   363,
     369,   370,   371,   372,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   364,     0,
     522,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   496,
     499,   504,     0,   587,     0,   581,   584,     0,     0,     0,
       0,     0,     0,     0,   255,   258,     0,     0,     0,   283,
     630,   631,   387,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   358,   361,   368,   494,   391,   392,   393,   394,
     395,   396,   397,   398,   399,     0,   390,   388,   389,     0,
     550,   551,   552,   553,   554,   555,   556,   523,   524,   525,
     526,   527,     0,   558,   559,   560,   561,   562,   563,   565,
     566,   564,   567,   568,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,     0,
     570,   571,   544,   543,     0,   573,   574,   575,   576,   545,
     546,   547,   548,   549,   579,   591,   590,   588,   589,     0,
       0,     0,     0,     0,     0,   237,   259,   256,     0,     0,
       0,     0,     0,   275,     0,   273,     0,     0,     0,     0,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   401,   468,   468,   468,
     468,   468,   468,   468,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,     0,   475,   476,   477,   478,
     479,     0,   427,   428,   429,   430,   431,   426,   432,     0,
     483,   481,   400,   557,   569,   572,   577,     0,     0,     0,
       0,     0,   206,   257,   250,     0,     0,     0,     0,     0,
     300,   274,   309,   318,     0,   460,   472,   473,   461,   469,
     462,   463,   464,   465,   466,   467,   480,   423,   424,   433,
     425,     0,   486,   487,   488,   489,   490,   491,     0,     0,
       0,     0,     0,     0,     0,   270,   271,   272,   276,   277,
     278,   279,   284,   470,   471,   492,   482,     0,    39,     0,
       0,     0,     0,   280,     0,   484,     0,     0,     0,     0,
     281,     0,     0,     0,     0,     0,     0,     0,     0,    33,
       0,     0,    37,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    32,     0,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     3,     4,    10,    15,    23,    24,    25,   320,    12,
      13,   225,   148,   534,   348,   387,   385,   383,   381,   339,
     260,   261,  1105,    26,    42,    27,    43,   215,    59,    60,
     175,    61,    62,   101,    63,   102,    64,   103,    65,   104,
      66,   105,    67,   106,    68,   107,    69,   108,    70,    28,
      48,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,    29,    72,   272,   255,   143,   314,
     364,   144,   145,   273,   366,   392,   393,   274,   367,   396,
     397,   398,   399,   400,   401,   402,   403,   404,   460,   461,
     462,   535,   736,   463,   464,   465,   538,   466,   539,   467,
     540,   468,   744,  1044,   947,   409,   275,   472,   276,   405,
     277,   406,   278,   407,   279,   953,   954,   955,   500,   647,
     958,   501,   502,   503,   504,   612,   836,   505,   506,   507,
     613,   837,   508,   509,   510,   614,   838,   280,   372,   422,
     423,   424,   425,   426,   427,   428,   429,   430,   557,   558,
     431,   432,   433,   434,   565,   511,   512,   679,   773,   774,
     775,   776,   777,   778,   700,   877,   878,   780,   781,   782,
     783,   784,   785,   786,   787,   788,   789,   790,   791,   792,
     793,   794,   795,   796,   797,  1028,  1070,   986,   994,  1058,
    1059,  1021,   798,  1031,  1079,  1078,   513,   680,   656,   701,
     702,   703,   704,   705,   706,   707,   708,   709,   710,   711,
     712,   713,   714,   715,   716,   717,   718,   719,   720,   721,
     722,   887,   904,   923,   929,   514,   723,   658,   726,   727,
     728,   729,   515,   516,   281,   435,   282,   283,   284,    30,
      74,   195,   517,   616,   650,   651,   652,   653,   750,   751,
      31,    75,   155,   156,   157,   158,   294,   332,   333,   334,
     335,   336,   337,   159,   290,   291,   292,   293,   149,   150,
      32,    73
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -718
static const short int yypact[] =
{
      52,  -301,    29,    51,  -718,  -718,    88,  -718,  -718,  -718,
      38,  -718,    68,  -718,  -718,    19,    95,   128,    75,   122,
     152,   174,   234,   192,    19,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,   238,   244,   286,   308,  -718,
    -718,  -718,   742,   742,  -718,  -718,  -718,  -718,   342,   218,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,   289,   567,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,   647,   573,   100,    38,   114,    32,   295,   299,   381,
     410,   434,   461,   469,   545,   551,   553,    56,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
     142,   554,   563,   565,   566,   589,   590,   591,   592,   142,
    -718,  -718,  -718,   294,   599,   384,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,   635,   641,   773,  -718,  -718,  -718,   654,   100,
     670,  -718,   664,   666,   674,    33,  -718,  -718,  -718,  -718,
     655,   656,   657,   659,   683,   684,   685,   686,   687,   754,
     764,  -718,  -718,  -718,  -718,   652,   142,   783,   783,   783,
     783,   783,   783,   142,    45,  -718,  -718,  -718,   767,   665,
    -718,  -718,   512,  -718,  -718,   442,   768,   164,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,   769,   770,   142,   142,   142,   142,   142,
     142,   772,  -718,    97,  -718,  -718,   665,   472,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,   292,  -718,  -718,  -718,  -718,
    -718,   784,   774,   795,   798,   777,   777,   777,   777,   800,
     793,   777,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,   802,   442,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,   232,   808,   799,   772,   772,   772,
     772,   772,   772,   809,  -718,   292,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,   582,   772,  -718,  -718,  -718,  -718,
     815,   816,   817,   848,  -718,   100,   855,  -718,  -718,  -718,
    -718,  -718,     0,  -718,   835,  -718,  -718,  -718,   240,  -718,
    -718,   853,   854,   856,   857,  -718,  -718,   860,  -718,   582,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,   838,  -718,   842,    47,  -718,  -718,
    -718,   722,    71,  -718,  -718,  -718,  -718,   100,   845,   846,
     849,  -718,   864,  -718,   866,  -718,   868,  -718,   851,   852,
    -718,   100,    93,  -718,   555,   863,    59,  -718,  -718,   850,
    -718,  -718,  -718,  -718,  -718,    40,    40,    40,   865,  -718,
     556,   869,   871,   872,   873,   874,   875,   876,   877,   878,
     879,   880,   287,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,   408,   881,   870,   885,   882,
     883,   884,   886,   887,  -718,   888,  -718,  -718,  -718,   332,
    -718,  -718,  -718,  -718,    57,  -718,  -718,  -718,   889,   890,
       9,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,    14,
      25,   891,    40,  -718,   867,   892,   893,   894,   895,   896,
     897,   901,   903,    49,   904,  -718,  -718,    87,   102,   104,
     898,   899,   905,   260,   268,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,   906,  -718,  -718,   907,  -718,  -718,   908,  -718,
    -718,  -718,  -718,  -718,  -718,    53,  -718,  -718,  -718,   910,
    -718,  -718,   900,   917,   922,   923,   914,   915,   913,  -718,
    -718,  -718,  -718,  -718,  -718,   741,   388,   422,   100,   100,
     100,   916,   918,  -718,  -718,  -718,  -718,  -718,    30,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,   902,  -718,  -718,
     574,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,   722,
    -718,   777,   777,   909,   777,   938,   919,   920,   921,  -718,
    -718,   926,  -718,   924,   925,   927,   936,   100,   941,   940,
     929,   930,   931,   932,   933,   934,   935,   937,   315,  -718,
     901,  -718,   743,   745,   750,  -718,   198,   957,   959,  -718,
     960,  -718,  -718,  -718,  -718,  -718,   912,   951,   952,   943,
     953,  -718,   901,   945,   946,  -718,  -718,   947,   948,   949,
     942,  -718,  -718,  -718,   962,   963,   964,   756,   974,   966,
      27,  -718,  -718,  -718,  -718,  -718,   110,  -718,    21,   975,
     956,   958,   275,   961,   902,  -718,  -718,   965,   772,  -718,
     193,   901,   901,   901,   970,   967,   968,  -718,  -718,   314,
     110,   587,   971,   972,   976,   978,   979,   981,   982,   984,
     985,   986,   987,   988,   989,   990,   991,   992,   993,   994,
    -718,   983,   147,  -718,  -718,   954,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,    21,   595,   995,  1004,    22,  -718,  -718,
     996,  1003,   997,  1006,   998,   999,    40,  -718,  -718,   596,
    -718,  -718,  -718,  -718,  1000,   902,   902,   902,   901,  -718,
      61,  -718,  -718,   688,  1002,  1005,  1008,  1010,  1011,  1012,
    1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,
    1024,  1025,  1026,  1035,   334,  -718,  -718,  -718,  1009,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  1036,
    -718,     7,    20,    20,    20,    86,     4,     4,     4,   124,
     125,   126,   171,   220,   222,    15,    46,   -10,   195,  -718,
    -718,  -718,  1038,  -718,   191,  -718,  -718,  1030,  1013,  1027,
    1028,  1039,  1029,    35,  -718,   807,   459,   459,   459,   902,
    -718,  -718,  -718,   236,   312,   312,   312,   236,   312,   312,
     312,   312,  1032,    50,   231,   239,   271,   386,   387,    31,
     354,   151,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  1031,  -718,  -718,  -718,   692,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,   720,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,   723,
    -718,  -718,  -718,  -718,   724,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  1033,
    1034,  1040,  1045,  1044,  1043,  -718,  -718,   843,  1048,  1049,
    1050,  1051,  1052,  -718,  1061,   459,  1062,  1063,   459,   744,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,   176,   176,   176,
     176,   176,   176,   176,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,   746,  -718,  -718,  -718,  -718,
    -718,    -2,  -718,  -718,  -718,  -718,  -718,  -718,   911,    43,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  1065,  1056,  1067,
    1053,  1054,  -718,  -718,  -718,    85,  1055,  1057,  1046,  1059,
    -718,  -718,  -718,  -718,  1070,  -718,   818,   822,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,   755,  -718,  -718,  -718,  -718,  -718,  -718,  1060,    62,
    1073,  1064,  1066,  1071,  1075,  -718,  -718,  -718,  -718,  -718,
    -718,  1068,  -718,  -718,  -718,  -718,  -718,  1072,  -718,  1077,
    1078,  1074,  1076,  1069,  1081,  -718,  1079,  1080,  1090,  1085,
    -718,  1082,  1094,  1088,  1096,  1083,  1095,  1099,  1086,  -718,
    1097,  1089,  -718,  1098,  1091,  1100,  1092,  1111,  1101,  1112,
    1113,  1106,  1114,  -718,  1102,  -718,  1107,  1103,  1110,  1104,
    1115,  1108,  1116,  1109,  1117,  1118,  1120,  1119,  1121,  1122,
    1123,  1124,  1125,  1126,  1127,  1128,  1130,  1129,  -718
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -718,  -718,  1134,  -718,  -718,  -718,  -718,  1131,  -258,  -718,
    1084,   939,  -191,     1,  -718,  -718,  -718,  -718,  -718,  -718,
    -221,  -718,  -718,  -718,  -718,  -718,  -718,   371,  1105,   173,
     379,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  1001,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,   944,  -718,   834,
     792,  -718,  1037,  -718,  -718,  -718,   758,  -398,  -718,  -718,
     760,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -403,  -458,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,   578,  -397,  -718,  -393,  -718,
    -392,  -718,  -389,  -718,  -718,  -718,  -717,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
     730,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -568,   536,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
     385,  -718,  -718,  -718,  -650,   336,   337,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,   316,  -176,  -405,
    -718,  -718,  -718,  -718,  -718,    89,  -718,  -718,  -718,   484,
    -718,   463,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,
    -718,  -314,  -315,  -718,  -718,  -718,  -718,  -718,   443,  -718,
     440,  -718,  -718,   658,  -718,  -718,  -718,  -718,   928,  -718,
    -718,  -718,  -718,  -718,  -718,   519,  -718,  -718,  -718,   421,
    -718,  -718,  -718,  1041,  -718,  -718,  -718,  -718,   840,  -718,
    -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  -718,  1042,
    -188,  -718
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -584
static const short int yytable[] =
{
     303,   259,   544,   469,   470,   892,   375,   285,   321,   322,
     323,   544,   544,   326,   453,   543,   919,   930,   528,   453,
     545,   879,   724,   724,  1067,     5,   528,  -582,  -583,   779,
     453,   546,  1015,   677,   866,   453,   609,   495,   496,   199,
     453,   945,   497,   498,  1071,   453,   499,   924,   394,   222,
     560,     7,     1,     1,    16,    17,     6,  -474,   160,   589,
     394,   528,   171,  1071,   664,   450,    11,   840,    18,   548,
    -485,   223,   410,   224,   395,  1003,   341,   342,   343,   344,
     345,   346,     2,     2,   146,   147,   395,  -485,   749,  1085,
     544,   567,   568,     9,   365,   285,    14,   528,   411,   446,
      33,   222,    35,   745,   746,   747,   571,   572,   575,   576,
     890,   681,  1086,  1087,   569,   570,  -497,   495,   496,   391,
     956,   957,   497,   498,   779,   224,   499,   146,   147,   573,
     574,   577,   578,    34,   373,   528,   528,   528,   454,   455,
     456,   457,   458,   454,   455,   456,   457,   458,   681,    36,
     907,   909,   911,  -498,   454,   455,   456,   457,   458,   454,
     455,   456,   457,   458,   454,   455,   456,   457,   458,   454,
     455,   456,   457,   458,   173,   174,   329,   330,   331,    37,
     839,   264,   528,    19,    20,    21,   436,   265,   487,   266,
     488,   267,   489,   268,   739,   648,   649,   913,    40,   490,
     445,    38,   528,   152,   153,   536,   528,  1068,   154,   561,
     562,   563,   564,   740,   741,   742,   743,    19,   935,   932,
     491,   412,   413,   414,   415,   416,   417,   418,   419,   420,
     421,   528,   111,   528,   492,   583,   585,   959,  1051,    39,
    -474,  1054,   528,    44,   111,   100,   915,   518,   917,    45,
     528,  1016,  1017,  1018,  1019,  1020,  1005,  1072,  1073,  1074,
    1075,  1076,  1077,   537,  1007,   378,   379,   867,   868,   869,
     870,   871,   872,   873,   874,   875,  1072,  1073,  1074,  1075,
    1076,  1077,   528,   733,   152,   153,   582,   319,   410,   154,
     734,    46,   682,   485,   584,   319,  1009,   893,   894,   895,
     896,   897,   898,   899,   900,   901,   902,   880,   881,   882,
     883,   884,   885,    47,   411,   753,   109,   931,   920,   921,
    -359,   161,   493,   617,   618,   162,   620,   518,   903,   682,
      22,   725,   725,   833,   459,   753,   287,   288,   289,   459,
    -360,   641,   642,   528,   886,   601,   603,   604,   605,   606,
     459,   925,   926,   927,   928,   459,   529,   530,   531,   532,
     459,   533,   494,  1029,  1030,   459,   648,   649,    76,    77,
      78,    79,    80,    81,    82,   544,    83,    84,    85,    86,
     683,   684,   685,   686,   687,   688,   689,   690,   691,   692,
     693,   694,   695,   696,   697,   698,   699,   528,   528,   307,
     308,   309,   310,   311,   312,   313,   630,   163,   329,   330,
     331,  1011,  1013,   187,   600,   146,   147,   683,   684,   685,
     686,   687,   688,   689,   690,   691,   692,   693,   694,   695,
     696,   697,   698,   699,  1056,  1057,   164,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   738,   602,   146,
     147,   867,   868,   869,   870,   871,   872,   873,   874,   875,
     165,   960,   961,   962,   963,   964,   965,   966,   967,   968,
     969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
     979,   980,   981,   982,   983,   984,   985,   166,   184,   888,
     889,   262,   905,   906,   263,   167,   682,   754,   755,   756,
     757,   758,   759,   760,   761,   762,   763,   764,   765,   766,
     767,   768,   769,   770,   771,   772,   682,   754,   755,   756,
     757,   758,   759,   760,   761,   762,   763,   764,   765,   766,
     767,   768,   769,   770,   771,   772,   264,   256,   257,   146,
     147,   258,   265,   487,   266,   488,   267,   489,   268,   216,
     217,   218,   219,   220,   490,   213,  1022,  1023,  1024,  1025,
    1026,  1027,   221,   987,   988,   989,   990,   991,   992,   993,
     264,   168,    19,   110,   113,   491,   265,   169,   266,   170,
     267,   176,   268,  1060,  1061,  1062,  1063,  1064,  1065,   492,
     177,   269,   178,   179,   297,   298,   299,   300,   301,   302,
     948,   949,   950,   951,   952,    49,    19,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   180,   181,   182,   183,
     185,   270,   271,   114,   115,   186,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   112,    76,    77,    78,    79,    80,    81,
      82,   188,    83,    84,    85,    86,   227,   189,   192,   995,
     996,  -117,   998,   999,  1000,  1001,   194,   493,   196,   198,
     197,   212,   201,   202,   203,    49,   204,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   350,   351,   352,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
     205,   206,   207,   208,   209,  -117,  -117,   494,  -117,  -117,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,  -117,
    -117,  -117,  -117,  -117,  -117,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,  -117,  -117,  -117,  -117,  -117,  -117,  -117,   190,
      49,   210,    50,    51,    52,    53,    54,    55,    56,    57,
      58,   211,   214,   226,   286,   295,   296,   256,   306,   315,
     317,   316,   876,   318,   319,   324,   891,   325,   327,   340,
     908,   910,   912,   914,   916,   918,   922,   338,   347,   933,
     368,   369,   370,   114,   115,   936,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   371,  1004,  1006,  1008,  1010,  1012,  1014,
     374,   377,   380,   382,   390,   384,   386,   388,   391,   408,
     437,   440,   438,   441,   439,   442,   443,   449,   444,   471,
     520,   448,   473,   474,   452,   475,   476,   477,   478,   479,
     480,   481,   482,   483,   484,   521,   549,   599,   519,   644,
     611,   645,   526,   579,   580,   527,   646,   522,   523,   524,
     592,   525,   674,   800,   619,   610,   541,   542,   547,   550,
     659,   823,   834,   551,   552,   553,   554,   555,   556,   559,
     566,   581,   593,   586,   587,   588,   591,   594,   595,   596,
     598,   597,   607,   621,   625,   608,   622,   623,   624,   626,
     627,   629,   628,   631,   632,   633,   634,   635,   636,   637,
     638,   639,   654,   640,   655,   657,   660,   661,   662,   670,
     663,   665,   666,   667,   668,   669,   671,   672,   673,   675,
     676,   731,   730,   732,   748,   801,   802,   735,   821,   819,
     803,   737,   804,   805,   749,   806,   807,   752,   808,   809,
     810,   811,   812,   813,   814,   815,   816,   817,   818,   824,
     825,   828,   829,   830,   842,   832,   843,   946,  1033,   844,
     940,   827,   845,   831,   846,   847,   848,   835,   849,   850,
     851,   852,   853,   854,   855,   856,   857,   858,   859,   860,
     861,   862,   865,   864,   934,   939,  1034,   943,  1039,  1035,
    1036,  1041,   941,   942,  1043,   944,  1002,  1032,  1037,  1038,
    1040,  1042,  1045,  1046,  1047,  1048,  1049,  1050,  1052,  1053,
    1055,  1081,  1066,  1080,  1082,  1090,  1092,  1093,  1083,  1084,
    1094,  1095,  1088,  1098,  1089,  1091,  1101,  1096,   172,  1099,
    1102,  1100,  1106,  1107,  1103,  1110,  1111,  1104,  1114,  1108,
    1115,  1109,  1117,  1118,  1112,  1113,  1119,  1116,  1120,  1122,
    1121,  1123,  1124,  1126,  1125,  1128,  1127,  1129,  1069,  1130,
    1132,  1134,  1137,  1133,  1135,  1139,  1131,  1136,  1138,  1140,
    1141,  1143,  1145,  1142,  1144,  1147,  1149,     8,  1151,   349,
    1153,   389,  1155,  1146,  1148,  1157,   643,  1150,    71,  1152,
     447,  1154,   486,  1156,  1158,    41,   451,   615,   151,   863,
     937,   938,   304,   997,   799,   820,   822,   826,  1097,   678,
     305,   841,   376,   590,     0,     0,     0,     0,     0,     0,
       0,   191,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   193,     0,     0,     0,     0,   200,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   328
};

static const short int yycheck[] =
{
     221,   192,   460,   406,   407,     1,     6,   195,   266,   267,
     268,   469,   470,   271,     5,     6,     1,    27,    11,     5,
       6,     1,     1,     1,    26,   326,    11,     6,     6,   679,
       5,     6,     1,     6,    27,     5,     6,   435,   435,     6,
       5,     6,   435,   435,     1,     5,   435,     1,     1,     4,
       1,     0,     1,     1,    35,    36,    27,    26,    26,     6,
       1,    11,     6,     1,   632,     6,    28,     6,    49,   472,
      27,    26,     1,    28,    27,    25,   297,   298,   299,   300,
     301,   302,    31,    31,    27,    28,    27,    25,    27,     4,
     548,     4,     5,     5,   315,   283,    28,    11,    27,     6,
       5,     4,    27,   671,   672,   673,     4,     5,     4,     5,
      24,     1,    27,    28,    27,    28,     6,   515,   515,    26,
     837,   838,   515,   515,   774,    28,   515,    27,    28,    27,
      28,    27,    28,     5,   325,    11,    11,    11,   129,   130,
     131,   132,   133,   129,   130,   131,   132,   133,     1,    27,
      26,    26,    26,     6,   129,   130,   131,   132,   133,   129,
     130,   131,   132,   133,   129,   130,   131,   132,   133,   129,
     130,   131,   132,   133,    32,    33,   176,   177,   178,    27,
     748,   128,    11,   164,   165,   166,   377,   134,   135,   136,
     137,   138,   139,   140,     1,   168,   169,    26,     6,   146,
     391,    27,    11,   170,   171,   148,    11,   209,   175,   160,
     161,   162,   163,    20,    21,    22,    23,   164,    27,    24,
     167,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,    11,    59,    11,   181,   493,   494,     1,   955,     5,
     209,   958,    11,     5,    71,    27,    26,   435,    26,     5,
      11,   220,   221,   222,   223,   224,    25,   214,   215,   216,
     217,   218,   219,   454,    25,    25,    26,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   214,   215,   216,   217,
     218,   219,    11,     8,   170,   171,    26,    27,     1,   175,
      15,     5,   182,     6,    26,    27,    25,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   287,   288,   289,
     290,   291,   292,     5,    27,     1,    27,   327,   303,   304,
       6,    26,   269,   581,   582,    26,   584,   515,   324,   182,
     311,   310,   310,   736,   325,     1,   172,   173,   174,   325,
       6,    26,    27,    11,   324,   536,   537,   538,   539,   540,
     325,   305,   306,   307,   308,   325,    24,    25,    26,    27,
     325,    29,   309,   212,   213,   325,   168,   169,   312,   313,
     314,   315,   316,   317,   318,   833,   320,   321,   322,   323,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,    11,    11,   107,
     108,   109,   110,   111,   112,   113,   597,    26,   176,   177,
     178,    25,    25,    29,    26,    27,    28,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   258,   259,    26,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   668,    26,    27,
      28,   260,   261,   262,   263,   264,   265,   266,   267,   268,
      26,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,    26,   109,   803,
     804,    49,   807,   808,    52,    26,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   128,    25,    26,    27,
      28,    29,   134,   135,   136,   137,   138,   139,   140,   178,
     179,   180,   181,   182,   146,   176,   202,   203,   204,   205,
     206,   207,   183,   251,   252,   253,   254,   255,   256,   257,
     128,    26,   164,     6,     1,   167,   134,    26,   136,    26,
     138,    27,   140,   988,   989,   990,   991,   992,   993,   181,
      27,   149,    27,    27,   215,   216,   217,   218,   219,   220,
     141,   142,   143,   144,   145,    38,   164,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    27,    27,    27,    27,
     326,   179,   180,    50,    51,    26,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     6,   312,   313,   314,   315,   316,   317,
     318,    26,   320,   321,   322,   323,     1,    26,    14,   845,
     846,     6,   848,   849,   850,   851,     6,   269,    14,     5,
      14,    29,    27,    27,    27,    38,    27,    40,    41,    42,
      43,    44,    45,    46,    47,    48,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
      27,    27,    27,    27,    27,    50,    51,   309,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,     6,
      38,    27,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    27,     9,    26,    26,    26,    26,    25,   326,    15,
       5,    27,   801,     5,    27,     5,   805,    14,     6,    10,
     809,   810,   811,   812,   813,   814,   815,     9,     9,   818,
       5,     5,     5,    50,    51,   824,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     5,   853,   854,   855,   856,   857,   858,
       5,    26,     9,     9,    26,     9,     9,     7,    26,   147,
      25,     7,    26,     7,    25,     7,    25,    14,    26,    14,
      10,   326,   326,    14,    34,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    10,    29,   156,    17,   156,
     326,   156,    15,     5,     5,    17,   156,    25,    25,    25,
      10,    25,   156,   326,     5,    13,    27,    27,    27,    27,
       8,   326,   326,    30,    30,    30,    30,    30,    27,    26,
      26,    26,    15,    27,    27,    27,    26,    15,    15,    25,
      27,    26,    26,     5,    18,    27,    27,    27,    27,    25,
      25,    15,    25,    12,    14,    26,    26,    26,    26,    26,
      26,    26,     5,    26,     5,     5,    15,    15,    25,    27,
      17,    26,    26,    26,    26,    26,    14,    14,    14,     5,
      14,    25,     7,    25,    14,    14,    14,    26,    34,     6,
      14,    26,    14,    14,    27,    14,    14,    29,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
       6,     8,    15,     7,   326,    16,    14,   210,   326,    14,
       7,    25,    14,    25,    14,    14,    14,    27,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,     6,     6,    34,     6,    15,   326,     8,     8,   326,
     326,     7,    25,    25,   211,    26,    24,    26,    25,    25,
      15,    18,    14,    14,    14,    14,    14,     6,     6,     6,
     326,    15,   326,     8,     7,    29,     6,   259,    25,    25,
     258,   326,    27,    10,    27,    26,    15,    27,    87,    25,
      15,    25,    15,    15,    26,    26,    15,    25,     8,    25,
      15,    25,     8,    15,    25,    25,    10,    25,    25,    10,
      15,    25,    15,    15,    25,    15,    25,    25,   207,     8,
       8,    15,    15,    10,    10,    15,    25,    25,    25,    25,
      15,    15,    15,    25,    25,    15,    15,     3,    15,   305,
      15,   349,    15,    25,    25,    15,   610,    25,    43,    25,
     392,    25,   422,    25,    25,    24,   396,   579,    74,   774,
     824,   824,   223,   847,   680,   702,   723,   727,  1079,   650,
     226,   750,   332,   515,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   149,    -1,    -1,    -1,    -1,   155,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   283
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     1,    31,   329,   330,   326,    27,     0,   330,     5,
     331,    28,   337,   338,    28,   332,    35,    36,    49,   164,
     165,   166,   311,   333,   334,   335,   351,   353,   377,   392,
     567,   578,   598,     5,     5,    27,    27,    27,    27,     5,
       6,   335,   352,   354,     5,     5,     5,     5,   378,    38,
      40,    41,    42,    43,    44,    45,    46,    47,    48,   356,
     357,   359,   360,   362,   364,   366,   368,   370,   372,   374,
     376,   356,   393,   599,   568,   579,   312,   313,   314,   315,
     316,   317,   318,   320,   321,   322,   323,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
      27,   361,   363,   365,   367,   369,   371,   373,   375,    27,
       6,   357,     6,     1,    50,    51,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,   396,   399,   400,    27,    28,   340,   596,
     597,   338,   170,   171,   175,   580,   581,   582,   583,   591,
      26,    26,    26,    26,    26,    26,    26,    26,    26,    26,
      26,     6,   380,    32,    33,   358,    27,    27,    27,    27,
      27,    27,    27,    27,   358,   326,    26,    29,    26,    26,
       6,   400,    14,   597,     6,   569,    14,    14,     5,     6,
     581,    27,    27,    27,    27,    27,    27,    27,    27,    27,
      27,    27,    29,   358,     9,   355,   355,   355,   355,   355,
     355,   358,     4,    26,    28,   339,    26,     1,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   395,    25,    26,    29,   340,
     348,   349,    49,    52,   128,   134,   136,   138,   140,   149,
     179,   180,   394,   401,   405,   434,   436,   438,   440,   442,
     465,   562,   564,   565,   566,   598,    26,   172,   173,   174,
     592,   593,   594,   595,   584,    26,    26,   358,   358,   358,
     358,   358,   358,   348,   339,   395,   326,   107,   108,   109,
     110,   111,   112,   113,   397,    15,    27,     5,     5,    27,
     336,   336,   336,   336,     5,    14,   336,     6,   566,   176,
     177,   178,   585,   586,   587,   588,   589,   590,     9,   347,
      10,   348,   348,   348,   348,   348,   348,     9,   342,   397,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   398,   348,   402,   406,     5,     5,
       5,     5,   466,   340,     5,     6,   586,    26,    25,    26,
       9,   346,     9,   345,     9,   344,     9,   343,     7,   398,
      26,    26,   403,   404,     1,    27,   407,   408,   409,   410,
     411,   412,   413,   414,   415,   437,   439,   441,   147,   433,
       1,    27,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   478,   479,   480,   481,   563,   340,    25,    26,    25,
       7,     7,     7,    25,    26,   340,     6,   404,   326,    14,
       6,   408,    34,     5,   129,   130,   131,   132,   133,   325,
     416,   417,   418,   421,   422,   423,   425,   427,   429,   416,
     416,    14,   435,   326,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,     6,   468,   135,   137,   139,
     146,   167,   181,   269,   309,   405,   434,   436,   438,   440,
     446,   449,   450,   451,   452,   455,   456,   457,   460,   461,
     462,   483,   484,   524,   553,   560,   561,   570,   598,    17,
      10,    10,    25,    25,    25,    25,    15,    17,    11,    24,
      25,    26,    27,    29,   341,   419,   148,   340,   424,   426,
     428,    27,    27,     6,   417,     6,     6,    27,   416,    29,
      27,    30,    30,    30,    30,    30,    27,   476,   477,    26,
       1,   160,   161,   162,   163,   482,    26,     4,     5,    27,
      28,     4,     5,    27,    28,     4,     5,    27,    28,     5,
       5,    26,    26,   336,    26,   336,    27,    27,    27,     6,
     561,    26,    10,    15,    15,    15,    25,    26,    27,   156,
      26,   340,    26,   340,   340,   340,   340,    26,    27,     6,
      13,   326,   453,   458,   463,   433,   571,   336,   336,     5,
     336,     5,    27,    27,    27,    18,    25,    25,    25,    15,
     340,    12,    14,    26,    26,    26,    26,    26,    26,    26,
      26,    26,    27,   477,   156,   156,   156,   447,   168,   169,
     572,   573,   574,   575,     5,     5,   526,     5,   555,     8,
      15,    15,    25,    17,   476,    26,    26,    26,    26,    26,
      27,    14,    14,    14,   156,     5,    14,     6,   573,   485,
     525,     1,   182,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     492,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   554,     1,   310,   556,   557,   558,   559,
       7,    25,    25,     8,    15,    26,   420,    26,   348,     1,
      20,    21,    22,    23,   430,   476,   476,   476,    14,    27,
     576,   577,    29,     1,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   486,   487,   488,   489,   490,   491,   492,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   520,   527,
     326,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,     6,
     529,    34,   556,   326,    14,     6,   558,    25,     8,    15,
       7,    25,    16,   416,   326,    27,   454,   459,   464,   476,
       6,   577,   326,    14,    14,    14,    14,    14,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    14,     6,   488,    34,     6,    27,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   341,   493,   494,     1,
     287,   288,   289,   290,   291,   292,   324,   549,   549,   549,
      24,   341,     1,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   324,   550,   550,   550,    26,   341,    26,
     341,    26,   341,    26,   341,    26,   341,    26,   341,     1,
     303,   304,   341,   551,     1,   305,   306,   307,   308,   552,
      27,   327,    24,   341,     6,    27,   341,   493,   494,    15,
       7,    25,    25,     8,    26,     6,   210,   432,   141,   142,
     143,   144,   145,   443,   444,   445,   444,   444,   448,     1,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   515,   251,   252,   253,
     254,   255,   256,   257,   516,   516,   516,   515,   516,   516,
     516,   516,    24,    25,   341,    25,   341,    25,   341,    25,
     341,    25,   341,    25,   341,     1,   220,   221,   222,   223,
     224,   519,   202,   203,   204,   205,   206,   207,   513,   212,
     213,   521,    26,   326,   326,   326,   326,    25,    25,     8,
      15,     7,    18,   211,   431,    14,    14,    14,    14,    14,
       6,   444,     6,     6,   444,   326,   258,   259,   517,   518,
     517,   517,   517,   517,   517,   517,   326,    26,   209,   207,
     514,     1,   214,   215,   216,   217,   218,   219,   523,   522,
       8,    15,     7,    25,    25,     4,    27,    28,    27,    27,
      29,    26,     6,   259,   258,   326,    27,   523,    10,    25,
      25,    15,    15,    26,    25,   350,    15,    15,    25,    25,
      26,    15,    25,    25,     8,    15,    25,     8,    15,    10,
      25,    15,    10,    25,    15,    25,    15,    25,    15,    25,
       8,    25,     8,    10,    15,    10,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25,    15,
      25,    15,    25,    15,    25,    15,    25,    15,    25
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
    */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

    {
                g_bGlobalAttributes = false;
                g_pkCurrAttribTable = 0;
                g_pkCurrObjectTable = 0;
                g_uiCurrPDStream = 0;    
                g_bCurrPDFixedFunction = false;
                g_pkCurrPackingDef = 0;
                g_pkCurrRequirements = 0;
                g_uiCurrImplementation = 0;
                g_pkCurrImplementation = 0;
                g_pkCurrRSGroup = 0;
                memset(
                    g_auiCurrImplemConstantMap, 
                    0, 
                    sizeof(g_auiCurrImplemConstantMap));
                memset(
                    g_auiCurrPassConstantMap, 
                    0, 
                    sizeof(g_auiCurrPassConstantMap));
                g_pkCurrConstantMap = 0;
                g_uiCurrPassIndex = 0;
                g_pkCurrPass = 0;
                g_pkCurrTextureStage = 0;
                g_uiCurrTextureSlot = 0;
                g_pkCurrTexture = 0;
                g_eCurrentShaderType = 
                    (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT;
                
                g_pkCurrShader = NiNew NSFParsedShader();
                if (g_pkCurrShader)
                {
                    g_kParsedShaderList.AddTail(g_pkCurrShader);
                    g_pkCurrNSBShader = g_pkCurrShader->GetShader();
                    g_pkCurrNSBShader->SetName((yyvsp[-1].sval));
                    g_uiCurrImplementation = 0;
                }
                else
                {
                    DebugStringOut("Failed to create NSFParsedShader!\n");
                }
                
                g_iDSOIndent = 0;
                NiSprintf(g_acDSO, 1024, "\nNSF Shader - %s\n",(yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 5:

    {
                    NiSprintf(g_acDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_acDSO);
                    
                    if (g_pkCurrNSBShader)
                        g_pkCurrNSBShader->SetDescription((yyvsp[0].sval));
                        
                    NiFree((yyvsp[0].sval));
                ;}
    break;

  case 6:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Completed NSF Shader - %s\n",(yyvsp[-6].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrShader = 0;
                g_pkCurrNSBShader = 0;
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 7:

    {
                NSFParsererror("Syntax Error: shader");
                yyclearin;
            ;}
    break;

  case 19:

    {   (yyval.sval) = 0;    ;}
    break;

  case 20:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 21:

    {
                (yyval.sval) = (yyvsp[0].sval);
            ;}
    break;

  case 22:

    {
                // Assumes $1 is a null-terminated string
                NIASSERT((yyvsp[-1].sval));
                size_t stLen = strlen((yyvsp[-1].sval)) + 1;
                if (stLen < MAX_QUOTE_LENGTH)
                    stLen = MAX_QUOTE_LENGTH;
                (yyval.sval) = NiStrcat((yyvsp[-1].sval), stLen, (yyvsp[0].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 23:

    {   (yyval.sval) = 0;    ;}
    break;

  case 24:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 25:

    {   (yyval.sval) = 0;    ;}
    break;

  case 26:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 27:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 28:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 29:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 30:

    {   (yyval.sval) = (yyvsp[-1].sval);    ;}
    break;

  case 31:

    {
                g_bRanged = false;
            ;}
    break;

  case 32:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 33:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 34:

    {
                g_bRanged = false;
            ;}
    break;

  case 35:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-17].fval));
                AddFloatToLowArray((yyvsp[-15].fval));
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToHighArray((yyvsp[-8].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 36:

    {
                g_bRanged = false;
            ;}
    break;

  case 37:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-13].fval));
                AddFloatToLowArray((yyvsp[-11].fval));
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToHighArray((yyvsp[-6].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 38:

    {
                g_bRanged = false;
            ;}
    break;

  case 39:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-9].fval));
                AddFloatToLowArray((yyvsp[-7].fval));
                AddFloatToHighArray((yyvsp[-4].fval));
                AddFloatToHighArray((yyvsp[-2].fval));
            ;}
    break;

  case 40:

    {
                g_bRanged = false;
            ;}
    break;

  case 41:

    {
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 42:

    {
                g_bRanged = false;
            ;}
    break;

  case 43:

    {
                g_bRanged    = true;
                AddFloatToLowArray((float)(yyvsp[-2].ival));
                AddFloatToHighArray((float)(yyvsp[-1].ival));
            ;}
    break;

  case 44:

    {
                // Allow floats for backwards compatibility
                g_bRanged    = true;
                AddFloatToLowArray((yyvsp[-2].fval));
                AddFloatToHighArray((yyvsp[-1].fval));
            ;}
    break;

  case 47:

    {
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 48:

    {
                AddFloatToValueArray((yyvsp[-30].fval));
                AddFloatToValueArray((yyvsp[-28].fval));
                AddFloatToValueArray((yyvsp[-26].fval));
                AddFloatToValueArray((yyvsp[-24].fval));
                AddFloatToValueArray((yyvsp[-22].fval));
                AddFloatToValueArray((yyvsp[-20].fval));
                AddFloatToValueArray((yyvsp[-18].fval));
                AddFloatToValueArray((yyvsp[-16].fval));
                AddFloatToValueArray((yyvsp[-14].fval));
                AddFloatToValueArray((yyvsp[-12].fval));
                AddFloatToValueArray((yyvsp[-10].fval));
                AddFloatToValueArray((yyvsp[-8].fval));
                AddFloatToValueArray((yyvsp[-6].fval));
                AddFloatToValueArray((yyvsp[-4].fval));
                AddFloatToValueArray((yyvsp[-2].fval));
                AddFloatToValueArray((yyvsp[0].fval));
            ;}
    break;

  case 49:

    {
                NiSprintf(g_acDSO, 1024, "Attribute Table Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 50:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Attribute Table End\n");
                DebugStringOut(g_acDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 51:

    {
                NiSprintf(g_acDSO, 1024, "Global Attribute Table Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = true;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetGlobalAttributeTable();
                }
                else
                {
                    g_pkCurrAttribTable = 0;
                }
            ;}
    break;

  case 52:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Global Attribute Table End\n");
                DebugStringOut(g_acDSO);
                g_pkCurrAttribTable = 0;
            ;}
    break;

  case 53:

    {
        (yyval.ival) = 0;
    ;}
    break;

  case 54:

    {
        (yyval.ival) = (yyvsp[-1].ival);
    ;}
    break;

  case 67:

    {   (yyval.bval) = true;      ;}
    break;

  case 68:

    {   (yyval.bval) = false;     ;}
    break;

  case 69:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Bool(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        (yyvsp[0].bval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Bool at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                
                NiSprintf(g_acDSO, 1024, "    Boolean: %16s - %6s - %s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 70:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 71:

    {
                if (g_pkCurrAttribTable)
                {
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival),
                            (unsigned int)g_afLowValues[0], 
                            (unsigned int)g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_UnsignedInt(
                            (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            (unsigned int)(yyvsp[-1].ival)))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_UnsignedInt at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_acDSO, 1024, "       uint: %16s - %6s - %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_acDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_acDSO, 1024, "             Range: "
                        "[%4d..%4d]\n", 
                        (unsigned int)g_afLowValues[0], 
                        (unsigned int)g_afHighValues[0]);
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 72:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 73:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 1;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Float(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            *(g_afValues->GetBase()),
                            g_afLowValues[0], g_afHighValues[0]))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Float at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }

                NiSprintf(g_acDSO, 1024, 
                    "      Float: %16s - %6s - %8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    *(g_afValues->GetBase()));
                DebugStringOut(g_acDSO);

                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_acDSO, 1024, "             Range: "
                        "[%8.5f..%8.5f]\n",
                        g_afLowValues[0], g_afHighValues[0]);
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 74:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 75:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 2;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point2(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point2 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_acDSO, 1024, "     Point2: %16s - %6s - "
                    "%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1));
                DebugStringOut(g_acDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_acDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f)..(%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1],
                        g_afHighValues[0], g_afHighValues[1]);
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 76:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 77:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 3;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point3(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                }
            
                NiSprintf(g_acDSO, 1024, "     Point3: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2));
                DebugStringOut(g_acDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_acDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f)..(%8.5f,%8.5f,%8.5f)]"
                        "\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afHighValues[0],
                        g_afHighValues[1], g_afHighValues[2]);
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 78:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 79:

    {
                unsigned int uiExpectedLength = (((yyvsp[-3].ival) != 0) ? (yyvsp[-3].ival) : 1) * 4;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    // test for array
                    if ((yyvsp[-3].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4,
                            (yyvsp[-3].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Point4(
                            (yyvsp[-4].sval), 0, (yyvsp[-2].bval) ? false : true, 
                            g_afValues->GetBase(),
                            g_afLowValues, g_afHighValues))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Point4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-4].sval));
                        }
                    }

                }

                NiSprintf(g_acDSO, 1024, "     Point4: %16s - %6s - "
                    "%8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-4].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0), g_afValues->GetAt(1), 
                    g_afValues->GetAt(2), g_afValues->GetAt(3));
                DebugStringOut(g_acDSO);
                if (g_bRanged && !g_bGlobalAttributes)
                {
                    NiSprintf(g_acDSO, 1024, "             Range: "
                        "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                        "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                        g_afLowValues[0], g_afLowValues[1], 
                        g_afLowValues[2], g_afLowValues[3],
                        g_afHighValues[0], g_afHighValues[1], 
                        g_afHighValues[2], g_afHighValues[3]);
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 80:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 81:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 9;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix3(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix3 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }

                }

                NiSprintf(g_acDSO, 1024, 
                    "    Matrix3: %16s - %6s - %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_afValues->GetAt(0),
                    g_afValues->GetAt(1),
                    g_afValues->GetAt(2),
                    " ", " ",
                    g_afValues->GetAt(3),
                    g_afValues->GetAt(4),
                    g_afValues->GetAt(5),
                    " ", " ",
                    g_afValues->GetAt(6),
                    g_afValues->GetAt(7),
                    g_afValues->GetAt(8));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 82:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 83:

    {
                unsigned int uiExpectedLength = (((yyvsp[-2].ival) != 0) ? (yyvsp[-2].ival) : 1) * 16;
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < uiExpectedLength)
                {
                    // Pad out to the correct length with 0.0f's
                    for (; uiFoundLength < uiExpectedLength; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > uiExpectedLength)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected %d float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiExpectedLength, uiFoundLength);
                    
                    g_afValues->SetSize(uiExpectedLength);
                }

                if (g_pkCurrAttribTable)
                {
                    if ((yyvsp[-2].ival))
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Array(
                            (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4,
                            (yyvsp[-2].ival),
                            g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, 
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                    else
                    {
                        if (!g_pkCurrAttribTable->AddAttribDesc_Matrix4(
                                (yyvsp[-3].sval), 0, (yyvsp[-1].bval) ? false : true, 
                                g_afValues->GetBase()))
                        {
                            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                                true, "* PARSE ERROR: %s\n"
                                "    AddAttribDesc_Matrix4 at line %d\n"
                                "    Desc name = %s\n",
                                g_pkFile->GetFilename(), 
                                NSFParserGetLineNumber(), (yyvsp[-3].sval));
                        }
                    }
                }                

                NiSprintf(g_acDSO, 1024, 
                    "  Transform: %16s - %6s - %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n"
                    "             %16s   %6s   %8.5f,%8.5f,%8.5f,%8.5f\n",
                    (yyvsp[-3].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                            g_afValues->GetAt( 0), g_afValues->GetAt( 1), 
                            g_afValues->GetAt( 2), g_afValues->GetAt( 3),
                    " ", " ", g_afValues->GetAt( 4), g_afValues->GetAt( 5),
                            g_afValues->GetAt( 6), g_afValues->GetAt( 7),
                    " ", " ", g_afValues->GetAt( 8), g_afValues->GetAt( 9),
                            g_afValues->GetAt(10), g_afValues->GetAt(11),
                    " ", " ", g_afValues->GetAt(12), g_afValues->GetAt(13),
                            g_afValues->GetAt(14), g_afValues->GetAt(15));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 84:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 85:

    {
                // This one is a bit special, the expected length is 3 *or* 4
                unsigned int uiFoundLength = g_afValues->GetSize();
                if (uiFoundLength < 3)
                {
                    // Pad out to 3 if too short (with 0.0f's)
                    for (; uiFoundLength < 3; ++uiFoundLength)
                    {
                        g_afValues->Add(0.0f);
                    }
                }
                else if (uiFoundLength > 4)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s(%d):\n"
                        "    Expected 3 or 4 float values but found %d\n",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        uiFoundLength);

                    g_afValues->SetSize(4);
                }

                if (g_afValues->GetSize() == 3)
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_Color(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_Color at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_acDSO, 1024, "      Color: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0),
                        g_afValues->GetAt(1),
                        g_afValues->GetAt(2));
                    DebugStringOut(g_acDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_acDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2]);
                        DebugStringOut(g_acDSO);
                    }
                }
                else
                {
                    if (g_pkCurrAttribTable)
                    {
                        if (g_bRanged && !g_bGlobalAttributes)
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                        else
                        {
                            if (!g_pkCurrAttribTable->AddAttribDesc_ColorA(
                                (yyvsp[-3].sval), 0, 
                                (yyvsp[-2].bval) ? false : true, 
                                g_afValues->GetBase(),
                                g_afLowValues, g_afHighValues))
                            {
                                NiShaderFactory::ReportError(
                                    NISHADERERR_UNKNOWN, 
                                    true, "* PARSE ERROR: %s\n"
                                    "    AddAttribDesc_ColorA at line %d\n"
                                    "    Desc name = %s\n",
                                    g_pkFile->GetFilename(), 
                                    NSFParserGetLineNumber(), (yyvsp[-3].sval));
                            }
                        }
                    }                

                    NiSprintf(g_acDSO, 1024, "     ColorA: %16s - %6s - "
                        "%8.5f,%8.5f,%8.5f,%8.5f\n",
                        (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                        g_afValues->GetAt(0), g_afValues->GetAt(1), 
                        g_afValues->GetAt(2), g_afValues->GetAt(3));
                    DebugStringOut(g_acDSO);
                    if (g_bRanged && !g_bGlobalAttributes)
                    {
                        NiSprintf(g_acDSO, 1024, "             Range: "
                            "[(%8.5f,%8.5f,%8.5f,%8.5f).."
                            "(%8.5f,%8.5f,%8.5f,%8.5f)]\n",
                            g_afLowValues[0], g_afLowValues[1], 
                            g_afLowValues[2], g_afLowValues[3],
                            g_afHighValues[0], g_afHighValues[1], 
                            g_afHighValues[2], g_afHighValues[3]);
                        DebugStringOut(g_acDSO);
                    }
                }

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 86:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-3].sval), 0, (yyvsp[-2].bval) ? false : true, 
                        (unsigned int)(yyvsp[-1].ival), (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].bval));
                    }
                }                
                NiSprintf(g_acDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-3].sval), (yyvsp[-2].bval) ? "ARTIST" : "HIDDEN", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-3].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 87:

    {
                if (g_pkCurrAttribTable)
                {
                    if (!g_pkCurrAttribTable->AddAttribDesc_Texture(
                        (yyvsp[-2].sval), 0, (yyvsp[-1].bval) ? false : true, 
                        g_uiCurrTextureSlot, (yyvsp[0].sval)))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    AddAttribDesc_Texture at line %d\n"
                            "    Desc name = %s\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].bval));
                    }
                }                
                NiSprintf(g_acDSO, 1024, "    Texture: %16s - %6s - Slot %d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].bval) ? "ARTIST" : "HIDDEN", 
                    g_uiCurrTextureSlot);
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
                    
                g_uiCurrTextureSlot++;
            ;}
    break;

  case 88:

    {
                NiSprintf(g_acDSO, 1024, "Object Table Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                else
                {
                    g_pkCurrObjectTable = 0;
                }
            ;}
    break;

  case 89:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Object Table End\n");
                DebugStringOut(g_acDSO);
                g_pkCurrObjectTable = 0;
            ;}
    break;

  case 103:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_GeneralLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 104:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_PointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 105:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_DirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 106:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_SpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 107:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowPointLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 108:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowDirectionalLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 109:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ShadowSpotLight");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 110:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_EnvironmentMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 111:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedShadowMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 112:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_ProjectedLightMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 113:

    {
                // ObjectType, ObjectIndex, LocalName
                AddObjectToObjectTable(
                    NiShaderAttributeDesc::OT_EFFECT_FOGMAP,
                    (yyvsp[-1].ival), (yyvsp[0].sval), "Effect_FogMap");
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 114:

    {
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[-1].sval), true);
                }
                else
                {
                    g_pkCurrPackingDef = 0;
                }

                g_bCurrPDFixedFunction = false;
                
                NiSprintf(g_acDSO, 1024, "PackingDefinition Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 115:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "PackingDefinition End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrPackingDef = 0;
                
                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 116:

    {
                NiSprintf(g_acDSO, 1024, "Using PackingDefinition %s\n",(yyvsp[0].sval));
                DebugStringOut(g_acDSO);
                if (g_pkCurrNSBShader)
                {
                    NSBPackingDef* pkPackingDef = 
                        g_pkCurrNSBShader->GetPackingDef((yyvsp[0].sval), false);
                    if (!pkPackingDef)
                    {
                        NiSprintf(g_acDSO, 1024, "    WARNING: PackingDefinition %s "
                            "NOT FOUND\n",(yyvsp[0].sval));
                        DebugStringOut(g_acDSO);
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition %s\n"
                            "    at line %d\n"
                            "    NOT FOUND!\n",
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                    }
                    else
                    {
                        if (g_pkCurrImplementation)
                        {
                            g_pkCurrImplementation->SetPackingDef(
                                (yyvsp[0].sval));
                        }
                    }
                }

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 117:

    {   (yyval.ival) = 0x7fffffff;                          ;}
    break;

  case 118:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT1;        ;}
    break;

  case 119:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT2;        ;}
    break;

  case 120:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT3;        ;}
    break;

  case 121:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT4;        ;}
    break;

  case 122:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_UBYTECOLOR;    ;}
    break;

  case 123:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_SHORT1;        ;}
    break;

  case 124:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_SHORT2;        ;}
    break;

  case 125:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_SHORT3;        ;}
    break;

  case 126:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_SHORT4;        ;}
    break;

  case 127:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_UBYTE4;        ;}
    break;

  case 128:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMSHORT1;    ;}
    break;

  case 129:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMSHORT2;    ;}
    break;

  case 130:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMSHORT3;    ;}
    break;

  case 131:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMSHORT4;    ;}
    break;

  case 132:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMPACKED3;   ;}
    break;

  case 133:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_PBYTE1;        ;}
    break;

  case 134:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_PBYTE2;        ;}
    break;

  case 135:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_PBYTE3;        ;}
    break;

  case 136:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_PBYTE4;        ;}
    break;

  case 137:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT2H;       ;}
    break;

  case 138:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMUBYTE4;    ;}
    break;

  case 139:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMUSHORT2;   ;}
    break;

  case 140:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMUSHORT4;   ;}
    break;

  case 141:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_UDEC3;         ;}
    break;

  case 142:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_NORMDEC3;      ;}
    break;

  case 143:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT16_2;     ;}
    break;

  case 144:

    {   (yyval.ival) = NSBPackingDef::NSB_PD_FLOAT16_4;     ;}
    break;

  case 145:

    {
                NSFParsererror("Syntax Error: packing_definition_type");
                yyclearin;
            ;}
    break;

  case 146:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       ;}
    break;

  case 147:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION0;       ;}
    break;

  case 148:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION1;       ;}
    break;

  case 149:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION2;       ;}
    break;

  case 150:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION3;       ;}
    break;

  case 151:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION4;       ;}
    break;

  case 152:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION5;       ;}
    break;

  case 153:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION6;       ;}
    break;

  case 154:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_POSITION7;       ;}
    break;

  case 155:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;    ;}
    break;

  case 156:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;   ;}
    break;

  case 157:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_NORMAL;         ;}
    break;

  case 158:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 159:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_COLOR;          ;}
    break;

  case 160:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 161:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0;      ;}
    break;

  case 162:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1;      ;}
    break;

  case 163:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2;      ;}
    break;

  case 164:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3;      ;}
    break;

  case 165:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4;      ;}
    break;

  case 166:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5;      ;}
    break;

  case 167:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6;      ;}
    break;

  case 168:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7;      ;}
    break;

  case 169:

    {   (yyval.ival) =    0;                                                  ;}
    break;

  case 170:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_TANGENT;        ;}
    break;

  case 171:

    {   (yyval.ival) = NiShaderDeclaration::SHADERPARAM_NI_BINORMAL;       ;}
    break;

  case 172:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_DEFAULT;            ;}
    break;

  case 173:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_PARTIALU;           ;}
    break;

  case 174:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_PARTIALV;           ;}
    break;

  case 175:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_CROSSUV;            ;}
    break;

  case 176:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_UV;                 ;}
    break;

  case 177:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_LOOKUP;             ;}
    break;

  case 178:

    {   (yyval.ival) = NiShaderDeclaration::SPTESS_LOOKUPPRESAMPLED;   ;}
    break;

  case 179:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITION;      ;}
    break;

  case 180:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDWEIGHT;   ;}
    break;

  case 181:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BLENDINDICES;  ;}
    break;

  case 182:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_NORMAL;        ;}
    break;

  case 183:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_PSIZE;         ;}
    break;

  case 184:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TEXCOORD;      ;}
    break;

  case 185:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TANGENT;       ;}
    break;

  case 186:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_BINORMAL;      ;}
    break;

  case 187:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_TESSFACTOR;    ;}
    break;

  case 188:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_POSITIONT;     ;}
    break;

  case 189:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_COLOR;         ;}
    break;

  case 190:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_FOG;           ;}
    break;

  case 191:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_DEPTH;         ;}
    break;

  case 192:

    {   (yyval.ival) = NiShaderDeclaration::SPUSAGE_SAMPLE;        ;}
    break;

  case 195:

    {
                NSFParsererror("Syntax Error: packing_definition_entries");
                yyclearin;
            ;}
    break;

  case 196:

    {
                g_uiCurrPDStream    = (unsigned int)(yyvsp[0].ival);
                NiSprintf(g_acDSO, 1024, "Stream %d\n", g_uiCurrPDStream);
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 197:

    {
                g_bCurrPDFixedFunction = (yyvsp[0].bval);
                if (g_pkCurrPackingDef)
                    g_pkCurrPackingDef->SetFixedFunction((yyvsp[0].bval));
            ;}
    break;

  case 198:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);

                NSBPackingDef::NSBPackingDefEnum eType = 
                    (NSBPackingDef::NSBPackingDefEnum)(yyvsp[-3].ival);
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pcParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pcType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_acDSO, 1024, "    %16s %2d - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pcParam, (int)(yyvsp[-5].ival), uiRegister, pcType, eTess, 
                    eUsage, uiUsageIndex);
                DebugStringOut(g_acDSO);

                NiSprintf(g_acDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pcParam, (int)(yyvsp[-5].ival), uiRegister, pcType);
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 199:

    {
                unsigned int uiParam = 
                    NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;
                uiParam |= (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NSBPackingDefEnum eType = 
                    (NSBPackingDef::NSBPackingDefEnum)(yyvsp[0].ival);
                
                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(g_uiCurrPDStream, 
                        uiRegister, uiParam, eType,

                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }
                
                const char* pcParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pcType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_acDSO, 1024, "    %16s %2d - Reg %3d - %16s\n", 
                    pcParam, (int)(yyvsp[-2].ival), uiRegister, pcType);
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 200:

    {
                unsigned int uiParam = (yyvsp[-5].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-4].ival);
                NSBPackingDef::NSBPackingDefEnum eType = 
                    (NSBPackingDef::NSBPackingDefEnum)(yyvsp[-3].ival);
                NiShaderDeclaration::ShaderParameterTesselator eTess = 
                    (NiShaderDeclaration::ShaderParameterTesselator)
                    (yyvsp[-2].ival);
                NiShaderDeclaration::ShaderParameterUsage eUsage = 
                    (NiShaderDeclaration::ShaderParameterUsage)
                    (yyvsp[-1].ival);
                unsigned int uiUsageIndex = (unsigned int)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType, 
                        eTess, eUsage, uiUsageIndex))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pcParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pcType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_acDSO, 1024, "    %16s    - Reg %3d - %16s - "
                    "0x%08x, 0x%08x, 0x%08x\n", 
                    pcParam, uiRegister, pcType, eTess, eUsage, 
                    uiUsageIndex);
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 201:

    {
                unsigned int uiParam = (yyvsp[-2].ival);
                unsigned int uiRegister = (unsigned int)(yyvsp[-1].ival);
                NSBPackingDef::NSBPackingDefEnum eType = 
                    (NSBPackingDef::NSBPackingDefEnum)(yyvsp[0].ival);

                // Add the entry to the current stream
                if (g_pkCurrPackingDef)
                {
                    if (!g_pkCurrPackingDef->AddPackingEntry(
                        g_uiCurrPDStream, uiRegister, uiParam, eType,
                        NiShaderDeclaration::SPTESS_DEFAULT, 
                        NiShaderDeclaration::SPUSAGE_COUNT, 0))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    PackingDefinition failed AddPackingEntry\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                    }
                }

                const char* pcParam = NSBPackingDef::GetParameterName(
                    (NiShaderDeclaration::ShaderParameter)uiParam);
                const char* pcType = NSBPackingDef::GetTypeName(eType);

                NiSprintf(g_acDSO, 1024, "    %16s    - Reg %3d - %16s\n", 
                    pcParam, uiRegister, pcType);
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 202:

    {
                NiSprintf(g_acDSO, 1024, "SemanticAdapterTable List Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 203:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "SemanticAdapterTable List End\n");
                DebugStringOut(g_acDSO);

                //g_pkCurrRSGroup = 0;
            ;}
    break;

  case 206:

    {
                NiSprintf(g_acDSO, 1024, "SAT entry [%d elements map from "
                    "%s,%d to %s,%d,%d]\n", (yyvsp[-9].ival), (yyvsp[-8].sval), (yyvsp[-6].ival), (yyvsp[-5].sval), (yyvsp[-3].ival), (yyvsp[-1].ival));
                DebugStringOut(g_acDSO);

                if (g_pkCurrImplementation)
                {
                    NiSemanticAdapterTable& kTable =
                        g_pkCurrImplementation->GetSemanticAdapterTable();
                    unsigned int uiEntryID = kTable.GetFreeEntry();
                    
                    kTable.SetComponentCount(uiEntryID, (yyvsp[-9].ival));
                    kTable.SetGenericSemantic(uiEntryID, (yyvsp[-8].sval), (yyvsp[-6].ival));
                    kTable.SetRendererSemantic(uiEntryID, (yyvsp[-5].sval), (yyvsp[-3].ival));
                    kTable.SetSharedRendererSemanticPosition(uiEntryID, (yyvsp[-1].ival));
                }

                NiFree((yyvsp[-8].sval));
                NiFree((yyvsp[-5].sval));
            ;}
    break;

  case 207:

    {
                NiSprintf(g_acDSO, 1024, "RenderState List Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            
                // Check the pass first...    
                if (g_pkCurrPass)
                {
                    g_pkCurrRSGroup = g_pkCurrPass->GetRenderStateGroup();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrRSGroup = 
                        g_pkCurrImplementation->GetRenderStateGroup();
                }
                else
                {
                    g_pkCurrRSGroup = 0;
                }
            ;}
    break;

  case 208:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "RenderState List End\n");
                DebugStringOut(g_acDSO);

                g_pkCurrRSGroup = 0;
            ;}
    break;

  case 211:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, true);

                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 212:

    {
                if (g_bCurrStateValid)
                {
                    g_pkCurrRSGroup->SetState(g_uiCurrStateState, 
                        g_uiCurrStateValue, false);

                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 219:

    {
                NSFParsererror("Syntax Error: renderstate_entry");
                yyclearin;
            ;}
    break;

  case 220:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                }
                
                NiSprintf(g_acDSO, 1024, "    %32s = ATTRIBUTE - %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 221:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue; 
                    if (!NSBRenderStates::LookupRenderStateValue(eRS, 
                        (yyvsp[0].sval), uiValue))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    Invalid RenderStateValue (%s)\n"
                            "    at line %d\n", 
                            g_pkFile->GetFilename(), (yyvsp[0].sval), 
                            NSFParserGetLineNumber());
                        g_bCurrStateValid = false;
                    }
                    else
                    {
                        g_uiCurrStateState = (unsigned int)eRS;
                        g_uiCurrStateValue = uiValue;
                        g_bCurrStateValid = true;
                    }
                }

                NiSprintf(g_acDSO, 1024, "    %32s = %s", 
                    (yyvsp[-2].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 222:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].bval) ? 1 : 0;

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_acDSO, 1024, "    %32s = %s", (yyvsp[-2].sval), 
                    (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 223:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = F2DW((yyvsp[0].fval));
                    
                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }

                NiSprintf(g_acDSO, 1024, "    %32s = %8.5f", 
                    (yyvsp[-2].sval), (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 224:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].dword);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_acDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].dword));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 225:

    {
                NSBRenderStates::NSBRenderStateEnum eRS = 
                    NSBRenderStates::LookupRenderState((yyvsp[-2].sval));
                if (eRS == NSBRenderStates::NSB_RS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (eRS == (NSBRenderStates::NSB_RS_DEPRECATED))
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    DeprecatedRenderState (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[-2].sval), 
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    unsigned int uiValue = (yyvsp[0].ival);

                    g_uiCurrStateState = (unsigned int)eRS;
                    g_uiCurrStateValue = uiValue;
                    g_bCurrStateValid = true;
                }
                NiSprintf(g_acDSO, 1024, "    %32s = 0x%08x", 
                    (yyvsp[-2].sval), (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 235:

    {
                if (g_bConstantMapPlatformBlock)
                {
                    NSFParsererror("Syntax Error: "
                        "Embedded ConstantMap Platform-block");
                }
                NiSprintf(g_acDSO, 1024,"ConstantMap Platform-block Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                g_bConstantMapPlatformBlock = true;
            ;}
    break;

  case 236:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 237:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "ConstantMap Platform-block End\n");
                DebugStringOut(g_acDSO);
                g_bConstantMapPlatformBlock = false;
            ;}
    break;

  case 238:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        NULL);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, (unsigned int)(yyvsp[-2].ival), uiRegCount, 
                        NULL);
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NSBRequirements::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_acDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%3d %3d %3d\n", (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 239:

    {
                // Name, RegStart, StartBoneIndex, BoneCount
                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                unsigned int uiExtra = (unsigned int)(yyvsp[-1].ival) |
                    ((unsigned int)(yyvsp[0].ival) << 16);
                unsigned int uiRegCount = 3 * (unsigned int)(yyvsp[0].ival);
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, "SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry("SkinBoneMatrix3", uiFlags, 
                        uiExtra, -1, uiRegCount, (yyvsp[-2].sval));
                }

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                    if (g_pkCurrRequirements)
                    {
                        g_pkCurrRequirements->SetBoneMatrixRegisters(3);
                        g_pkCurrRequirements->SetBoneCalcMethod(
                            NSBRequirements::BONECALC_SKIN);
                    }
                }
                    
                NiSprintf(g_acDSO, 1024,
                    "    Defined: SkinBoneMatrix3          "
                    "%24s %3d %3d\n", (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    int((yyvsp[0].ival)));
                DebugStringOut(g_acDSO);
                
                NiFree((yyvsp[-2].sval))
            ;}
    break;

  case 240:

    {
                // Name, RegStart, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    (yyvsp[-2].sval)) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                            g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), 
                            (unsigned int)(yyvsp[-1].ival), 0, 0);
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), 
                            (unsigned int)(yyvsp[-1].ival), 0, 0);
                    }

                    NiSprintf(g_acDSO, 1024, "    Defined: %24s %3d %3d\n",
                        (yyvsp[-2].sval), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 241:

    {
                // Name, VariableName, Extra
                if (NiShaderConstantMap::LookUpPredefinedMappingType(
                    (yyvsp[-2].sval)) == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
                {
                    char acBuffer[1024];
                    NiSprintf(acBuffer, sizeof(acBuffer),
                        "PARSE ERROR: %s (line %d)\n"
                        "Predefined mapping %s is deprecated or invalid.",
                        g_pkFile->GetFilename(), NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    NSFParsererror(acBuffer);
                    yyclearin;
                }
                else
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_DEFINED;
                    if (g_bConstantMapPlatformBlock)
                    {
                        g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-2].sval), uiFlags, 
                        (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                    }
                    else
                    {
                        g_pkCurrConstantMap->AddEntry((yyvsp[-2].sval), uiFlags, 
                            (unsigned int)(yyvsp[0].ival), -1, 0, (yyvsp[-1].sval));
                    }

                    NiSprintf(g_acDSO, 1024,
                        "    Defined: %24s %3d %24s %3d\n",
                        (yyvsp[-2].sval), -1, (yyvsp[-1].sval), (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
                
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 242:

    {
                // CM_Object, NameOfObject, Parameter, RegStart
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }

                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Object name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }

                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), (unsigned int) (yyvsp[0].ival), 0, 0);
                }
                
                NiSprintf(g_acDSO, 1024, "    Object: %24s %16s %3d\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].ival));
                DebugStringOut(g_acDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 243:

    {
                // CM_Object, NameOfObject, Parameter, VariableName
                
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[-2].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval));
                    break;
                }
                
                // Verify mapping is supported by object type.
                unsigned int uiMapping;
                bool bSuccess = NiShaderConstantMap::LookUpObjectMapping((yyvsp[-1].sval),
                    uiMapping);
                if (bSuccess)
                {
                    bSuccess = NiShaderConstantMap::IsObjectMappingValidForType(
                        (NiShaderConstantMap::ObjectMappings) uiMapping,
                        pkDesc->GetType());
                }
                if (!bSuccess)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    IsObjectMappingValidForType at line %d\n"
                        "    Local name = %s\n"
                        "    Object mapping = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[-2].sval),
                        (yyvsp[-1].sval));
                    break;
                }
                
                unsigned int uiFlags =
                    NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                        pkDesc->GetType());
                
                size_t stBufSize = strlen((yyvsp[-1].sval)) + strlen((yyvsp[-2].sval)) + 3;
                char* pcKey = NiAlloc(char, stBufSize);
                NiSprintf(pcKey, stBufSize, "%s@@%s", (yyvsp[-1].sval), (yyvsp[-2].sval));
                
                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry(pcKey, uiFlags,
                        pkDesc->GetIndex(), -1, 0, (yyvsp[0].sval));
                }
                
                NiSprintf(g_acDSO, 1024, "    Object: %24s %16s %16s\n",
                    (yyvsp[-2].sval), (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);
                
                NiFree(pcKey);
                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
                
            ;}
    break;

  case 244:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 245:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-3].sval), (yyvsp[-2].ival), (yyvsp[-1].ival), (yyvsp[0].ival), false))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_acDSO, 1024, "     Attrib: %24s %3d %3d %3d\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), (int)(yyvsp[-1].ival), (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
                                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 246:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 247:

    {
                // Name, RegStart, RegCount, Values

                unsigned int uiFlags = 
                    NiShaderConstantMapEntry::SCME_MAP_CONSTANT |
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

                if (g_bConstantMapPlatformBlock)
                {
                    g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, (yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                else
                {
                    g_pkCurrConstantMap->AddEntry((yyvsp[-3].sval), uiFlags, 
                        0, (unsigned int)(yyvsp[-2].ival), 
                        (unsigned int)(yyvsp[-1].ival), NULL,
                        g_afValues->GetSize() * sizeof(float),
                        sizeof(float), g_afValues->GetBase(), true);
                }
                
                NiSprintf(g_acDSO, 1024, "      Const: %24s %3d %3d - %3d values\n",
                    (yyvsp[-3].sval), (int)(yyvsp[-2].ival), 
                    (int)(yyvsp[-1].ival), g_afValues->GetSize());
                DebugStringOut(g_acDSO);
                
                NiSprintf(g_acDSO, 1024, "             %24s         - ", " ");
                DebugStringOut(g_acDSO);
                
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    NiSprintf(g_acDSO, 1024, "%8.5f,", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui == (g_afValues->GetSize() - 1)))
                    {
                        NiStrcat(g_acDSO, 1024, "\n");
                    }
                    DebugStringOut(g_acDSO, false);
                    if ((((ui + 1) % 4) == 0) &&
                        (ui < (g_afValues->GetSize() - 1)))
                    {
                        NiSprintf(g_acDSO, 1024, "             %24s         - ", " ");
                        DebugStringOut(g_acDSO);
                    }
                }
                
                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 248:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 249:

    {
                // Name, RegStart, RegCount
                if (!AddAttributeToConstantMap((yyvsp[-2].sval), (yyvsp[-1].ival), (yyvsp[0].ival), 0, true))
                {
                    // Report the error
                    NSFParsererror(
                        "Syntax Error: AddAttributeToConstantMap!");
                    yyclearin;
                }

                NiSprintf(g_acDSO, 1024, "     Global: %24s %3d %3d\n",
                    (yyvsp[-2].sval), (int)(yyvsp[-1].ival), 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
                                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 250:

    {
                // Name, type, RegStart, RegCount, Entry1 * Entry2
                if (!SetupOperatorEntry((yyvsp[-7].sval), (yyvsp[-6].ival), 
                    (yyvsp[-5].ival), (yyvsp[-4].sval), (int)(yyvsp[-3].ival), 
                    (yyvsp[-2].sval), (yyvsp[-1].bval), (yyvsp[0].bval)))
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                    // Report the error
                    NSFParsererror("Syntax Error: SetupOperatorEntry!");
                    yyclearin;
                }
                else
                {
                    NiFree((yyvsp[-7].sval));
                    NiFree((yyvsp[-4].sval));
                    NiFree((yyvsp[-2].sval));
                }
            ;}
    break;

  case 251:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY;   ;}
    break;

  case 252:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE;     ;}
    break;

  case 253:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_ADD;        ;}
    break;

  case 254:

    {   (yyval.ival) = NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT;   ;}
    break;

  case 255:

    {
                NSFParsererror("Syntax Error: operator_type");
                yyclearin;
            ;}
    break;

  case 256:

    {   (yyval.bval) = false;     ;}
    break;

  case 257:

    {   (yyval.bval) = true;      ;}
    break;

  case 258:

    {   (yyval.bval) = false;     ;}
    break;

  case 259:

    {   (yyval.bval) = true;      ;}
    break;

  case 260:

    {
                NiGPUProgram::ProgramType eProgramType = 
                    DecodeShaderTypeString((yyvsp[0].sval));
                if (eProgramType == 
                    (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT)
                {
                    NiSprintf(g_acDSO, 1024, "Invalid shader type %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[0].sval));
                
                (yyval.ival) = eProgramType;
            ;}
    break;

  case 261:

    {
                NiSprintf(g_acDSO, 1024, "ConstantMap Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                NiGPUProgram::ProgramType eProgramType = (NiGPUProgram::ProgramType)(yyvsp[0].ival);

                g_pkCurrConstantMap = ObtainConstantMap(eProgramType);
                g_pkCurrConstantMap->SetName((yyvsp[-2].sval));
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 262:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "ConstantMap End\n");
                DebugStringOut(g_acDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 263:

    {
                NiSprintf(g_acDSO, 1024, "ConstantMap Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                g_pkCurrConstantMap = ObtainConstantMap(NiGPUProgram::PROGRAM_VERTEX);
                g_pkCurrConstantMap->SetName((yyvsp[-1].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 264:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "VertexShader ConstantMap End\n");
                DebugStringOut(g_acDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 265:

    {
                NiSprintf(g_acDSO, 1024, "ConstantMap Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                g_pkCurrConstantMap = ObtainConstantMap(NiGPUProgram::PROGRAM_GEOMETRY);
                g_pkCurrConstantMap->SetName((yyvsp[-1].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 266:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "GeometryShader ConstantMap End\n");
                DebugStringOut(g_acDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 267:

    {
                NiSprintf(g_acDSO, 1024, "ConstantMap Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                g_pkCurrConstantMap = ObtainConstantMap(NiGPUProgram::PROGRAM_PIXEL);
                g_pkCurrConstantMap->SetName((yyvsp[-1].sval));
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 268:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "PixelShader ConstantMap End\n");
                DebugStringOut(g_acDSO);

                g_pkCurrConstantMap = 0;
            ;}
    break;

  case 269:

    {
                if (g_pkCurrImplementation)
                    g_pkCurrImplementation->SetClassName((yyvsp[0].sval));

                NiSprintf(g_acDSO, 1024, "ClassName = %s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 270:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 271:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 272:

    {   (yyval.sval) = (yyvsp[0].sval);    ;}
    break;

  case 275:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    g_eCurrentShaderType);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 276:

    {
                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    g_eCurrentShaderType);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 277:

    {
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    g_uiCurrentPlatforms,
                    g_eCurrentShaderType);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 278:

    {
                if (g_pkCurrPass && g_eCurrentShaderType == NiGPUProgram::PROGRAM_VERTEX)
                {
                    g_pkCurrPass->SetSoftwareVertexProcessing((yyvsp[0].bval));
                }
            ;}
    break;

  case 279:

    {
                if (g_pkCurrPass && g_eCurrentShaderType == NiGPUProgram::PROGRAM_COMPUTE)
                {
                    g_pkCurrPass->SetComputeThreadGroupCounts((yyvsp[0].ival), 1, 1);
                }
            ;}
    break;

  case 280:

    {
                if (g_pkCurrPass && g_eCurrentShaderType == NiGPUProgram::PROGRAM_COMPUTE)
                {
                    g_pkCurrPass->SetComputeThreadGroupCounts((yyvsp[-1].ival), (yyvsp[0].ival), 1);
                }
            ;}
    break;

  case 281:

    {
                if (g_pkCurrPass && g_eCurrentShaderType == NiGPUProgram::PROGRAM_COMPUTE)
                {
                    g_pkCurrPass->SetComputeThreadGroupCounts((yyvsp[-2].ival), (yyvsp[-1].ival), (yyvsp[0].ival));
                }
            ;}
    break;

  case 282:

    {
                g_eCurrentShaderType = (NiGPUProgram::ProgramType)(yyvsp[0].ival);
                NiSprintf(g_acDSO, 1024, "ShaderProgram Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 283:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 284:

    {
                g_eCurrentShaderType = 
                    (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT;
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "ShaderProgram End\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 292:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 293:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 294:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 295:

    {
                // shader, entry point, shader target
                NiSprintf(g_acDSO, 1024, "VertexShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 296:

    {
                // shader, entry point
                NiSprintf(g_acDSO, 1024, "VertexShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_VERTEX);
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 297:

    {
                // shader
                NiSprintf(g_acDSO, 1024, "VertexShader File\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 298:

    {
                g_eCurrentShaderType = NiGPUProgram::PROGRAM_VERTEX;
                NiSprintf(g_acDSO, 1024, "VertexShader Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 299:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 300:

    {
                g_eCurrentShaderType = NiGPUProgram::PROGRAM_PIXEL;
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "VertexShader End\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 301:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 302:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 303:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 304:

    {
                // shader, entry point, shader target
                NiSprintf(g_acDSO, 1024, "GeometryShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 305:

    {
                // shader, entry point
                NiSprintf(g_acDSO, 1024, "GeometryShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_GEOMETRY);
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 306:

    {
                // shader
                NiSprintf(g_acDSO, 1024, "GeometryShader File\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 307:

    {
                g_eCurrentShaderType = NiGPUProgram::PROGRAM_GEOMETRY;
                NiSprintf(g_acDSO, 1024, "GeometryShader Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 308:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 309:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "GeometryShader End\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 310:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 311:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 312:

    {
                SetShaderProgramFile(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 313:

    {
                // shader, entry point, shader target
                NiSprintf(g_acDSO, 1024, "PixelShader File EP %s, ST %s\n", 
                    (yyvsp[-1].sval), (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[-1].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                SetShaderProgramShaderTarget(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                    
                NiFree((yyvsp[-1].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 314:

    {
                // shader, entry point
                NiSprintf(g_acDSO, 1024, "PixelShader File EP %s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                SetShaderProgramEntryPoint(g_pkCurrPass, (yyvsp[0].sval),
                    NiShader::NISHADER_AGNOSTIC,
                    NiGPUProgram::PROGRAM_PIXEL);
                    
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 315:

    {
                // shader
                NiSprintf(g_acDSO, 1024, "PixelShader File\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 316:

    {
                g_eCurrentShaderType = NiGPUProgram::PROGRAM_PIXEL;
                NiSprintf(g_acDSO, 1024, "PixelShader Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
            ;}
    break;

  case 317:

    {
                    g_uiCurrentPlatforms = (yyvsp[0].ival);
                ;}
    break;

  case 318:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "PixelShader End\n");
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 319:

    {
                NiSprintf(g_acDSO, 1024, "Requirement List Start\n");
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrImplementation)
                {
                    g_pkCurrRequirements = 
                        g_pkCurrImplementation->GetRequirements();
                }
            ;}
    break;

  case 320:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Requirement List End\n");
                DebugStringOut(g_acDSO);

                if (g_pkCurrNSBShader && g_pkCurrRequirements)
                {
                    // Update the shader requirements
                    g_pkCurrNSBShader->AddVertexShaderVersion(
                        g_pkCurrRequirements->GetVSVersion());
                    g_pkCurrNSBShader->AddGeometryShaderVersion(
                        g_pkCurrRequirements->GetGSVersion());
                    g_pkCurrNSBShader->AddPixelShaderVersion(
                        g_pkCurrRequirements->GetPSVersion());
                    g_pkCurrNSBShader->AddComputeShaderVersion(
                        g_pkCurrRequirements->GetCSVersion());
                    g_pkCurrNSBShader->AddUserVersion(
                        g_pkCurrRequirements->GetUserVersion());
                    g_pkCurrNSBShader->AddFeatureLevel(
                        g_pkCurrRequirements->GetFeatureLevel());
                    g_pkCurrNSBShader->AddPlatform(
                        g_pkCurrRequirements->GetPlatformFlags());
                }
                g_pkCurrRequirements = 0;
            ;}
    break;

  case 334:

    {
                NSFParsererror("Syntax Error: requirement_entry");
                yyclearin;
            ;}
    break;

  case 335:

    {
                NiSprintf(g_acDSO, 1024, "    VSVersion   0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetVSVersion((yyvsp[0].vers));
            ;}
    break;

  case 336:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the GS method here.
                unsigned int uiMaj = NSBSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = NSBSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = NSBGS_VERSION(uiMaj, uiMin);
            
                NiSprintf(g_acDSO, 1024, "    GSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetGSVersion(uiVers);
            ;}
    break;

  case 337:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the PS method here
                unsigned int uiMaj = NSBSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = NSBSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = NSBPS_VERSION(uiMaj, uiMin);

                NiSprintf(g_acDSO, 1024, "    PSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPSVersion(uiVers);
            ;}
    break;

  case 338:

    {
                // The N_VERSION method uses the VS method to set the value.
                // We need to flip it back to the CS method here
                unsigned int uiMaj = NSBSHADER_VERSION_MAJOR((yyvsp[0].vers));
                unsigned int uiMin = NSBSHADER_VERSION_MINOR((yyvsp[0].vers));
                unsigned int uiVers = NSBCS_VERSION(uiMaj, uiMin);

                NiSprintf(g_acDSO, 1024, "    CSVersion   0x%08x\n", uiVers);
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetCSVersion(uiVers);
            ;}
    break;

  case 339:

    {
                unsigned int uiFeatureLevel = 
                    DecodeFeatureLevelString((yyvsp[0].sval));
                if (uiFeatureLevel == 0)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE ERROR: %s\n"
                        "    InvalidFeatureLevel (%s)\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(), (yyvsp[0].sval), 
                        NSFParserGetLineNumber());
                }

                NiSprintf(g_acDSO, 1024, "    FeatureLevel   %s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetFeatureLevel(
                        (NSBRequirements::NSBFeatureLevel)uiFeatureLevel);
                }
                                
                NiFree((yyvsp[0].sval))
            ;}
    break;

  case 340:

    {
                NiSprintf(g_acDSO, 1024, "    UserDefined 0x%08x\n", (yyvsp[0].vers));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetUserVersion((yyvsp[0].vers));
            ;}
    break;

  case 341:

    {
                NiSprintf(g_acDSO, 1024, "       Platform 0x%08x\n", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetPlatformFlags((yyvsp[0].ival));
            ;}
    break;

  case 342:

    {
                (yyval.ival) = (yyvsp[-2].ival) | (yyvsp[0].ival);
            ;}
    break;

  case 343:

    {
                (yyval.ival) = (yyvsp[0].ival);
            ;}
    break;

  case 344:

    {
                unsigned int uiPlatform = DecodePlatformString((yyvsp[0].sval));
                if (uiPlatform == 0)
                {
                    NiSprintf(g_acDSO, 1024, "Invalid Platform %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_acDSO);
                }

                NiFree((yyvsp[0].sval));
                
                (yyval.ival) = uiPlatform;
            ;}
    break;

  case 345:

    {
                NiSprintf(g_acDSO, 1024, "%15s %s\n", (yyvsp[-2].sval),
                    (yyvsp[0].bval) ? "true" : "false");
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                {
                    // Determine the requirement field
                    if (NiStricmp((yyvsp[-2].sval), "USESNIRENDERSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiRenderState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "USESNILIGHTSTATE") == 0)
                    {
                        g_pkCurrRequirements->SetUsesNiLightState(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPREQUIRED") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPRequired(
                            (yyvsp[0].bval));
                    }
                    else if (NiStricmp((yyvsp[-2].sval), "SOFTWAREVPFALLBACK") == 
                        0)
                    {
                        g_pkCurrRequirements->SetSoftwareVPAcceptable(
                            (yyvsp[0].bval));
                    }
                }
                
                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 346:

    {
                NiSprintf(g_acDSO, 1024, "Bones/Partition %d\n", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                    g_pkCurrRequirements->SetBonesPerPartition((yyvsp[0].ival));
            ;}
    break;

  case 347:

    {
                NiSprintf(g_acDSO, 1024, "BinormalTangentUVSource %d\n", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentUVSource((yyvsp[0].ival));
                }
            ;}
    break;

  case 348:

    {
                NiSprintf(g_acDSO, 1024, "BinormalTangent %d\n", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                if (g_pkCurrRequirements)
                {
                    g_pkCurrRequirements->SetBinormalTangentMethod(
                        (NiShaderRequirementDesc::NBTFlags)(yyvsp[0].ival));
                }
            ;}
    break;

  case 349:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NONE;   ;}
    break;

  case 350:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NDL;    ;}
    break;

  case 351:

    {   (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_ATI;    ;}
    break;

  case 352:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    Deprecated value for BinormalTangentMethod "
                    "Requirement\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                (yyval.ival) = NiShaderRequirementDesc::NBT_METHOD_NDL;    
            ;}
    break;

  case 353:

    {
                NSFParsererror("Syntax Error: binormaltanget_method");
                yyclearin;
            ;}
    break;

  case 357:

    {
                NiSprintf(g_acDSO, 1024, "Stage Start %3d - %s\n", (yyvsp[-2].ival), 
                    (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    if (g_pkCurrTextureStage)
                    {
                        g_pkCurrTextureStage->SetUseTextureTransformation(
                            false);
                    }
                }
            ;}
    break;

  case 358:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Stage End %3d - %s\n", (int)(yyvsp[-5].ival), 
                    (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 366:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, true);
                        }
                    }
                
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 367:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetTextureStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false);
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 387:

    {
                NSFParsererror("Syntax Error: stage_entry");
                yyclearin;
            ;}
    break;

  case 388:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetNDLMap((yyvsp[0].ival));
            ;}
    break;

  case 389:

    {
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetDecalMap((yyvsp[0].ival));
            ;}
    break;

  case 390:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pcValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pcValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTextureStage)
                                g_pkCurrTextureStage->SetShaderMap(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTextureStage)
                            {
                                g_pkCurrTextureStage->SetObjTextureSettings(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 391:

    {           
        NSBObjectTable::ObjectDesc* pkDesc = NULL;
        NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
        if (pkTable)
        {
            pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
        }
        if (!pkDesc)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                false,
                "* PARSE ERROR: %s\n"
                "    GetObjectByName at line %d\n"
                "    Local name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(),
                (yyvsp[0].sval));
            NiFree((yyvsp[0].sval));                             
            break;
        }
        if (g_pkCurrTextureStage)
        {
            g_pkCurrTextureStage->SetObjTextureSettings(
                pkDesc->GetType(), pkDesc->GetIndex());
        }
                
        NiFree((yyvsp[0].sval));             
    ;}
    break;

  case 392:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_BASE;      ;}
    break;

  case 393:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_DARK;      ;}
    break;

  case 394:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_DETAIL;    ;}
    break;

  case 395:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_GLOSS;     ;}
    break;

  case 396:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_GLOW;      ;}
    break;

  case 397:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_BUMP;      ;}
    break;

  case 398:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_NORMAL;      ;}
    break;

  case 399:

    {   (yyval.ival) = NiTextureStage::TSTF_NDL_PARALLAX;      ;}
    break;

  case 400:

    {
                (yyval.ival) = NiTextureStage::TSTF_MAP_DECAL | (yyvsp[0].ival);
            ;}
    break;

  case 401:

    {
                NiSprintf(g_acDSO, 1024, "ColorOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_COLOROP;
                g_uiCurrStateValue = (yyvsp[0].ival);  
                g_bCurrStateValid = true;  
            ;}
    break;

  case 402:

    {
                NiSprintf(g_acDSO, 1024, "ColorArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_COLORARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 403:

    {
                NiSprintf(g_acDSO, 1024, "ColorArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_COLORARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 404:

    {
                NiSprintf(g_acDSO, 1024, "ColorArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_COLORARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 405:

    {
                NiSprintf(g_acDSO, 1024, "AlphaOp            = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_ALPHAOP;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 406:

    {
                NiSprintf(g_acDSO, 1024, "AlphaArg0          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_ALPHAARG0;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 407:

    {
                NiSprintf(g_acDSO, 1024, "AlphaArg1          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_ALPHAARG1;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 408:

    {
                NiSprintf(g_acDSO, 1024, "AlphaArg2          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_ALPHAARG2;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 409:

    {
                NiSprintf(g_acDSO, 1024, "ResultArg          = 0x%08x", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_RESULTARG;
                g_uiCurrStateValue = (yyvsp[0].ival);    
                g_bCurrStateValid = true;  
            ;}
    break;

  case 410:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_INVALID;
                g_uiCurrStateValue = (yyvsp[0].dword);
                    
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedTextureStageState (TSS_CONSTANT)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(),
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;  
            ;}
    break;

  case 411:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat00       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT00;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 412:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat00       = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 413:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat01       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT01;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 414:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat01       = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 415:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat10       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT10;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 416:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat10       = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 417:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat11       = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVMAT11;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 418:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvMat11       = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 419:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvLScale      = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVLSCALE;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 420:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvLScale      = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 421:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvLOffset     = %8.5f", (yyvsp[0].fval));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_BUMPENVLOFFSET;
                g_uiCurrStateValue = F2DW((yyvsp[0].fval));
                g_bCurrStateValid = true;  
            ;}
    break;

  case 422:

    {
                NiSprintf(g_acDSO, 1024, "BumpEnvLOffset     = %s", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 423:

    {
                int iValue = (yyvsp[-1].ival) | (yyvsp[0].ival);
                NiSprintf(g_acDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  
            ;}
    break;

  case 424:

    {
                int iValue = (yyvsp[-1].ival);
                NiSprintf(g_acDSO, 1024, "TexCoordIndex      = 0x%08x", iValue);
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_TEXCOORDINDEX;
                g_uiCurrStateValue = iValue;
                g_bCurrStateValid = true;  

                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseIndexFromMap(true);
            ;}
    break;

  case 425:

    {
                NiSprintf(g_acDSO, 1024, "TextureTransformFlags= COUNT%d", 
                    (int)(yyvsp[-1].ival));
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[-1].ival) | 
                    (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 426:

    {
                NiSprintf(g_acDSO, 1024, "TextureTransformFlags= NSB_TTFF_PROJECTED");
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = (unsigned int)
                    NSBStageAndSamplerStates::NSB_TTFF_PROJECTED;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 427:

    {
                NiSprintf(g_acDSO, 1024, "TextureTransformFlags= DISABLE");
                DebugStringOut(g_acDSO);

                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_TSS_TEXTURETRANSFORMFLAGS;
                g_uiCurrStateValue = 
                    NSBStageAndSamplerStates::NSB_TTFF_DISABLE;
                g_bCurrStateValid = true;  
                
                if (g_pkCurrTextureStage)
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
            ;}
    break;

  case 428:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TTFF_COUNT1; ;}
    break;

  case 429:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TTFF_COUNT2; ;}
    break;

  case 430:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TTFF_COUNT3; ;}
    break;

  case 431:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TTFF_COUNT4; ;}
    break;

  case 432:

    {   (yyval.ival) = 0; ;}
    break;

  case 433:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TTFF_PROJECTED;  ;}
    break;

  case 434:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_DISABLE;                    
            ;}
    break;

  case 435:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_SELECTARG1;                 
            ;}
    break;

  case 436:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_SELECTARG2;                 
            ;}
    break;

  case 437:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATE;                   
            ;}
    break;

  case 438:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATE2X;                 
            ;}
    break;

  case 439:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATE4X;                 
            ;}
    break;

  case 440:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_ADD;                        
            ;}
    break;

  case 441:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_ADDSIGNED;                  
            ;}
    break;

  case 442:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_ADDSIGNED2X;                
            ;}
    break;

  case 443:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_SUBTRACT;                   
            ;}
    break;

  case 444:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_ADDSMOOTH;                  
            ;}
    break;

  case 445:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BLENDDIFFUSEALPHA;          
            ;}
    break;

  case 446:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BLENDTEXTUREALPHA;          
            ;}
    break;

  case 447:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BLENDFACTORALPHA;           
            ;}
    break;

  case 448:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BLENDTEXTUREALPHAPM;        
            ;}
    break;

  case 449:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BLENDCURRENTALPHA;          
            ;}
    break;

  case 450:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_PREMODULATE;                
            ;}
    break;

  case 451:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATEALPHA_ADDCOLOR;     
            ;}
    break;

  case 452:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATECOLOR_ADDALPHA;     
            ;}
    break;

  case 453:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATEINVALPHA_ADDCOLOR;
            ;}
    break;

  case 454:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MODULATEINVCOLOR_ADDALPHA;  
            ;}
    break;

  case 455:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BUMPENVMAP;                 
            ;}
    break;

  case 456:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_BUMPENVMAPLUMINANCE;        
            ;}
    break;

  case 457:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_DOTPRODUCT3;                
            ;}
    break;

  case 458:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_MULTIPLYADD;                
            ;}
    break;

  case 459:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TOP_LERP;                       
            ;}
    break;

  case 460:

    {
                NSFParsererror("Syntax Error: stage_texture_operation");
                yyclearin;
            ;}
    break;

  case 461:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_CURRENT | 
                (yyvsp[0].ival); ;}
    break;

  case 462:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_DIFFUSE | 
                (yyvsp[0].ival); ;}
    break;

  case 463:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_SELECTMASK | 
                (yyvsp[0].ival); ;}
    break;

  case 464:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_SPECULAR | 
                (yyvsp[0].ival); ;}
    break;

  case 465:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_TEMP | 
                (yyvsp[0].ival); ;}
    break;

  case 466:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_TEXTURE | 
                (yyvsp[0].ival); ;}
    break;

  case 467:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_TFACTOR | 
                (yyvsp[0].ival); ;}
    break;

  case 468:

    {   (yyval.ival) = 0;     ;}
    break;

  case 469:

    {   (yyval.ival) = (yyvsp[0].ival);    ;}
    break;

  case 470:

    {
                (yyval.ival) = 
                    NSBStageAndSamplerStates::NSB_TA_ALPHAREPLICATE | 
                    NSBStageAndSamplerStates::NSB_TA_COMPLEMENT;
            ;}
    break;

  case 471:

    {
                (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_COMPLEMENT |
                    NSBStageAndSamplerStates::NSB_TA_ALPHAREPLICATE;
            ;}
    break;

  case 472:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_ALPHAREPLICATE;    ;}
    break;

  case 473:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TA_COMPLEMENT;        ;}
    break;

  case 474:

    {   (yyval.ival) = 
                0;
            ;}
    break;

  case 475:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TSI_PASSTHRU;
            ;}
    break;

  case 476:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TSI_CAMERASPACENORMAL;
            ;}
    break;

  case 477:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TSI_CAMERASPACEPOSITION;
            ;}
    break;

  case 478:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TSI_CAMERASPACEREFLECTIONVECTOR;
            ;}
    break;

  case 479:

    {   (yyval.ival) = 
                NSBStageAndSamplerStates::NSB_TSI_SPHEREMAP;
            ;}
    break;

  case 480:

    {
                NSFParsererror("Syntax Error: stage_texcoordindex_flags");
                yyclearin;
            ;}
    break;

  case 481:

    {
                // The set will be performed in the assignment section!
            ;}
    break;

  case 482:

    {
                unsigned int uiFlags = NiTextureStage::TSTTF_GLOBAL;
                
                uiFlags |= (int)(yyvsp[-1].ival);

                if (g_pkCurrTextureStage)
                {
                    const char* pcGlobalName = (yyvsp[0].sval);
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetGlobalName(pcGlobalName);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 483:

    {
                ResetFloatValueArray();
                ResetFloatRangeArrays();
            ;}
    break;

  case 484:

    {
                unsigned int uiFlags = NiTextureStage::TSTTF_CONSTANT;
        
                uiFlags |= (int)(yyvsp[-1].ival);

#ifndef _PS3
                if (g_pkCurrTextureStage)
                {
                    float afTrans[16];
                    
                    afTrans[ 0] = g_afValues->GetAt( 0);
                    afTrans[ 1] = g_afValues->GetAt( 1);
                    afTrans[ 2] = g_afValues->GetAt( 2);
                    afTrans[ 3] = g_afValues->GetAt( 3);
                    afTrans[ 4] = g_afValues->GetAt( 4);
                    afTrans[ 5] = g_afValues->GetAt( 5);
                    afTrans[ 6] = g_afValues->GetAt( 6);
                    afTrans[ 7] = g_afValues->GetAt( 7);
                    afTrans[ 8] = g_afValues->GetAt( 8);
                    afTrans[ 9] = g_afValues->GetAt( 9);
                    afTrans[10] = g_afValues->GetAt(10);
                    afTrans[11] = g_afValues->GetAt(11);
                    afTrans[12] = g_afValues->GetAt(12);
                    afTrans[13] = g_afValues->GetAt(13);
                    afTrans[14] = g_afValues->GetAt(14);
                    afTrans[15] = g_afValues->GetAt(15);
                    
                    g_pkCurrTextureStage->SetTextureTransformFlags(uiFlags);
                    g_pkCurrTextureStage->SetTextureTransformation(afTrans);
                    g_pkCurrTextureStage->SetUseTextureTransformation(true);
                }
#endif
                
            ;}
    break;

  case 485:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_NO_CALC;           ;}
    break;

  case 486:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_PARALLEL;    ;}
    break;

  case 487:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_PERSPECTIVE; ;}
    break;

  case 488:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_WORLD_SPHERE_MAP;  ;}
    break;

  case 489:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_CAMERA_SPHERE_MAP; ;}
    break;

  case 490:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_SPECULAR_CUBE_MAP; ;}
    break;

  case 491:

    {   (yyval.ival) = NiTextureStage::TSTTF_NI_DIFFUSE_CUBE_MAP;  ;}
    break;

  case 492:

    {
                NSFParsererror("Syntax Error: stage_textransmatrix_option");
                yyclearin;
            ;}
    break;

  case 493:

    {
                NiSprintf(g_acDSO, 1024, "Sampler Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage((int)(yyvsp[-2].ival));
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 494:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Sampler End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 495:

    {
                NiSprintf(g_acDSO, 1024, "Sampler Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTextureStage = 
                        g_pkCurrPass->GetStage(g_pkCurrPass->GetStageCount());
                    g_pkCurrTextureStage->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 496:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Sampler End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrTextureStage = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 501:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState,
                                g_uiCurrStateValue, true, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("    SAVE\n", false);
                }
            ;}
    break;

  case 502:

    {
                if (g_bCurrStateValid)
                {
                    if (g_pkCurrTextureStage)
                    {
                        NSBStateGroup* pkGroup = 
                            g_pkCurrTextureStage->GetSamplerStageGroup();
                        if (pkGroup)
                        {
                            pkGroup->SetState(g_uiCurrStateState, 
                                g_uiCurrStateValue, false, g_bUseMapValue);
                            g_bUseMapValue = false;
                        }
                    }
                    
                    DebugStringOut("\n", false);
                }
            ;}
    break;

  case 503:

    {
                // Do nothing. It's handles in the stage_texture block!
                DebugStringOut("Sampler Texture!\n");
            ;}
    break;

  case 522:

    {
                NSFParsererror("Syntax Error: sampler_entry");
                yyclearin;
            ;}
    break;

  case 523:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_ADDRESSU;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSU "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_acDSO, 1024, "AddressU             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }                
            ;}
    break;

  case 524:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_ADDRESSV;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSV "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_acDSO, 1024, "AddressV             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
            ;}
    break;

  case 525:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_ADDRESSW;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (g_bUseMapValue)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false, "* PARSE ERROR: %s\n"
                        "    UseMapValue not allowed for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else if (g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TADDRESS_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_ADDRESSW "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_acDSO, 1024, "AddressW             = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }                
            ;}
    break;

  case 526:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_BORDERCOLOR;
                g_uiCurrStateValue = (yyvsp[0].dword);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "BorderColor         = 0x%08x", 
                    (int)(yyvsp[0].dword));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 527:

    {
                NiSprintf(g_acDSO, 1024, "BorderColor         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 528:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MAGFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MAGFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;

                    NiSprintf(g_acDSO, 1024, "MagFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
            ;}
    break;

  case 529:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MINFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MINFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_acDSO, 1024, "MinFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
            ;}
    break;

  case 530:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MIPFILTER;
                g_uiCurrStateValue = (yyvsp[0].ival);
                if (!g_bUseMapValue && g_uiCurrStateValue == 
                    NSBStageAndSamplerStates::NSB_TEXF_INVALID)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* PARSE WARNING: %s\n"
                        "    Deprecated value for TSAMP_MIPFILTER "
                        "SamplerState\n"
                        "    at line %d\n", 
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber());
                    g_bCurrStateValid = false;
                }
                else
                {
                    g_bCurrStateValid = true;
                    
                    NiSprintf(g_acDSO, 1024, "MipFilter          = 0x%08x", 
                        (int)(yyvsp[0].ival));
                    DebugStringOut(g_acDSO);
                }
            ;}
    break;

  case 531:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MIPMAPLODBIAS;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "MipMapLODBias      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 532:

    {
                NiSprintf(g_acDSO, 1024, "MipMapLODBias      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 533:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MAXMIPLEVEL;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "MaxMipLevel        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 534:

    {
                NiSprintf(g_acDSO, 1024, "MaxMipLevel        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 535:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_MAXANISOTROPY;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "MaxAnisotropy      = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 536:

    {
                NiSprintf(g_acDSO, 1024, "MaxAnisotropy      = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 537:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_SRGBTEXTURE;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "SRGBTexture        = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 538:

    {
                NiSprintf(g_acDSO, 1024, "SRGBTexture        = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 539:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_ELEMENTINDEX;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "ElementIndex       = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 540:

    {
                NiSprintf(g_acDSO, 1024, "ElementIndex       = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 541:

    {
                g_uiCurrStateState = 
                    NSBStageAndSamplerStates::NSB_SAMP_DMAPOFFSET;
                g_uiCurrStateValue = (unsigned int)(yyvsp[0].ival);
                g_bCurrStateValid = true;
                
                NiSprintf(g_acDSO, 1024, "DMapOffset         = %d", 
                    (int)(yyvsp[0].ival));
                DebugStringOut(g_acDSO);
            ;}
    break;

  case 542:

    {
                NiSprintf(g_acDSO, 1024, "DMapOffset         = %s", 
                    (yyvsp[0].sval));
                DebugStringOut(g_acDSO);

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 543:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 544:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_ALPHAKILL)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 545:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYOP)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 546:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 547:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORSIGN)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 548:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;
            ;}
    break;

  case 549:

    {
                NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    true, "* PARSE WARNING: %s\n"
                    "    DeprecatedSamplerState (TSAMP_COLORKEYCOLOR)\n"
                    "    at line %d\n", 
                    g_pkFile->GetFilename(), 
                    NSFParserGetLineNumber());
                g_bCurrStateValid = false;

                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 550:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_WRAP;          ;}
    break;

  case 551:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_MIRROR;        ;}
    break;

  case 552:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_CLAMP;         ;}
    break;

  case 553:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_BORDER;        ;}
    break;

  case 554:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_MIRRORONCE;    ;}
    break;

  case 555:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_INVALID;       ;}
    break;

  case 556:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NSB_TADDRESS_INVALID;
            ;}
    break;

  case 557:

    {
                NSFParsererror("Syntax Error: sampler_texture_address");
                yyclearin;
            ;}
    break;

  case 558:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_NONE;              ;}
    break;

  case 559:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_POINT;             ;}
    break;

  case 560:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_LINEAR;            ;}
    break;

  case 561:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_ANISOTROPIC;       ;}
    break;

  case 562:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_PYRAMIDALQUAD;     ;}
    break;

  case 563:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_GAUSSIANQUAD;      ;}
    break;

  case 564:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_INVALID;           ;}
    break;

  case 565:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_INVALID;           ;}
    break;

  case 566:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_INVALID;           ;}
    break;

  case 567:

    {   (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_INVALID;           ;}
    break;

  case 568:

    {
                g_bUseMapValue = true;
                (yyval.ival) = NSBStageAndSamplerStates::NSB_TEXF_INVALID;
            ;}
    break;

  case 569:

    {
                NSFParsererror("Syntax Error: sampler_texture_filter");
                yyclearin;
            ;}
    break;

  case 570:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 571:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 572:

    {
                NSFParsererror("Syntax Error: sampler_texture_alphakill");
                yyclearin;
            ;}
    break;

  case 573:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 574:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 575:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 576:

    {   (yyval.ival) = 0x7fffffff;    ;}
    break;

  case 577:

    {
                NSFParsererror("Syntax Error: sampler_texture_colorkeyop");
                yyclearin;
            ;}
    break;

  case 578:

    {
                NiSprintf(g_acDSO, 1024, "Texture Start %3d - %s\n", 
                    (yyvsp[-2].ival), (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = g_pkCurrPass->GetTexture((int)(yyvsp[-2].ival));
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 579:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Texture End %3d - %s\n", 
                    (int)(yyvsp[-5].ival), (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrTexture = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 580:

    {
                NiSprintf(g_acDSO, 1024, "Texture Start - %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrPass)
                {
                    g_pkCurrTexture = 
                        g_pkCurrPass->GetTexture(g_pkCurrPass->GetTextureCount());
                    g_pkCurrTexture->SetName((yyvsp[-1].sval));
                }
            ;}
    break;

  case 581:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Texture End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrTexture = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 587:

    {
                NSFParsererror("Syntax Error: texture_entry");
                yyclearin;
            ;}
    break;

  case 588:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSource((yyvsp[0].ival));
            ;}
    break;

  case 589:

    {
                if (g_pkCurrTexture)
                    g_pkCurrTexture->SetSourceDecal((yyvsp[0].ival));
            ;}
    break;

  case 590:

    {
                bool bFoundAttribute = false;
                NSBObjectTable* pkObjectTable = 0;
                if (g_pkCurrNSBShader)
                {
                    g_bGlobalAttributes = false;
                    g_pkCurrAttribTable = 
                        g_pkCurrNSBShader->GetAttributeTable();
                    pkObjectTable = g_pkCurrNSBShader->GetObjectTable();
                }
                if (g_pkCurrAttribTable)
                {
                    NSBAttributeDesc* pkAttrib = 
                        g_pkCurrAttribTable->GetAttributeByName((yyvsp[0].sval));
                    if (pkAttrib)
                    {
                        bFoundAttribute = true;
                        
                        unsigned int uiValue;
                        const char* pcValue;
                        
                        if (pkAttrib->GetValue_Texture(uiValue, pcValue))
                        {
                            uiValue |= NiTextureStage::TSTF_MAP_SHADER;
                            if (g_pkCurrTexture)
                                g_pkCurrTexture->SetSourceShader(uiValue);
                        }
                        else
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    GetValue_Texture at line %d\n"
                                "    Attribute name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                    }
                    g_pkCurrAttribTable = 0;
                }
                
                if (!bFoundAttribute && pkObjectTable)
                {
                    NSBObjectTable::ObjectDesc* pkDesc =
                        pkObjectTable->GetObjectByName((yyvsp[0].sval));
                    if (pkDesc)
                    {
                        NiShaderAttributeDesc::ObjectType eObjectType =
                            pkDesc->GetType();
                        if (eObjectType <
                            NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
                            eObjectType >
                            NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
                        {
                            NiShaderFactory::ReportError(
                                NISHADERERR_UNKNOWN, true,
                                "* PARSE ERROR: %s\n"
                                "    InvalidObjectType at line %d\n"
                                "    Object name = %s\n",
                                g_pkFile->GetFilename(),
                                NSFParserGetLineNumber(),
                                (yyvsp[0].sval));
                        }
                        else
                        {
                            if (g_pkCurrTexture)
                            {
                                g_pkCurrTexture->SetSourceObject(
                                    eObjectType, pkDesc->GetIndex());
                            }
                        }
                    }
                    else
                    {
                        NiShaderFactory::ReportError(
                            NISHADERERR_UNKNOWN, true,
                            "* PARSE ERROR: %s\n"
                            "    TextureNotFound at line %d\n"
                            "    Attribute/Object name = %s\n",
                            g_pkFile->GetFilename(),
                            NSFParserGetLineNumber(),
                            (yyvsp[0].sval));
                    }
                }
                
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 591:

    {           
                NSBObjectTable::ObjectDesc* pkDesc = NULL;
                NSBObjectTable* pkTable = g_pkCurrNSBShader->GetObjectTable();
                if (pkTable)
                {
                    pkDesc = pkTable->GetObjectByName((yyvsp[0].sval));
                }
                if (!pkDesc)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        false,
                        "* PARSE ERROR: %s\n"
                        "    GetObjectByName at line %d\n"
                        "    Local name = %s\n",
                        g_pkFile->GetFilename(),
                        NSFParserGetLineNumber(),
                        (yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));                             
                    break;
                }
                if (g_pkCurrTexture)
                {
                    g_pkCurrTexture->SetSourceObject(
                        pkDesc->GetType(), pkDesc->GetIndex());
                }
                        
                NiFree((yyvsp[0].sval));             
            ;}
    break;

  case 603:

    {
                NiSprintf(g_acDSO, 1024, "Pass Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                
                if (g_pkCurrImplementation)
                {
                    g_pkCurrPass = 
                        g_pkCurrImplementation->GetPass(g_uiCurrPassIndex);
                }
            ;}
    break;

  case 604:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Pass End %s\n", (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrPass = 0;
                memset(
                    g_auiCurrPassConstantMap, 
                    0, 
                    sizeof(g_auiCurrPassConstantMap));
                g_uiCurrPassIndex++;

                NiFree((yyvsp[-4].sval));
            ;}
    break;

  case 620:

    {
                NiSprintf(g_acDSO, 1024, "Implementation Start %s\n", (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                if (g_pkCurrNSBShader)
                {
                    g_pkCurrImplementation = 
                        g_pkCurrNSBShader->GetImplementation((yyvsp[-1].sval), 
                            true, g_uiCurrImplementation);
                    if (g_pkCurrImplementation->GetIndex() == 
                        g_uiCurrImplementation)
                    {
                        g_uiCurrImplementation++;
                    }
                    g_uiCurrPassIndex = 0;
                }                    
            ;}
    break;

  case 621:

    {
                    NiSprintf(g_acDSO, 1024, "Description: %s\n", (yyvsp[0].sval));
                    DebugStringOut(g_acDSO);
                    
                    if (g_pkCurrImplementation)
                        g_pkCurrImplementation->SetDesc((yyvsp[0].sval));
                    NiFree((yyvsp[0].sval));
                ;}
    break;

  case 622:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "Implementation End %s\n", (yyvsp[-6].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrImplementation = 0;
                memset(
                    g_auiCurrPassConstantMap, 
                    0, 
                    sizeof(g_auiCurrPassConstantMap));
                
                NiFree((yyvsp[-6].sval));
            ;}
    break;

  case 623:

    {
            NIASSERT(g_pkCurrPass);
        ;}
    break;

  case 629:

    {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.SetStreamOutAppend((yyvsp[0].bval));
            ;}
    break;

  case 633:

    {
                NIASSERT(g_pkCurrPass);
                NiStreamOutSettings& kStreamOutSettings = 
                    g_pkCurrPass->GetStreamOutSettings();
                kStreamOutSettings.AppendStreamOutTargets((yyvsp[0].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 634:

    {
                // Create a new OSD and set its name
                g_pkCurrentOutputStreamDescriptor = 
                NiNew NiOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor->SetName((yyvsp[-1].sval));
                
                NiFree((yyvsp[-1].sval));
            ;}
    break;

  case 635:

    {
                // Add the new OSD
                g_pkCurrNSBShader->AddOutputStreamDescriptor(
                    *g_pkCurrentOutputStreamDescriptor);
            
                NiDelete g_pkCurrentOutputStreamDescriptor;
                g_pkCurrentOutputStreamDescriptor = NULL;
            ;}
    break;

  case 641:

    {
                // note: 0 = same # of verts as in source mesh.
                g_pkCurrentOutputStreamDescriptor->SetMaxVertexCount((yyvsp[0].ival));
            ;}
    break;

  case 642:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            ;}
    break;

  case 643:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_MAX;
            ;}
    break;

  case 646:

    {
                NiSprintf(g_acDSO, 1024, "vertex format entry [%d elements "
                    "using %s,%d]\n", (yyvsp[-4].ival), (yyvsp[-3].sval), (yyvsp[-1].ival));
                DebugStringOut(g_acDSO);

                NiOutputStreamDescriptor::VertexFormatEntry kEntry;
                
                NIASSERT(
                    g_eDataType != NiOutputStreamDescriptor::DATATYPE_MAX);
                kEntry.m_eDataType = g_eDataType;
                kEntry.m_uiComponentCount = ((yyvsp[-4].ival));
                kEntry.m_kSemanticName = ((yyvsp[-3].sval));
                kEntry.m_uiSemanticIndex = ((yyvsp[-1].ival));
                g_pkCurrentOutputStreamDescriptor->AppendVertexFormat(kEntry);

                NiFree((yyvsp[-3].sval));
            ;}
    break;

  case 650:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_FLOAT;
            ;}
    break;

  case 651:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_INT;
            ;}
    break;

  case 652:

    {
                g_eDataType = NiOutputStreamDescriptor::DATATYPE_UINT;
            ;}
    break;

  case 657:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_POINTS);
            ;}
    break;

  case 658:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_LINES);
            ;}
    break;

  case 659:

    {
                g_pkCurrentOutputStreamDescriptor->SetPrimType(
                    NiPrimitiveType::PRIMITIVE_TRIANGLES);
            ;}
    break;

  case 660:

    {
                NiSprintf(g_acDSO, 1024, "Integer............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_acDSO, 1024, "%d\n", (yyvsp[0].ival));
                DebugStringOut(g_acDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiValue = (unsigned int)(yyvsp[0].ival);
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(unsigned int), sizeof(unsigned int), 
                        (void*)&uiValue, true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 661:

    {
                NiSprintf(g_acDSO, 1024, "Boolean............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_acDSO, 1024, "%s\n", (yyvsp[0].bval) ? "TRUE" : "FALSE");
                DebugStringOut(g_acDSO);
                g_iDSOIndent -= 4;

                if (g_pkCurrUDDataBlock)
                {
                    bool bValue = (yyvsp[0].bval) ? true : false;
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(bool), sizeof(bool), (void*)&bValue, 
                        true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 662:

    {
                NiSprintf(g_acDSO, 1024, "Floats (%2d)........%s\n", 
                    g_afValues->GetSize(), (yyvsp[-2].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                for (unsigned int ui = 0; ui < g_afValues->GetSize(); ui++)
                {
                    if ((ui % 4) == 0)
                        DebugStringOut("");
                    NiSprintf(g_acDSO, 1024, "%-8.5f", g_afValues->GetAt(ui));
                    if ((((ui + 1) % 4) == 0) ||
                        (ui + 1 == g_afValues->GetSize()))
                    {
                        NiStrcat(g_acDSO, 1024, "\n");
                    }
                    DebugStringOut(g_acDSO, false);
                }
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 0;
                    
                    switch (g_afValues->GetSize())
                    {
                    case 1:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                        break;
                    case 2:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                        break;
                    case 3:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                        break;
                    case 4:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                        break;
                    case 8:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
                        break;
                    case 9:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                        break;
                    case 12:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
                        break;
                    case 16:
                        uiFlags = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                        break;
                    default:
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            false, "* %s(%d): Unsupported or unexpected "
                            "attribute size\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber());
                        break;
                    }
                    uiFlags |= 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;
                    
                    if (uiFlags == 0)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Invalid number of floats!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                    else if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(float) * g_afValues->GetSize(), 
                        sizeof(float), (void*)g_afValues->GetBase(), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                // Reset the float arrays, so any entries that follow this
                // one will be handled correctly
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                NiFree((yyvsp[-2].sval));
            ;}
    break;

  case 663:

    {
                NiSprintf(g_acDSO, 1024, "String.............%s\n", (yyvsp[-2].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;
                NiSprintf(g_acDSO, 1024, "%s\n", (yyvsp[0].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent -= 4;
                
                if (g_pkCurrUDDataBlock)
                {
                    unsigned int uiFlags = 
                        NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE | 
                        NiShaderAttributeDesc::ATTRIB_TYPE_STRING;
                    if (!g_pkCurrUDDataBlock->AddEntry((yyvsp[-2].sval), 
                        uiFlags, sizeof(char) * strlen((yyvsp[0].sval)), 
                        sizeof(char) * strlen((yyvsp[0].sval)), 
                        (void*)(yyvsp[0].sval), true))
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedData at line %d\n"
                            "    Name = %s\n"
                            "    Failed to add!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-2].sval));
                    }
                }                

                NiFree((yyvsp[-2].sval));
                NiFree((yyvsp[0].sval));
            ;}
    break;

  case 666:

    {
                NiSprintf(g_acDSO, 1024, "UserDefinedDataBlock Start %s\n", 
                    (yyvsp[-1].sval));
                DebugStringOut(g_acDSO);
                g_iDSOIndent += 4;

                // Reset the float arrays, then each one will reset them
                // when they are done being processed
                ResetFloatValueArray();
                ResetFloatRangeArrays();

                if (g_pkCurrPass)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrPass->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrImplementation)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrImplementation->GetUserDefinedDataSet();
                }
                else
                if (g_pkCurrNSBShader)
                {
                    g_pkCurrUDDataSet = 
                        g_pkCurrNSBShader->GetUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataSet = NiNew NSBUserDefinedDataSet();
                }

                if (!g_pkCurrUDDataSet)
                {
                    NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                        true, "* ALLOCATION ERROR: %s\n"
                        "    UserDefinedDataSet creation at line %d\n", 
                        g_pkFile->GetFilename(), 
                        NSFParserGetLineNumber());
                }
                else
                {
                    if (g_pkCurrPass)
                    {
                        g_pkCurrPass->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrImplementation)
                    {
                        g_pkCurrImplementation->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                    else
                    if (g_pkCurrNSBShader)
                    {
                        g_pkCurrNSBShader->SetUserDefinedDataSet(
                            g_pkCurrUDDataSet);
                    }
                }
                
                if (g_pkCurrUDDataSet)
                {
                    g_pkCurrUDDataBlock = 
                        g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), false);
                    if (g_pkCurrUDDataBlock)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                            true, "* PARSE ERROR: %s\n"
                            "    UserDefinedDataBlock at line %d\n"
                            "    Name = %s\n"
                            "    ALREADY EXISTS!\n",
                            g_pkFile->GetFilename(), 
                            NSFParserGetLineNumber(), (yyvsp[-1].sval));
                    }
                    else
                    {
                        g_pkCurrUDDataBlock = 
                            g_pkCurrUDDataSet->GetBlock((yyvsp[-1].sval), true);
                    }
                    NIASSERT(g_pkCurrUDDataBlock);
                }
            ;}
    break;

  case 667:

    {
                g_iDSOIndent -= 4;
                NiSprintf(g_acDSO, 1024, "UserDefinedDataBlock End %s\n", 
                    (yyvsp[-4].sval));
                DebugStringOut(g_acDSO);

                g_pkCurrUDDataSet = 0;
                g_pkCurrUDDataBlock = 0;

                NiFree((yyvsp[-4].sval));
            ;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}





//------------------------------------------------------------------------------------------------
int iErrors = 0;
extern void NSFParserResetLineNumber();
extern void NSFParserReset(); 
extern void NSFParserReleaseBuffer();
//------------------------------------------------------------------------------------------------
int ParseShader(const char* pcFileName)
{ 
    NSFParserResetLineNumber();
    g_pkFile = NiNew NSFTextFile();

    NSFParserlval.fval = 0.0f;
    NSFParserlval.ival = 0;
    NSFParserlval.sval = 0;
    NSFParserlval.dword = 0;
    NSFParserlval.word = 0;
    NSFParserlval.byte = 0;
    NSFParserlval.bval = false;

    if (!pcFileName)
    {
        NiDelete g_pkFile;
        NiSprintf(g_acDSO, 1024, "err: need input file\n");
        DebugStringOut(g_acDSO);
        return -1;
    };

    NSFParsedShader* pkParsedShader;
    NiTListIterator    pos = g_kParsedShaderList.GetHeadPos();
    while (pos)
    {
        pkParsedShader = g_kParsedShaderList.GetNext(pos);
        NiDelete pkParsedShader;
    }
    g_kParsedShaderList.RemoveAll();
    g_pkCurrShader = 0;

    if (g_pkFile->Load(pcFileName) != 0)
    {
        NiDelete g_pkFile;
        NiSprintf(g_acDSO, 1024, "err: file not found!\n");
        DebugStringOut(g_acDSO);
        return -1;
    }

    // create array
    g_afValues = NiNew NiTPrimitiveArray<float>;

    iErrors = -1;
    yyparse ();
    if (iErrors != -1)
    {
        iErrors++;
        printf ( "*FAILURE! %d errors found.\n" , iErrors);
        NiSprintf(g_acDSO, 1024, "*FAILURE! %d errors found.\n" , iErrors);
        DebugStringOut(g_acDSO);

        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
            false, "* PARSE ERROR: %s\n"
            "    FAILED - %d errors found\n",
            g_pkFile->GetFilename(), iErrors);
    }
        
    NiDelete g_afValues;
    NiDelete g_pkFile;
    g_pkFile = 0;

    if (iErrors == -1)    
        return 0;

    return iErrors;
}
//------------------------------------------------------------------------------------------------
void ResetParser()
{
    NSFParserReset();
} 
//------------------------------------------------------------------------------------------------
void CleanupParser()
{
    NSFParserReleaseBuffer();
}
//------------------------------------------------------------------------------------------------
void DebugStringOut(const char* pcOut, bool bIndent)
{
    NI_UNUSED_ARG(bIndent);
    NI_UNUSED_ARG(pcOut);
#if defined(_ENABLE_DEBUG_STRING_OUT_)
    if (g_bFirstDSOFileAccess)
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "wt");
        g_bFirstDSOFileAccess = false;
    }
    else
    {
        g_pfDSOFile = fopen("NSFShaderParser.out", "at");
    }

    if (bIndent)
    {
        for (int ii = 0; ii < g_iDSOIndent; ii++)
        {
            printf(" ");
            NiOutputDebugString(" ");
            if (g_pfDSOFile)
                fprintf(g_pfDSOFile, " ");
        }
    }
    printf(pcOut);
    NiOutputDebugString(pcOut);
    if (g_pfDSOFile)
    {
        fprintf(g_pfDSOFile, pcOut);
        fclose(g_pfDSOFile);
    }
#endif    //#if defined(_ENABLE_DEBUG_STRING_OUT_)
}
//------------------------------------------------------------------------------------------------
unsigned int ResetFloatValueArray(void)
{
    g_afValues->RemoveAll();
    return g_afValues->GetSize();
}
//------------------------------------------------------------------------------------------------
unsigned int AddFloatToValueArray(float fValue)
{
    g_afValues->Add(fValue);
    return g_afValues->GetSize();
}
//------------------------------------------------------------------------------------------------
void ResetFloatRangeArrays(void)
{
    g_uiLowFloatValues    = 0;
    g_uiHighFloatValues    = 0;

    g_afLowValues[0] = 0.0f;
    g_afLowValues[1] = 0.0f;
    g_afLowValues[2] = 0.0f;
    g_afLowValues[3] = 0.0f;
    g_afHighValues[0] = 0.0f;
    g_afHighValues[1] = 0.0f;
    g_afHighValues[2] = 0.0f;
    g_afHighValues[3] = 0.0f;
}
//------------------------------------------------------------------------------------------------
unsigned int AddFloatToLowArray(float fValue)
{
    if ((g_uiLowFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("Low Float array overflow!");
        return 0;
    }
    g_afLowValues[g_uiLowFloatValues++] = fValue;
    return g_uiLowFloatValues;
}
//------------------------------------------------------------------------------------------------
unsigned int AddFloatToHighArray(float fValue)
{
    if ((g_uiHighFloatValues + 1) == FLOAT_ARRAY_SIZE)
    {
        DebugStringOut("High Float array overfHigh!");
        return 0;
    }
    g_afHighValues[g_uiHighFloatValues++] = fValue;
    return g_uiHighFloatValues;
}
//------------------------------------------------------------------------------------------------
void AddObjectToObjectTable(NiShaderAttributeDesc::ObjectType eType,
    unsigned int uiIndex, const char* pcName, const char* pcDebugString)
{
    if (g_pkCurrObjectTable)
    {
        if (!g_pkCurrObjectTable->AddObject(pcName, eType, uiIndex))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                true, "* PARSE ERROR: %s\n"
                "    AddObject at line %d\n"
                "    Object name = %s\n",
                g_pkFile->GetFilename(),
                NSFParserGetLineNumber(), pcName);
        }
    }
    NiSprintf(g_acDSO, 1024, "    %24s: %d - %16s\n", pcDebugString, uiIndex,
        pcName);
    DebugStringOut(g_acDSO);
}
//------------------------------------------------------------------------------------------------
unsigned int DecodeAttribTypeString(char* pcAttribType)
{
    if (!pcAttribType || pcAttribType[0] == '\0')
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        
    // We need to look up the attribute in the attribute map, and then
    // return the type
    if (!g_pkCurrNSBShader)
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

    // Local attributes override global!
    g_pkCurrAttribTable = g_pkCurrNSBShader->GetAttributeTable();
    if (!g_pkCurrAttribTable)
    {
        NIASSERT(!"Invalid attribute table during parse!");
        return 0;
    }
    
    NSBAttributeDesc* pkAttribDesc = 
        g_pkCurrAttribTable->GetAttributeByName(pcAttribType);
    if (!pkAttribDesc)
    {
        g_pkCurrAttribTable = g_pkCurrNSBShader->GetGlobalAttributeTable();
        if (!g_pkCurrAttribTable)
        {
            NIASSERT(!"Invalid attribute table during parse!");
            return 0;
        }
        pkAttribDesc = g_pkCurrAttribTable->GetAttributeByName(pcAttribType);
        if (!pkAttribDesc)
        {
            NIASSERT(!"Attribute not found!");
            return 0;
        }
    }
    
    g_pkCurrAttribTable = 0;
    
    return (unsigned int)(pkAttribDesc->GetType());
}
//------------------------------------------------------------------------------------------------
unsigned int DecodePlatformString(char* pcPlatform)
{
    if (!pcPlatform || pcPlatform[0] == '\0')
        return 0;
    
    if (NiStricmp(pcPlatform, "DX9") == 0)
        return NiShader::NISHADER_DX9;
    if (NiStricmp(pcPlatform, "XENON") == 0)
        return NiShader::NISHADER_XENON;
    if (NiStricmp(pcPlatform, "PS3") == 0)
        return NiShader::NISHADER_PS3;
    if (NiStricmp(pcPlatform, "D3D10") == 0)
        return NiShader::NISHADER_D3D10;
    if (NiStricmp(pcPlatform, "D3D11") == 0)
        return NiShader::NISHADER_D3D11;

    return 0;
}
//------------------------------------------------------------------------------------------------
NiGPUProgram::ProgramType DecodeShaderTypeString(char* pcShaderType)
{
    if (!pcShaderType || pcShaderType[0] == '\0')
        return (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT;
    
    if (NiStricmp(pcShaderType, "VERTEX") == 0)
        return NiGPUProgram::PROGRAM_VERTEX;
    if (NiStricmp(pcShaderType, "HULL") == 0)
        return NiGPUProgram::PROGRAM_HULL;
    if (NiStricmp(pcShaderType, "DOMAIN") == 0)
        return NiGPUProgram::PROGRAM_DOMAIN;
    if (NiStricmp(pcShaderType, "GEOMETRY") == 0)
        return NiGPUProgram::PROGRAM_GEOMETRY;
    if (NiStricmp(pcShaderType, "PIXEL") == 0)
        return NiGPUProgram::PROGRAM_PIXEL;
    if (NiStricmp(pcShaderType, "COMPUTE") == 0)
        return NiGPUProgram::PROGRAM_COMPUTE;
        
    return (NiGPUProgram::ProgramType)NSBConstantMap::NSB_SHADER_TYPE_COUNT;
}

//------------------------------------------------------------------------------------------------
unsigned int DecodeFeatureLevelString(char* pcFeatureLevel)
{
    if (!pcFeatureLevel || pcFeatureLevel[0] == '\0')
        return 0;
        
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_9_1") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_9_1;
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_9_2") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_9_2;
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_9_3") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_9_3;
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_10_0") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_10_0;
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_10_1") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_10_1;
    if (NiStricmp(pcFeatureLevel, "FEATURE_LEVEL_11_0") == 0)
        return NSBRequirements::NSB_FEATURE_LEVEL_11_0;
        
    return 0;
}

//------------------------------------------------------------------------------------------------
NSBConstantMap* ObtainConstantMap(NiGPUProgram::ProgramType eProgramType)
{
    NSBConstantMap* pkMap = NULL;

    if (eProgramType < NSBConstantMap::NSB_SHADER_TYPE_COUNT)
    {
        // Check the pass first...    
        if (g_pkCurrPass)
        {
            pkMap = g_pkCurrPass->GetConstantMap(
                eProgramType,
                g_auiCurrPassConstantMap[eProgramType]);
            if (pkMap == NULL)
            {
                g_auiCurrPassConstantMap[eProgramType] = g_pkCurrPass->
                    AddConstantMap(eProgramType);
                pkMap = g_pkCurrPass->GetConstantMap(
                    eProgramType,
                    g_auiCurrPassConstantMap[eProgramType]);
            }
        }
        else if (g_pkCurrImplementation)
        {
            pkMap = g_pkCurrImplementation->GetConstantMap(
                eProgramType,
                g_auiCurrImplemConstantMap[eProgramType]);
            if (pkMap == NULL)
            {
                g_auiCurrImplemConstantMap[eProgramType] = g_pkCurrImplementation->
                    AddConstantMap(eProgramType);
                pkMap = g_pkCurrImplementation->GetConstantMap(
                    eProgramType,
                    g_auiCurrImplemConstantMap[eProgramType]);
            }
        }
    }
    
    return pkMap;
}
//------------------------------------------------------------------------------------------------
void SetShaderProgramFile(NSBPass* pkPass, const char* pcFile,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D11)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_D3D11, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramFile(pcFile,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//------------------------------------------------------------------------------------------------
void SetShaderProgramEntryPoint(NSBPass* pkPass, const char* pcEntryPoint,
    unsigned int uiPlatforms, NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D11)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_D3D11, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramEntryPoint(pcEntryPoint,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//------------------------------------------------------------------------------------------------
void SetShaderProgramShaderTarget(NSBPass* pkPass,
    const char* pcShaderTarget, unsigned int uiPlatforms,
    NiGPUProgram::ProgramType eType)
{
    if (!pkPass)
        return;

    if (uiPlatforms & NiShader::NISHADER_DX9)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_DX9, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D10)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_D3D10, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_D3D11)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_D3D11, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_PS3)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_PS3, eType);
    }
    if (uiPlatforms & NiShader::NISHADER_XENON)
    {
        pkPass->SetShaderProgramShaderTarget(pcShaderTarget,
            NiSystemDesc::RENDERER_XENON, eType);
    }
}
//------------------------------------------------------------------------------------------------
bool AddAttributeToConstantMap(char* pcName, 
    unsigned int uiRegisterStart, unsigned int uiRegisterCount, 
    unsigned int uiExtraNumber, bool bIsGlobal)
{
    if (!g_pkCurrConstantMap)
        return false;
    
    // Cheat to force a copy of the data
    unsigned int uiFlags = bIsGlobal
        ? NiShaderConstantMapEntry::SCME_MAP_GLOBAL
        : NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;

    unsigned int uiSize;

    // Look up the global attribute
    if (!g_pkCurrNSBShader)
    {
        return false;
    }
    else
    {
        if (bIsGlobal)
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetGlobalAttributeTable();
        }
        else
        {
            g_pkCurrAttribTable = 
                g_pkCurrNSBShader->GetAttributeTable();
        }

        NSBAttributeDesc* pkAttribDesc = 
            g_pkCurrAttribTable->GetAttributeByName(pcName);

        if (!pkAttribDesc)
        {
            return false;
        }
        else
        {
            if (NiShaderConstantMapEntry::IsBool(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                uiSize = sizeof(bool);
                bool bValue;
                
                pkAttribDesc->GetValue_Bool(bValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&bValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsUnsignedInt(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                uiSize = sizeof(unsigned int);
                unsigned int uiValue;
                
                pkAttribDesc->GetValue_UnsignedInt(uiValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&uiValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsFloat(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                uiSize = sizeof(float);
                float fValue;
                
                pkAttribDesc->GetValue_Float(fValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&fValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint2(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                uiSize = sizeof(NiPoint2);
                NiPoint2 kPt2Value;
                
                pkAttribDesc->GetValue_Point2(kPt2Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt2Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                uiSize = sizeof(NiPoint3);
                NiPoint3 kPt3Value;
                
                pkAttribDesc->GetValue_Point3(kPt3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount, 
                        (char*)0, uiSize, uiSize, (void*)&kPt3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsPoint4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                uiSize = sizeof(float) * 4;
                float afValue[4];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Point4(pfValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix3(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                uiSize = sizeof(NiMatrix3);
                NiMatrix3 kMat3Value;
                
                pkAttribDesc->GetValue_Matrix3(kMat3Value);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber, 
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kMat3Value, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsMatrix4(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                uiSize = sizeof(float) * 16;
                float afValue[16];
                float* pfValue = &afValue[0];
                
                pkAttribDesc->GetValue_Matrix4(pfValue, uiSize);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)afValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsColor(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                uiSize = sizeof(NiColorA);
                NiColorA kClrValue;
                
                pkAttribDesc->GetValue_ColorA(kClrValue);
                
                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0,
                        uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags,
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiSize, (void*)&kClrValue, true))
                    {
                        // PROBLEM!
                        return false;
                    }
                }
                
                return true;
            }
            else
            if (NiShaderConstantMapEntry::IsArray(
                pkAttribDesc->GetType()))
            {
                uiFlags |= 
                    NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;

                // get description of array data
                NiShaderAttributeDesc::AttributeType eType;
                unsigned int uiElementSize;
                unsigned int uiNumElements;
                pkAttribDesc->GetArrayParams(
                    eType,
                    uiElementSize,
                    uiNumElements);

                // get copy of data
                uiSize = uiElementSize*uiNumElements;
                float* pfValues = NiAlloc(float,uiSize/sizeof(float));
                pkAttribDesc->GetValue_Array(pfValues,uiSize);

                if (g_bConstantMapPlatformBlock)
                {
                    if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
                        g_uiCurrentPlatforms, pcName, uiFlags, uiExtraNumber,
                        uiRegisterStart, uiRegisterCount, (char*)0, 
                        uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM
                        NiFree(pfValues);
                        return false;
                    }
                }
                else
                {
                    if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, 
                        uiExtraNumber, uiRegisterStart, uiRegisterCount,
                        (char*)0, uiSize, uiElementSize, pfValues, true))
                    {
                        // PROBLEM!
                        NiFree(pfValues);
                        return false;
                    }
                }
                
                NiFree(pfValues);
                return true;
            }
            else
            {
                NIASSERT(!"Invalid Attribute Type");
                return false;
            }
        }
    }            
}
//------------------------------------------------------------------------------------------------
bool SetupOperatorEntry(char* pcName, int iRegStart, int iRegCount, 
    char* pcEntry1, int iOperation, char* pcEntry2, bool bInverse, 
    bool bTranspose)
{
    if (!g_pkCurrConstantMap)
        return false;

    // Look up the 2 entries
    NSBConstantMap::NSBCM_Entry* pkEntry1;
    NSBConstantMap::NSBCM_Entry* pkEntry2;
    
    pkEntry1 = g_pkCurrConstantMap->GetEntryByKey(pcEntry1);
    pkEntry2 = g_pkCurrConstantMap->GetEntryByKey(pcEntry2);

    if (!pkEntry1 || !pkEntry2)    
    {
        NSFParsererror("CM_Operator operand not found\n");
        return false;
    }
    if (!(pkEntry1->IsDefined() || pkEntry1->IsGlobal() ||
          pkEntry1->IsAttribute() || pkEntry1->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }
    if (!(pkEntry2->IsDefined() || pkEntry2->IsGlobal() ||
          pkEntry2->IsAttribute() || pkEntry2->IsConstant()))
    {
        NSFParsererror("CM_Operator operand INVALID TYPE\n");
        return false;
    }

    unsigned int uiEntry1;
    unsigned int uiEntry2;
    
    uiEntry1 = g_pkCurrConstantMap->GetEntryIndexByKey(pcEntry1);
    uiEntry2 = g_pkCurrConstantMap->GetEntryIndexByKey(pcEntry2);

    if ((uiEntry1 == 0xffffffff) || (uiEntry2 == 0xffffffff))    
    {
        NSFParsererror("CM_Operator operand INVALID INDEX\n");
        return false;
    }

    // Determine the results data type and set it in the flags
    NiShaderAttributeDesc::AttributeType eType1 = 
        pkEntry1->GetAttributeType();
    NiShaderAttributeDesc::AttributeType eType2 = 
        pkEntry2->GetAttributeType();

    if (pkEntry1->IsDefined())
    {
        // We have to look-up the type
        eType1 = NiShaderConstantMap::LookUpPredefinedMappingType(
            pkEntry1->GetKey());
    }
    if (pkEntry2->IsDefined())
    {
        eType2 = NiShaderConstantMap::LookUpPredefinedMappingType(
            pkEntry2->GetKey());
    }
        
    if ((eType1 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType1) ||
        NiShaderConstantMapEntry::IsString(eType1) ||
        NiShaderConstantMapEntry::IsTexture(eType1) ||
        (eType2 == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED) ||
        NiShaderConstantMapEntry::IsBool(eType2) ||
        NiShaderConstantMapEntry::IsString(eType2) ||
        NiShaderConstantMapEntry::IsTexture(eType2))
    {
        NSFParsererror("Invalid Operator Type");
        return false;
    }

    NiShaderAttributeDesc::AttributeType eResultType = 
        DetermineOperatorResult(iOperation, eType1, eType2);
    if (eResultType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        NSFParsererror("Invalid Operator - Result was invalid");
        return false;
    }
    
    // We have two valid entries, so let's setup the ConstantMapEntry
    // NOTE: If NSBConstantMap did not store it's list the proper way,
    // ie, via AddTail, this method would not work!
    unsigned int uiExtra = iOperation | uiEntry1 | 
        (uiEntry2 << NiShaderConstantMapEntry::SCME_OPERATOR_ENTRY2_SHIFT);
    if (bTranspose)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_TRANSPOSE;
    if (bInverse)
        uiExtra |= NiShaderConstantMapEntry::SCME_OPERATOR_RESULT_INVERSE;

    unsigned int uiFlags = NiShaderConstantMapEntry::SCME_MAP_OPERATOR | 
        NiShaderConstantMapEntry::GetAttributeFlags(eResultType);

    if (g_bConstantMapPlatformBlock)
    {
        if (!g_pkCurrConstantMap->AddPlatformSpecificEntry(
            g_uiCurrentPlatforms, pcName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
    else
    {
        if (!g_pkCurrConstantMap->AddEntry(pcName, uiFlags, uiExtra, 
            iRegStart, iRegCount, (char*)0))
        {
            // PROBLEM
            return false;
        }
    }
        
    return true;
}
//------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineOperatorResult(int iOperation, 
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (iOperation)
    {
    case NiShaderConstantMapEntry::SCME_OPERATOR_MULTIPLY:
        return DetermineResultMultiply(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_DIVIDE:
        return DetermineResultDivide(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_ADD:
        return DetermineResultAdd(eType1, eType2);
    case NiShaderConstantMapEntry::SCME_OPERATOR_SUBTRACT:
        return DetermineResultSubtract(eType1, eType2);
    default:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultMultiply(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultDivide(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        break;
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultAdd(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//------------------------------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType DetermineResultSubtract(
    NiShaderAttributeDesc::AttributeType eType1, 
    NiShaderAttributeDesc::AttributeType eType2)
{
    switch (eType1)
    {
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
    case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
    case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
    case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
    default:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
                return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
                return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            }
        }
        break;
    case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
        {
            switch (eType2)
            {
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED:
            case NiShaderAttributeDesc::ATTRIB_TYPE_BOOL:
            case NiShaderAttributeDesc::ATTRIB_TYPE_STRING:
            case NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT2:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_POINT4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3:
            case NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4:
            case NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE:
            default:
                return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
            case NiShaderAttributeDesc::ATTRIB_TYPE_COLOR:
                return NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
            }
        }
        break;
    }
}
//------------------------------------------------------------------------------------------------

