/***************************************************************************
 * Copyright 2010 Pablo Daniel Pareja Obregon                              *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef SYNTAXHIGHLIGHTERS_H
#define SYNTAXHIGHLIGHTERS_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

// Forward declarations
class QTextDocument;

namespace Caneda
{
    class Highlighter : public QSyntaxHighlighter
    {
        Q_OBJECT

    public:
        Highlighter(QTextDocument *parent = 0);

    protected:
        void highlightBlock(const QString &text);

        struct HighlightingRule
        {
            QRegExp pattern;
            QTextCharFormat format;
        };
        QVector<HighlightingRule> highlightingRules;

        QRegExp commentStartExpression;
        QRegExp commentEndExpression;

        QTextCharFormat keywordFormat;
        QTextCharFormat typeFormat;
        QTextCharFormat signalFormat;
        QTextCharFormat blockFormat;
        QTextCharFormat classFormat;
        QTextCharFormat quotationFormat;
        QTextCharFormat singleLineCommentFormat;
        QTextCharFormat multiLineCommentFormat;
    };


    class VhdlHighlighter : public Highlighter
    {
        Q_OBJECT
    public:
        VhdlHighlighter(QTextDocument *parent = 0);
    };


    class VerilogHighlighter : public Highlighter
    {
        Q_OBJECT
    public:
        VerilogHighlighter(QTextDocument *parent = 0);
    };

} // namespace Caneda

#endif // SYNTAXHIGHLIGHTERS_H
