/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**/
#include "qqmldompath_p.h"
#include "qqmldomitem_p.h"
#include "qqmldomerrormessage_p.h"
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QChar>

#include <cstdint>

QT_BEGIN_NAMESPACE
namespace QQmlJS {
namespace Dom {
class ErrorMessage;

namespace PathEls {

/*!
\internal
\class QQmljs::Dom::QmlPath::Path

\brief Represents an immutable JsonPath like path in the Qml code model (from a DomItem to another
 DomItem)

It can be created either from a string, with the path static functions
or by modifying an existing path
\code
Path qmlFilePath =
    Path::fromString(u"$env.qmlFilesByPath[\"/path/to/file\"]");
Path imports = qmlFilePath.subField(u"imports")
Path currentComponentImports = Path::current(u"component").subField(u"imports");
\endcode

This is a way to refer to elements in the Dom models that is not dependent from the source location,
and thus can be used also be used in visual tools.
A Path is quite stable toward reallocations or changes in the Dom model, and accessing it is safe
even when "dangling", thus it is a good long term reference to an element in the Dom model.

Path objects are a value type that have a shared pointer to extra data if needed, thus one should
use them as value objects.
The implementation has still optimization potential, but the behavior for the user should be already
the final one.

Path is both a range, and a single element (a bit like strings and characters in python).

The root contexts are:
\list
\li \l{$modules} All the known modules (even not imported), this is a global, rename independent
       reference
\li \l{$cpp} The Cpp names (namespaces, and Cpp types) visible in the current component
\li \l{$libs} The plugins/libraries and their contents
\li \l{$top} A top level entry in the DOM model, either $env or $universe (stepping in the universe
       one looses the reference to its environment)
\li \l{$env} The environment containing the currently available modules, i.e. the top level entry in
       the DOM model
\li \l{$universe} The dom unverse used by ths environment, and potentially shared with others that
       contains all the known parse entries, and also the latest, potentially invalid entries
\li \l{$} ? undecided, one the previous ones?
\endlist

The current contexts are:
\list
\li \l{@obj} The current object (if in a map or list goes up until it is in the current object) .
\li \l{@component} The root object of the current component.
\li \l{@module} The current module instantiation.
\li \l{@ids} The ids in the current component.
\li \l{@types} All the types in the current component (reachable through imports, respecting renames)
\li \l{@instantiation} The current instantiation, either component instantiation or module
       instantiation
\li \l{@lookupStrict} The strict lookup inside the current object: localJS, ids, properties, proto
       properties, component, its properties, global context, oterwise error
\li \l{@lookupDynamic} The default lookup inside the current object: localJS, ids, properties, proto
       properties, component, its properties, global context, ..
\li \l{@lookup} Either lookupStrict or lookupDynamic depending on the current component and context.
\li \l{@} ? undecided, one the previous ones
\endlist
 */

void Base::dump(Sink sink) const {
    if (hasSquareBrackets())
        sink(u"[");
    sink(name());
    if (hasSquareBrackets())
        sink(u"]");
}

Filter::Filter(function<bool(DomItem)> f, QStringView filterDescription): filterFunction(f), filterDescription(filterDescription) {}

QString Filter::name() const {
    return QLatin1String("?(%1)").arg(filterDescription); }

bool Filter::checkName(QStringView s) const
{
    return s.startsWith(u"?(")
            && s.mid(2, s.length()-3) == filterDescription
            && s.endsWith(u")");
}

enum class ParserState{
    Start,
    IndexOrKey,
    End
};

} // namespace PathEls

using namespace PathEls;

PathComponent::~PathComponent(){
}

int PathComponent::cmp(const PathComponent &p1, const PathComponent &p2)
{
    int k1 = static_cast<int>(p1.kind());
    int k2 = static_cast<int>(p2.kind());
    if (k1 < k2)
        return -1;
    if (k1 > k2)
        return 1;
    switch (p1.kind()) {
    case Kind::Empty:
        return 0;
    case Kind::Field:
        return p1.data.field.fieldName.compare(p2.data.field.fieldName);
    case Kind::Index:
        if (p1.data.index.indexValue < p2.data.index.indexValue)
            return -1;
        if (p1.data.index.indexValue > p2.data.index.indexValue)
            return 1;
        return 0;
    case Kind::Key:
        return p1.data.key.keyValue.compare(p2.data.key.keyValue);
    case Kind::Root:
    {
        int c = int(p1.data.root.contextKind) - int(p2.data.root.contextKind);
        if (c != 0)
            return c;
        return p1.data.root.contextName.compare(p2.data.root.contextName);
    }
    case Kind::Current:
    {
        int c = int(p1.data.current.contextKind) - int(p2.data.current.contextKind);
        if (c != 0)
            return c;
        return p1.data.current.contextName.compare(p2.data.current.contextName);
    }
    case Kind::Any:
        return 0;
    case Kind::Filter:
    {
        int c = p1.data.filter.filterDescription.compare(p2.data.filter.filterDescription);
        if (c != 0)
            return c;
        if (p1.data.filter.filterDescription.startsWith(u"<")) {
            // assuming non comparable native code (target comparison is not portable)
            auto pp1 = &p1;
            auto pp2 = &p2;
            if (pp1 < pp2)
                return -1;
            if (pp1 > pp2)
                return 1;
        }
        return 0;
    }
    }
    Q_ASSERT(false && "unexpected PathComponent in PathComponent::cmp");
    return 0;
}

PathComponent Path::component(int i) const
{
    if (i < 0)
        i += m_length;
    if (i >= m_length || i < 0) {
        Q_ASSERT(false && "index out of bounds");
        return Component();
    }
    i = i - m_length - m_endOffset;
    auto data = m_data.get();
    while (data) {
        i += data->components.length();
        if (i >= 0)
            return data->components.at(i);
        data = data->parent.get();
    }
    Q_ASSERT(false && "Invalid data reached while resolving a seemengly valid index in Path (inconsisten Path object)");
    return Component();
}

Path Path::operator[](int i) const
{
    return mid(i,1);
}

QQmlJS::Dom::Path::operator bool() const
{
    return length() != 0;
}

PathRoot Path::headRoot() const
{
    auto comp = component(0);
    if (Root const * r = comp.base()->asRoot())
        return r->contextKind;
    return PathRoot::Other;
}

PathCurrent Path::headCurrent() const
{
    auto comp = component(0);
    if (Current const * c = comp.base()->asCurrent())
        return c->contextKind;
    return PathCurrent::Other;
}

Path::Kind Path::headKind() const
{
    return component(0).kind();
}

QString Path::headName() const
{
    return component(0).name();
}

bool Path::checkHeadName(QStringView name) const
{
    return component(0).checkName(name);
}

index_type Path::headIndex(index_type defaultValue) const
{
    return component(0).index(defaultValue);
}

function<bool (DomItem)> Path::headFilter() const
{
    auto comp = component(0);
    if (Filter const * f = comp.base()->asFilter()) {
        return f->filterFunction;
    }
    return {};
}

Path Path::head() const
{
    return mid(0,1);
}

Path Path::last() const
{
    return mid(m_length-1, 1);
}

Source Path::split() const
{
    int i = length();
    while (i>0) {
        const PathEls::PathComponent &c=component(--i);
        if (c.kind() == Kind::Field || c.kind() == Kind::Root || c.kind() == Kind::Current) {
            return Source{mid(0, i), mid(i)};
        }
    }
    return Source{Path(), *this};
}

bool inQString(QStringView el, QString base)
{
    if (quintptr(base.constData()) > quintptr(el.begin())
        || quintptr(base.constData() + base.size()) < quintptr(el.begin()))
        return false;
    ptrdiff_t diff = base.constData() - el.begin();
    return diff >= 0 && diff < base.size();
}

bool inQString(QString el, QString base)
{
    if (quintptr(base.constData()) > quintptr(el.constData())
        || quintptr(base.constData() + base.size()) < quintptr(el.constData()))
        return false;
    ptrdiff_t diff = base.constData() - el.constData();
    return diff >= 0 && diff < base.size() && diff + el.size() < base.size();
}

Path Path::fromString(QStringView s, ErrorHandler errorHandler)
{
    if (s.isEmpty())
        return Path();
    int len=1;
    const QChar dot = QChar::fromLatin1('.');
    const QChar lsBrace = QChar::fromLatin1('[');
    const QChar rsBrace = QChar::fromLatin1(']');
    const QChar dollar = QChar::fromLatin1('$');
    const QChar at = QChar::fromLatin1('@');
    const QChar quote = QChar::fromLatin1('"');
    const QChar backslash = QChar::fromLatin1('\\');
    const QChar underscore = QChar::fromLatin1('_');
    const QChar tilda = QChar::fromLatin1('~');
    for (int i=0; i < s.length(); ++i)
        if (s.at(i) == lsBrace || s.at(i) == dot)
            ++len;
    QVector<Component> components;
    components.reserve(len);
    int i = 0;
    int i0 = 0;
    ParserState state = ParserState::Start;
    QStringList strVals;
    while (i < s.length()) {
        // skip space
        while (i < s.length() && s.at(i).isSpace())
            ++i;
        if (i >= s.length())
            break;
        QChar c = s.at(i++);
        switch (state) {
        case ParserState::Start:
            if (c == dollar) {
                i0 = i;
                while (i < s.length() && s.at(i).isLetterOrNumber()){
                    ++i;
                }
                components.append(Component(Root(s.mid(i0,i-i0))));
                state = ParserState::End;
            } else if (c == at) {
                i0 = i;
                while (i < s.length() && s.at(i).isLetterOrNumber()){
                    ++i;
                }
                components.append(Component(Current(s.mid(i0,i-i0))));
                state = ParserState::End;
            } else if (c.isLetter()) {
                myErrors().warning(tr("Field expressions should start with dot even a the start of the path %1.")
                                 .arg(s)).handle(errorHandler);
                return Path();
            } else {
                --i;
                state = ParserState::End;
            }
            break;
        case ParserState::IndexOrKey:
            if (c.isDigit()) {
                i0 = i-1;
                while (i < s.length() && s.at(i).isDigit())
                    ++i;
                bool ok;
                components.append(Component(static_cast<index_type>(s.mid(i0,i-i0).toString()
                                                                    .toLongLong(&ok))));
                if (!ok) {
                    myErrors().warning(tr("Error extracting integer from '%1' at char %2.")
                        .arg(s.mid(i0,i-i0))
                        .arg(QString::number(i0))).handle(errorHandler);
                }
            } else if (c.isLetter() || c == tilda || c == underscore) {
                i0 = i-1;
                while (i < s.length() && (s.at(i).isLetterOrNumber() || s.at(i) == underscore || s.at(i) == tilda))
                    ++i;
                components.append(Component(Key(s.mid(i0, i-i0))));
            } else if (c == quote) {
                i0 = i;
                QString strVal;
                QStringView key;
                bool needsConversion = false;
                bool properEnd = false;
                while (i < s.length()) {
                    c = s.at(i);
                    if (c == quote) {
                        properEnd = true;
                        break;
                    } else if (c == backslash) {
                        needsConversion = true;
                        strVal.append(s.mid(i0, i - i0).toString());
                        c = s.at(++i);
                        i0 = i + 1;
                        if (c == QChar::fromLatin1('n'))
                            strVal.append(QChar::fromLatin1('\n'));
                        else if (c == QChar::fromLatin1('r'))
                            strVal.append(QChar::fromLatin1('\r'));
                        else if (c == QChar::fromLatin1('t'))
                            strVal.append(QChar::fromLatin1('\t'));
                        else
                            strVal.append(c);
                    }
                    ++i;
                }
                if (properEnd) {
                    if (needsConversion) {
                        strVal.append(s.mid(i0, i - i0).toString());
                        strVals.append(strVal);
                        key=strVal;
                    } else {
                        key = s.mid(i0, i - i0);
                    }
                    ++i;
                } else {
                    myErrors().error(tr("Unclosed quoted string at char %1.")
                                     .arg(QString::number(i - 1))).handle(errorHandler);
                    return Path();
                }
                components.append(Key(key));
            } else if (c == QChar::fromLatin1('*')) {
                components.append(Component(Any()));
            } else if (c == QChar::fromLatin1('?')) {
                while (i < s.length() && s.at(i).isSpace())
                    ++i;
                if (i >= s.length() || s.at(i) != QChar::fromLatin1('(')) {
                    myErrors().error(tr("Expected a brace in filter after the question mark (at char %1).")
                            .arg(QString::number(i))).handle(errorHandler);
                    return Path();
                }
                i0 = ++i;
                while (i < s.length() && s.at(i) != QChar::fromLatin1(')')) ++i; // check matching braces when skipping??
                if (i >= s.length() || s.at(i) != QChar::fromLatin1(')')) {
                    myErrors().error(tr("Expected a closing brace in filter after the question mark (at char %1).")
                            .arg(QString::number(i))).handle(errorHandler);
                    return Path();
                }
                //auto filterDesc = s.mid(i0,i-i0);
                ++i;
                myErrors().error(tr("Filter from string not yet implemented.")).handle(errorHandler);
                return Path();
            } else {
                myErrors().error(tr("Unexpected character '%1' after square braket at %2.")
                                 .arg(c).arg(i-1)).handle(errorHandler);
                return Path();
            }
            while (i < s.length() && s.at(i).isSpace()) ++i;
            if (i >= s.length() || s.at(i) != rsBrace) {
                myErrors().error(tr("square braces misses closing brace at char %1.")
                                 .arg(QString::number(i))).handle(errorHandler);
                return Path();
            } else {
                ++i;
            }
            state = ParserState::End;
            break;
        case ParserState::End:
            if (c == dot) {
                while (i < s.length() && s.at(i).isSpace()) ++i;
                if (i == s.length()) {
                    components.append(Component());
                    state = ParserState::End;
                } else if (s.at(i).isLetter() || s.at(i) == underscore || s.at(i) == tilda) {
                    i0 = i;
                    while (i < s.length() && (s.at(i).isLetterOrNumber() || s.at(i) == underscore || s.at(i) == tilda)) {
                        ++i;
                    }
                    components.append(Component(Field(s.mid(i0,i-i0))));
                    state = ParserState::End;
                } else if (s.at(i).isDigit()) {
                    i0 = i;
                    while (i < s.length() && s.at(i).isDigit()){
                        ++i;
                    }
                    bool ok;
                    components.append(Component(static_cast<index_type>(s.mid(i0,i-i0).toString().toLongLong(&ok))));
                    if (!ok) {
                        myErrors().warning(tr("Error extracting integer from '%1' at char %2.")
                                           .arg(s.mid(i0,i-i0))
                                           .arg(QString::number(i0))).handle(errorHandler);
                        return Path();
                    } else {
                        myErrors().hint(tr("Index shound use square brackets and not a dot (at char %1).")
                                           .arg(QString::number(i0))).handle(errorHandler);
                    }
                    state = ParserState::End;
                } else if (s.at(i) == dot || s.at(i) == lsBrace) {
                    components.append(Component());
                    state = ParserState::End;
                } else if (s.at(i) == at) {
                    i0 = ++i;
                    while (i < s.length() && s.at(i).isLetterOrNumber()){
                        ++i;
                    }
                    components.append(Component(Current(s.mid(i0,i-i0))));
                    state = ParserState::End;
                } else if (s.at(i) == dollar) {
                    i0 = ++i;
                    while (i < s.length() && s.at(i).isLetterOrNumber()){
                        ++i;
                    }
                    components.append(Component(Root(s.mid(i0,i-i0))));
                    state = ParserState::End;
                } else {
                    c=s.at(i);
                    myErrors().error(tr("Unexpected character '%1' after dot (at char %2).")
                                     .arg(QStringView(&c,1))
                                     .arg(QString::number(i-1))).handle(errorHandler);
                    return Path();
                }
            } else if (c == lsBrace) {
                state = ParserState::IndexOrKey;
            } else {
                myErrors().error(tr("Unexpected character '%1' after end of component (char %2).")
                                 .arg(QStringView(&c,1))
                                 .arg(QString::number(i-1))).handle(errorHandler);
                return Path();
            }
            break;
        }
    }
    switch (state) {
    case ParserState::Start:
        return Path();
    case ParserState::IndexOrKey:
        errorHandler(myErrors().error(tr("unclosed square brace at end.")));

        return Path();
    case ParserState::End:
        return Path(0, components.length(), std::make_shared<PathData>(strVals, components));
    }
    Q_ASSERT(false && "Unexpected state in Path::fromString");
    return Path();
}

Path Path::root(PathRoot s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Root(s)))));
}

