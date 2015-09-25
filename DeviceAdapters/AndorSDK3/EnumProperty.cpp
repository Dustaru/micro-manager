/*
 * Copyright (c) 2007, Regents of the University of California
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "EnumProperty.h"
#include "AndorSDK3.h"
#include "SnapShotControl.h"
#include <vector>


using namespace andor;
using namespace std;

TEnumProperty::TEnumProperty(const string & MM_name, IEnum * enum_feature, CAndorSDK3Camera * camera,
                             MySequenceThread * thd, SnapShotControl * snapShotController, bool readOnly,
                             bool needsCallBack)
: MM_name_(MM_name),
  enum_feature_(enum_feature),
  camera_(camera),
  thd_(thd),
  snapShotController_(snapShotController),
  callBackRegistered_(needsCallBack)
{
   if (enum_feature->IsImplemented())
   {
      CPropertyAction * pAct = new CPropertyAction (this, &TEnumProperty::OnEnum);
      camera_->CreateProperty(MM_name_.c_str(), "", MM::String, readOnly, pAct);

      try
      {
         if (needsCallBack)
         {
            enum_feature_->Attach(this);
         }
         else
         {
            Update(NULL);
         }
      }
      catch (exception & e)
      {
         // Callback not implemented for this feature
         camera_->LogMessage(e.what());
      }
   }
   else
   {
      callBackRegistered_ = false;
   }
}

TEnumProperty::~TEnumProperty()
{
   if (callBackRegistered_)
   {
      try
      {
         enum_feature_->Detach(this);
      }
      catch (exception & e)
      {
         // Callback not implemented for this feature
         camera_->LogMessage(e.what());
      }
   }
   //Clean up memory, created as passed in
   camera_->GetCameraDevice()->Release(enum_feature_);
}


inline wchar_t * convertToWString(const string & str, wchar_t * outBuf, unsigned int bufSize)
{
   wmemset(outBuf, L'\0', bufSize);
   mbstowcs(outBuf, str.c_str(), str.size());
   return outBuf;
}

inline char * convertFromWString(const wstring & wstr, char * outBuf, unsigned int bufSize)
{
   memset(outBuf, '\0', bufSize);
   wcstombs(outBuf, wstr.c_str(), wstr.size());
   return outBuf;
}

void TEnumProperty::Update(ISubject * /*Subject*/)
{
   // This property has been changed in SDK3. The new value will be set by a
   // call to TEnumProperty::OnEnum, in here reset the list of allowed values
   //  No clear required as this is always done by base impl, if call SetAllowedValues
   vector<string> allowed_values;
   char buf[MAX_CHARS_ENUM_VALUE_BUFFER];
   for (int i = 0; i < enum_feature_->Count(); i++)
   {
      if (enum_feature_->IsIndexImplemented(i))
      {
         if (enum_feature_->IsIndexAvailable(i))
         {
            wstring value_ws = enum_feature_->GetStringByIndex(i);
            allowed_values.push_back(convertFromWString(value_ws, buf, MAX_CHARS_ENUM_VALUE_BUFFER));
         }
         //else
         //{
         //   stringstream ss("Enum feature ");
         //   ss << MM_name_ << "; Index " << i << " Not Available";
         //   camera_->LogMessage(ss.str().c_str(), true);
         //}
      }
      //else
      //{
      //   stringstream ss("Enum feature ");
      //   ss << MM_name_ << "; Index " << i << " Not Implemented";
      //   camera_->LogMessage(ss.str().c_str(), true);
      //}
   }
   camera_->SetAllowedValues(MM_name_.c_str(), allowed_values);
}

// Action handler for OnEnum
int TEnumProperty::OnEnum(MM::PropertyBase * pProp, MM::ActionType eAct)
{
   if (!enum_feature_->IsImplemented())
   {
      return DEVICE_OK;
   }
   if (eAct == MM::BeforeGet)
   {
      char buf[MAX_CHARS_ENUM_VALUE_BUFFER];
      wstring temp_ws = enum_feature_->GetStringByIndex(enum_feature_->GetIndex());
      pProp->Set(convertFromWString(temp_ws, buf, MAX_CHARS_ENUM_VALUE_BUFFER));
   }
   else if (eAct == MM::AfterSet)
   {
      if (!thd_->IsStopped())
      {
         camera_->StopSequenceAcquisition();
      }

      bool was_poised = false;
      if (snapShotController_->isPoised())
      {
         snapShotController_->leavePoisedMode();
         was_poised = true;
      }

      if (enum_feature_->IsWritable())
      {
         wchar_t buf[MAX_CHARS_ENUM_VALUE_BUFFER];
         string temp_s;
         pProp->Get(temp_s);
         enum_feature_->Set(convertToWString(temp_s, buf, MAX_CHARS_ENUM_VALUE_BUFFER));
      }
      camera_->UpdateProperty(MM_name_.c_str());

      if (0 == MM_name_.compare(MM::g_Keyword_Binning))
      {
         camera_->ResizeImageBuffer();
      }

      if (was_poised)
      {
         snapShotController_->poiseForSnapShot();
      }
   }

   return DEVICE_OK;
}
