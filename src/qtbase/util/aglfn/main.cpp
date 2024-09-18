// REUSE-IgnoreStart
// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// REUSE-IgnoreEnd

#include <qbytearray.h>
#include <qlist.h>
#include <qmap.h>
#include <qfile.h>

static QByteArray fileVersion;

static QList<QByteArray> glyphNames;
static QList<ushort> glyphNameOffsets;
static QMap<ushort, ushort> uni_to_agl_map; // sort by code point for bsearch

static void readGlyphList()
{
    qDebug("Reading aglfn.txt:");
    QFile f("data/aglfn.txt");
    if (!f.exists())
        qFatal("Couldn't find aglfn.txt");

    f.open(QFile::ReadOnly);

    glyphNames.append(".notdef");
    glyphNameOffsets.append(0);
    uni_to_agl_map.insert(0, 0);
    while (!f.atEnd()) {
        QByteArray line;
        line.resize(1024);
        int len = f.readLine(line.data(), 1024);
        line.resize(len-1);

        int comment = line.indexOf('#');
        if (comment != -1) {
            if (fileVersion.isEmpty()) {
                int commentTableVersion = line.indexOf("Table version:", comment);
                if (commentTableVersion != -1)
                    fileVersion = line.mid(commentTableVersion + 15).trimmed();
            }
            line = line.left(comment);
        }
        line = line.trimmed();

        if (line.isEmpty())
            continue;

        QList<QByteArray> l = line.split(';');
        Q_ASSERT(l.size() == 3);

        bool ok;
        ushort codepoint = l[0].toUShort(&ok, 16);
        Q_ASSERT(ok);
        QByteArray glyphName = l[1].trimmed();

        int glyphIndex = glyphNames.indexOf(glyphName);
        if (glyphIndex == -1) {
            glyphIndex = glyphNames.size();
            glyphNameOffsets.append(glyphNameOffsets.last() + glyphNames.last().size() + 1);
            glyphNames.append(glyphName);
        }
        uni_to_agl_map.insert(codepoint, glyphIndex);
    }

    qDebug("    %d unique glyph names found", glyphNames.size());
}

static QByteArray createGlyphList()
{
    qDebug("createGlyphList:");

    QByteArray out;

    out += "static const char glyph_names[] =\n\"";
    int linelen = 2;
    for (int i = 0; i < glyphNames.size(); ++i) {
        if (linelen + glyphNames.at(i).size() + 2 >= 80) {
            linelen = 2;
            out += "\"\n\"";
        }
        linelen += glyphNames.at(i).size() + 2;
        out += glyphNames.at(i) + "\\0";
    }
    if (out.endsWith("\""))
        out.chop(2);
    out += "\"\n;\n\n";

    out += "struct AGLEntry {\n"
           "    unsigned short uc;\n"
           "    unsigned short index;\n"
           "};\n"
           "\n"
           "inline bool operator<(unsigned short uc, AGLEntry entry)\n"
           "{ return uc < entry.uc; }\n"
           "inline bool operator<(AGLEntry entry, unsigned short uc)\n"
           "{ return entry.uc < uc; }\n"
           "\n"
           "static const AGLEntry unicode_to_agl_map[] = {";

    int i = 0;
    QMap<ushort, ushort>::const_iterator it = uni_to_agl_map.constBegin();
    while (it != uni_to_agl_map.constEnd()) {
        if (i++ % 4 == 0)
            out += "\n   ";
        out += " { 0x" + QByteArray::number(it.key(), 16).rightJustified(4, '0') + ", ";
        out += QByteArray::number(glyphNameOffsets.at(it.value())).leftJustified(4, ' ') + " },";
        ++it;
    }
    out.chop(1);
    out += "\n};\n\n";

    out += "enum { unicode_to_agl_map_size = sizeof(unicode_to_agl_map) / sizeof(unicode_to_agl_map[0]) };\n\n";

    qDebug("    Glyph names list uses : %d bytes", glyphNameOffsets.last() + glyphNames.last().size() + 1);
    qDebug("    Unicode to Glyph names map uses : %d bytes", uni_to_agl_map.size()*4);

    return out;
}


int main(int, char **)
{
    readGlyphList();
// REUSE-IgnoreStart
    QByteArray header =
        "// Copyright (C) 2016 The Qt Company Ltd.\n"
        "// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only\n"
        "\n";
// REUSE-IgnoreEnd

    QByteArray note =
        "/* This file is autogenerated from the Adobe Glyph List database" +
        (!fileVersion.isEmpty() ? " v" + fileVersion : "") + ". Do not edit */\n\n";

    QFile f("../../src/gui/text/qfontsubset_agl.cpp");
    f.open(QFile::WriteOnly|QFile::Truncate);
    f.write(header);
    f.write(note);
    f.write("namespace {\n\n");
    f.write(createGlyphList());
    f.write("}\n");
    f.close();
}