Path Path::root(QString s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(s), QVector<Component>(1,Component(Root(s)))));
}

Path Path::index(index_type i)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Index(i)))));
}

Path Path::root(QStringView s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Root(s)))));
}


Path Path::field(QStringView s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Field(s)))));
}

Path Path::field(QString s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(s), QVector<Component>(1,Component(Field(s)))));
}

Path Path::key(QStringView s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Key(s)))));
}

Path Path::key(QString s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(s), QVector<Component>(1,Component(Key(s)))));
}

Path Path::current(PathCurrent s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Current(s)))));
}

Path Path::current(QString s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(s), QVector<Component>(1,Component(Current(s)))));
}

Path Path::current(QStringView s)
{
    return Path(0,1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Current(s)))));
}

Path Path::empty()
{
    return Path();
}

Path Path::subEmpty() const
{
    if (m_endOffset != 0)
        return noEndOffset().subEmpty();
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component()), m_data));
}

Path Path::subField(QString name) const
{
    auto res = subField(QStringView(name));
    res.m_data->strData.append(name);
    return res;
}

Path Path::subField(QStringView name) const
{
    if (m_endOffset != 0)
        return noEndOffset().subField(name);
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Field(name))), m_data));
}

Path Path::subKey(QString name) const
{
    auto res = subKey(QStringView(name));
    res.m_data->strData.append(name);
    return res;
}

