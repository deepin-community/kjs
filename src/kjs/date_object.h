/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef DATE_OBJECT_H
#define DATE_OBJECT_H

#include "function.h"
#include "JSWrapperObject.h"

struct tm;

namespace KJS
{

class FunctionPrototype;
class ObjectPrototype;

class DateInstance : public JSWrapperObject
{
public:
    DateInstance(JSObject *proto);

    bool getTime(tm &t, int &gmtoffset) const;
    bool getUTCTime(tm &t) const;
    bool getTime(double &ms, int &gmtoffset) const;
    bool getUTCTime(double &ms) const;

    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;

    JSObject *valueClone(Interpreter *targetCtx) const override;
};

/**
 * @internal
 *
 * The initial value of Date.prototype (and thus all objects created
 * with the Date constructor
 */
class DatePrototype : public DateInstance
{
public:
    DatePrototype(ExecState *, ObjectPrototype *);
    using KJS::JSObject::getOwnPropertySlot;
    bool getOwnPropertySlot(ExecState *, const Identifier &, PropertySlot &) override;
    const ClassInfo *classInfo() const override
    {
        return &info;
    }
    static const ClassInfo info;
};

/**
    * @internal
 *
 * Class to implement all methods that are properties of the
 * Date.prototype object
 */
class DateProtoFunc : public InternalFunctionImp
{
public:
    DateProtoFunc(ExecState *, int i, int len, const Identifier &date);

    JSValue *callAsFunction(ExecState *, JSObject *thisObj, const List &args) override;

    enum { ToString, ToDateString, ToTimeString, ToLocaleString,
           ToLocaleDateString, ToLocaleTimeString, ValueOf, GetTime,
           GetFullYear, GetMonth, GetDate, GetDay, GetHours, GetMinutes,
           GetSeconds, GetMilliSeconds, GetTimezoneOffset, SetTime,
           SetMilliSeconds, SetSeconds, SetMinutes, SetHours, SetDate,
           SetMonth, SetFullYear, ToUTCString, ToISOString, ToJSON,
           // non-normative properties (Appendix B)
           GetYear, SetYear, ToGMTString
         };
private:
    short id;
    bool utc;
};

/**
 * @internal
 *
 * The initial value of the global variable's "Date" property
 */
class DateObjectImp : public InternalFunctionImp
{
    using InternalFunctionImp::construct;
public:
    DateObjectImp(ExecState *, FunctionPrototype *, DatePrototype *);

    bool implementsConstruct() const override;
    JSObject *construct(ExecState *, const List &args) override;
    JSValue *callAsFunction(ExecState *, JSObject *thisObj, const List &args) override;

    Completion execute(const List &);
    JSObject *construct(const List &);
};

} // namespace

#endif
