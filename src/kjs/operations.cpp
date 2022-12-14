/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "operations.h"

#include "object.h"
#include "internal.h"

namespace KJS
{
// ECMA 11.9.3
bool equal(ExecState *exec, JSValue *v1, JSValue *v2)
{
    JSType t1 = JSValue::type(v1);
    JSType t2 = JSValue::type(v2);

    if (t1 != t2) {
        if (t1 == UndefinedType) {
            t1 = NullType;
        }
        if (t2 == UndefinedType) {
            t2 = NullType;
        }

        if (t1 == BooleanType) {
            t1 = NumberType;
        }
        if (t2 == BooleanType) {
            t2 = NumberType;
        }

        if (t1 == NumberType && t2 == StringType) {
            // use toNumber
        } else if (t1 == StringType && t2 == NumberType) {
            t1 = NumberType;
        }
        // use toNumber
        else {
            if ((t1 == StringType || t1 == NumberType) && t2 >= ObjectType) {
                return equal(exec, v1, JSValue::toPrimitive(v2, exec));
            }
            if (t1 == NullType && t2 == ObjectType) {
                return static_cast<JSObject *>(v2)->masqueradeAsUndefined();
            }
            if (t1 >= ObjectType && (t2 == StringType || t2 == NumberType)) {
                return equal(exec, JSValue::toPrimitive(v1, exec), v2);
            }
            if (t1 == ObjectType && t2 == NullType) {
                return static_cast<JSObject *>(v1)->masqueradeAsUndefined();
            }
            if (t1 != t2) {
                return false;
            }
        }
    }

    if (t1 == UndefinedType || t1 == NullType) {
        return true;
    }

    if (t1 == NumberType) {
        double d1 = JSValue::toNumber(v1, exec);
        double d2 = JSValue::toNumber(v2, exec);
        return d1 == d2;
    }

    if (t1 == StringType) {
        return JSValue::toString(v1, exec) == JSValue::toString(v2, exec);
    }

    if (t1 == BooleanType) {
        return JSValue::toBoolean(v1, exec) == JSValue::toBoolean(v2,  exec);
    }

    // types are Object
    return v1 == v2;
}

bool strictEqual(ExecState *exec, JSValue *v1, JSValue *v2)
{
    JSType t1 = JSValue::type(v1);
    JSType t2 = JSValue::type(v2);

    if (t1 != t2) {
        return false;
    }
    if (t1 == UndefinedType || t1 == NullType) {
        return true;
    }
    if (t1 == NumberType) {
        double n1 = JSValue::toNumber(v1, exec);
        double n2 = JSValue::toNumber(v2, exec);
        if (n1 == n2) {
            return true;
        }
        return false;
    } else if (t1 == StringType) {
        return JSValue::toString(v1, exec) == JSValue::toString(v2, exec);
    } else if (t2 == BooleanType) {
        return JSValue::toBoolean(v1, exec) == JSValue::toBoolean(v2,  exec);
    }

    if (v1 == v2) {
        return true;
    }
    /* TODO: joined objects */

    return false;
}

int relation(ExecState *exec, JSValue *v1, JSValue *v2, bool leftFirst)
{
    double n1;
    double n2;
    JSValue *p1;
    JSValue *p2;

    bool wasNotString1;
    bool wasNotString2;

    if (leftFirst) {
        wasNotString1 = JSValue::getPrimitiveNumber(v1, exec, n1, p1);
        if (exec->hadException()) {
            return -1;
        }
        wasNotString2 = JSValue::getPrimitiveNumber(v2, exec, n2, p2);
    } else {
        wasNotString1 = JSValue::getPrimitiveNumber(v2, exec, n2, p2);
        if (exec->hadException()) {
            return -1;
        }
        wasNotString2 = JSValue::getPrimitiveNumber(v1, exec, n1, p1);
    }

    if (wasNotString1 || wasNotString2) {
        if (n1 < n2) {
            return 1;
        }
        if (n1 >= n2) {
            return 0;
        }
        return -1; // must be NaN, so undefined
    }

    assert(JSValue::isString(p1) && JSValue::isString(p2));
    return static_cast<const StringImp *>(p1)->value() < static_cast<const StringImp *>(p2)->value() ? 1 : 0;
}

//EMCA 9.12
bool sameValue(ExecState *exec, JSValue *v1, JSValue *v2)
{
    using namespace std;
    JSType t1 = JSValue::type(v1);
    JSType t2 = JSValue::type(v2);

    if (t1 != t2) {
        return false;
    }
    if (t1 == UndefinedType || t1 == NullType) {
        return true;
    }
    if (t1 == NumberType) {
        double n1 = JSValue::toNumber(v1, exec);
        double n2 = JSValue::toNumber(v2, exec);
        if (isNaN(n1) && isNaN(n2)) {
            return true;
        }
        if (signbit(n1) != signbit(n2)) {
            return false;
        }
        if (n1 == n2) {
            return true;
        }
        return false;
    } else if (t1 == StringType) {
        return JSValue::toString(v1, exec) == JSValue::toString(v2, exec);
    } else if (t2 == BooleanType) {
        return JSValue::toBoolean(v1, exec) == JSValue::toBoolean(v2,  exec);
    }

    if (v1 == v2) {
        return true;
    }
    /* TODO: joined objects */

    return false;
}

int relation(ExecState *exec, JSValue *v1, double n2)
{
    double n1;
    JSValue *p1;

    JSValue::getPrimitiveNumber(v1, exec, n1, p1);
    if (exec->hadException()) {
        return -1;
    }

    if (n1 < n2) {
        return 1;
    }
    if (n1 >= n2) {
        return 0;
    }
    return -1; // must be NaN, so undefined
}

int maxInt(int d1, int d2)
{
    return (d1 > d2) ? d1 : d2;
}

int minInt(int d1, int d2)
{
    return (d1 < d2) ? d1 : d2;
}

double exponentiation(double base, double exponent)
{
    if (isNaN(exponent))
	return NaN;
    if (abs(base) == 1 && isInf(exponent))
	return NaN;
    return pow(base, exponent);
}

}
