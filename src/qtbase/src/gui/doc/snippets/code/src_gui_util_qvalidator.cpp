/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QLineEdit>
#include <QValidator>
#include <QWidget>

namespace src_gui_util_qvalidator {

struct Wrapper : public QWidget {
    void wrapper0();
    void wrapper1();
    void wrapper2();
    void wrapper3();
};

void Wrapper::wrapper0() {
//! [0]
QValidator *validator = new QIntValidator(100, 999, this);
QLineEdit *edit = new QLineEdit(this);

// the edit lineedit will only accept integers between 100 and 999
edit->setValidator(validator);
//! [0]


//! [1]
QString str;
int pos = 0;
QIntValidator v(100, 900, this);

str = "1";
v.validate(str, pos);     // returns Intermediate
str = "012";
v.validate(str, pos);     // returns Intermediate

str = "123";
v.validate(str, pos);     // returns Acceptable
str = "678";
v.validate(str, pos);     // returns Acceptable

str = "999";
v.validate(str, pos);    // returns Intermediate

str = "1234";
v.validate(str, pos);     // returns Invalid
str = "-123";
v.validate(str, pos);     // returns Invalid
str = "abc";
v.validate(str, pos);     // returns Invalid
str = "12cm";
v.validate(str, pos);     // returns Invalid
//! [1]
} // Wrapper::wrapper0

void Wrapper::wrapper1() {
QString s;
QIntValidator v(100, 900, this);

//! [2]
int pos = 0;

s = "abc";
v.validate(s, pos);    // returns Invalid

s = "5";
v.validate(s, pos);    // returns Intermediate

s = "50";
v.validate(s, pos);    // returns Acceptable
//! [2]
} // Wrapper::wrapper1


void Wrapper::wrapper2() {

//! [5]
// regexp: optional '-' followed by between 1 and 3 digits
QRegularExpression rx("-?\\d{1,3}");
QValidator *validator = new QRegularExpressionValidator(rx, this);

QLineEdit *edit = new QLineEdit(this);
edit->setValidator(validator);
//! [5]

//! [6]
// integers 1 to 9999
QRegularExpression re("[1-9]\\d{0,3}");
// the validator treats the regexp as "^[1-9]\\d{0,3}$"
QRegularExpressionValidator v(re, 0);
QString s;
int pos = 0;

s = "0";     v.validate(s, pos);    // returns Invalid
s = "12345"; v.validate(s, pos);    // returns Invalid
s = "1";     v.validate(s, pos);    // returns Acceptable

re.setPattern("\\S+");            // one or more non-whitespace characters
v.setRegularExpression(re);
s = "myfile.txt";  v.validate(s, pos); // Returns Acceptable
s = "my file.txt"; v.validate(s, pos); // Returns Invalid

// A, B or C followed by exactly five digits followed by W, X, Y or Z
re.setPattern("[A-C]\\d{5}[W-Z]");
v.setRegularExpression(re);
s = "a12345Z"; v.validate(s, pos);        // Returns Invalid
s = "A12345Z"; v.validate(s, pos);        // Returns Acceptable
s = "B12";     v.validate(s, pos);        // Returns Intermediate

// match most 'readme' files
re.setPattern("read\\S?me(\\.(txt|asc|1st))?");
re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
v.setRegularExpression(re);
s = "readme";      v.validate(s, pos); // Returns Acceptable
s = "README.1ST";  v.validate(s, pos); // Returns Acceptable
s = "read me.txt"; v.validate(s, pos); // Returns Invalid
s = "readm";       v.validate(s, pos); // Returns Intermediate
//! [6]

} // Wrapper::wrapper2

void Wrapper::wrapper3()
{
//! [7]
QString input = "0.98765e2";
QDoubleValidator val;
val.setLocale(QLocale::C);
val.setNotation(QDoubleValidator::ScientificNotation);
val.fixup(input); // input == "9.8765e+01"
//! [7]
//! [8]
input = "-1234.6789";
val.setDecimals(2);
val.setLocale(QLocale::C);
val.setNotation(QDoubleValidator::StandardNotation);
val.fixup(input); // input == "-1234.68"
//! [8]
} // Wrapper::wrapper3

} // src_gui_util_qvalidator