Path Path::subKey(QStringView name) const
{
    if (m_endOffset != 0)
        return noEndOffset().subKey(name);
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Key(name))), m_data));
}

Path Path::subIndex(index_type i) const
{
    if (m_endOffset != 0)
        return noEndOffset().subIndex(i);
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(i)), m_data));
}

Path Path::subAny() const
{
    if (m_endOffset != 0)
        return noEndOffset().subAny();
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Any())), m_data));
}

Path Path::subFilter(function<bool (DomItem)> filter, QString desc) const
{
    auto res = subFilter(filter, QStringView(desc));
    res.m_data->strData.append(desc);
    return res;
}

Path Path::subFilter(function<bool (DomItem)> filter, QStringView desc) const
{
    if (m_endOffset != 0)
        return noEndOffset().subFilter(filter, desc);
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Filter(filter, desc))), m_data));
}

Path Path::subCurrent(PathCurrent s) const
{
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Current(s))), m_data));
}

Path Path::subCurrent(QString s) const
{
    auto res = subCurrent(QStringView(s));
    res.m_data->strData.append(s);
    return res;
}

Path Path::subCurrent(QStringView s) const
{
    if (m_endOffset != 0)
        return noEndOffset().subCurrent(s);
    return Path(0,m_length+1,std::make_shared<PathData>(QStringList(), QVector<Component>(1,Component(Current(s))), m_data));
}

