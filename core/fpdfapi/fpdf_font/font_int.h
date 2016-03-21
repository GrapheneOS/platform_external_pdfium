// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_FONT_FONT_INT_H_
#define CORE_FPDFAPI_FPDF_FONT_FONT_INT_H_

#include <map>
#include <memory>

#include "core/include/fpdfapi/fpdf_resource.h"
#include "core/include/fxcrt/fx_basic.h"

class CPDF_CID2UnicodeMap;
class CPDF_CMap;
class CPDF_Font;
class CPDF_Stream;

typedef void* FXFT_Library;

short TT2PDF(int m, FXFT_Face face);
FX_BOOL FT_UseTTCharmap(FXFT_Face face, int platform_id, int encoding_id);

class CPDF_CMapManager {
 public:
  CPDF_CMapManager();
  ~CPDF_CMapManager();
  void* GetPackage(FX_BOOL bPrompt);
  CPDF_CMap* GetPredefinedCMap(const CFX_ByteString& name, FX_BOOL bPromptCJK);
  CPDF_CID2UnicodeMap* GetCID2UnicodeMap(CIDSet charset, FX_BOOL bPromptCJK);
  void ReloadAll();

 private:
  CPDF_CMap* LoadPredefinedCMap(const CFX_ByteString& name, FX_BOOL bPromptCJK);
  CPDF_CID2UnicodeMap* LoadCID2UnicodeMap(CIDSet charset, FX_BOOL bPromptCJK);

  FX_BOOL m_bPrompted;
  std::map<CFX_ByteString, CPDF_CMap*> m_CMaps;
  CPDF_CID2UnicodeMap* m_CID2UnicodeMaps[6];
};

class CFX_StockFontArray {
 public:
  CFX_StockFontArray();
  ~CFX_StockFontArray();

  // Takes ownership of |pFont|.
  void SetFont(int index, CPDF_Font* pFont);
  CPDF_Font* GetFont(int index) const;

 private:
  std::unique_ptr<CPDF_Font> m_StockFonts[14];
};

class CPDF_FontGlobals {
 public:
  CPDF_FontGlobals();
  ~CPDF_FontGlobals();

  void Clear(CPDF_Document* pDoc);
  CPDF_Font* Find(CPDF_Document* pDoc, int index);

  // Takes ownership of |pFont|.
  void Set(CPDF_Document* key, int index, CPDF_Font* pFont);

  CPDF_CMapManager m_CMapManager;
  struct {
    const struct FXCMAP_CMap* m_pMapList;
    int m_Count;
  } m_EmbeddedCharsets[CIDSET_NUM_SETS];
  struct {
    const uint16_t* m_pMap;
    int m_Count;
  } m_EmbeddedToUnicodes[CIDSET_NUM_SETS];

 private:
  std::map<CPDF_Document*, std::unique_ptr<CFX_StockFontArray>> m_StockMap;
};

struct CMap_CodeRange {
  int m_CharSize;
  uint8_t m_Lower[4];
  uint8_t m_Upper[4];
};

class CPDF_CMapParser {
 public:
  CPDF_CMapParser();
  ~CPDF_CMapParser() {}
  FX_BOOL Initialize(CPDF_CMap* pMap);
  void ParseWord(const CFX_ByteStringC& str);
  CFX_BinaryBuf m_AddMaps;

 private:
  friend class fpdf_font_cid_CMap_GetCode_Test;
  friend class fpdf_font_cid_CMap_GetCodeRange_Test;

  static FX_DWORD CMap_GetCode(const CFX_ByteStringC& word);
  static bool CMap_GetCodeRange(CMap_CodeRange& range,
                                const CFX_ByteStringC& first,
                                const CFX_ByteStringC& second);

  CPDF_CMap* m_pCMap;
  int m_Status;
  int m_CodeSeq;
  FX_DWORD m_CodePoints[4];
  CFX_ArrayTemplate<CMap_CodeRange> m_CodeRanges;
  CFX_ByteString m_Registry, m_Ordering, m_Supplement;
  CFX_ByteString m_LastWord;
};

enum CIDCoding : uint8_t {
  CIDCODING_UNKNOWN = 0,
  CIDCODING_GB,
  CIDCODING_BIG5,
  CIDCODING_JIS,
  CIDCODING_KOREA,
  CIDCODING_UCS2,
  CIDCODING_CID,
  CIDCODING_UTF16,
};

class CPDF_CMap {
 public:
  enum CodingScheme : uint8_t {
    OneByte,
    TwoBytes,
    MixedTwoBytes,
    MixedFourBytes
  };

  CPDF_CMap();
  FX_BOOL LoadPredefined(CPDF_CMapManager* pMgr,
                         const FX_CHAR* name,
                         FX_BOOL bPromptCJK);
  FX_BOOL LoadEmbedded(const uint8_t* pData, FX_DWORD dwSize);
  void Release();
  FX_BOOL IsLoaded() const { return m_bLoaded; }
  FX_BOOL IsVertWriting() const { return m_bVertical; }
  uint16_t CIDFromCharCode(FX_DWORD charcode) const;
  FX_DWORD CharCodeFromCID(uint16_t CID) const;
  int GetCharSize(FX_DWORD charcode) const;
  FX_DWORD GetNextChar(const FX_CHAR* pString, int nStrLen, int& offset) const;
  int CountChar(const FX_CHAR* pString, int size) const;
  int AppendChar(FX_CHAR* str, FX_DWORD charcode) const;

 protected:
  ~CPDF_CMap();
  friend class CPDF_CMapParser;
  friend class CPDF_CMapManager;
  friend class CPDF_CIDFont;

  CFX_ByteString m_PredefinedCMap;
  FX_BOOL m_bVertical;
  CIDSet m_Charset;
  int m_Coding;
  CodingScheme m_CodingScheme;
  int m_nCodeRanges;
  uint8_t* m_pLeadingBytes;
  uint16_t* m_pMapping;
  uint8_t* m_pAddMapping;
  FX_BOOL m_bLoaded;
  const FXCMAP_CMap* m_pEmbedMap;
  CPDF_CMap* m_pUseMap;
};

class CPDF_CID2UnicodeMap {
 public:
  CPDF_CID2UnicodeMap();
  ~CPDF_CID2UnicodeMap();
  FX_BOOL Initialize();
  FX_BOOL IsLoaded();
  void Load(CPDF_CMapManager* pMgr, CIDSet charset, FX_BOOL bPromptCJK);
  FX_WCHAR UnicodeFromCID(uint16_t CID);

 protected:
  CIDSet m_Charset;
  const uint16_t* m_pEmbeddedMap;
  FX_DWORD m_EmbeddedCount;
};

class CPDF_ToUnicodeMap {
 public:
  void Load(CPDF_Stream* pStream);
  CFX_WideString Lookup(FX_DWORD charcode);
  FX_DWORD ReverseLookup(FX_WCHAR unicode);

 protected:
  std::map<FX_DWORD, FX_DWORD> m_Map;
  CPDF_CID2UnicodeMap* m_pBaseMap;
  CFX_WideTextBuf m_MultiCharBuf;

 private:
  friend class fpdf_font_StringToCode_Test;
  friend class fpdf_font_StringToWideString_Test;

  static FX_DWORD StringToCode(const CFX_ByteStringC& str);
  static CFX_WideString StringToWideString(const CFX_ByteStringC& str);
};

void FPDFAPI_LoadCID2UnicodeMap(CIDSet charset,
                                const uint16_t*& pMap,
                                FX_DWORD& count);

#endif  // CORE_FPDFAPI_FPDF_FONT_FONT_INT_H_
