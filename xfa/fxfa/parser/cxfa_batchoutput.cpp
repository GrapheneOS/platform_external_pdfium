// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_batchoutput.h"

#include "fxjs/xfa/cjx_node.h"
#include "third_party/base/ptr_util.h"

namespace {

const CXFA_Node::AttributeData kBatchOutputAttributeData[] = {
    {XFA_Attribute::Format, XFA_AttributeType::Enum,
     (void*)XFA_AttributeValue::None},
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
};

}  // namespace

CXFA_BatchOutput::CXFA_BatchOutput(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET_Config,
                XFA_ObjectType::Node,
                XFA_Element::BatchOutput,
                {},
                kBatchOutputAttributeData,
                pdfium::MakeUnique<CJX_Node>(this)) {}

CXFA_BatchOutput::~CXFA_BatchOutput() = default;