Path Path::subPath(Path toAdd, bool avoidToAddAsBase) const
{
    if (toAdd.length() == 0)
        return *this;
    int resLength = length() + toAdd.length();
    if (m_endOffset != 0) {
        Path thisExtended = this->expandBack();
        if (thisExtended.length() > resLength)
            thisExtended = thisExtended.mid(0, resLength);
        Path added = thisExtended.mid(length(), thisExtended.length() - length());
        if (added == toAdd.mid(0, toAdd.length())) {
            if (resLength == thisExtended.length())
                return thisExtended;
            else
                return thisExtended.subPath(toAdd.mid(added.length(), resLength - thisExtended.length()));
        }
    }
    if (!avoidToAddAsBase) {
        Path toAddExtended = toAdd.expandFront();
        if (toAddExtended.length() >= resLength) {
            toAddExtended = toAddExtended.mid(toAddExtended.length() - resLength, resLength);
            if (toAddExtended.mid(0,length()) == *this)
                return toAddExtended;
        }
    }
    QVector<Component> components;
    components.reserve(toAdd.length());
    QStringList addedStrs;
    bool addHasStr = false;
    auto data = toAdd.m_data.get();
    while (data) {
        if (!data->strData.isEmpty()) {
            addHasStr = true;
            break;
        }
        data = data->parent.get();
    }
    if (addHasStr) {
        QStringList myStrs;
        data = m_data.get();
        while (data) {
            myStrs.append(data->strData);
            data = data->parent.get();
        }
        data = toAdd.m_data.get();
        while (data) {
            for (int ij = 0; ij < data->strData.length(); ++ij) {
                bool hasAlready = false;
                for (int ii = 0; ii < myStrs.length() && !hasAlready; ++ii)
                    hasAlready = inQString(data->strData[ij], myStrs[ii]);
                if (!hasAlready)
                    addedStrs.append(data->strData[ij]);
            }
            data = data->parent.get();
        }
    }
    QStringList toAddStrs;
    for (int i = 0; i < toAdd.length(); ++i) {
        components.append(toAdd.component(i));
        QStringView compStrView = toAdd.component(i).stringView();
        if (!compStrView.isEmpty()) {
            for (int j = 0; j < addedStrs.length(); ++j) {
                if (inQString(compStrView, addedStrs[j])) {
                    toAddStrs.append(addedStrs[j]);
                    addedStrs.removeAt(j);
                    break;
                }
            }
        }
    }
    return Path(0, m_length + toAdd.length(),
                std::make_shared<PathData>(toAddStrs, components, ((m_endOffset == 0) ? m_data : noEndOffset().m_data)));
}

