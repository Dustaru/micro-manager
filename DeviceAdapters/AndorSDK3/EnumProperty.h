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

#ifndef _ENUMPROPERTY_H_
#define _ENUMPROPERTY_H_

#include <map>
#include "atcore++.h"
#include "MMDeviceConstants.h"
#include "Property.h"


class MySequenceThread;
class CAndorSDK3Camera;
class SnapShotControl;

class TEnumProperty : public andor::IObserver
{
public:
   TEnumProperty(const std::string & MM_name, andor::IEnum* enum_feature,
                 CAndorSDK3Camera* camera, MySequenceThread* thd,
                 SnapShotControl* snapShotController, bool readOnly, bool needsCallBack);
   ~TEnumProperty();

   void Update(andor::ISubject* Subject);
   int OnEnum(MM::PropertyBase* pProp, MM::ActionType eAct);
   typedef MM::Action<TEnumProperty> CPropertyAction;

private:
   static const unsigned int MAX_CHARS_ENUM_VALUE_BUFFER = 256;
   bool callBackRegistered_;
   andor::IEnum* enum_feature_;
   CAndorSDK3Camera* camera_;
   std::string MM_name_;
   MySequenceThread * thd_;
   SnapShotControl* snapShotController_;
};

class TAndorEnumFilter : public andor::IEnum
{
public:
   TAndorEnumFilter(andor::IEnum* _enum):m_enum(_enum){}
   virtual ~TAndorEnumFilter() {};
   int GetIndex(){return m_enum->GetIndex();}
   void Set(int Index) {m_enum->Set(Index);}
   void Set(std::wstring Value){m_enum->Set(Value);}
   int Count(){return m_enum->Count();}
   std::wstring GetStringByIndex(int Index){return m_enum->GetStringByIndex(Index);}
   bool IsIndexAvailable(int Index){return m_enum->IsIndexAvailable(Index);}
   bool IsIndexImplemented(int Index){return m_enum->IsIndexImplemented(Index);}
   bool IsImplemented(){return m_enum->IsImplemented();}
   bool IsReadable(){return m_enum->IsReadable();}
   bool IsWritable(){return m_enum->IsWritable();}
   bool IsReadOnly(){return m_enum->IsReadOnly();}
   void Attach(andor::IObserver* _observer){m_enum->Attach(_observer);}
   void Detach(andor::IObserver* _observer){m_enum->Detach(_observer);}

protected:
   andor::IEnum* m_enum;
};

class TAndorEnumValueMapper : public TAndorEnumFilter
{
public:
   TAndorEnumValueMapper(andor::IEnum* _enum, std::map<std::wstring, std::wstring> _map)
   :TAndorEnumFilter(_enum), m_map(_map)
   {
      // Create the revese map
      for (std::map<std::wstring, std::wstring>::iterator it=m_map.begin();
           it != m_map.end(); ++it) {
              m_rmap[it->second] = it->first;
      }      
   }
   ~TAndorEnumValueMapper() {}
  
   void Set(std::wstring Value)
   {
      std::map<std::wstring, std::wstring>::iterator it = m_rmap.find(Value);
      if (it != m_rmap.end()) {
         m_enum->Set(it->second);
      }
      else {
         m_enum->Set(Value);
      }
   }
  
   std::wstring GetStringByIndex(int Index)
   {
      std::map<std::wstring, std::wstring>::iterator it =
                                    m_map.find(m_enum->GetStringByIndex(Index));
      if (it != m_map.end()) {
         return it->second;
      }
      else {
         return m_enum->GetStringByIndex(Index);
      }
   }
  
private:
   std::map<std::wstring, std::wstring> m_map;
   std::map<std::wstring, std::wstring> m_rmap;
};

#endif // _ENUMPROPERTY_H_