Path Path::expandFront() const
{
    int newLen = 0;
    auto data = m_data.get();
    while (data) {
        newLen += data->components.length();
        data = data->parent.get();
    }
    newLen -= m_endOffset;
    return Path(m_endOffset, newLen, m_data);
}

Path Path::expandBack() const
{
    if (m_endOffset > 0)
        return Path(0, m_length + m_endOffset, m_data);
    return *this;
}

Path &Path::operator++()
{
    if (m_length > 0)
        m_length -= 1;
    return *this;
}

Path Path::operator++(int)
{
    Path res = *this;
    if (m_length > 0)
        m_length -= 1;
    return res;
}

int Path::cmp(const Path &p1, const Path &p2)
{
    // lexicographic ordering
    const int lMin = qMin(p1.m_length, p2.m_length);
    if (p1.m_data.get() == p2.m_data.get() && p1.m_endOffset == p2.m_endOffset && p1.m_length == p2.m_length)
        return 0;
    for (int i = 0; i < lMin; ++i) {
        int c = Component::cmp(p1.component(i), p2.component(i));
        if (c != 0)
            return c;
    }
    if (lMin < p2.m_length)
        return -1;
    if (p1.m_length > lMin)
        return 1;
    return 0;
}

Path::Path(quint16 endOffset, quint16 length, std::shared_ptr<PathData> data)
    :m_endOffset(endOffset), m_length(length), m_data(data)
{
}

Path Path::noEndOffset() const
{
    if (m_length == 0)
        return Path();
    if (m_endOffset == 0)
        return *this;
    // peel back
    qint16 endOffset = m_endOffset;
    std::shared_ptr<PathData> lastData = m_data;
    while (lastData && endOffset >= lastData->components.length()) {
        endOffset -= lastData->components.length();
        lastData = lastData->parent;
    }
    if (endOffset > 0) {
        Q_ASSERT(lastData && "Internal problem, reference to non existing PathData");
        return Path(0, m_length, std::make_shared<PathData>(lastData->strData, lastData->components.mid(0, lastData->components.length() - endOffset), lastData->parent));
    }
    return Path(0, m_length, lastData);
}

ErrorGroups Path::myErrors()
{
    static ErrorGroups res = {{NewErrorGroup("PathParsing")}};
    return res;
}

void Path::dump(Sink sink) const
{
    bool first = true;
    for (int i = 0; i < m_length; ++i) {
        auto c = component(i);
        if (!c.hasSquareBrackets()) {
            if (!first || (c.kind() != Kind::Root && c.kind() != Kind::Current))
                sink(u".");
        }
        c.dump(sink);
        first = false;
    }
}

QString Path::toString() const
{
    QString res;
    QTextStream stream(&res);
    dump([&stream](QStringView str){ stream << str; });
    stream.flush();
    return res;
}

Path Path::dropFront() const
{
    if (m_length > 0)
        return Path(m_endOffset, m_length - 1, m_data);
    return Path();
}

Path Path::dropTail() const
{
    if (m_length > 0)
        return Path(m_endOffset + 1, m_length - 1, m_data);
    return Path();
}

Path Path::mid(int offset, int length) const
{
    length = qMin(m_length - offset, length);
    if (offset < 0 || offset >= m_length || length <= 0 || length > m_length)
        return Path();
    int newEndOffset = m_endOffset + m_length - offset - length;
    return Path(newEndOffset, length, m_data);
}

Path Path::mid(int offset) const
{
    return mid(offset, m_length - offset);
}

Path Path::fromString(QString s, ErrorHandler errorHandler)
{
    Path res = fromString(QStringView(s), errorHandler);
    if (res.m_data)
        res.m_data->strData.append(s);
    return res;
}

} // end namespace Dom
} // end namespace QQmlJS
QT_END_NAMESPACE
